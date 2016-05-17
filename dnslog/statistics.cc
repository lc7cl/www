#include "statistics.h"
#include <map>
#include <memory>

using namespace std;

namespace statistics {

using StatisticsMap = map<const string, DomainStatisticsPtr>;

StatisticsMap domain_statistics_map;

DomainStatisticsPtr GetStatistics(const string &name)
{
  DomainStatisticsPtr ptr = nullptr;
  StatisticsMap::iterator itor = domain_statistics_map.find(name);
  if (itor != domain_statistics_map.end()) {
    ptr = make_shared<DomainStatistics>();
  }
  return ptr;     
}  

void DeleteStatistice(const string &name)
{
  DomainStatisticsPtr ptr = GetStatistics(name);
  if (ptr != nullptr) {
    domain_statistics_map.erase(name);
  }  
}

DomainStatisticsPtr NewStatistics(const string &name)
{
  DomainStatisticsPtr ptr = nullptr;
  StatisticsMap::iterator itor = domain_statistics_map.find(name);
  if (itor == domain_statistics_map.end()) {
    DomainStatistics statistics;
    domain_statistics_map.insert(statistics);
    return statistics;
  }
  ptr = make_shared<DomainStatistics>(*itor);
  return ptr;
}     
    
}