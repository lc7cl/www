#include <stdio.h>
#include <arpa/inet.h>

#include <rte_mbuf.h>
#include <rte_log.h>
#include <rte_ethdev.h>

#include <sk.h>
#include <port_queue_map.h>
#include <common/common.h>
#include <common/dns.h>

#define NB_MBUF 1024
#define RTE_LOGTYPE_TEST_UDP (RTE_LOGTYPE_TEST+2)
#define RX_BURST_NUM 32

static uint16_t tx_rings = 1;
static uint16_t rx_rings = 1;
static uint8_t 	nb_ports = 1;
static unsigned nb_rx_queue_per_core = 1;
static uint32_t nb_txd = 32;
static uint32_t nb_rxd = 32;

static struct rte_mempool * pkt_mbuf_pool = NULL;

struct mbuf_table {
	struct rte_mbuf *mb[RX_BURST_NUM];
	int len;
};

static const struct rte_eth_conf default_rte_eth_conf = {
    .rxmode = { .max_rx_pkt_len = ETHER_MAX_LEN,
                .mq_mode = ETH_MQ_RX_RSS},
    .txmode = { .mq_mode = ETH_MQ_TX_NONE },
};

static struct queue_conf {
	struct mbuf_table mtables[RTE_MAX_ETHPORTS];
} queue_conf[RTE_MAX_LCORE];

static void process_udp(struct rte_mbuf *m, uint32_t src_addr, uint16_t src_port) 
{
	struct dns_hdr *dns_hdr;
	struct rr *r;

	dns_hdr = rte_pktmbuf_mtod(m, struct dns_hdr *);
	r = (struct rr *)(dns_hdr + 1);
	if (dns_hdr->qr == 0
		&& dns_hdr->qdcount) {
		printf("question : %s\n", r->name);
	} else {
		printf("format error!\n");
	}	
}

static int packet_launch_one_lcore(__rte_unused void *unused)
{
	unsigned lcore;
	int i;
	struct rx_queue *rxq;
	struct lcore_queue_conf *lcore_q;
	struct rte_mbuf **pmb;
    char path[64];
    struct queue_conf *qconf;

	lcore = rte_lcore_id();
	lcore_q = lcore_q_conf_get(lcore);
	if (lcore_q->nb_rxq == 0) {
		RTE_LOG(ALERT, TEST_UDP, "lcore %u hasn't any rx queue!\n", lcore);
		return 1;
	}
    qconf = &queue_conf[lcore];

	for (;;) {
		for (i = 0; i < lcore_q->nb_rxq; i++) {
			rxq = &lcore_q->rxq[i];
			pmb = qconf->mtables[rxq->port].mb;
			qconf->mtables[rxq->port].len = 
				rte_eth_rx_burst(rxq->port, rxq->qid, pmb, RX_BURST_NUM);
            if (qconf->mtables[rxq->port].len) {
                netif_rx(pmb, qconf->mtables[rxq->port].len);
            }
		}
	}	
	
	return 0;
}

int main(int argc, char ** argv)
{
	int ret, i, socket;
	uint8_t pid, nb_ports;	
	unsigned lcore_id;
	struct port_queue_conf *port_q;
	struct lcore_queue_conf *lcore_q;	
	struct rte_mbuf *mbuf;
	struct rx_queue *rxq;
	struct sock *sk;
	struct sock_parameter sk_param;
	uint32_t default_addr = 0;
	uint16_t default_port = 53;
	
	ret = rte_eal_init(argc, argv);
	if (ret < 0)
		return -1;
	argc -= ret;
	argv += ret;

	if (argc > 1) {
		argc--;
		default_addr = inet_addr(argv[1]);
		if (argc == 3)
			default_port = rte_cpu_to_be_16((uint16_t)strtol(argv[2], NULL, 10));
		else
			rte_exit(EXIT_FAILURE, "invalid arguments\n");
	}

	/*init proto stack*/
	ret = inet_init();
	if (ret)
		rte_exit(EXIT_FAILURE, "Cannot init af_inet\n");

	/*init net device*/
	nb_ports = rte_eth_dev_count();
    if (nb_ports == 0)
        rte_exit(EXIT_FAILURE, "No ethernet ports!\n");	
	for (pid = 0; pid < nb_ports; pid++) {
		ret = net_device_init(pid);
		if (ret) {
			RTE_LOG(WARNING, TEST_UDP, "init net device error!\n");
			goto release_net_device;
		}		
	}
	
	pkt_mbuf_pool = rte_pktmbuf_pool_create("pkt_mbuf_pool", NB_MBUF, 32, 0, RTE_MBUF_DEFAULT_BUF_SIZE, rte_socket_id());
    if (pkt_mbuf_pool == NULL)
        rte_exit(EXIT_FAILURE, "Cannot init mbuf pool\n");

	/*create sock*/
	sk_param.mode = SOCK_MODE_COMPLETE;
	sk_param.func = process_udp;
	sk = create_sock(0, SOCK_PTOTO_IPPROTO_UDP, &sk_param);
	if (sk == NULL)
		rte_exit(EXIT_FAILURE, "Cannot create sock\n");
	if (sock_bind(sk, 0, 53))
		rte_exit(EXIT_FAILURE, "Sock bind error!\n");

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
		queue_conf[lcore_id].mtables[pid].len = RX_BURST_NUM;
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
