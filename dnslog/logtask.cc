#include "logdir.h"
#include "logdb.h"
#include "logtask.h"

void logtask::doAction()
{
    dns_item item;
    enum stream_state ret;
    int line_nb = 0;
    logdir dir(m_path);
    vector<logfile> files;    
    
    while (1)
    {
        vector<logfile>().swap(files);
        const boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time(); 
        if (now.time_of_day().minutes() >= 20)
        {
            dir.scan(files, false);
        }
                
        vector<logfile>::iterator it = files.begin();
        for (it; it != files.end(); it++)
        {
            line_nb = 0;
            logstream stream(*it);
            const boost::posix_time::ptime now1 = boost::posix_time::microsec_clock::local_time();  
            cout << "+++++ " << it->f_path << endl;
            ret = stream.read(item);
            while (ret != STREAM_STATE_EOF && ret != STREAM_STATE_FILEERROR)
            {
                if (ret == STREAM_STATE_OK)
                {
                    line_nb++;
                    logdb::getInstance()->put(item);
                }  
                ret = stream.read(item);                  
            }  
            if (ret == STREAM_STATE_EOF)    
            {
                cout << "----- " << it->f_path << "  "  << line_nb << endl;
            }
            else if (ret == STREAM_STATE_FILEERROR)
            {
                cout << "!!!!! " << it->f_path << endl;
            }
            const boost::posix_time::ptime now2 = boost::posix_time::microsec_clock::local_time(); 
            const boost::posix_time::time_duration td = now2 - now1;
            cout << "total time :" << td.total_milliseconds() << " (ms)" << endl;    
            fs::remove(it->f_path);                                                              
        }
        logdb::getInstance()->flush();
        logdb::getInstance()->flush_all();
        sleep(6);
    }
}
