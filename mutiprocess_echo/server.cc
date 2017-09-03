#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <memory.h>
#include <errno.h>
#include <unistd.h>
#include <vector>

#define BUF_SIZE 512
#define PID_NUM 3


int handle_connect(int sock_fd)
{

	struct sockaddr_in client;
	socklen_t  clent_addr_len = sizeof(client);
	while(true)
	{
		int connfd = accept(sock_fd, (struct sockaddr*)&client,  &clent_addr_len);
		if(connfd < 0)
		{
			printf("error:%d\n", errno);
			continue;
		}
		else
		{
			char buf[BUF_SIZE];
			memset(buf, 0, sizeof(buf));
			while(recv(connfd, buf, BUF_SIZE-1, 0))
			{
				printf("recv message:%s\n", buf);
				send(connfd, buf, strlen(buf), 0);
			}
			close(connfd);
		}
	}
}


int main(int argc, char * argv[])
{

	if (argc < 2)
	{
		std::cout<<"params error"<<std::endl;
		return -1;
	}
	int port = atoi(argv[1]);

	int sock_fd = socket(PF_INET, SOCK_STREAM, 0);
	if(sock_fd == -1)
	{
		return -1;
	}
	int ret = 0;
	sockaddr_in * addr = new sockaddr_in();
	memset(addr, 0, sizeof(sockaddr_in));
	addr->sin_family = AF_INET;
	addr->sin_addr.s_addr = htonl(INADDR_ANY);
	addr->sin_port = htons(port);
	//inet_pton(AF_INET, ip, &(addr->sin_addr));

	ret = bind(sock_fd, (struct sockaddr *)addr, sizeof(sockaddr));
	if(ret != 0)
	{
		printf("bind error\n");
		return -2;
	}
	else
	{
		puts("binding ...");
	}

	int backlog = 5;
	ret = listen(sock_fd, backlog);
	if(ret != 0)
	{
		printf("listen error\n");
		return -2;
	}
	else
	{
		puts("listening ...");
	}

	std::vector<pid_t> pids(PID_NUM);
	for(int i = 0; i < PID_NUM; ++i)
	{
		pids[i] = fork();
		if(pids[i] == 0)
		{
			handle_connect(sock_fd);
		}
	}
	for(;;);
	close(sock_fd);
	delete addr;
	return 0;
}