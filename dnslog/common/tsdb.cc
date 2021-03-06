#include "tsdb.h"

#include "service.h"
#include <dlfcn.h>
#include <boost/lexical_cast.hpp>
#include <boost/property_tree/ptree.hpp>
using boost::property_tree::ptree;
#include <boost/property_tree/json_parser.hpp>
#include <boost/format.hpp>     
#include <boost/tokenizer.hpp>     
#include <boost/algorithm/string.hpp> 

#include "jansson.h"

namespace tsdb {

Service tsdb_service;
string default_addr = "";
string default_port = "4242";
string default_put_uri = "";
string default_get_uri = "";

string make_one_json(const string &metric, const TsdbData &data) {
    json_t *tags;
    tags = json_pack("{s:s , s:s}",  
                        "geo", name.geo_.c_str(),
                        "dname", name.dns_name_.c_str());
    if (tags == NULL)
	    return string("");
    json_t *j;
    j = json_pack("{s:s, s:i, s:i, s:O}", 
		"metric", metric.c_str(),
		"timestamp", statics.utc,
		"value", data.get_value(),
		"tags", tags);
    if (j == NULL)
	    return string("");
    return string(json_dumps(j, 0));
}

int SetParameters(const string &addr, const string &port,
                const string &put_uri, const string &get_uri) {
  default_addr = addr;
  default_port = port;
  default_put_uri = put_uri;
  default_get_uri = get_uri;
  return 0;
}

int Put(const string &metric, const TsdbData &data) {
  int ret;
  string json = string("");

  json = make_one_json(metric, data);

  ret = tsdb_service.Connect(default_addr, default_port);
  if (ret < 0)
    return -1;
  ret = tsdb_service.HTTPSend(default_put_uri, json);
  if (ret < 0)
    return -1;
  return 0;
}

int Put(const string &metric, const vector<TsdbData> &datas) {
  int ret;
  string json = string("");

  if (datas.size() == 1) {
    json = make_one_json(metric, datas[0]);
  } else {
    for (auto itor; itor != datas.end(); itor++) {
        if (json == "") {
            json += make_one_json(metric, *itor);
        } else {
            json += ", " + make_one_json(metric, *itor);
        }
    }
    json = "[" + json + "]";
  }

  ret = tsdb_service.Connect(default_addr, default_port);
  if (ret < 0)
    return -1;
  ret = tsdb_service.HTTPSend(default_put_uri, json);
  if (ret < 0)
    return -1;
  return 0;
}

};



int logdb::set_acllib(const string& path)
{
    m_handle = dlopen(path.c_str(), RTLD_LAZY);
    if (!m_handle) 
    {
        std::string error("Cannot open library: ");
        throw std::runtime_error(error + dlerror());
        return 1;
    }
    dlerror();
    return 0;
}

int logdb::load_acl(const string& path)
{
    int ret;
    typedef int (*Func)(const char* file);
    Func func = (Func) dlsym(m_handle, "build_acl");
    const char *dlsym_error = dlerror();
    if (dlsym_error) 
    {
        dlclose(m_handle);
        m_handle = NULL;
        std::string error("Cannot load symbol: build_acl");
        throw std::runtime_error(error + dlsym_error);
        return 1;
    }
    ret = func(path.c_str());
    if (ret != 0)
        return 1;
    m_func = (ACL_FUNC) dlsym(m_handle, "get_acl");
    dlsym_error = dlerror();
    if (dlsym_error)
    {
        dlclose(m_handle);
        m_handle = NULL;
        std::string error("Cannot load symbol: get_acl");
        throw std::runtime_error(error + dlsym_error);
        return 1;
    }

    return ret == 0 ? 0 : 1;
}

string logdb::get_line(const string& ip)
{
    string str(m_func(inet_addr(ip.c_str())));
    return str;
}



string logdb::make_jsons(string& metric, vector<pair<statis_key, statistics> >& pool)
{
    string json = string("");
    vector<pair<statis_key, statistics> >::iterator it = pool.begin();

    if (pool.size() == 1)
        return make_one_json(metric, pool[0].first, pool[0].second);
    for (it; it != pool.end(); it++)
    {
        if (json == "") 
        {
            json += make_one_json(metric, it->first, it->second);
        }
        else
        {
            json += ", " + make_one_json(metric, it->first, it->second);
        }
    }
    return "[" + json + "]";
}

int logdb::save(pair<statis_key, statistics> statics)
{
    int ret = 0;
    this->m_pool.push_back(statics);
    if (this->m_threshold > 0 && this->m_pool.size() >= this->m_threshold)
    {
        string metric = string("dns_hour");
        string str = make_jsons(metric, this->m_pool);
        insert_db(str);
        vector<pair<statis_key, statistics> >().swap(this->m_pool);
        ret = 1;
    }
    return ret;
}

void logdb::flush()
{
    string json = "";
    
    map<statis_key, statistics>::iterator it = m_statics.begin();
    if (m_statics.size() == 0)
        return;
    for (it; it != m_statics.end(); it++)
    {
        string metric = string("dns_hour");
        if (json == "") 
        {
            json += make_one_json(metric, it->first, it->second);
        }
        else
        {
            json += ", " + make_one_json(metric, it->first, it->second);
        }
    }    
    insert_db("[" + json + "]");
    m_statics.clear();
}

void logdb::flush_all()
{
    string json = "";
    
    map<string, statistics>::iterator it = m_all_statics.begin();
    if (m_all_statics.size() == 0)
        return;
    for (it; it != m_all_statics.end(); it++)
    {
        string metric = string("dns_hour_all");
        if (json == "") 
        {
            json += make_one_json(metric, statis_key(it->first, string("all")), it->second);
        }
        else
        {
            json += ", " + make_one_json(metric, statis_key(it->first, string("all")), it->second);
        }
    }    
    insert_db("[" + json + "]");    
    m_all_statics.clear();
}

int logdb::put(struct dns_item& item)
{
    time_t tm;
    time(&tm);
    string& dname = item.dns_name;
    string geo;
    
    if (item.timestamp > m_utc)
    {
        //flush();        
    }
    if (item.timestamp > m_all_utc)
    {
        //flush_all();        
    }

    if (item.ecs_addr == "")
        geo = get_line(item.sip);
    else
	    geo = get_line(item.ecs_addr);
    if (geo == "")
	    geo = "default";
        
    statistics *new_statics = NULL;
    map<statis_key, statistics>::iterator itor1 = this->m_statics.find(statis_key(dname, geo));
    if (itor1 == this->m_statics.end())
    {
        new_statics = new statistics();
        if (new_statics == NULL)
            return 1;
        new_statics->utc = item.timestamp;
        new_statics->count = 0;
        this->m_statics.insert(make_pair(statis_key(dname, geo), *new_statics));
	    itor1 = this->m_statics.find(statis_key(dname, geo));
        delete new_statics;
    }
    itor1->second.count++;
    
    map<string, statistics>::iterator itor2 = this->m_all_statics.find(dname);
    if (itor2 == this->m_all_statics.end())
    {
        new_statics = new statistics();
        if (new_statics == NULL)
            return 1;
        new_statics->utc = item.timestamp;
        new_statics->count = 0;
        this->m_all_statics.insert(make_pair(dname, *new_statics));
	    itor2 = this->m_all_statics.find(dname);
        delete new_statics;
    }
    itor2->second.count++;
    
    if (item.timestamp > m_utc)
    {
        //flush();        
    }
    if (item.timestamp > m_all_utc)
    {
        //flush_all();        
    }
    m_utc = item.timestamp;
    m_all_utc = item.timestamp;
    return 0;
}

int logdb::insert_db(const string& data)
{
    // Get a list of endpoints corresponding to the server name.
    boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::address::from_string(m_addr), m_port);
    
    // Try each endpoint until we successfully establish a connection.
    tcp::socket socket(io_service);
    socket.connect(endpoint);
    //boost::asio::connect(socket, endpoint);
    
    // Form the request. We specify the "Connection: close" header so that the
    // server will close the socket after transmitting the response. This will
    // allow us to treat all data up until the EOF as the content.
    boost::asio::streambuf request;
    std::ostream request_stream(&request);
    request_stream << "POST " << this->m_uri << " HTTP/1.0\r\n";
    request_stream << "Host: " << "\r\n";
    request_stream << "Accept:*/*\r\n";
    request_stream << "Content-Length: " << data.length() << "\r\n";
    request_stream << "Connection: close\r\n\r\n";
    request_stream << data;
    
    // Send the request.
    boost::asio::write(socket, request);
    // Read response status line. 
    // The response streambuf will automatically grow to accommodate the entire line.
    // The growth may be limited by passing a maximum size to the streambuf constructor.
    boost::asio::streambuf response;
    boost::asio::read_until(socket, response, "\r\n");

    // Check that response is OK.
    std::istream response_stream(&response);
    std::string http_version;
    response_stream >> http_version;
    unsigned int status_code;
    response_stream >> status_code;
    std::string status_message;
    std::getline(response_stream, status_message);
    if (!response_stream || http_version.substr(0, 5) != "HTTP/")
    {
        std::cout << "Invalid response\n";
        return 1;
    }
    if (status_code != 204)
    {
        std::cout << "Response returned with status code " << status_code << "\n";
        return 1;
    }
    return 0;
}


