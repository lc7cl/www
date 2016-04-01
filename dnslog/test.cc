#include <iostream>
#include <string>
#include <iterator>     
#include <vector>      
#include <algorithm>  

#include "logdir.h"

void print_list(std::string& s)
{
    std::cout << s << endl;
}

int main(int argc, char** argv)
{
    string path = string("/home/lee/code/www/dnslog");
    logdir dir(path);
    vector<std::string>* list = dir.scan(false);
    for_each(list->begin(), list->end(), print_list);

    return 0;
}
