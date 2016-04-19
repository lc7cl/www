#ifndef _LOG_STREAM_H_
#define _LOG_STREAM_H_

#include <iostream>
#include <fstream>
#include "logdns.h"
#include "logfile.h"

using namespace std;

enum stream_state 
{
    STREAM_STATE_OK = 0,
    STREAM_STATE_ERROR,
    STREAM_STATE_EOF,
    STREAM_STATE_NOINPUT,
    STREAM_STATE_FILEERROR,
};

class logstream {
public:
    logstream(logfile&);
    ~logstream();
    enum stream_state read(dns_item& item);

private:
    void parse_line(const string& line, vector<string>& vStr);
    
    ifstream m_in;
    logfile m_curr_file;
};

#endif
