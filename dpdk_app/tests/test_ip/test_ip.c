#include <stdio.h>

#include <rte_ethdev.h>
#include <af_inet.h>
#include <netif.h>
#include <hook.h>
#include <port_queue_map.h>

#define NB_MBUF 1024
#define RTE_LOGTYPE_TEST_IPV4 (RTE_LOGTYPE_TEST+1)
#define RX_BURST_NUM 32
#define DUMP_FILE_PATH "/var/log/dpdk_rx_dump_lcore%u"

static uint16_t tx_rings = 1;
static uint16_t rx_rings = 1;
static uint8_t 	nb_ports = 1;
static unsigned nb_rx_queue_per_core = 1;
static uint32_t nb_txd = 32;
static uint32_t nb_rxd = 32;

static struct rte_mempool * pkt_mbuf_pool = NULL;

static const struct rte_eth_conf default_rte_eth_conf = {
    .rxmode = { .max_rx_pkt_len = ETHER_MAX_LEN,
                .mq_mode = ETH_MQ_RX_RSS},
    .txmode = { .mq_mode = ETH_MQ_TX_NONE },
};

struct mbuf_table {
	struct rte_mbuf *mb[RX_BURST_NUM];
	int len;
};

static struct queue_conf {
	struct mbuf_table rx_mbufs[RTE_MAX_ETHPORTS];
    FILE *dumpf;
} lcore_queue_conf[RTE_MAX_LCORE];

static int test_ip_rcv(struct rte_mbuf *mb)
{
    struct queue_conf *qconf;

    qconf = &lcore_queue_conf[rte_lcore_id()];
    if (qconf->dumpf) {
        rte_pktmbuf_dump(qconf->dumpf, mb, rte_pktmbuf_data_len(mb));
    }

	return HOOK_RET_ACCEPT;
}

static struct hook_ops test_ip_hook = {
	.proto = HOOK_PROTO_IPV4,
	.pos = HOOK_PRE_ROUTING,
	.func = test_ip_rcv,
};

static int packet_launch_one_lcore(__rte_unused void *unused)
{
	unsigned lcore;
	int i;
	struct txrx_queue *rxq;
	struct lcore_queue_conf *lcore_q;
	struct rte_mbuf **pmb;
    char path[64];
    struct queue_conf *qconf;

	lcore = rte_lcore_id();
	lcore_q = lcore_q_conf_get(lcore);
	if (lcore_q->nb_rxq == 0) {
		RTE_LOG(ALERT, TEST_IPV4, "lcore %u hasn't any rx queue!\n", lcore);
		return 1;
	}
    qconf = &lcore_queue_conf[lcore];

    snprintf(path, sizeof path, DUMP_FILE_PATH, lcore);
    if (qconf->dumpf == NULL) {
        qconf->dumpf = fopen(path, "w+");
        if (qconf->dumpf == NULL) {
            RTE_LOG(ERR, TEST_IPV4, "open file %s error:%s!\n", path, strerror(errno));
            return 1;
        }
    }

	for (;;) {
		for (i = 0; i < lcore_q->nb_rxq; i++) {
			rxq = &lcore_q->rxq[i];
			pmb = qconf->rx_mbufs[rxq->port].mb;
			qconf->rx_mbufs[rxq->port].len = 
				rte_eth_rx_burst(rxq->port, rxq->qid, pmb, RX_BURST_NUM);
            if (qconf->rx_mbufs[rxq->port].len) {
                netif_rx(pmb, qconf->rx_mbufs[rxq->port].len);
            }
		}
	}	

    if (qconf->dumpf)
        fclose(qconf->dumpf);
	
	return 0;
}

int main(int argc, char* argv[])
{
	int ret, i, socket;
	uint8_t pid, nb_ports;
	unsigned lcore_id;
	struct port_queue_conf *port_q;
	struct lcore_queue_conf *lcore_q;
	struct rte_mbuf *mbuf;
	struct rx_queue *rxq;

	ret = rte_eal_init(argc, argv);
    if (ret < 0)
        return -1;

    argc -= ret;
    argv -= ret;

	/*init proto stack*/
	ret = inet_init();
	if (ret)
		rte_exit(EXIT_FAILURE, "Cannot init af_inet\n");

	/*register test hook*/
    hook_register(&test_ip_hook);

	/*init net device*/
	nb_ports = rte_eth_dev_count();
    if (nb_ports == 0)
        rte_exit(EXIT_FAILURE, "No ethernet ports!\n");	
	for (pid = 0; pid < nb_ports; pid++) {
		ret = net_device_init(pid);
		if (ret) {
			RTE_LOG(WARNING, TEST, "init net device error!\n");
			goto release_net_device;
		}		
	} 
	
	pkt_mbuf_pool = rte_pktmbuf_pool_create("pkt_mbuf_pool", NB_MBUF, 32, 0, RTE_MBUF_DEFAULT_BUF_SIZE, rte_socket_id());
    if (pkt_mbuf_pool == NULL)
        rte_exit(EXIT_FAILURE, "Cannot init mbuf pool\n");

	/*init eth dev*/
	lcore_id = 0;
	lcore_q = lcore_q_conf_get(lcore_id);
    for (pid = 0; pid < nb_ports; pid++) {
		port_q = port_q_conf_get(pid);
		/*config eth dev*/
        ret = rte_eth_dev_configure(pid, rx_rings, tx_rings, &default_rte_eth_conf);
        if (ret != 0)
            rte_exit(EXIT_FAILURE, "port %d configure error\n", pid);

		/*get available lcore id*/
        while (lcore_id == rte_get_master_lcore() || 
                !rte_lcore_is_enabled(lcore_id) || 
                lcore_q->nb_rxq == nb_rx_queue_per_core) {
            lcore_id++;
            if (lcore_id >= RTE_MAX_LCORE)
                rte_exit(EXIT_FAILURE, "Not enough cores!\n");
            lcore_q = lcore_q_conf_get(lcore_id);
        }
		lcore_queue_conf[lcore_id].rx_mbufs[pid].len = RX_BURST_NUM;
		/*port - lcore - queue map*/
		rxq = &lcore_q->rxq[lcore_q->nb_rxq];
        rxq->port = pid;
		rxq->lcore = lcore_id;
		rxq->qid = port_q->nb_rxq;
        lcore_q->nb_rxq++;
		port_q->nb_rxq++;

		/*init lcore_queue_conf*/
        socket = (int) rte_lcore_to_socket_id(lcore_id);
        if (socket == SOCKET_ID_ANY)
            socket = 0;

		/*set tx/rx queue and start eth dev*/
        ret = rte_eth_tx_queue_setup(pid, rxq->qid, nb_txd, socket, NULL);
        if (ret < 0)
            rte_exit(EXIT_FAILURE, "txq on port %d setup error\n", pid);

        ret = rte_eth_rx_queue_setup(pid, rxq->qid, nb_rxd, socket, NULL, pkt_mbuf_pool);
        if (ret < 0)
            rte_exit(EXIT_FAILURE, "rxq on port %d setup error\n", pid);

        ret = rte_eth_dev_start(pid);
        if (ret < 0)
            rte_exit(EXIT_FAILURE, "rte_eth_dev_start err=%d port=%u\n", ret, pid);
    }

    rte_eal_mp_remote_launch(packet_launch_one_lcore, NULL, SKIP_MASTER);
    RTE_LCORE_FOREACH_SLAVE(lcore_id) {
        if (rte_eal_wait_lcore(lcore_id) < 0)
            return -1;        
    }

	return 0;
release_net_device:
	for (pid; pid > 0; pid--)
		net_device_release(pid - 1);	
	return -1;
}
