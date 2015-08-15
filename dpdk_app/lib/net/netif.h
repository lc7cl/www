#ifndef _NETIF_H_
#define _NETIF_H_

#ifdef __cplusplus
extern "C" {
#endif

int netif_rx(struct rte_mbuf **mbuf, unsigned num);

#ifdef __cplusplus
}
#endif

#endif
