#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>

using boost::lexical_cast;
using boost::char_separator;
using boost::tokenizer;
#include "logwatcher.h"
#include "logstream.h"

logstream::logstream(const string& name) : m_name(name)
{
}

struct dns_item* logstream::read()
{
    struct dns_item ret;
    vector<string> tmp;
    string line;

    ifstream in;
    in.open(this->m_curr.c_str());

    if (in.eof())
    {
        in.close();
        this->m_curr = this->m_files.front();
        this->m_files.erase(this->m_files.begin());
        in.open(this->m_curr.c_str());
    }
    
    getline(in, line);
    // timestamp
    // req/resps:sip|sport|dip|dport|dnsid|dnsname|dnsclass|dnstype|ECSaddr|rcode
    char_separator<char> sep("  :|");
    tokenizer<char_separator<char> > tok(line, sep);
    for(tokenizer<char_separator<char> >::iterator beg = tok.begin(); beg != tok.end(); ++beg)  
    {  
        tmp.push_back(*beg);  
    } 
    if (tmp.size() < 10) 
    {
        std::cout << "line %s may be trunct" << line << endl;  
        return NULL;
    }
    ret.timestamp = lexical_cast<unsigned long long>(tmp[0]);
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
        std::cout << "line %s format error" << line << endl;
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
                std::cout << "line %s may be trunct" << line << endl;  
                return NULL;
            }
            ret.dns_rcode = lexical_cast<int>(tmp[11]);
        }
    } 
    else 
    {
        if (tmp.size() != 10)
        {
            std::cout << "line %s may be error" << line << endl;  
            return NULL;
        }
        ret.dns_type = lexical_cast<int>(tmp[10]);
    }
    in.close();

    struct dns_item *p = (struct dns_item*) operator new(sizeof(struct dns_item));
    *p = ret;

    return p;
}

int logstream::bind_watcher(logwatcher& w)
{
    w.watch(this->m_name, &this->m_files);
    return 1;
}
