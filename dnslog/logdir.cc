#include <map>

#include "logdir.h"

bool filetm_cmp(const fs::path& p1, const fs::path &p2)
{
    return fs::last_write_time(p1) < fs::last_write_time(p2);
}

vector<string>* logdir::scan(bool recursive)
{
    vector<string>* flist = new vector<string>();
    map<time_t, fs::path> set;
    fs::directory_iterator itor(this->m_path), end_itor;
    for (itor; itor != end_itor; itor++)
    {
        set.insert(make_pair(fs::last_write_time(*itor), *itor));
    }
    map<time_t, fs::path>::iterator it = set.begin(), end = set.end();

    for (it; it != end; it++)
    {
        if (it->first > this->m_timestamp)
        {
            flist->push_back(it->second.string());
            this->m_timestamp = it->first;
        }
    }

    return flist;
}

logdir::logdir(const string& path) :m_path(path), m_timestamp(0)
{
}
