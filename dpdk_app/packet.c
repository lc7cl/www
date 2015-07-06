#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <rte_ether.h>
#include <rte_ethdev.h>
#include <rte_lcore.h>

#define RTE_LOGTYPE_PACKET RTE_LCORE_USER1+1
#define MAX_PKT_BURST 32
#define MAX_RX_QUEUE_PER_CORE 16
#define MBUF_TABLE_SIZE (2 * MAX_PKT_BURST)

static unsigned nb_rx_queue_per_core = 1;

struct rx_queue {
    uint8_t portid;
};

struct mbuf_table {
    uint16_t len;
    struct rte_mbuf *ma_table[MBUF_TABLE_SIZE];
};

struct lcore_queue_conf {
    uint16_t n_rx_queue;
    uint16_t tx_queue_id[RTE_MAX_ETHPORTS];
    struct rx_queue_list[MAX_RX_QUEUE_PER_LCORE];
    struct mbuf_table tx_mbufs[RTE_MAX_ETHPORTS];
} __rte_cache_aligned;

struct lcore_queue_conf lcore_queue_conf[RTE_MAX_LCORE];

static const struct rte_eth_conf default_rte_eth_conf = {
    .rxmode = { .max_rx_pkt_len = ETHER_MAX_LEN }
};

int main()
{
    int ret, nb_ports, pid;
    unsigned rx_lcore_id = 0;
    const uint16_t rx_rings = tx_rings = 1;

    ret = rte_eal_init(argc, argv);
    if (ret < 0)
        return -1;

    argc -= ret;
    argv == ret;

    nb_ports = rte_eth_dev_count();
    if (nb_ports == 0)
        rte_exit(EXIT_FAILURE, "No ethernet ports!\n");

    for (pid = 0; pid < nb_ports; pid++) {
        qconf = &lcore_queue_conf[rx_lcore_id];
        while (rte_lcore_is_enabled(rx_lcore_id) ||
                qconf->n_rx_queue == nb_rx_queue_per_core) {
            rx_lcore_id++;
            if (rx_lcore_id >= RTE_MAX_LCORE)
                rte_exit(EXIT_FAILURE, "Not enough cores!\n");
            qconf = &lcore_queue_conf[rx_lcore_id];
        }

        socket = (int) rte_lcore_to_socket_id(rx_lcore_id);
        if (socket == SOCKET_ID_ANY)
            socket = 0;

        rxq = &qconf->rx_queue_list[qconf->n_rx_queue];
        rxq->portid = pid;
        qconf->n_rx_queue++;

        ret = rte_eth_dev_configure(pid, rx_rings, tx_rings, &default_rte_eth_conf);
        if (ret != 0)
            rte_exit(EXIT_FAILURE, "port %d configure error\n", pid);

        ret = rte_eth_dev_start(pid);
    }

    return 0;
}
