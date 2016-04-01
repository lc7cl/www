#ifndef _LOGTASK_H_
#define _LOGTASK_H_

#include "logstream.h"
#include "logdb.h"

class logtask {

public:
    logtask(const string&, const vector<string>&, logdb *db);
    ~logtask() {};
    void doAction();

private:
    vector<string> m_session_names;
    const string m_task_name;
    logdb *m_db;
    vector<logstream> m_streams;

};

#endif
