#include <string>
using namespace std;
#include <boost/thread.hpp>
#include <boost/bind.hpp>

#include "logstream.h"
#include "logwatcher.h"
#include "logdb.h"
#include "logtask.h"

#define LOG_DIR "/home/lee/code/www/misc"

static const string log_file_names[] = {
    "shpbs01-sec-sec113004.shpbs01.ksyun.com.task_0",
    "shpbs01-sec-sec113004.shpbs01.ksyun.com.task_1",
    "shpbs01-sec-sec113004.shpbs01.ksyun.com.task_2",
    "shpbs01-sec-sec113004.shpbs01.ksyun.com.task_3",
    "shpbs01-sec-sec113004.shpbs01.ksyun.com.task_4",
    ""
};

int main(int argc, char** argv)
{
    vector<string> sessions = vector<string>();
    int i = 0;
    while (true)
    {
        if (log_file_names[i] == "")
            break;
        sessions.push_back(log_file_names[i]);
        i++;
    }

    vector<string>& s = sessions;
    logdb *db = new logdb(500);

    if (db == NULL)
        return -1;

    logwatcher *watcher = logwatcher::getInstance();
    watcher->set_watchdir(LOG_DIR);

    logtask worker = logtask("dnslog1", s, db);

    boost::thread guarder_thrd(boost::bind(&logwatcher::start, watcher));
    boost::thread worker_thrd(boost::bind(&logtask::doAction, &worker));

    guarder_thrd.join();
    worker_thrd.join();

    return 0;
}
