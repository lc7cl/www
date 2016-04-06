#ifndef _LOGWATCHER_H_
#define _LOGWATCHER_H_

#include <iostream>
#include <map>

#include <boost/lockfree/queue.hpp>

#include "logdir.h"

using namespace std;

class logwatcher {

public:
    int watch(const string& name, boost::lockfree::queue<logfile*> *filelist);
    int unwatch(const string& name);
    void start();
    static logwatcher* getInstance();
    void destroyInstance();
    void set_watchdir(const string&);

private:
    map<string, boost::lockfree::queue<logfile*>* > m_objs;
    logdir m_dir;
    static logwatcher * m_instance;

protected:
    logwatcher() {};

};

#endif
