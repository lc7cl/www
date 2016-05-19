#include <string>
using namespace std;
#include <boost/thread.hpp>
#include <boost/bind.hpp>

#include "logstream.h"
#include "logdb.h"
#include "journal_worker.h"

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
    
    FileScanner::GetInstance().SetPath();
    
    
    vector<string> v;
    v.clear();
    v.push_back(log_file_names[0]);
    v.push_back(log_file_names[1]);
    v.push_back(log_file_names[2]);
    v.push_back(log_file_names[3]);
    JournalWorker jnl_worker1 = JournalWorker();
    jnl_worker1.BindStream(v);
    
    v.clear();
    v.push_back(log_file_names[4]);
    v.push_back(log_file_names[5]);
    v.push_back(log_file_names[6]);
    v.push_back(log_file_names[7]);
    JournalWorker jnl_worker2 = JournalWorker();
    jnl_worker2.BindStream(v);

    boost::thread jnl_worker_thrd1(
        boost::bind(&JournalWorker::Work, 
        &jnl_worker1));
    boost::thread jnl_worker_thrd2(
        boost::bind(&JournalWorker::Work, 
        &jnl_worker2));
    jnl_worker_thrd1.join();
    jnl_worker_thrd2.join();
    cout << "exit..." << endl;

    return 0;
}
