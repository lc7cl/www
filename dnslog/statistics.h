#ifndef STATISTICS_H_
#define STATISTICS_H_

#include <stdint.h>

namespace statistics {
  
using DomainStatisticsPtr = std::shared_ptr<DomainStatistics>;
    
class DomainStatistics {
 public:
  DomainStatistics(const string &name) : domain_name_(name) { counts_ = 0; };
  ~DomainStatistics() {};
  
  boost::uint64_t get_count() { return counts_; } const;  
 
 private:
  string domain_name_;
  int64_t counts_;
}; 

DomainStatisticsPtr GetStatistics(const string &name);
void DeleteStatistice(const string &name);
DomainStatisticsPtr NewStatistics(const string &name);
   
}

#endif  // STATISTICS_H_