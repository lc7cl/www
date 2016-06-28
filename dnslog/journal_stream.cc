#include "journal_stream.h"
#include "journal_scanner.h"

void JournalStream::ParseLine(const string& line, const string& buf,                JournalRecord &record) {
  vector<string> vStr;
	using tokenizer = boost::tokenizer<boost::char_separator<char> >;
	boost::char_separator<char> sep("  :|");

	tokenizer tokens(line, sep);
	for(tokenizer::iterator tok_iter = tokens.begin(); tok_iter != tokens.end(); 
      ++tok_iter) {  
    vStr.push_back(*tok_iter);  
  } 

  if (vStr.size() < 10) {
    return 1;
  }

  record.timestamp = this->jnl_.f_utc;
  if (vStr[1] == "req") {
    record.item_type = 1;
  } else if (vStr[1] == "resp") {
    record.item_type = 0;
  } else {
    return -1;
  }
  record.sip = vStr[2];
  try {
    record.sport = lexical_cast<unsigned short>(vStr[3]);
	  record.dport = lexical_cast<unsigned short>(vStr[5]);
    record.dns_id = lexical_cast<unsigned short>(vStr[6]);
  } catch {
    return -1;
  }
  
  record.dip = vStr[4];
  record.dns_name = vStr[7];
  record.dns_class = vStr[8];
  record.dns_type = vStr[9];
  if (vStr[10].length() >= 3) {
    if (vStr[10].substr(0, 3) == "ECS") {
      record.ecs_addr = vStr[10].substr(3);
      if (vStr.size() != 12) {  
        return 1;
      }
      record.dns_rcode = lexical_cast<int>(vStr[11]);
    }
  } else {
    if (vStr.size() != 11) {
      return -1;
    }
    record.dns_rcode = lexical_cast<int>(vStr[10]);
  }
}

int JournalStream::GetJournal(Journal &jnl) {
  if (this->jnl_list_.empty()) {
    FileScanner::GetInstance().Scan(this->jnl_list_, this->stream_name_);
    if (this->jnl_list_.empty())
      return -1;
  }
  jnl = this->jnl_list_.front();
  this->jnl_list_.pop();
  return 0;
}

int JournalStream::Read(JournalRecord &record) {
  int ret;
  Journal jnl;
  while (!this->in_.is_open()) {
    ret = GetJournal(jnl);
    if (ret < 0) {
      return STREAM_STATE_NONE_JOURNAL;
    }    
    this->in_.open(jnl.f_path);
    if (!this->in_.good())
      continue;
    
    this->is_first_line_ = true;
    this->jnl = jnl;
  }
  
  if (!this->in_.is_open()) {
    return STREAM_STATE_ERROR;
  }
  
  string line;
  vector<string> vStr;  
  getline(this->in_, line);  
  ParseLine(line, record);
  
  if (this->is_first_line_)
    this->is_first_line_ = false; 

  if (this->in_.eof())
    this->in_.close();
  return STREAM_STATE_OK;
}