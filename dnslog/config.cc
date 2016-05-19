#include "config.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

namespace config {

using boost::property_tree::ini_parser;

int ParseConfig(const string &cfg_file) {
  Ptree pt;  
  try {
    read_init(cfg_file, pt);
  } catch(file_parser_error &e) {
    std::cout << "config parse error %s %d : %s" 
              << e.filename() << e.line() << e.messsage() << endl;
    return -1; 
  }
  
  boost::optional<string> dir_path = 
      pt.get_optional<string>("journal_directory");
  if (dir_path == boost::none) {
    FileScanner::SetPath(boost::filesystem::current_path().string());
  } else {
    FilScanner::SetPath(*dir_path);
  }
      
  
  
  
  
}

};