/*
*                           _ooOoo_
*                          o8888888o
*                          8888
*                          (| ^_^ |)
*                          O\  =  /O
*                       ____/`---'\____
*                     .'  \\|     |//  `.
*                    /  \\|||  :  |||//  \
*                   /  _||||| -:- |||||-  \
*                   |   | \\\  -  /// |   |
*                   | \_|  ''\---/''  |   |
*                   \  .-\__  `-`  ___/-. /
*                 ___`. .'  /--.--  `. . ___ 
*               ."" '<  `.___\_<|>_/___.'  >'"".
*             | | :  `- \`.;`\ _ /`;.`/ - ` : | |
*             \  \ `-.   \_ __\ /__ _/   .-` /  /
*       ========`-.____`-.___\_____/___.-`____.-'======== 
*                            `=---='
*       ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
*            FoZu Bless       No Bug       No Crash
*
* Author: lee - lee@www.com
*
* Last modified: 2014-08-22 21:49
*
* Filename: ../www/sender.c
*
* Description: 
*
*/
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
