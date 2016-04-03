#ifndef _DATA_H_
#define _DATA_H_

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "jansson.h"

using namespace std;

class data 
{
public:
    string metric;
    vector<pair<string, string> > tags;
    vector<pair<string, string> > aggregateTags;
    vector<pair<int64_t, int> > dps;
};

class dnssum
{
public:
    string dname;
    string geo;
    int64_t timestamp;
    int value;
};

#include <boost/asio.hpp>

using boost::asio::ip::tcp;

class db
{
public:
    db() : m_port(0), m_addr("") {};
    db(const string& addr, unsigned short port)
        : m_addr(addr), m_port(port) {};
    void start(int type);
    map< pair<string, string>, int> sum_by_name_geo(vector<data>& datas, int64_t tm);

    map<pair<string, string>, int> query(const string& metric, int64_t starttm, int64_t endtm, 
        vector<pair<string, string> > &tags);
    void insert(const string& metric, int64_t tm, map<pair<string, string>, int> &m);


private:
    string make_one_query_json(const string& metric, int64_t start_tm, int64_t end_tm, vector<pair<string, string> >& tags);
    string make_one_put_json(const string& metric, int64_t tm, int value, vector<pair<string, string> >& tags);
    vector<data> parse_response(json_t *json);
    int send(const string& uri, const string& data, const string& fname);

    boost::asio::io_service m_io_service;
    unsigned short m_port;
    string m_addr;

};

#endif
