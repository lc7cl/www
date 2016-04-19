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

#include <boost/filesystem.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
namespace fs = boost::filesystem;
#include "logstream.h"

logstream::logstream(logfile& f) 
    : m_curr_file(f)
{
    this->m_in.open(f.f_path.c_str());
    this->m_curr_file = f;
}

logstream::~logstream() 
{
    if (this->m_in.is_open())
        this->m_in.close();
}

void logstream::parse_line(const string& line, vector<string>& vStr)
{
    typedef boost::tokenizer<boost::char_separator<char> >  tokenizer;
    boost::char_separator<char> sep("  :|");
    tokenizer tokens(line, sep);
    vector<string>().swap(vStr);
    //std::copy(tokens.begin(), tokens.end(), std::back_inserter(tmp));
    for(tokenizer::iterator tok_iter = tokens.begin(); tok_iter != tokens.end(); ++tok_iter)  
    {  
        vStr.push_back(*tok_iter);  
    } 
    
    if (vStr.size() < 10) 
    {
        std::cout << "(" << this->m_curr_file.f_path << ")" 
            << "line may be trunct " << line << " (size" << vStr.size() << ")" << endl;          
    }    
}

enum stream_state logstream::read(dns_item& item)
{
    enum stream_state ret = STREAM_STATE_ERROR;
    string line;
    
    if (!m_in.is_open())
        return STREAM_STATE_FILEERROR;
    if (m_in.eof())
        return STREAM_STATE_EOF;
    
    getline(m_in, line);
    if (!m_in.good())
    {
        cout << "read file " << m_curr_file.f_path << " error" << endl;
        return STREAM_STATE_ERROR;        
    }
    
    // timestamp
    // req/resp:sip|sport|dip|dport|dnsid|dnsname|dnsclass|dnstype|ECSaddr|rcode
    vector<string> vStr;
    
    parse_line(line, vStr);
    if (vStr.size() == 0)
    {
        return STREAM_STATE_ERROR;
    }
    
    item.timestamp = this->m_curr_file.f_utc;
    if (vStr[1] == "req") 
    {
        item.item_type = 1;
    } 
    else if (vStr[1] == "resp")
    {
        item.item_type = 0;
    } 
    else 
    {
        std::cout << "(" << this->m_curr_file.f_path << ")" << "line %s format error" << line << endl;
        return STREAM_STATE_ERROR;
    }
    item.sip = vStr[2];
    item.sport = lexical_cast<unsigned short>(vStr[3]);
    item.dip = vStr[4];
    item.dport = lexical_cast<unsigned short>(vStr[5]);
    item.dns_id = lexical_cast<unsigned short>(vStr[6]);
    item.dns_name = vStr[7];
    item.dns_class = vStr[8];
    item.dns_type = vStr[9];
    if (vStr[10].length() >= 3) 
    {
        if (vStr[10].substr(0, 3) == "ECS")
        {
            item.ecs_addr = vStr[10].substr(3);
            if (vStr.size() != 12)
            {
                std::cout << "(" << this->m_curr_file.f_path << ")" << "line may be trunct:" << line << endl;  
                return STREAM_STATE_ERROR;
            }
            item.dns_rcode = lexical_cast<int>(vStr[11]);
        }
    } 
    else 
    {
        if (vStr.size() != 11)
        {
            std::cout << "(" << this->m_curr_file.f_path << ")" << "line may be error:" << line << endl;  
            return STREAM_STATE_ERROR;
        }
        item.dns_rcode = lexical_cast<int>(vStr[10]);
    }
    
    return STREAM_STATE_OK;
}
