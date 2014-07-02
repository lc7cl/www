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
	ret = bind(fd, (struct sockaddr*)&addr, sizeof(addr));
	if(ret == -1)
	{
		printf("bind() error:%d %s\n", errno, strerror(errno));
	}
	FD_ZERO(&r_set);
	FD_SET(fd, &r_set);
	while(select(fd, &r_set, NULL, NULL, NULL) > 0)
	{
		if(FD_ISSET(fd, &r_set))
		{
			ret = recvfrom(fd, buff, sizeof(buff), 0, (struct sockaddr*)&peer, &len);
			if(ret > 0)
			{
				write(stdout, buff, 2);
				printf("\n");
			}

			FD_CLR(fd, &r_set);
			FD_SET(fd, &r_set);
			memset(buff, 0, sizeof(buff));
		}
	}


	

	return 0;

}
