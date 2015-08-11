#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/queue.h>

#include <rte_ether.h>
#include <rte_ethdev.h>
#include <rte_lcore.h>
#include <rte_cycles.h>
#include <rte_ip.h>
#include <rte_udp.h>

#include "netif.h"

#define RTE_LOGTYPE_PACKET RTE_LOGTYPE_USER1+1
#define MAX_PKT_BURST 32
#define MAX_RX_QUEUE_PER_CORE 16
#define MBUF_TABLE_SIZE (2 * MAX_PKT_BURST)
#define NB_MBUF 8192

#define IP_DEFTTL  64   /* from RFC 1340. */
#define IP_VERSION 0x40
#define IP_HDRLEN  0x05 /* default IP header length == five 32-bits words. */
#define IP_VHL_DEF (IP_VERSION | IP_HDRLEN)


static unsigned nb_rx_queue_per_core = 1;

struct rx_queue {
    uint8_t portid;
	uint8_t queue_id;
};

struct mbuf_table {
    uint16_t len;
    struct rte_mbuf *ma_table[MBUF_TABLE_SIZE];
};

struct lcore_queue_conf {
    uint16_t n_rx_queue;
    uint16_t tx_queue_id[RTE_MAX_ETHPORTS];
    struct rx_queue rx_queue_list[MAX_RX_QUEUE_PER_CORE];
	struct mbuf_table rx_mbufs;
	uint8_t cur_rx_pkt_pos;
    struct mbuf_table tx_mbufs[RTE_MAX_ETHPORTS];
	FILE *out;
} __rte_cache_aligned;

struct lcore_queue_conf lcore_queue_conf[RTE_MAX_LCORE];

static const struct rte_eth_conf default_rte_eth_conf = {
    .rxmode = { .max_rx_pkt_len = ETHER_MAX_LEN,
                .mq_mode = ETH_MQ_RX_RSS},
    .txmode = { .mq_mode = ETH_MQ_TX_NONE },
};

static struct rte_mempool * pkt_mbuf_pool = NULL;
static uint16_t nb_rxd = 32;
static uint16_t nb_txd = 32;

static int 
build_packet(struct rte_mbuf *mbuf)
{
#define PKT_SIZE 64
	static struct ether_addr src_addr;
	static struct ether_addr dst_addr;
	struct ether_hdr *eth_hdr;
	struct ipv4_hdr *ip_hdr;
	struct udp_hdr  *udp_hdr;
	char *data;
	int i;

	dst_addr.addr_bytes[0] = 0;
	dst_addr.addr_bytes[1] = 80;
	dst_addr.addr_bytes[2] = 86;
	dst_addr.addr_bytes[3] = 192;
	dst_addr.addr_bytes[4] = 0;
	dst_addr.addr_bytes[5] = 8;
	
	src_addr.addr_bytes[0] = 0;
	src_addr.addr_bytes[1] = 12;
	src_addr.addr_bytes[2] = 41;
	src_addr.addr_bytes[3] = 239;
	src_addr.addr_bytes[4] = 132;
	src_addr.addr_bytes[5] = 227;
	

    //rte_memcpy(src_addr.addr_bytes, "000c29ef84e3", ETHER_ADDR_LEN);
    //rte_memcpy(dst_addr.addr_bytes, "005056c00008", ETHER_ADDR_LEN);
    //src_addr.addr_bytes = { 0, 12, 41, 239, 132, 227 };
    //dst_addr.addr_bytes = { 0, 80, 86, 198, 0, 8 };

	eth_hdr = rte_pktmbuf_mtod(mbuf, struct ether_hdr *);
	eth_hdr->ether_type = rte_cpu_to_be_16(ETHER_TYPE_IPv4);
	ether_addr_copy(&src_addr, &eth_hdr->s_addr);
	ether_addr_copy(&dst_addr, &eth_hdr->d_addr);

	ip_hdr = (struct ipv4_hdr *)(eth_hdr + 1);
	memset(ip_hdr, 0, sizeof(*ip_hdr));
	ip_hdr->version_ihl = IP_VHL_DEF;
	ip_hdr->type_of_service	= 0;
	ip_hdr->fragment_offset	= 0;
	ip_hdr->time_to_live	= IP_DEFTTL;
	ip_hdr->next_proto_id	= IPPROTO_UDP;
	ip_hdr->packet_id	= 0;
	ip_hdr->total_length = rte_cpu_to_be_16(PKT_SIZE + sizeof(*ip_hdr) + sizeof(*udp_hdr));
	ip_hdr->hdr_checksum = 0;//rte_ipv4_cksum(ip_hdr);
	ip_hdr->src_addr = rte_cpu_to_be_32(IPv4(192,168,179,100));
	ip_hdr->dst_addr = rte_cpu_to_be_32(IPv4(192,168,179,1));

	udp_hdr = (struct udp_hdr  *)(ip_hdr + 1);
	memset(udp_hdr, 0, sizeof(*udp_hdr));
	udp_hdr->dst_port = rte_cpu_to_be_16(53);
	udp_hdr->src_port = rte_cpu_to_be_16(12306);
	udp_hdr->dgram_cksum = 0;
	udp_hdr->dgram_len = rte_cpu_to_be_16(PKT_SIZE + sizeof(*udp_hdr));

	data = (char *)(udp_hdr + 1);
	for (i = 0; i < PKT_SIZE; i++) {
		*(data++) = 'a';
	}
	udp_hdr->dgram_cksum = rte_ipv4_udptcp_cksum(ip_hdr, udp_hdr);
	ip_hdr->hdr_checksum = rte_ipv4_cksum(ip_hdr);
	mbuf->pkt_len = PKT_SIZE + sizeof(*ip_hdr) + sizeof(*udp_hdr) + sizeof(*eth_hdr);
	mbuf->l2_len = sizeof(*eth_hdr);
	mbuf->l3_len = sizeof(*ip_hdr);
	mbuf->data_len = PKT_SIZE + sizeof(*ip_hdr) + sizeof(*udp_hdr) + sizeof(*eth_hdr);
	mbuf->next = NULL;
	mbuf->nb_segs = 1;
#undef PKT_SIZE
	return 1;
}
	

static void 
packet_main_loop(void)
{
    unsigned lcore_id;
    struct lcore_queue_conf *qconf;
    int i, nb_rx;
    uint64_t cur_tsc, portid;
    struct rx_queue *rxq;
	struct rte_mbuf *pkt;

    lcore_id = rte_lcore_id();
    qconf = &lcore_queue_conf[lcore_id];
    if (qconf->n_rx_queue == 0) {
        RTE_LOG(INFO, PACKET, "lcore %u has nothing to do\n", lcore_id);
        return;
    }

    RTE_LOG(INFO, PACKET, "entering main loop on lcore %u\n", lcore_id);

    while (1) {
        cur_tsc = rte_rdtsc();

        
        for (i = 0; i < qconf->n_rx_queue; i++) {
            rxq = &qconf->rx_queue_list[i];
            portid = rxq->portid;

			qconf->cur_rx_pkt_pos = 0;	
			qconf->rx_mbufs.len = rte_eth_rx_burst(
				portid, rxq->queue_id, 
				qconf->rx_mbufs.ma_table, MAX_PKT_BURST);
			if (qconf->rx_mbufs.len) {
				pkt = qconf->rx_mbufs.ma_table[qconf->cur_rx_pkt_pos++];
				netif_rx(pkt, qconf->rx_mbufs.len);
				if (qconf->out)
					rte_pktmbuf_dump(qconf->out, pkt, pkt->data_len);
			}
        }
        
		for (i = 0; i < RTE_MAX_ETHPORTS; i++) {
			if (qconf->tx_mbufs[i].len == 0)
				continue;

            //pkt = qconf->tx_mbufs[i].ma_table[0];
            //rte_pktmbuf_dump(qconf->out, pkt, pkt->data_len);
			qconf->tx_mbufs[i].len = rte_eth_tx_burst(
				i, 0, 
				qconf->tx_mbufs[i].ma_table, MAX_PKT_BURST);
            printf("result:%d\n", qconf->tx_mbufs[i].len);
            break;
		}
    }
}

static int 
packet_launch_one_lcore(__attribute__((unused)))
{
    packet_main_loop();
    return 0;
}

int 
main(int argc, char** argv)
{
    int ret, nb_ports, pid, socket, i;
    unsigned lcore_id, rx_lcore_id = 0, *pidlist;
    const uint16_t rx_rings = 1 , tx_rings = 1;
    struct lcore_queue_conf *qconf;
    struct rx_queue *rxq;
	char filename[64];
	struct rte_mbuf *mbuf;

    ret = rte_eal_init(argc, argv);
    if (ret < 0)
        return -1;

    argc -= ret;
    argv -= ret;


    pkt_mbuf_pool = rte_pktmbuf_pool_create("pkt_mbuf_pool", NB_MBUF, 32, 0, RTE_MBUF_DEFAULT_BUF_SIZE, rte_socket_id());
    if (pkt_mbuf_pool == NULL)
        rte_exit(EXIT_FAILURE, "Cannot init mbuf pool\n");

    nb_ports = rte_eth_dev_count();
    if (nb_ports == 0)
        rte_exit(EXIT_FAILURE, "No ethernet ports!\n");

	pidlist = malloc(sizeof(*pidlist) * nb_ports);
	if (pidlist) {
		ret = net_device_init(pidlist, nb_ports);
		free(pidlist);
		if (ret) {
			RTE_LOG(WARNING, NET, "init net device error!\n");
		}
		return -1;
	} else {
		return -1;
	}

    for (pid = 0; pid < nb_ports; pid++) {
        ret = rte_eth_dev_configure(pid, rx_rings, tx_rings, &default_rte_eth_conf);
        if (ret != 0)
            rte_exit(EXIT_FAILURE, "port %d configure error\n", pid);

        while (rx_lcore_id == rte_get_master_lcore() || 
                !rte_lcore_is_enabled(rx_lcore_id) || 
                qconf->n_rx_queue == nb_rx_queue_per_core) {
            rx_lcore_id++;
            if (rx_lcore_id >= RTE_MAX_LCORE)
                rte_exit(EXIT_FAILURE, "Not enough cores!\n");
            qconf = &lcore_queue_conf[rx_lcore_id];
        }
		
		for (i = 0; i < MBUF_TABLE_SIZE; i++) {
			mbuf = rte_pktmbuf_alloc(pkt_mbuf_pool);
			if (mbuf != NULL) {
				build_packet(mbuf);
				qconf->tx_mbufs[pid].ma_table[i] = mbuf;
				qconf->tx_mbufs[pid].len++;
			}			
		}

        socket = (int) rte_lcore_to_socket_id(rx_lcore_id);
        if (socket == SOCKET_ID_ANY)
            socket = 0;

        rxq = &qconf->rx_queue_list[qconf->n_rx_queue];
        rxq->portid = pid;
		rxq->queue_id = 0;
        qconf->n_rx_queue++;

        ret = rte_eth_tx_queue_setup(pid, 0, nb_txd, socket, NULL);
        if (ret < 0)
            rte_exit(EXIT_FAILURE, "txq on port %d setup error\n", pid);

        ret = rte_eth_rx_queue_setup(pid, 0, nb_rxd, socket, NULL, pkt_mbuf_pool);
        if (ret < 0)
            rte_exit(EXIT_FAILURE, "rxq on port %d setup error\n", pid);

        ret = rte_eth_dev_start(pid);
        if (ret < 0)
            rte_exit(EXIT_FAILURE, "rte_eth_dev_start err=%d port=%u\n", ret, pid);

		snprintf(filename, 63, "../dump_file_%d", rx_lcore_id);
		qconf->out = fopen(filename, "w");
		if (qconf->out == NULL)
			RTE_LOG(INFO, PACKET, "dump file %s init error has nothing to do\n", filename);
    }

    rte_eal_mp_remote_launch(packet_launch_one_lcore, NULL, SKIP_MASTER);
    RTE_LCORE_FOREACH_SLAVE(lcore_id) {
        if (rte_eal_wait_lcore(lcore_id) < 0)
            return -1;        
    }

    return 0;
}

