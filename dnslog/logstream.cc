#include "logstream.h"

#include "file_scanner.h"

void LogStream::ParseLine(const string& line, vector<string>& vStr)
{
    using tokenizer = boost::tokenizer<boost::char_separator<char> >;
    boost::char_separator<char> sep("  :|");
    tokenizer tokens(line, sep);
    vector<string>().swap(vStr);
    //std::copy(tokens.begin(), tokens.end(), std::back_inserter(tmp));
    for(tokenizer::iterator tok_iter = tokens.begin(); tok_iter != tokens.end(); ++tok_iter)  
    {  
        vStr.push_back(*tok_iter);  
    } 
}

int LogStream::Read(dns_item &item) {
  logfile file;
  while (!this->in_.is_open() || this->in_.eof()) {
    file = this->filelist.front();
    this->in_.open(file);
    this->filelist.pop();
    if (this->filelist.empty()) {
      FileScanner::GetInstance().Scan(this->filelist, this->stream_name_);
      if (this->filelist_.empty())
        return STREAM_STATE_EOF;
    }
        
    if (this->in_.good()) {
      this->is_first_line_ = true;
      this->file_ = file;
    }      
  }
  
  if (this->in_.is_open() || this->in_.eof())
    return STREAM_STATE_EOF;
  
  string line;
  vector<string> vStr;  
  getline(this->in_, line);  
  ParseLine(line, vStr);
  
  if (vStr.size() < 10) {
    if (this->in_.eof()) {
      this->buf_ = line;
    } else if (this->is_first_line_) {
      line = line + this->buf_;  
      vStr.clear();
      ParseLine(line, vStr); 
    }
  }
  
  item.timestamp = this->file_.f_utc;
  if (vStr[1] == "req") {
    item.item_type = 1;
  } else if (vStr[1] == "resp") {
    item.item_type = 0;
  } else {
    std::cout << "(" << this->m_curr_file.f_path << ")" 
              << "line %s format error" << line << endl;
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
  if (vStr[10].length() >= 3) {
    if (vStr[10].substr(0, 3) == "ECS") {
      item.ecs_addr = vStr[10].substr(3);
      if (vStr.size() != 12) {
        std::cout << "(" << this->m_curr_file.f_path << ")" 
                  << "line may be trunct:" << line << endl;  
        return STREAM_STATE_ERROR;
      }
      item.dns_rcode = lexical_cast<int>(vStr[11]);
    }
  } else {
    if (vStr.size() != 11) {
      std::cout << "(" << this->m_curr_file.f_path << ")" 
                << "line may be error:" << line << endl;  
      return STREAM_STATE_ERROR;
    }
    item.dns_rcode = lexical_cast<int>(vStr[10]);
  }
  
  if (this->is_first_line_)
    this->is_first_line_ = false;  
}