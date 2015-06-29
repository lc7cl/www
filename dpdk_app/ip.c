/*
*                           _ooOoo_
*                          o8888888o
*                          88" . "88
*                          (| ^_^ |)
*                          O\  =  /O
*                       ____/`---'\____
*                     .'  \\|     |//  `.
*                    /  \\|||  :  |||//  \
*                   /  _||||| -:- |||||-  \
*                   |   | \\\  -  /// |   |
*                   | \_|  ''\---/''  |   |
*                   \  .-\__  `-`  ___/-. /
*                 ___`. .'  /--.--  `. . ___ 
*               ."" '<  `.___\_<|>_/___.'  >'"".
*             | | :  `- \`.;`\ _ /`;.`/ - ` : | |
*             \  \ `-.   \_ __\ /__ _/   .-` /  /
*       ========`-.____`-.___\_____/___.-`____.-'======== 
*                            `=---='
*       ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
*            FoZu Bless       No Bug       No Crash
*
* Author: lee - lee@www.com
*
* Last modified: 2015-06-27 21:01
*
* Filename: ip.c
*
* Description: 
*
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <rte_ether.h>
#include <rte_ethdev.h>
#include <rte_lcore.h>

#define RTE_LOGTYPE_IP RTE_LOGTYPE_USER1+1
#define MAX_PKT_BURST 32
#define MAX_RX_QUEUE_PER_LCORE 16
#define MBUF_TABLE_SIZE (2 * MAX_PKT_BURST)



static  unsigned nb_rx_queue_per_core = 1;

struct mbuf_table {
    uint16_t len;
    struct rte_mbuf *ma_table[MBUF_TABLE_SIZE];
};

struct rx_queue {
    uint8_t portid;
};

struct lcore_queue_conf {
    uint16_t n_rx_queue;
    uint16_t tx_queue_id[RTE_MAX_ETHPORTS];
    struct rx_queue rx_queue_list[MAX_RX_QUEUE_PER_LCORE];
    struct mbuf_table tx_mbufs[RTE_MAX_ETHPORTS];
} __rte_cache_aligned;

struct lcore_queue_conf lcore_queue_conf[RTE_MAX_LCORE];

static void ip_main_loop(void)
{
    struct lcore_queue_conf *qconf;
    unsigned lcore_id, portid, i, nb_rx;
    struct rte_mbuf *pkts_burst[MAX_PKT_BURST];
    struct rx_queue *rxq;

    lcore_id = rte_lcore_id();

    qconf = &lcore_queue_conf[lcore_id];
    if (qconf->n_rx_queue == 0)
        RTE_LOG(INFO, IP, "lcore %u has nothing to do\n", lcore_id);

    while (1) {    
        for (i = 0; i < qconf->n_rx_queue; i++) {
            rxq = &qconf->rx_queue_list[i];
            portid = rxq->portid;
            nb_rx = rte_eth_rx_burst((uint8_t)portid, 0, pkts_burst, MAX_PKT_BURST);
            RTE_LOG(INFO, IP, "rx %u packets\n", nb_rx);
        }
    }
}


static int ip_launch_one_lcore(__attribute__((unused)) void* dummy)
{
    ip_main_loop();
    return 0;
}


int main(int argc, char **argv)
{
    int ret;
    int nb_ports, socket;
    struct rte_eth_dev_info dev_info;
    uint8_t portid;
    unsigned lcore_id, rx_lcore_id = 0;
    struct lcore_queue_conf *qconf;
    struct rx_queue *rxq;

    ret = rte_eal_init(argc, argv);
    if (ret < 0)
        rte_exit(EXIT_FAILURE, "Invalid arguments!\n");
    argc -= ret;
    argv += ret;

    nb_ports = rte_eth_dev_count();
    if (nb_ports == 0)
        rte_exit(EXIT_FAILURE, "No ethernet ports!\n");

    for (portid = 0; portid < nb_ports; portid++) {

        qconf = &lcore_queue_conf[rx_lcore_id];
        while (rte_lcore_is_enable(rx_lcore_id) || 
                qconf->n_rx_queue == nb_rx_queue_per_core) {

            rx_lcore_id++;
            if (rx_lcore_id >= RTE_MAX_LCORE)
                rte_exit(EXIT_FAILURE, "Not enough cores\n");
            qconf = &lcore_queue_conf[rx_lcore_id];
        }

        socket = (int) rte_lcore_to_socket_id(rx_lcore_id);
        if (socket == SOCKET_ID_ANY)
            socket = 0;

        rxq = &qconf->rx_queue_list[qconf->n_rx_queue];
        rxq->portid = portid;
        qconf->n_rx_queue++;
        
        ret = rte_eth_dev_start(portid);    
    }

    rte_eal_mp_remote_launch(ip_launch_one_lcore, NULL, CALL_MASTER);
    RTE_LCORE_FOREACH_SLAVE(lcore_id) {
        if (rte_eal_wait_lcore(lcore_id) < 0)
            return -1;
    }





    return 0;
}
