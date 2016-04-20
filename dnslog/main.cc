#include <string>
using namespace std;
#include <boost/thread.hpp>
#include <boost/bind.hpp>

#include "logstream.h"
#include "logdb.h"
#include "logtask.h"

#define LOG_DIR "/data/dns/misc"

static const string log_file_names[] = {
    "shpbs01-sec-sec113004.shpbs01.ksyun.com.task_0",
    "shpbs01-sec-sec113004.shpbs01.ksyun.com.task_1",
    "shpbs01-sec-sec113004.shpbs01.ksyun.com.task_2",
    "shpbs01-sec-sec113004.shpbs01.ksyun.com.task_3",
    "shpbs01-sec-sec113004.shpbs01.ksyun.com.task_4",
    "bjzjm01-sec-sec029008.bjzjm01.ksyun.com.task_0",
    "bjzjm01-sec-sec029008.bjzjm01.ksyun.com.task_1",
    "bjzjm01-sec-sec029008.bjzjm01.ksyun.com.task_2",
    "bjzjm01-sec-sec029008.bjzjm01.ksyun.com.task_3",
    "bjzjm01-sec-sec029008.bjzjm01.ksyun.com.task_4",
    ""
};

int main(int argc, char** argv)
{
    logdb *db = logdb::getInstance();
    if (db == NULL)
        return -1;
    db->set_flush_threshold(1);
    db->set_db_server("10.16.49.12:4242");
    db->set_db_uri("/api/put");
    db->set_acllib("acl/dnsacl.so");
    db->load_acl("acl/ksacl");
    logtask worker = logtask(LOG_DIR);

    boost::thread worker_thrd(boost::bind(&logtask::doAction, &worker));
    worker_thrd.join();
    cout << "exit..." << endl;

    return 0;
}
