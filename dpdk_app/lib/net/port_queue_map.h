#ifndef _PORT_QUEUE_MAP_H_
#define _PORT_QUEUE_MAP_H_

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_RX_QUEUE_PER_CORE 	16
#define MAX_RX_QUEUE_PER_PORT 	8
#define MAX_TX_QUEUE_PER_CORE 	16
#define MAX_TX_QUEUE_PER_PORT 	8

struct txrx_queue {
	int qid;
	unsigned lcore;
	unsigned port;
};

struct lcore_queue_conf {
	struct txrx_queue rxq[MAX_RX_QUEUE_PER_CORE];
	int nb_rxq;
	struct txrx_queue txq[MAX_TX_QUEUE_PER_CORE];
	int nb_txq;
	int next_txq;
};

struct port_queue_conf {
	struct txrx_queue rxq[MAX_RX_QUEUE_PER_PORT];
	int nb_rxq;	
	struct txrx_queue txq[MAX_TX_QUEUE_PER_PORT];
	int nb_txq;
};

extern struct lcore_queue_conf per_lcore_q_conf[];
extern struct port_queue_conf per_port_q_conf[];

static inline struct lcore_queue_conf* lcore_q_conf_get(int lcore)
{
	return &per_lcore_q_conf[lcore];
}

static inline struct port_queue_conf* port_q_conf_get(int port)
{
	return &per_port_q_conf[port];
}

#ifdef __cplusplus
}
#endif

#endif
