#include "logdb.h"

logdb* logdb::m_instance = NULL;

logdb* logdb::getInstance()
{
    if (m_instance == NULL)
    {
        m_instance = new logdb();
    }
    return m_instance;
}

void logdb:destroyInstance()
{
    if (m_instance)
        delete m_instance;
}

void logdb::set_flush_threshold(int threshold)
{
    this->m_threshold = threshold;
}

void logdb::set_db_server(const string& addr, int port)
{
    this->m_server_addr = addr;
    this->m_server_port = port;
}

int logdb::connection()
{
    return 1;
}

int logdb::flush()
{
    return 1;
}

int logdb::put(struct dns_item* item)
{
    if (this->m_threshold > 0 && this->m_pool.size() == this->m_threshold)
    {
        flush();
    }
    this->m_pool.push_back(*item);
    return 1;
}
