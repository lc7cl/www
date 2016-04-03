#ifndef _LOG_STREAM_H_
#define _LOG_STREAM_H_

#include <iostream>
#include <fstream>
#include "logdns.h"
#include "logwatcher.h"

using namespace std;

enum {
    STREAM_STATE_0 = 0,
};

class logstream {

public:
    logstream(const string& name);
    ~logstream() {};
    int bind_watcher(logwatcher&);
    dns_item* read();
    boost::lockfree::queue<string*> *m_files;
    const string& get_name() {return m_name;};

private:
    int m_state;
    string *m_curr;
    boost::uint64_t m_curr_utc;
    string m_name;
    string m_frag;
    ifstream *m_in;
};

#endif
