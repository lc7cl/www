#ifndef JOURNAL_H_
#define JOURNAL_H_

class JournalRecord {
 public:
  JournalRecord& operator=(const JournalRecord& J) {
    timestamp = J.timestamp;
    item_type = J.item_type;
    sip = J.sip;
    sport = J.sport;
    dip = J.dip;
    dport = J.dport;
    dns_id = J.dns_id;
    dns_name = J.dns_name;
    dns_class = J.dns_class;
    dns_type = J.dns_type;
    ecs_addr = J.ecs_addr;
    dns_rcode = J.dns_rcode;
  }

  int64_t timestamp;
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

struct Journal
{
    int64_t f_utc;
    string f_session;
    string f_path;   
};

#endif