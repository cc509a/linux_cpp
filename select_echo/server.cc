#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <memory.h>
#include <errno.h>
#include <unistd.h>
#include <vector>
#include <pthread.h>

#define BUF_SIZE 512
#define PID_NUM 3

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

static void * handle_request(void *argv)
{
	
	int sock_fd = *((int*)argv);
	int max_fd = -1;
	struct timeval time_out;
	time_out.tv_sec = 1;
	time_out.tv_usec = 0;
	fd_set scanfd;

	int connect_host[10];
	int conn_num = 0;

	int err = -1;
	for(;;)
	{
		max_fd = -1;
		FD_ZERO(&scanfd);
		for(int i = 0; i < 10; ++i)
		{
			if(connect_host[i] != -1)
			{
				FD_SET(connect_host[i], &scanfd);
				if(nax_fd < connect_host[i])
				{
					max_fd = connect_host[i];
				}
			}

		}
	}

	struct sockaddr_in client;
	socklen_t  clent_addr_len = sizeof(client);

	while(true)
	{
		pthread_mutex_lock(&lock);
		int connfd = accept(sock_fd, (struct sockaddr*)&client,  &clent_addr_len);
		pthread_mutex_unlock(&lock);
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
	return 0;
}


static void  handle_connect(int sock_fd)
{
	std::vector<pthread_t> pool(5);
	for(size_t i = 0; i < pool.size(); ++i)
	{
		pthread_create(&pool[i], NULL, handle_request, &sock_fd);
	}
	for(size_t i = 0; i < pool.size(); ++i)
	{
		pthread_join(pool[i], NULL);
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
	/*处理客户端请求*/
	std::vector<pthread_t> pool(2);
	pthread_create(&pool[0], NULL, handle_connect,(void *)&sock_fd);
	pthread_create(&pool[1], NULL, handle_request, NULL);

	for(size_t i = 0; i < pool.size(); ++i)
	{
		pthread_join(pool[i], NULL);
	}

	close(sock_fd);
	delete addr;
	return 0;
}