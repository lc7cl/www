
#include "logdb.h"
#include "logtask.h"

logtask::logtask(const string& task_name, const vector<string>& snames, logdb *db) 
    : m_task_name(task_name), m_session_names(snames), m_db(db)
{
    logstream *stream;
    vector<string>::iterator it = this->m_session_names.begin();
    vector<string>::iterator end = this->m_session_names.end();
    for (it; it != end; it++) 
    {
        stream = new logstream(*it);
        if (stream) 
        {
            this->m_streams.push_back(*stream);
            delete stream;
        }
    }
}

void logtask::doAction()
{
    struct dns_item *item;
    vector<logstream>::iterator s_itor = this->m_streams.begin();
    vector<logstream>::iterator s_end = this->m_streams.end();

    for (s_itor; s_itor != s_end; s_itor++) 
    {
        item = s_itor->read();
        while (item != NULL) {
            //insert db
            this->m_db->put(item);
        }
    }
}
