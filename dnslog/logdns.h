#ifndef _LOGDNS_H_
#define _LOGDNS_H_

#include <string>
#include <stdint.h>

using namespace std;

class statis_key {
 public:
  statis_key(): dns_name_(""), geo_("") {};
  statis_key(const string& name, const string& geo) 
    : dns_name_(name), geo_(geo) {};
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
  statistics() : utc(0), count(0) {};
  int64_t utc;
  int count;
};

#endif
