#include "logdir.h"

vector<string>& logdir::scan(bool recursive)
{
    vector<string> flist = new vector<string>();
    fs::directory_iterator end_itr;

    for (fs::directory_iterator itr(this->m_path); itr != end_itr; itr++)
    {
        time_t tm;
        tm = fs::last_write_time(itr);
        if (tm > this->m_timestamp)
        {
            flist.push_back(itr->native_file_string());
            this->m_timestamp = tm;
        }
    }

    return flist;
}
