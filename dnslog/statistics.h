#ifndef STATISTICS_H_
#define STATISTICS_H_

#include <stdint.h>

namespace statistics {
  
using DomainStatisticsPtr = std::shared_ptr<DomainStatistics>;
    
class DomainStatistics {
 public:
  DomainStatistics() = delete;
  DomainStatistics(const string &name);
  DomainStatistics(const string &name, time_duration duration);
  ~DomainStatistics() {};
  
  inline int64_t get_count() { return counts_; } const; 
  void ResetCycle(ptime timestamp);
  void ResetCycle();
  void Add(ptime timestamp, int64_t count);
  int64_t Get_Period_Stats(ptime timestamp);
  void Delete_Period_Stats(ptime timestamp);
 
 private:
  boost::mutex mutex_;
  string domain_name_;
  time_duration duration_;
  int64_t counts_;
  bool isready_;
  map<time_period, int64_t> period_stat_;
}; 

DomainStatisticsPtr GetStatistics(const string &name);
void DeleteStatistice(const string &name);
DomainStatisticsPtr NewStatistics(const string &name);

time_period Calculate_Period(ptime start, time_duration duration, ptime timestamp) {
  time_duration diff = timestamp - start;
  return time_period(start, diff / duration);
};
   
}

#endif  // STATISTICS_H_