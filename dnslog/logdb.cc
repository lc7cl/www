#include <dlfcn.h>
#include <boost/lexical_cast.hpp>
#include <boost/property_tree/ptree.hpp>
using boost::property_tree::ptree;
#include <boost/property_tree/json_parser.hpp>
#include <boost/format.hpp>     
#include <boost/tokenizer.hpp>     
#include <boost/algorithm/string.hpp> 

#include "jansson.h"
#include "logdb.h"

string logdb::make_one_json(string& name, struct statistics& statics)
{
    json_t *tags;
    tags = json_pack("{s:s}", "country", "China");
    if (tags == NULL)
	return string("");
    json_t *j;
    j = json_pack("{s:s, s:i, s:i, s:O}", 
		"metric", name.c_str(),
		"timestamp", boost::lexical_cast<int>(statics.lasttime),
		"value", statics.count,
		"tags", tags);
    if (j == NULL)
	return string("");
    return string(json_dumps(j, 0));
}

string logdb::make_jsons(vector<pair<string, struct statistics> >& pool)
{
    string json = string("");
    vector<pair<string, struct statistics> >::iterator it = pool.begin();

    if (pool.size() == 1)
        return make_one_json(pool[0].first, pool[0].second);
    for (it; it != pool.end(); it++)
    {
	if (json == "") 
	{
	    json += make_one_json(it->first, it->second);
	}
	else
	{
	    json += ", " + make_one_json(it->first, it->second);
	}
    }
    return "[" + json + "]";
}


logdb* logdb::m_instance = NULL;

logdb* logdb::getInstance()
{
    if (m_instance == NULL)
    {
        m_instance = new logdb();
    }
    return m_instance;
}

void logdb::destroyInstance()
{
    if (m_instance)
        delete m_instance;
}

void logdb::set_flush_threshold(int threshold)
{
    this->m_threshold = threshold;
}

void logdb::set_db_server(const string& server)
{
    this->m_server = server;
    vector<string> v;
    boost::split(v, server, boost::is_any_of(":"));
    if (v.size() == 2)
    {
	this->m_addr = v[0];
	this->m_port = boost::lexical_cast<unsigned short>(v[1]);
    }
}

void logdb::set_db_uri(const string& uri)
{
    this->m_uri = uri;
}

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

int logdb::flush(pair<string, struct statistics> statics)
{
    if (this->m_threshold > 0 && this->m_pool.size() == this->m_threshold)
    {
        string str = make_jsons(this->m_pool);
        insert_db(str);
        vector<pair<string, struct statistics> >().swap(this->m_pool);
    }
    else
    {
        this->m_pool.push_back(statics);
    }
    return 1;
}

int logdb::put(struct dns_item* item)
{
    time_t tm;
    time(&tm);
    string& dname = item->dns_name;
    map<string, struct statistics>::iterator itor = this->m_statics.find(dname);
    if (itor == this->m_statics.end())
    {
        struct statistics *new_statics = (struct statistics*)operator new(sizeof(struct statistics));
        if (new_statics == NULL)
            return 1;
        new_statics->lasttime = tm;
        new_statics->count = 0;
        new_statics->sip = "";
        this->m_statics.insert(make_pair(dname, *new_statics));
        return 0;
    }
    if (tm - itor->second.lasttime> 10) 
    {
        flush(*itor);
        itor->second.lasttime = tm;
        itor->second.count = 0;
    }
    itor->second.sip = item->sip;
    itor->second.count++;
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


