#include "port_queue_map.h"

struct lcore_queue_conf per_lcore_q_conf[RTE_MAX_LCORE];
struct port_queue_conf per_port_q_conf[RTE_MAX_ETHPORTS];
