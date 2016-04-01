#ifndef _DIRECTORY_H_
#define _DIRECTORY_H_

class directory {

public:
    vector<boost::path> scan();
    vector<boost::path> rescan();

private:
    std::string m_path;
    std::time_t timestamp;
};

#endif
