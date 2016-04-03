#include <string>
#include <vector>
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

#include "logwatcher.h"
#include "logstream.h"

logstream::logstream(const string& name) 
    : m_name(name)
{
    this->m_files = new boost::lockfree::queue<string*>(256);
    this->m_in = new ifstream();
}

dns_item* logstream::read()
{
    struct dns_item ret;
    string line;

    if (m_in == NULL || m_in->is_open() == false || m_in->eof())
    {
	if (m_in && m_in->is_open())
	{
            m_in->close();
            if (this->m_curr)
                delete this->m_curr;
	}
        this->m_curr = NULL;
        if (this->m_files->pop(this->m_curr) == 0)
        {
            return NULL;
        }
	vector<string> vStr;
	boost::split(vStr, *this->m_curr, boost::is_any_of("."), boost::token_compress_on);
        if (vStr.size() < 2)
	    return NULL; 
        int y = lexical_cast<int>(vStr[0].substr(0, 4));
        int m = lexical_cast<int>(vStr[0].substr(4, 2));
        int d = lexical_cast<int>(vStr[0].substr(6, 2));
        int h = lexical_cast<int>(vStr[0].substr(8, 2));
	ptime p(boost::gregorian::date(y, m, d),hours(h));
	static ptime time_t_begin(boost::gregorian::date(1970,1,1)); 
	time_duration diff = p - time_t_begin;
        m_curr_utc = diff.seconds();
        m_in->open(this->m_curr->c_str());
    }
    
    if (!getline(*m_in, line))
	return NULL;
    // timestamp
    // req/resps:sip|sport|dip|dport|dnsid|dnsname|dnsclass|dnstype|ECSaddr|rcode
    vector<string> tmp;
    typedef boost::tokenizer<boost::char_separator<char> >  tokenizer;
    boost::char_separator<char> sep("  :|");
    tokenizer tokens(line, sep);
    cout << m_name << " : " << line << endl;
    tmp.clear();
    //std::copy(tokens.begin(), tokens.end(), std::back_inserter(tmp));
    for(tokenizer::iterator tok_iter = tokens.begin(); tok_iter != tokens.end(); ++tok_iter)  
    {  
        tmp.push_back(*tok_iter);  
    } 
    if (tmp.size() < 10) 
    {
        std::cout << "(" << this->m_name << ")" << "line may be trunct " << line << " (size" << tmp.size() << ")" << endl;  
        return NULL;
    }
    ret.timestamp = m_curr_utc;
    if (tmp[1] == "req") 
    {
        ret.item_type = 1;
    } 
    else if (tmp[1] == "resp")
    {
        ret.item_type = 0;
    } 
    else 
    {
        std::cout << "(" << this->m_name << ")" << "line %s format error" << line << endl;
        return NULL;
    }
    ret.sip = tmp[2];
    ret.sport = lexical_cast<unsigned short>(tmp[3]);
    ret.dip = tmp[4];
    ret.dport = lexical_cast<unsigned short>(tmp[5]);
    ret.dns_id = lexical_cast<unsigned short>(tmp[6]);
    ret.dns_name = tmp[7];
    ret.dns_class = tmp[8];
    ret.dns_type = tmp[9];
    if (tmp[10].length() >= 3) 
    {
        if (tmp[10].substr(0, 3) == "ECS")
        {
            ret.ecs_addr = tmp[10].substr(3);
            if (tmp.size() != 11)
            {
                std::cout << "( << this->m_name << )" << "line may be trunct:" << line << endl;  
                return NULL;
            }
            ret.dns_rcode = lexical_cast<int>(tmp[11]);
        }
    } 
    else 
    {
        if (tmp.size() != 11)
        {
            std::cout << "( << this->m_name << )" << "line may be error:" << line << endl;  
            return NULL;
        }
        ret.dns_rcode = lexical_cast<int>(tmp[10]);
    }

    dns_item *p = new dns_item(ret);

    return p;
}

int logstream::bind_watcher(logwatcher& w)
{
    w.watch(this->m_name, this->m_files);
    return 1;
}
