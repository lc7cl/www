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

int logwatcher::watch(const string& name, boost::lockfree::queue<string*> *filelist)
{
    if (this->m_objs.count(name) == 0)
        return -1;

    this->m_objs.insert(pair<string, boost::lockfree::queue<string*>*  >(name, filelist));
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
    vector<string>* list;
    while (1) 
    {
        list = this->m_dir.scan(false);
        if (list) {
            vector<string>::iterator itor = list->begin();
            vector<string>::iterator end = list->end();
            for (itor; itor != end; itor++)
            {
                vector<string> vStr;
                boost::split(vStr, *itor, boost::is_any_of("."), boost::token_compress_on);
                if (vStr.size() != 6) 
                {
                    std::cout << "format of filename %s error" << *itor << endl;
                    continue;
                }
                map<string, boost::lockfree::queue<string*>* >::iterator it = this->m_objs.find(vStr[1] + vStr[2]);
                if (it == this->m_objs.end())
                {
                    std::cout << "cannot find %s" << vStr[1] + vStr[2] << endl;
                    continue;            
                }
                if (it->second == NULL) 
                {
                    std::cout << "cannot find %s" << vStr[1] + vStr[2] << endl;
                    continue;            
                }
                string *fname = new string(*itor);
                it->second->push(fname);
            }
            delete(list);
        }

        sleep(6);
    }
}
