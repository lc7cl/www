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
    config::default_journal_path = boost::filesystem::current_path().string();
  } else {
    config::default_journal_path = *dir_path;
  }

  boost::optional<string> interval = 
      pt.get_optional<string>("interval");
  if (interval == boost::none) {
    std::cout << "Use 10 minutes as default statistics interval" << endl;
  } else {
    int value;
    try {
      value = boost::lexical_cast<int>(interval.substr(0, interval.length - 1));
      if (interval.substr(interval.length - 1) == "M") {
        default_interval = boost::posix_time::minutes(value);
      } else if (interval.substr(interval.length - 1) == "H") {
        default_interval = boost::posix_time::hours(value);
      } else {
        std::cout << "Invalid params" << interval << std::endl;
      }
    } catch {
      std::cout << "Invalid params" << interval << std::endl;
    }
  }
}

};