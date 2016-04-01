#ifndef _LOGDNS_H_
#define _LOGDNS_H_

#include <string>

using namespace std;

struct dns_item {
    unsigned long long timestamp;
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
