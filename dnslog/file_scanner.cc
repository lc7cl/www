#include "file_scanner.h"
#include <string>
#include <boost/filesystem.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

using fs = boost::filesystem;

void FileScanner::SetPath(const string &path) {
  this->dir_path_ = path;
}

void FileScanner::Scan(shared_ptr<vector<logfile>> filelist_ptr, 
                       const string &name, int64_t timestamp) {
  fs::path path(this->dir_path_);
  fs::directory_iterator itor(path), end_itor;
  logfile f;
  vector<string> temp_vec;
  map<int64_t, vector<logfile> > set;
  
  if (filelist_ptr == nullptr)
    return;  
  filelist_ptr->clear();
  
  for (itor; itor != end_itor; itor++) {
    temp_vec.clear();
    
    f.f_path = itor->path().string();
        
    boost::split(temp_vec, itor->path().filename().string(), 
                 boost::is_any_of("."), boost::token_compress_on);
    if (temp_vec.size() < 7) {
      std::cout << "skip invalid file \"" << f.f_path << "\"" << endl;
      continue; 
    }
    
    // process file timestamp  
    try {
      int y = boost::lexical_cast<int>(temp_vec[0].substr(0, 4));
      int m = boost::lexical_cast<int>(temp_vec[0].substr(4, 2));
      int d = boost::lexical_cast<int>(temp_vec[0].substr(6, 2));
      int h = boost::lexical_cast<int>(temp_vec[0].substr(8, 2));
      static ptime time_t_begin(boost::gregorian::date(1970,1,1), hours(8));      
      ptime p(boost::gregorian::date(y, m, d), hours(h));
      if (p - ptime(timestamp) <= 0)
        continue;
      time_duration diff = p - time_t_begin;
      f.f_utc = diff.total_seconds();
    } catch(boost::bad_lexical_cast const& e) {
      std::cout << "Invalid file name" << f.f_path << 
                   "Error: " << e.what() << "\n";
      continue;
    }
    
    for (int i = 1; i < temp_vec.size() - 4; i++) {
      if (f.f_session == "") {
        f.f_session += temp_vec[i];
      } else {
        f.f_session += "." + temp_vec[i];
      }
    }
    
    if (f.f_session != name) {
      continue;
    }
    
    map<int64_t, vector<logfile> >::iterator it = set.find(f.f_utc);
    if (it == set.end()) {
      vector<logfile> tmp;
      tmp.push_back(f);
      set.emplace(f.f_utc, tmp);            
    } else {
      it->second.push_back(f);
    }
  }  // ends file traversing  
  
  map<int64_t, vector<logfile> >::iterator it = set.begin();
  for (it; it != set.end(); it++) {
    filelist_ptr->insert(filelist_ptr->end(), 
                         it->second.begin(), it->second.end());
  }  
}
