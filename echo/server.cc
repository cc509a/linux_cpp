#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <memory.h>
#include <errno.h>
#include <unistd.h>
#define BUF_SIZE 1024
int main(int argc, char * argv[])
{



	if (argc < 3)
	{
		std::cout<<"params error"<<std::endl;
	}
	const char *ip = argv[1];
	int port = atoi(argv[2]);

	int sock_fd = socket(PF_INET, SOCK_STREAM, 0);
	if(sock_fd == -1)
	{
		return -1;
	}
	int ret = 0;
	sockaddr_in * addr = new sockaddr_in();
	memset(addr, 0, sizeof(sockaddr_in));
	addr->sin_family = AF_INET;
	inet_pton(AF_INET, ip, &(addr->sin_addr));
	addr->sin_port = htons(port);
	std::cout<<"sizeof sockaddr_in"<< sizeof(sockaddr_in)<<std::endl;

	ret = bind(sock_fd, (struct sockaddr *)addr, sizeof(sockaddr));
	if(ret != 0)
	{
		printf("bind error\n");
		return -2;
	}

	int backlog = 5;
	ret = listen(sock_fd, backlog);
	if(ret != 0)
	{
		printf("listen error\n");
		return -2;
	}


	struct sockaddr_in client;
	socklen_t  clent_addr_len = sizeof(client);
	while(true)
	{
		int connfd = accept(sock_fd, (struct sockaddr*)&client,  &clent_addr_len);
		if(connfd < 0)
		{
			printf("error:%d\n", errno);
		}
		else
		{
			char buf[BUF_SIZE];
			memset(buf, 0, sizeof(buf));
			ret = recv(connfd, buf, BUF_SIZE-1, 0);
			//std::cout<<"recv message:" << buf<<std::endl;
			printf("recv message:%s\n", buf);
			close(connfd);
		}
		close(sock_fd);
	}
	delete addr;
	return 0;
}