#include <iostream>
#include <map>
#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/split.hpp>                                      
#include <boost/algorithm/string.hpp> 
#include <boost/tokenizer.hpp>
#include <boost/date_time.hpp>

using boost::lexical_cast;
using boost::char_separator;
using boost::tokenizer;
using namespace boost::posix_time;

#include "logdir.h"

bool filetm_cmp(const fs::path& p1, const fs::path &p2)
{
    return fs::last_write_time(p1) < fs::last_write_time(p2);
}

void logdir::scan(vector<logfile>& flist, bool recursive)
{
    map<time_t, fs::path> set;
    fs::directory_iterator itor(this->m_path), end_itor;
    for (itor; itor != end_itor; itor++)
    {
        cout << "^^^" << fs::last_write_time(*itor) <<endl;
        set.insert(make_pair(fs::last_write_time(*itor), *itor));
    }
    map<time_t, fs::path>::iterator it = set.begin(), end = set.end();

    vector<logfile>().swap(flist);
    flist.clear();
    logfile f;
    for (it; it != end; it++)
    {
        f.f_path = "";
        f.f_utc = 0;
        f.f_session = "";
        
        cout << "******" << it->first << "   " << it->second.string() << endl;
        
        if (it->first > this->m_timestamp)
        {
            f.f_path = it->second.string();         
            
            vector<string> vStr;
            boost::split(vStr, fs::path(f.f_path).filename().string(), boost::is_any_of("."), boost::token_compress_on);
            if (vStr.size() < 7) 
            {
                std::cout << "skip invalid file \"" << f.f_path << "\"" << endl;
                continue; 
            }
                
            int y = boost::lexical_cast<int>(vStr[0].substr(0, 4));
            int m = boost::lexical_cast<int>(vStr[0].substr(4, 2));
            int d = boost::lexical_cast<int>(vStr[0].substr(6, 2));
            int h = boost::lexical_cast<int>(vStr[0].substr(8, 2));
            ptime p(boost::gregorian::date(y, m, d), hours(h));
            static ptime time_t_begin(boost::gregorian::date(1970,1,1), hours(8)); 
            time_duration diff = p - time_t_begin;
            f.f_utc = diff.total_seconds();
            
            string name;
            for (int i = 1; i < vStr.size() - 4; i++) 
            {
                if (f.f_session == "")
                {
                    f.f_session += vStr[i];
                }
                else
                {
                    f.f_session += "." + vStr[i];
                }
            }
            
            flist.push_back(f);
            this->m_timestamp = it->first;
        }
    }
}

logdir::logdir(const string& path) :m_path(path), m_timestamp(0)
{
}
