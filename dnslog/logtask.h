#ifndef _LOGTASK_H_
#define _LOGTASK_H_

#include "logstream.h"
#include "logdb.h"

class logtask {

public:
    ~logtask() {};
    void doAction();
    logtask(const string& path): m_path(path) { };

private:
    string m_path;
};

#endif
