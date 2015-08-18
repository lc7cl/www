#include "buffer.h"

void
stat_mbuf(struct rte_mbuf *mbuf, uint8_t in, uint8_t drop)
{
	struct net_device *ndev;

	ndev = net_device_get(mbuf->port);
	if ((ndev->flag & NET_DEV_F_DISABLE) == NET_DEV_F_DISABLE)
		return;

	if (in) {
		if (drop) {
			ndev->stat.rx.drop[rte_lcore_id()]++;
		} else {
			ndev->stat.rx.recv[rte_lcore_id()]++;
		}
	} else {
		if (drop) {
			ndev->stat.tx.drop[rte_lcore_id()]++;
		} else {
			ndev->stat.tx.xmit[rte_lcore_id()]++;
		}
	}	
}
