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
    map<int64_t, vector<logfile> > set;
    logfile f;    
    fs::directory_iterator itor(this->m_path), end_itor;
    for (itor; itor != end_itor; itor++)
    {
        f.f_path = itor->path().string();
        vector<string> vStr;
        boost::split(vStr, itor->path().filename().string(), boost::is_any_of("."), boost::token_compress_on);
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
        
        map<int64_t, vector<logfile> >::iterator it = set.find(f.f_utc);
        if (it == set.end())
        {
            vector<logfile> tmp;
            tmp.push_back(f);
            set.insert(make_pair(f.f_utc, tmp));            
        }
        else
        {
            it->second.push_back(f);
        }
    }

    vector<logfile>().swap(flist);
    flist.clear();
    map<int64_t, vector<logfile> >::iterator it = set.begin();
    for (it; it != set.end(); it++)
    {    
        if (it->first > this->m_timestamp)
        {   
            flist.insert(flist.end(), it->second.begin(), it->second.end());    
            this->m_timestamp = it->first;
        }
    }
}

logdir::logdir(const string& path) :m_path(path), m_timestamp(0)
{
}
