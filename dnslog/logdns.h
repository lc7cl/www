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

class statis_key 
{
public:
    statis_key(): dns_name_(""), geo_("") {};
    statis_key(const string& name, const string& geo) : dns_name_(name), geo_(geo) {};
    string dns_name_;
    string geo_;
};
    inline bool operator < (const statis_key& k1, const statis_key& k2)
    { 
	if (k1.dns_name_ < k2.dns_name_)
            return true;
	else
	    return false;
    }

class statistics {
public:
    statistics() : lasttime(0), count(0) {};
    time_t lasttime;
    int count;
};


#endif
