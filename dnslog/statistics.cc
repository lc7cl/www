#include "statistics.h"
#include <map>

using namespace std;

namespace statistics {

using StatisticsMap map<const string, DomainStatistics>;

StatisticsMap domain_statistics_map;

DomainStatistics& GetStatistics(const string &name)
{
  map<const>
  return domain_statistics_map.find(name);    
}    
    
    
}