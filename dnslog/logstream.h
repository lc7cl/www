#ifndef _LOG_STREAM_H_
#define _LOG_STREAM_H_

#include "logdns.h"
#include "logwatcher.h"

enum {
    STREAM_STATE_0 = 0,
};

class logstream {

public:
    logstream(const string& name);
    ~logstream() {};
    int bind_watcher(logwatcher&);
    struct dns_item* read();

private:
    vector<string> m_files;
    int m_state;
    string m_curr;
    string m_name;
};

#endif
