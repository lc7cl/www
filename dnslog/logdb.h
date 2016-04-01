#ifndef _LOGDB_H_
#define _LOGDB_H_

#include <vector>

#include "logdns.h"

class logdb {

public:
    int put(struct dns_item* item);
    static logdb* getInstanc();
    void destroyInstance();
    int set_server(const string&, int);
    void set_threshold(int threshold);

private:
    int m_threshold;
    string m_server_addr;
    int m_server_port;
    vector<dns_item> m_pool;

    int flush();

protected:
    logdb() {};
};

#endif

