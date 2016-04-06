#include <boost/lexical_cast.hpp>
#include "data.h"

string db::make_one_query_json(const string& metric, int64_t start_tm, int64_t end_tm, 
        vector<pair<string, string> >& tags)
{
    json_t *query;
    json_t *tags_json;
    vector<pair<string, string> >::iterator it;
    string geo, dname;
    for (it = tags.begin(); it != tags.end(); it++)
    {
        if (it->first == "geo")
            geo = it->second;
        else if (it->first == "dname")
            dname = it->second;
    }
    tags_json = json_pack("{s:s, s:s}",
            "dname", dname.c_str(),
            "geo", geo.c_str());
    if (tags_json == NULL)
        return "";
 
    query = json_pack("{s:s, s:s, s:O}",
            "aggregator", "sum", 
            "metric", metric.c_str(),
            "tags", tags_json);
    if (query == NULL)
        return string("");
    json_t *queries;
    queries = json_pack("[O]", query);
    if (queries == NULL)
        return string("");
    json_t *j;
    j = json_pack("{s:i, s:i, s:O}", 
		"start", start_tm,
		"end", end_tm,
		"queries", queries);
    if (j == NULL)
        return string("");
    return string(json_dumps(j, 0));
}

string db::make_one_put_json(const string& metric, int64_t tm, int value, vector<pair<string, string> >& tags)
{
    json_t *put;
    json_t *tags_json;
    vector<pair<string, string> >::iterator it;
    string geo, dname;
    for (it = tags.begin(); it != tags.end(); it++)
    {
        if (it->first == "geo")
            geo = it->second;
        else if (it->first == "dname")
            dname = it->second;
    }
    tags_json = json_pack("{s:s, s:s}",
            "dname", dname.c_str(),
            "geo", geo.c_str());
    if (tags_json == NULL)
        return "";
    put = json_pack("{s:s, s:i, s:i, s:O}",
            "metric", metric.c_str(),
            "timestamp", tm,
            "value", value,
            "tags", tags_json);
    if (put == NULL)
        return "";
    return string(json_dumps(put, 0));
}

vector<data> db::parse_response(json_t *json)
{
    vector<data> datas;
    int i;
    for (i = 0; i < json_array_size(json); i++)
    {
        data d;
        const char *tag_key, *dps_key;
        int64_t tm;
        int val;
        json_t *j, *tags_json, *tag_val, *dps_json, *dps_val;
        j = json_array_get(json, i);
        d.metric = string(json_string_value(json_object_get(j, "metric")));
        tags_json = json_object_get(j, "tags");
        json_object_foreach (tags_json, tag_key, tag_val)
        {
            d.tags.push_back(make_pair(string(tag_key), string(json_string_value(tag_val))));
        }

        dps_json = json_object_get(j, "dps");
        json_object_foreach(dps_json, dps_key, dps_val)
        {
            d.dps.push_back(make_pair(boost::lexical_cast<int64_t>(string(dps_key)), json_integer_value(dps_val)));
        }
        datas.push_back(d);
    }
    return datas;
}

map< pair<string, string>, int> db::sum_by_name_geo(vector<data>& datas, int64_t tm)
{
    vector<dnssum> sums;
    map< pair<string, string>, int> m;
    map< pair<string, string>, int>::iterator mit;

    vector<data>::iterator vit = datas.begin();
    for (vit; vit != datas.end(); vit++)
    {
        string dname, geo;
        vector<pair<string, string> >::iterator tagit;
        for (tagit = vit->tags.begin();
                tagit != vit->tags.end();
                tagit++)
        {
            if (tagit->first == "dname")
                dname = tagit->second;
            else if (tagit->first == "geo")
                geo = tagit->second;
        }
        if (dname == "" || geo == "")
        {
            cout << "KKKKKKK" << endl;
            continue;
        }
        pair<string, string> mkey = make_pair(dname, geo);
        mit = m.find(mkey);
        if (mit == m.end())
        {
            m.insert(make_pair(mkey, 0));
            mit = m.find(mkey);
        }
        int count = 0;
        vector<pair<int64_t, int> >::iterator dpsit;
        for (dpsit = vit->dps.begin();
                dpsit != vit->dps.end();
                dpsit++)
        {
            count += dpsit->second;
        }
        mit->second += count;
    }
    return m;
}

map<pair<string, string>, int> db::query(const string& metric, int64_t starttm, int64_t endtm, 
        vector<pair<string, string> > &tags)
{
    map<pair<string, string>, int> m;
    string d;
    json_error_t error;
    d = make_one_query_json(metric, starttm, endtm, tags);
    int ret = send("/api/query", d, "result_json");
    if (ret != 200)
        return m;
    json_t *json = json_load_file("result_json", 0, &error);
    if (json == NULL)
        return m;
    vector<data> datas = parse_response(json);
    if (datas.size() == 0)
        return m;
    return sum_by_name_geo(datas, starttm);
}

void db::insert(const string& metric, int64_t tm,
        map<pair<string, string>, int> &m)
{
    string d;
    map<pair<string, string>, int>::iterator it;
    for (it = m.begin(); it != m.end(); it++)
    {
        vector<pair<string, string> > v;
        v.push_back(make_pair(string("dname"), it->first.first));
        v.push_back(make_pair(string("geo"), it->first.second));
        string s = make_one_put_json(metric, tm, it->second, v);
        d += s;
    }
    int ret = send("/api/put", d, "");
    if (ret != 204)
    {
        cout << "insert error" << endl;
    }
}

int db::send(const string& uri, const string& data, const string& fname)
{
    boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::address::from_string(m_addr), m_port);
    tcp::socket socket(m_io_service);
    socket.connect(endpoint);
    boost::asio::streambuf request;
    std::ostream request_stream(&request);
    request_stream << "POST " << uri << " HTTP/1.0\r\n";
    request_stream << "Host: " << "\r\n";
    request_stream << "Accept:*/*\r\n";
    request_stream << "Content-Length: " << data.length() << "\r\n";
    request_stream << "Connection: close\r\n\r\n";
    request_stream << data;

    boost::asio::write(socket, request);

    boost::asio::streambuf response;
    boost::asio::read_until(socket, response, "\r\n");
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

    if (fname != "") 
    {
        ofstream out;

	    out.open(fname.c_str(), ios::out);
         
        boost::asio::read_until(socket, response, "\r\n\r\n");
        std::string header;
        while (std::getline(response_stream, header) && header != "\r");
        boost::system::error_code error;
        while (boost::asio::read(socket, response,
                    boost::asio::transfer_at_least(1), error))
            out << &response;
    }
    return status_code;
}
