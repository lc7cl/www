#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main() 
{
    int ret;
    in_addr_t address;

    ret = inet_pton(AF_INET, "1.1.0.1.www", &address);
    if (ret != 1) {
        printf("******************* %d \n", ret);
        return -1;
    }
    printf("@@@@@@@@@@\n");
    return 0;

}
