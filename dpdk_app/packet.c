
#include "packet.h"

LIST_HEAD(ptype_list, packet_type) ptype_base;

int 
packet_type_add(struct packet_type *pt)
{
	LIST_INSERT_HEAD(&ptype_base, pt, list);
	return 0;
}
