#ifndef CONFIG_H_
#define CONFIG_H_

namespace config {

boost::posix_time::time_duration default_interval = 
    boost::posix_time::minutes(10);
string default_journal_path;

int ParseConfig(const string &cfg_file);

}

#endif //CONFIG_H_