#ifndef LOGSTREAM_H_
#define LOGSTREAM_H_

#include <queue>

enum {
  STREAM_STATE_EOF,
};

class LogStream {
 public:
  LogStream(const string &name);
  int Read(dns_item &item);
  
 private:
  void ParseLine(const string& line, vector<string>& vStr);
  int GetLines(int count, vector<string> &res);
  
  string stream_name_;
  std::queue<logfile> filelist_;
  string buf_;
  ifstream in_;
  bool is_first_line_;
  string file_;
  std::queue<string> cache_;
};

shared_ptr<LogStream> GetStream(const string &name);
void DeleteStream(const string &name);
shared_ptr<LogStream> CreateStream(const string &name);

#endif
