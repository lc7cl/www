#ifndef _LOGDIR_H_
#define _LOGDIR_H_

#include <string>
#include <boost/filesystem.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
namespace fs = boost::filesystem;

using namespace std;

#include "logfile.h"

class logdir {

public:
    logdir() {};
    logdir(const string& path);
    ~logdir() {};
    void scan(vector<logfile>& flist, bool recursive);

private:
    fs::path m_path;
    time_t m_timestamp;
};

#endif
