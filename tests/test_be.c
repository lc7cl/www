#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
 
int main(int argc, char **argv)
{
    struct sockaddr_in addr;
    int i;  
    unsigned int ip;

    if (argc < 2)
        return -1;

    for (i = 1; i < argc; i++) {
        if (inet_aton(argv[i], &addr.sin_addr))
            printf("%s %u\n", argv[i], addr.sin_addr.s_addr);
        else
            printf("%s error\n", argv[i]);
        if(inet_pton(AF_INET, argv[i], &ip) <=0 ) {
            printf("%s error\n", argv[i]);
        } else {
            printf("%s %u\n", argv[i], ip);
        }

    }
    return 0;
}
