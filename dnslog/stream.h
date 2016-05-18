#ifndef STREAM_H_
#define STREAM_H_

#include <queue>

class Stream {
 public:
  Stream(const string &name);
  void Read(vector<dns_item> &list);
  
 private:
  string stream_name;
  vector<logfile> filelist;
  string buf;
  std::queue<dns_item> items;
};

shared_ptr<Stream> GetStream(const string &name);
void DeleteStream(const string &name);
shared_ptr<Stream> CreateStream(const string &name);

#endif