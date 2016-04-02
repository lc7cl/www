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

struct statistics {
    time_t lasttime;
    string sip;
    int count;
};

typedef char* (*ACL_FUNC)(unsigned int);

class logdb 
{
public:
    static logdb* getInstance();
    int put(struct dns_item* item);
    void destroyInstance();
    void set_db_server(const string&);
    void set_db_uri(const string&);
    void set_flush_threshold(int threshold);
    int connect();
    int set_acllib(const string&);
    int load_acl(const string&);

private:
    int flush(pair<string, struct statistics>);
    string make_one_json(string&, struct statistics &);
    string make_jsons(vector<pair<string, struct statistics> >&);
    int insert_db(const string&);
    string get_line(const string&);

    static logdb* m_instance;
    void* m_handle;
    ACL_FUNC m_func;
    int m_threshold;
    string m_server;
    string m_addr;
    unsigned short m_port;
    string m_uri;
    int m_server_port;
    map<string, struct statistics> m_statics;
    vector<pair<string, struct statistics> > m_pool;
    //tcp::resolver resolver_;
    //tcp::socket socket_;
    //boost::asio::streambuf request_;
    //boost::asio::streambuf response_;
    boost::asio::io_service io_service;


protected:
    logdb() {};
};

#endif

