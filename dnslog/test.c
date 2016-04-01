#include "logdir.h"

int main(int argc, char** argv)
{
    logdir dir("/home/lee/");
    vector<string> list = dir.scan(false);
    for_each(list.begin(), list.end(), ostream_iterator<string>(cout, " "));

    return 0;
}
