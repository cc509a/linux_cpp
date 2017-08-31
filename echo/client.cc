#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <memory.h>
#include <errno.h>
#include <unistd.h>
int main(int argc, char * argv[])
{

	if (argc < 3)
	{
		std::cout<<"params error"<<std::endl;
	}
	const char *ip = argv[1];
	int port = atoi(argv[2]);


	int ret = 0;
	sockaddr_in * addr = new sockaddr_in();
	memset(addr, 0, sizeof(sockaddr_in));

	addr->sin_family = AF_INET;
	inet_pton(AF_INET, ip, &(addr->sin_addr));
	addr->sin_port = htons(port);


	std::cout<< sizeof(sockaddr_in)<<std::endl;
	

	int sock_fd = socket(PF_INET, SOCK_STREAM, 0);
	if(sock_fd == -1)
	{
		return -1;
	}
	ret = connect(sock_fd, (struct sockaddr*) addr, sizeof(sockaddr_in));
	if(ret < 0)
	{
		printf("connetct error \n");
		return -2;
	}
	const char * msg  = "hello world";
	send(sock_fd, msg, strlen(msg), 0);

	close(sock_fd);

	delete addr;
	return 0;
}