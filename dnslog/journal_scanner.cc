#include "journal_scanner.h"
#include <string>
#include <boost/filesystem.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

using fs = boost::filesystem;

void JournalScanner::SetPath(const string &path) {
  this->dir_path_ = path;
}

void JournalScanner::Scan(shared_ptr<vector<Journal>> jnllist_ptr, 
                       const string &name, int64_t timestamp) {
  fs::path path(this->dir_path_);
  fs::directory_iterator itor(path), end_itor;
  Journal jnl;
  vector<string> temp_vec;
  map<int64_t, vector<journal> > set;
  
  if (jnllist_ptr == nullptr)
    return;  
  jnllist_ptr->clear();
  
  for (itor; itor != end_itor; itor++) {
    temp_vec.clear();
    
    jnl.f_path = itor->path().string();
        
    boost::split(temp_vec, itor->path().filename().string(), 
                 boost::is_any_of("."), boost::token_compress_on);
    if (temp_vec.size() < 7) {
      std::cout << "skip invalid file \"" << jnl.f_path << "\"" << endl;
      continue; 
    }
    
    // process file timestamp  
    try {
      int y = boost::lexical_cast<int>(temp_vec[0].substr(0, 4));
      int mon = boost::lexical_cast<int>(temp_vec[0].substr(4, 2));
      int d = boost::lexical_cast<int>(temp_vec[0].substr(6, 2));
      int h = boost::lexical_cast<int>(temp_vec[0].substr(8, 2));
      int min = boost::lexical_cast<int>(temp_vec[0].substr(10, 2));
      static ptime time_t_begin(boost::gregorian::date(1970,1,1), hours(8));      
      ptime p(boost::gregorian::date(y, mon, d), hours(h) + minutes(min));
      if (p - ptime(timestamp) <= 0)
        continue;
      time_duration diff = p - time_t_begin;
      jnl.f_utc = diff.total_seconds();
    } catch(boost::bad_lexical_cast const& e) {
      std::cout << "Invalid file name" << jnl.f_path << 
                   "Error: " << e.what() << "\n";
      continue;
    }
    
    for (int i = 1; i < temp_vec.size() - 4; i++) {
      if (jnl.f_session == "") {
        jnl.f_session += temp_vec[i];
      } else {
        jnl.f_session += "." + temp_vec[i];
      }
    }
    
    if (jnl.f_session != name) {
      continue;
    }
    
    map<int64_t, vector<journal> >::iterator it = set.find(jnl.f_utc);
    if (it == set.end()) {
      vector<journal> tmp;
      tmp.push_back(jnl);
      set.emplace(jnl.f_utc, tmp);            
    } else {
      it->second.push_back(jnl);
    }
  }  // ends file traversing  
  
  map<int64_t, vector<journal> >::iterator it = set.begin();
  for (it; it != set.end(); it++) {
    jnllist_ptr->insert(jnllist_ptr->end(), 
                         it->second.begin(), it->second.end());
  }  
}
