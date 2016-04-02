#ifndef _LOGDNS_H_
#define _LOGDNS_H_

#include <string>

using namespace std;

class dns_item {

public:

    dns_item& operator=(const dns_item& C)
    {
	timestamp = C.timestamp;
	item_type = C.item_type;
	sip = C.sip;
	sport = C.sport;
	dip = C.dip;
	dport = C.dport;
	dns_id = C.dns_id;
	dns_name = C.dns_name;
	dns_class = C.dns_class;
	dns_type = C.dns_type;
	ecs_addr = C.ecs_addr;
	dns_rcode = C.dns_rcode;
    }


    string timestamp;
    int item_type;
    string sip;
    unsigned short sport;
    string dip;
    unsigned short dport;
    unsigned short dns_id;
    string dns_name;
    string dns_class;
    string dns_type;
    string ecs_addr;
    int dns_rcode;
};

#endif
