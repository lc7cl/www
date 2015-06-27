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

#define MAX_PKT_BURST 32

static void ip_main_loop(void)
{
    struct lcore_queue_conf *qconf;
    unsigned lcore_id, portid, i, nb_rx;
    struct rte_mbuf *pkts_burst[MAX_PKT_BURST];

    lcore_id = rte_lcore_id();

    qconf = &lcore_queue_conf[lcore_id];
    if (qconf->n_rx_port == 0)
        RTE_LOG(INFO, IP, "lcore %u has nothing to do\n", lcore_id);

    while (1) {
    
        for (i = 0; i < qconf->n_rx_port; i++) {
            portid = qconf->rx_port_list[i];
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
    int nb_ports;
    struct rte_eth_dev_info dev_info;
    uint8_t portid;
    unsigned lcore_id;

    ret = rte_eal_init(argc, argv);
    if (ret < 0)
        rte_exit(EXIT_FAILURE, "Invalid arguments!\n");
    argc -= ret;
    argv += ret;

    nb_ports = rte_eth_dev_count();
    if (nb_ports == 0)
        rte_exit(EXIT_FAILURE, "No ethernet ports!\n");

    for (portid = 0; portid < nb_ports; portid++) {
        ret = rte_eth_dev_start(portid);    
    }

    rte_eal_mp_remote_launch(ip_launch_one_lcore, NULL, CALL_MASTER);
    RTE_LCORE_FOREACH_SLAVE(lcore_id) {
        if (rte_eal_wait_lcore(lcore_id) < 0)
            return -1;
    }





    return 0;
}
