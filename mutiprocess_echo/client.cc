#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <memory.h>
#include <errno.h>
#include <unistd.h>

#define BUF_SIZE 512
int main(int argc, char * argv[])
{

	if (argc < 2)
	{
		std::cout<<"params error"<<std::endl;
		return -1;
	}
	//const char *ip = argv[1];
	int port = atoi(argv[1]);
	int ret = 0;
	sockaddr_in * addr = new sockaddr_in();
	memset(addr, 0, sizeof(sockaddr_in));

	addr->sin_family = AF_INET;
	addr->sin_addr.s_addr = htonl(INADDR_ANY);
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
	else
	{
		puts("connecting ...");
	}
	char message[BUF_SIZE];
	memset(message, 0, sizeof(message));
	for(;;)
	{
		fputs("Input message (q to exit):", stdout);
		std::string str;
		std::cin>>str;
		if(str == std::string("q") || str == std::string("Q"))
		{
			break;
		}
		send(sock_fd, str.c_str(), str.size(), 0);
		recv(sock_fd, message, BUF_SIZE -1, 0);
   		std::cout<<"message :" << message << std::endl;
   	}
	close(sock_fd);
	delete addr;
	return 0;
}