#ifndef TSDB_H_
#define TSDB_H_

#include <vector>
#include <iostream>
#include <istream>
#include <ostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/bind.hpp>

using boost::asio::ip::tcp;

#include "logdns.h"

typedef char* (*ACL_FUNC)(unsigned int);

namespace tsdb {

class TsdbData {
 public:
  TsdbData(const boost::posix_time::time_duration timestamp, 
          const int value, 
          const map<string, string> tags) :
            timestamp_(timestamp), value_(value), tags_(tags) {};
  int get_value() { return value_; };
  boost::posix_time::time_duration get_timestamp() { return timestamp_; };
  map<string, string> get_tags() { return tags_; };

 private:
  const boost::posix_time::time_duration timestamp_;
  const int value_;
  const map<string, string> tags_;
};

class TsdbResult {
 public:
  TsdbResult(const boost::posix_time::time_duration timestamp, 
          const int value, 
          const map<string, string> tags) :
            timestamp_(timestamp), value_(value), tags_(tags) {};
  int get_value() { return value_; };
  boost::posix_time::time_duration get_timestamp() { return timestamp_; };
  map<string, string> get_tags() { return tags_; };

 private:
  const boost::posix_time::time_duration timestamp_;
  const int value_;
  const map<string, string> tags_;
};

int SetParameters(const string &addr, 
                  const string &port,
                  const string &put_uri,
                  const string &get_uri);
int Put(const string &metric, const TsdbData &data);
int Put(const string &metric, const vector<TsdbData> &datas);

int Get(const string &metric,
        const boost::posix_time::time_duration start,
        const boost::posix_time::time_duration end,
        const vector<TsdbData> &datas, 
        TsdbResult &ret);
        
};

public:
    static logdb* getInstance();
    int put(dns_item& item);
    void destroyInstance();
    void set_db_server(const string&);
    void set_db_uri(const string&);
    void set_flush_threshold(int threshold);
    int connect();
    int set_acllib(const string&);
    int load_acl(const string&);
    void flush();
    void flush_all();

private:
    int save(pair<statis_key, statistics>);
    string make_one_json(string&,const statis_key&, statistics &);
    string make_jsons(string&, vector<pair<statis_key, statistics> >&);
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
    map<statis_key, statistics> m_statics;
    int64_t m_utc;
    map<string, statistics> m_all_statics;
    int64_t m_all_utc;
    vector<pair<statis_key, struct statistics> > m_pool;
    //tcp::resolver resolver_;
    //tcp::socket socket_;
    //boost::asio::streambuf request_;
    //boost::asio::streambuf response_;
    boost::asio::io_service io_service;


protected:
    logdb() {};
};

#endif

