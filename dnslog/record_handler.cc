#include "record_handler.h"

namespace handler {

using namespace std;

vector<RecordHandler> handlers;

int AddHandler(const string &domain, RecordHandle h) {
  for (auto itor = handlers.begin(); itor != handlers.end(); itor++) {
    if (itor->Handle == h)
      return -1;
  }

  RecordHandle rh(domain, h);
  handlers.push_back(rh);
  return 0;
}

int HandleRecord(JournalRecord &record) {
  int ret = 0;
  for (auto itor = handlers.begin(); itor != handlers.end(); itor++) {
    ret = itor->Handle(record);
    if (ret < 0)
      break;
  }
  return ret;
}

int RecordHandler::Handle(JournalRecord &record) {
  return this->handle_(record);
}

};