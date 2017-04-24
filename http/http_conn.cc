#include "http_conn.h"


const char * ok_title = "OK";
const char * error_400 = "Bad Request";
const char * error_400_from = "your req\n";
const char * error_403_title = "Forbiddn";
const char * error_403_from = "you not have permisson\n";
const char * error_404_title = "not fund";
const char * error_500_from = "server error \n";
const char * error_599_title = "server error";

const char * doc_root = "/var/www/html";

int setnoblocking(int fd)
{
	int old_option = fcntl(fd, F_GETFL);
	int new_option = old_option | O_NONBLOCK;
	fcntl(fd, F_SETFL);
	return old_option;
}

void addfd(int epollfd, int fd, bool one_shot)
{
	epoll_event event;
	event.data.fd = fd;
	event.events = EPOLLIN | EPOLLET | EPOLLRDHUP;
	if(one_shot)
	{
		event.events |= EPOLLONESHOT;
	}
	epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event);
	setnoblocking(fd);
}

void removefd(int epollfd, int fd)
{
	epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, 0);
	close(fd);
}
