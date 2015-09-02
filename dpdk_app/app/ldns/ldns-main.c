#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <rte_ether.h>
#include <rte_ethdev.h>
#include <rte_lcore.h>

#include <netdev.h>
#include <port_queue_map.h>
#include <sk.h>
#include <common/dns.h>

#include "dispatch.h"

#define RTE_LOGTYPE_LDNS (RTE_LOGTYPE_USER7 + 1)

/*net config*/
char* default_gw_addr = "192.168.179.1";
char* default_gw_mac = "005056c00008";
char* default_host_addr = "192.168.179.128";
#define DNS_PORT 53

/*nic config*/
#define RX_BURST_NUM 32
static uint16_t rx_rings = 8;
static uint16_t tx_rings = 8;
static uint32_t nb_rxd = 32;
static uint32_t nb_txd = 32;
static unsigned nb_rx_queue_per_core = 1;
#define PKT_RX_NB 4096
static struct rte_mempool *pkt_rx_pool;

static const struct rte_eth_conf default_rte_eth_conf = {
    .rxmode = {
        .max_rx_pkt_len = ETHER_MAX_LEN,
        .mq_mode = ETH_MQ_RX_RSS
    },
    .txmode = {
        .mq_mode = ETH_MQ_TX_NONE,
    },
};

static void dns_process(struct rte_mbuf *mbuf, uint32_t addr, uint16_t port)
{
    dispatch_dns_pkt(mbuf, addr, port);
}

static int packet_launch_one_lcore(__rte_unused void* unused)
{
    unsigned lcore;
    int i;
    struct txrx_queue *rxq;
    struct lcore_queue_conf *lcore_q;

    lcore = rte_lcore_id();

    return 0;
}

int main(int argc, char ** argv)
{
    int ret, socket;
    unsigned pid, nb_ports, lcore_id, rx_lcore_id;
    struct sock_parameter sk_param;
    struct sock *sk;
    struct txrx_queue *rxq;
    struct port_queue_conf *port_q;
    struct lcore_queue_conf *lcore_q;

    ret = rte_eal_init(argc, argv);
    if (ret < 0)
        return -1;
    argc -= ret;
    argv += ret;

    /*parse gw ip and mac from cmdline*/
    if (argc > 1) {
        default_host_addr = argv[1];
        if (argc == 3)
            default_gw_addr = argv[2];
        else if (argc == 4)
            default_gw_mac = argv[3];
        else
            rte_exit(EXIT_FAILURE, "invalid arguments\n");
    }

    /*config nic*/
    nb_ports = rte_eth_dev_count();
    if (nb_ports == 0)
        rte_exit(EXIT_FAILURE, "No available NIC\n");
    for (pid = 0; pid < nb_ports; pid++) {
        ret = net_device_init(pid);
        if (ret) {
            RTE_LOG(WARNING, LDNS, "fail to initialize port %u\n", pid);
            goto release_net_device;
        }
    }
    pkt_rx_pool = rte_pktmbuf_pool_create("ldns rx pkt pool",
            PKT_RX_NB,
            32,
            0,
            RTE_MBUF_DEFAULT_BUF_SIZE,
            rte_socket_id());
    if (pkt_rx_pool == NULL)
        rte_exit(EXIT_FAILURE, "cannot alloc rx_mbuf_pool");
    
    /*sock create*/
    sk_param.mode = SOCK_MODE_COMPLETE;
    sk_param.func = dns_process;
    sk = create_sock(0, SOCK_PTOTO_IPPROTO_UDP, &sk_param);
    if (sk == NULL)
        rte_exit(EXIT_FAILURE, "cannot create sock\n");
    if (sock_bind(sk, inet_network(default_host_addr), DNS_PORT))
        rte_exit(EXIT_FAILURE, "cannot bind addr:%s port:%u",
                default_host_addr, DNS_PORT);

    /*init ethdev*/
    lcore_id = 0;
    lcore_q = lcore_q_conf_get(lcore_id);
    for (pid = 0; pid < nb_ports; pid++) {
        port_q = port_q_conf_get(pid);
        ret = rte_eth_dev_configure(pid, rx_rings, tx_rings, &default_rte_eth_conf);
        if (ret != 0)
            rte_exit(EXIT_FAILURE, "port %u configure error\n", pid);

        while (rx_lcore_id == rte_get_master_lcore()
                || !rte_lcore_is_enabled(rx_lcore_id)
                || lcore_q->nb_rxq == nb_rx_queue_per_core) {
            rx_lcore_id++;
            if (rx_lcore_id == RTE_MAX_LCORE)
                rte_exit(EXIT_FAILURE, "not enough core for port %u\n", pid);
            lcore_q = lcore_q_conf_get(lcore_id);
        }

        rxq = &lcore_q->rxq[lcore_q->nb_rxq];
        rxq->port = pid;
        rxq->lcore = rx_lcore_id;
        rxq->qid = port_q->nb_rxq;
        lcore_q->nb_rxq++;
        port_q->nb_rxq++;

        socket = rte_lcore_to_socket_id(rx_lcore_id);
        if (socket == SOCKET_ID_ANY)
            socket = 0;

        ret = rte_eth_tx_queue_setup(pid, rxq->qid, nb_txd, socket, NULL);
        if (ret < 0)
            rte_exit(EXIT_FAILURE, "fail to setup txq %u on port %u",
                    rxq->qid, pid);
        ret = rte_eth_rx_queue_setup(pid, rxq->qid, nb_rxd, socket, NULL, pkt_rx_pool);
        if (ret < 0)
            rte_exit(EXIT_FAILURE, "failt to setup rxq %u on port %u",
                    rxq->qid, pid);

        ret = rte_eth_dev_start(pid);
        if (ret < 0)
            rte_exit(EXIT_FAILURE, "fail to start port %u\n", pid);
    }

    rte_eal_mp_remote_launch(packet_launch_one_lcore, NULL, SKIP_MASTER);
    RTE_LCORE_FOREACH_SLAVE(lcore_id) {
        if (rte_eal_wait_lcore(lcore_id) < 0)
            return -1;
    }

    return 0;

release_net_device:
    for (pid; pid != 0; pid--) {
        net_device_release(pid - 1);
    }
    return -1;
}

