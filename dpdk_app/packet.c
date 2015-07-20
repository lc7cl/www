
#include "packet.h"

struct ptype_list ptype_base;

int 
packet_type_add(struct packet_type *pt)
{
	LIST_INSERT_HEAD(&ptype_base, pt, list);
	return 0;
}
