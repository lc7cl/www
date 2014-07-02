#include <sys/socket.h>
#include <errno.h>
#include <stdio.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/select.h>
#include <unistd.h>

int main()
{
	int fd, ret;
	char buff[512];
	struct sockaddr_in addr, peer;
	fd_set r_set;
	int len;

	fd = socket(AF_INET, SOCK_DGRAM, 0);
	if(fd < 0)
		printf("socket() error:%d  %s\n", errno, strerror(errno));
	bzero(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port   = htons(11111);
	addr.sin_addr.s_addr = inet_addr("192.168.137.153");
	sprintf(buff, "hello world!");
	ret = sendto(fd, buff, 20, 0, (struct sockaddr*)&addr, len);

	

	return 0;

}
