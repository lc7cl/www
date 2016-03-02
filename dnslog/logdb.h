#ifndef _LOGDB_H_
#define _LOGDB_H_

#include <vector>
#include <iostream>
#include <istream>
#include <ostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/bind.hpp>

using boost::asio::ip::tcp;

#include "logdns.h"

class logdb 
{
public:
    static logdb* getInstance();
    int put(struct dns_item* item);
    void destroyInstance();
    void set_db_server(const string&);
    void set_flush_threshold(int threshold);
    int connect();

private:
    int flush();

    static logdb* m_instance;
    int m_threshold;
    string m_server_addr;
    int m_server_port;
    vector<dns_item> m_pool;
    tcp::resolver resolver_;
    tcp::socket socket_;
    boost::asio::streambuf request_;
    boost::asio::streambuf response_;

protected:
    logdb() {};
};

#endif

