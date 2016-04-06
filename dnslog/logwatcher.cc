#include <boost/algorithm/string/split.hpp>                                      
#include <boost/algorithm/string.hpp> 
#include <boost/tokenizer.hpp>

#include "logwatcher.h"

logwatcher* logwatcher::m_instance = NULL;

logwatcher* logwatcher::getInstance()
{
    if (m_instance == NULL)
    {
        m_instance = new logwatcher();
    }
    return m_instance;
}

void logwatcher::destroyInstance()
{
    if (this->m_instance)
        delete this->m_instance;
}

void logwatcher::set_watchdir(const string& dir)
{
    this->m_dir = logdir(dir);
}

int logwatcher::watch(const string& name, boost::lockfree::queue<logfile*> *filelist)
{
    if (this->m_objs.count(name) != 0)
        return -1;

    this->m_objs.insert(pair<string, boost::lockfree::queue<logfile*>*  >(name, filelist));
    return 0;
}

int logwatcher::unwatch(const string& name) 
{
    if (this->m_objs.erase(name) == 1)
        return 0;
    else
        return -1;
}

void logwatcher::start()
{
    vector<logfile> flist;
    while (1) 
    {
        this->m_dir.scan(flist, false);
        if (flist.size() > 0) {
            vector<logfile>::iterator itor = flist.begin();
            vector<logfile>::iterator end = flist.end();
            for (itor; itor != end; itor++)
            {
                map<string, boost::lockfree::queue<logfile*>* >::iterator it = this->m_objs.find(itor->f_session);
                if (it == this->m_objs.end())
                {
                    std::cout << "cannot find :" << itor->f_session << "(" << itor->f_path << ")" << endl;
                    continue;            
                }
                if (it->second == NULL) 
                {
                    std::cout << "cannot find value :" << itor->f_session << "(" << itor->f_path << ")" << endl;
                    continue;            
                }
                logfile *log = new logfile(*itor);
                it->second->push(log);
            }
        }
        sleep(6);
    }
}
