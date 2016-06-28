#ifndef RECORD_HANDLER_H_
#define RECORD_HANDLER_H_

namespace handler {

using RecordHandle = int Handle(JournalRecord &record);

class RecordHandler {
 public:
  RecordHandler(const string &d, RecordHandle h) :
    domain_(d), handle_(h) {};
  int Handle(JournalRecord &record);

 private:
  RecordHandle handle_;
  string domain_;
};

int HandleRecord(JournalRecord &record);
int AddHandler(const string &domain, RecordHandle h);

};

#endif