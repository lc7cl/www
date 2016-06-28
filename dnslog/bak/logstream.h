#ifndef _LOG_STREAM_H_
#define _LOG_STREAM_H_

#include <iostream>
#include <fstream>
#include "logdns.h"
#include "logwatcher.h"
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
    logstream(const string& name);
    ~logstream() {};
    int bind_watcher(logwatcher&);
    enum stream_state read(dns_item& item);
    boost::lockfree::queue<logfile*> *m_files;
    const string& get_name() { return m_name; };
    string* get_curr_file() { return m_curr_file != NULL ? &(m_curr_file->f_path) : NULL; };

private:
    void parse_line(const string& line, vector<string>& vStr);
    
    int m_state;
    string m_name;
    string m_frag;
    ifstream *m_in;
    logfile *m_curr_file;
};

#endif
