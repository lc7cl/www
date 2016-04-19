

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
        
        dir.scan(files, false);
        vector<logfile>::iterator it = files.begin();
        for (it; it != files.end(); it++)
        {
            line_nb = 0;
            logstream stream(*it);
            cout << "+++++ " << it->f_path << endl;
            ret = stream.read(item);
            while (ret != STREAM_STATE_EOF && ret != STREAM_STATE_FILEERROR)
            {
                if (ret == STREAM_STATE_OK)
                {
                    line_nb++;
                    logdb::getInstance()->put(item);
                }                    
            }  
            if (ret == STREAM_STATE_EOF)    
            {
                cout << "----- " << it->f_path << "  "  << line_nb << endl;
            }
            else if (ret == STREAM_STATE_FILEERROR)
            {
                cout << "!!!!! " << it->f_path << endl;
            }
        }
        sleep(6);
    }
}
