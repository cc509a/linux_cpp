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

/*状态机*/
HttpConn::LINE_STATUS HttpConn::parse_line()
{
	char temp;
	for(; checked_idx_ < read_idx_; ++ checked_idx_)
	{
		temp = read_buf_[checked_idx_];
		if(temp == '\r')
		{
			if((checked_idx_ + 1) == read_idx_)
			{
				return LINE_OPEN;
			}
			else if (read_buf_[checked_idx_+1] == '\n')
			{
				/* code */
				read_buf_[checked_idx_++] ='\0';
				read_buf_[checked_idx_++] ='\0';
				return LINE_OK;
			}
			return LINE_BAD;
		}
		else if(temp == '\n')
		{
			if((checked_idx_ > 1) && (read_buf_[checked_idx_ - 1] == '\r')
			{
				read_buf_[checked_idx_-1] ='\0';
				read_buf_[checked_idx_++] ='\0';
				return LINE_OK;
			}
			return LINE_BAD;
		}
	}
	return LINE_OPEN;
}

/*读取客户端数据*/
bool HttpConn::read()
{
	if(read_idx_ > READ_BUFFER_SIZE)
	{
		return false;
	}
	int bytes_read = 0;
	for(;;)
	{
		bytes_read = recv(sockfd_, read_buf_ + read_idx_, READ_BUFFER_SIZE - read_idx_, 0);
		if(bytes_read == -1)
		{
			int errno = bytes_read;
			if(errno == EAGIN || errno == EWOULDBLOCK)
			{
				break;
			}
			return false;
		}
		else if(bytes_read == 0)
		{
			return false;
		}
		read_idx_ += bytes_read;
	}
	return true;
}

HttpConn::HTTP_CODE HttpConn::parse_request_line(char* text)
{
	url_ = strpbrk(text, "\t");
	if(!url_)
	{
		return BAD_REQUEST;
	}
	*url_++ = '\0';

	char* method = text;
	if(strcasecmp(method, "GET") == 0)
	{
		method_ = GET;
	}
	else
	{
		return BAD_REQUEST;
	}
	url_ += strspn(url_, "\t");
	version_ = strpbrk(url_, "\t");
	if(!version_)
	{
		return BAD_REQUEST;
	}
	version_ += strspn(version_, "\t");
	if(strcasecmp(version_, "HTTP/1.1") !=0)
	{
		return BAD_REQUEST;
	}
	if(strcasecmp(url_, "http://", 7) == 0)
	{
		url_ += 7;
		url_ = strchar(url_, '/');
	}
	if(!url_ || url_[0] != '/')
	{
		return BAD_REQUEST;
	}
	checked_state_ = CHECK_STATE_HEADER;
	return NO_REQUEST;
}
















































