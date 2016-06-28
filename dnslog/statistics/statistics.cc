#include "statistics.h"
#include <map>
#include <memory>
#include <boost/thread/mutex.hpp>

using namespace std;

namespace statistics {

using StatisticsMap = map<const string, DomainStatisticsPtr>;
using PeriodStatMap = map<time_period, int64_t>;

StatisticsMap domain_statistics_map;
boost::mutex map_mutex;

DomainStatisticsPtr GetStatistics(const string &name) {
  DomainStatisticsPtr ptr = nullptr;
  boost::mutex::scoped_lock lock(map_mutex);
  StatisticsMap::iterator itor = domain_statistics_map.find(name);
  if (itor != domain_statistics_map.end()) {
    ptr = make_shared<DomainStatistics>();
  }
  return ptr;     
}  

void DeleteStatistice(const string &name) {
  DomainStatisticsPtr ptr = GetStatistics(name);
  boost::mutex::scoped_lock lock(map_mutex);
  if (ptr != nullptr) {
    domain_statistics_map.erase(name);
  }  
}

DomainStatisticsPtr NewStatistics(const string &name) {
  DomainStatisticsPtr ptr = nullptr;
  boost::mutex::scoped_lock lock(map_mutex);
  StatisticsMap::iterator itor = domain_statistics_map.find(name);
  if (itor == domain_statistics_map.end()) {
    DomainStatistics statistics;
    domain_statistics_map.insert(statistics);
    return statistics;
  }
  ptr = make_shared<DomainStatistics>(*itor);
  return ptr;
} 

DomainStatistics::DomainStatistics(const string &name)
    : domain_name_(name), 
      duration_(config::default_interval),
      counts_(0),
      isready_(false) {
}  

DomainStatistics::DomainStatistics(const string &name, time_duration duration)
    : domain_name_(name), 
      duration_(duration),
      counts_(0),
      isready_(false) {
}

void DomainStatistics::Add(ptime timestamp, int64_t count) {  
  time_period period = Calculate_Period(ptime(0), this->duration_, timestamp);  
  if (period.is_null()) {
    std::cout << "Invalid timestamp : " << timestamp << endl;
    return;
  }
  
  boost::mutex::scoped_lock(this->mutex_); 
  this->period_stat_.emplace(period, 0);
  auto itor = this->period_stat_.find(period); 
  itor->second += count;  
  this->counts_ += count;
}

int64_t DomainStatistics::Get_Period_Stats(ptime timestamp) {
  time_period period = Calculate_Period(ptime(0), this->duration_, timestamp);  
  if (period.is_null()) {
    std::cout << "Invalid timestamp : " << timestamp << endl;
    return;
  }
  
  boost::mutex::scoped_lock(this->mutex_);
  auto itor = this->period_stat_.find(period);
  if (itor == this->period_stat_.end())
    return -1;
  return itor->second;
}

void DomainStatistics::Delete_Period_Stats(ptime timestamp) {
  time_period period = Calculate_Period(ptime(0), this->duration_, timestamp);  
  if (period.is_null()) {
    std::cout << "Invalid timestamp : " << timestamp << endl;
    return;
  }
  
  boost::mutex::scoped_lock(this->mutex_);
  this->period_stat_.erase(period).;
}
    
}