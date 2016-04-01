#ifndef _LOGWATCHER_H_
#define _LOGWATCHER_H_

#include <iostream>
#include <map>

#include "logdir.h"

using namespace std;

class logwatcher {

public:
    int watch(const string& name, vector<string> *filelist);
    int unwatch(const string& name);
    void start();
    static logwatcher* getInstance();
    void destroyInstance();
    void set_watchdir(const string&);

private:
    map<string, vector<string>* > m_objs;
    logdir m_dir;
    static logwatcher * m_instance;

protected:
    logwatcher() {};

};

#endif
