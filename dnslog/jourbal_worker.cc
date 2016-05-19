#include "jourbal_worker.h"
#include <memory>
#include "statistics.h"

using namespace std;

JournalWorker::Work() {
  dns_item item;
  DomainStatisticsPtr stat_ptr;
  for (auto itor = streamlist_.begin(); itor != streamlist_.end(); itor++) {
    while (itor->Read(item) != LogStream::STREAM_STATE_EOF) {
      //
      stat_ptr = GetStatistics(item.dns_name);
      if (stat_ptr == nullptr)
        stat_ptr = CreateStatistics(item.dns_name);
      stat_ptr->Add(ptime(item.timestamp), 1);
    }     
  }
}

void JournalWorker::BindStream(vector<string> namelist) {
  this->streamlist_.clear();
  for (auto itor = namelist.begin(); itor != namelist.end(); itor++) {
    this->streamlist_.push_back(make_shared(new LogStream(*itor)));
  }
}