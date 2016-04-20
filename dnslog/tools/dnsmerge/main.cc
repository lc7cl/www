#include "data.h"

#include <boost/lexical_cast.hpp>
using boost::lexical_cast;
using namespace boost::gregorian;  
using namespace boost::posix_time;

int get_duration(const string& type, int y, int m)
{
    if (type == "day")
    {
	    return 3600 * 24;
    }
    else if (type == "week")
    {
	    return 3600 * 24 * 7;
    }
    else if (type == "month")
    {
        switch (m)
        {
        case 1:
        case 3:
        case 5:
        case 9:
        case 11:
            return 3600 * 24 * 30;
        case 4:
        case 6:
        case 7:
        case 8:
        case 10:
        case 12:
            return 3600 * 24 * 31;
        case 2:
            if (y % 4 == 0)
            {
                return 3600 * 24 * 29;
            }
            else
            {
                return 3600 * 24 * 28;
            }
            break;
        }
    }
    return 0;
}

int main(int argc, char** argv)
{
    if (argc < 3)
        return 1;
    string type = string(argv[1]);
    if (type != "day" && type != "week" && type != "month")
    {
        return -1;
    }
    string tm = string(argv[2]);
    int y, m, d, h;
    if (tm.size() < 10)
    {
	    return -1;
    }
    y = lexical_cast<int>(tm.substr(0, 4));
    m = lexical_cast<int>(tm.substr(4, 2));
    d = lexical_cast<int>(tm.substr(6, 2));
    h = lexical_cast<int>(tm.substr(8, 2));
    ptime tbegin(boost::gregorian::date(1970,1,1), hours(8));
    ptime t(boost::gregorian::date(y,m,d), hours(h));
    time_duration diff = t - tbegin; 
    int64_t starttm = diff.total_seconds();
    int64_t endtm = starttm;
    db mydb("10.16.49.12", 4242);
    string query_metric, put_metric, query_metric_all, put_metric_all;
    if (type == "day")
    {
        query_metric = "dns_hour";
        put_metric = "dns_day";
        query_metric_all = "dns_hour_all";
        put_metric_all = "dns_day_all";
    }
    else if (type == "week")
    {
        query_metric = "dns_day";
        put_metric = "dns_week";
        query_metric_all = "dns_day_all";
        put_metric_all = "dns_week_all";
    }
    else if (type == "month")
    {
        query_metric = "dns_day";
        put_metric = "dns_month";
        query_metric_all = "dns_day_all";
        put_metric_all = "dns_month_all";
    }
    endtm += get_duration(type, y, m);
    vector<pair<string, string> > q_v;
    if (argc >= 4)
        q_v.push_back(make_pair("dname", argv[3]));
    else
        q_v.push_back(make_pair("dname", "*"));

    if (argc >= 5)
        q_v.push_back(make_pair("geo", argv[4]));
    else
        q_v.push_back(make_pair("geo", "*"));
    map<pair<string, string>, int> maap;
    maap = mydb.query(query_metric, starttm, endtm, q_v);
    map<pair<string, string>, int> maap_all;
    maap_all = mydb.query(query_metric_all, starttm, endtm, q_v);
    map<pair<string, string>, int>::iterator it;

    for (it = maap.begin(); it != maap.end(); it++)
    {
        cout<<"("<<it->first.first<<" , "<<it->first.second<<")="<<it->second<<endl;
    }
    
    if (maap.size() > 0)
    {
        mydb.insert(put_metric, starttm, maap);
    }
    
    for (it = maap_all.begin(); it != maap_all.end(); it++)
    {
        cout<<"("<<it->first.first<<" , "<<it->first.second<<")="<<it->second<<endl;
    }
    
    if (maap_all.size() > 0)
    {
        mydb.insert(put_metric_all, starttm, maap_all);
    }
        
    return 0;
}
