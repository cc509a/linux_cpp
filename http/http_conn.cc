#include "http_conn.h"


const char * ok_title = "OK";

const char * error_400_title = "Bad Request";
const char * error_400_from = "your request is Bad Request \n";

const char * error_403_title = "Forbiddn";
const char * error_403_from = "you not have permisson\n";

const char * error_404_title = "not found";
const char * error_404_from = "request file not found";

const char * error_500_from = "server error \n";
const char * error_500_title = "server error";

const char * doc_root = "/home/rugang/Documents/www/";

#define CHECK_CHAR_PTR(ptr) \
if ((ptr) == nullptr) \
    return -1         \

int setnoblocking(int fd)
{
	int old_option = fcntl(fd, F_GETFL);
	int new_option = old_option | O_NONBLOCK;
	fcntl(fd, F_SETFL, new_option);
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

void modfd(int epollfd, int fd, int ev)
{
    epoll_event event;
    event.data.fd = fd;
    event.events = ev | EPOLLET | EPOLLONESHOT | EPOLLRDHUP;
    epoll_ctl (epollfd, EPOLL_CTL_MOD, fd, &event);
}

int HttpConn::user_count_ = 0;
int HttpConn::epollfd_ = -1;

void HttpConn::init(int sockfd, const sockaddr_in& addr)
{
    sockfd_ = sockfd;
    address_ = addr;
    int reuse = 1;
    setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
    addfd(epollfd_, sockfd, true);
    ++user_count_;
    init();
}

void HttpConn::init()
{
    checked_state_ = CHECK_STATE_REQUESTLINE;
    linger_ = false;
    method_ = GET;
    url_ = 0;
    version_ = 0;
    content_length_ = 0;
    host_ = 0;
    start_line_ = 0;
    checked_idx_ = 0;
    read_idx_ = 0;
    write_idx_ = 0;
    memset(read_buf_, '\0', READ_BUFFER_SIZE);
    memset(write_buf_, '\0', WRITE_BUFFER_SIZE);
    memset(real_file_, '\0', FILE_NAME_LEN);
}

void HttpConn::close_conn(bool real_close)
{
    std::cout<<"close_conn"<<std::endl;
    if(real_close && (sockfd_ = -1))
    {
        removefd(epollfd_, sockfd_);
        sockfd_ = -1;
        --user_count_;
    }
}


/*从状态机*/
HttpConn::LINE_STATUS HttpConn::parse_line()
{
    std::cout<<"parse_line"<<std::endl;
	char temp;
	for(; checked_idx_ < read_idx_; ++ checked_idx_)
	{
        //printf("checked_idx_:%u,read_idx_:%u \n", checked_idx_, read_idx_);
		temp = read_buf_[checked_idx_];
        //std::cout<< temp << std::endl;
		if(temp == '\r')
		{
            printf("rrrrrr %d\n", read_buf_[checked_idx_+1]);
			if((checked_idx_ + 1) == read_idx_)
			{
                printf("line open\n");
				return LINE_OPEN;
			}
			else if (read_buf_[checked_idx_+1] == '\n')
			{
				/* code */
                printf("read buf ++");
				read_buf_[checked_idx_++] ='\0';
			    read_buf_[checked_idx_++] ='\0';
				return LINE_OK;
			}
			return LINE_BAD;
		}
		else if(temp == '\n')
		{
            printf("nnnnn \n");
			if((checked_idx_ > 1) && (read_buf_[checked_idx_ - 1] == '\r'))
			{
				read_buf_[checked_idx_-1] ='\0';
				read_buf_[checked_idx_++] ='\0';
                //
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
    std::cout<<"read"<<std::endl;
	if(read_idx_ > READ_BUFFER_SIZE)
	{
		return false;
	}
	int bytes_read = 0;
	for(;;)
	{
		bytes_read = recv(sockfd_, read_buf_ + read_idx_, READ_BUFFER_SIZE - read_idx_, 0);
        //printf("%s\n", read_buf_);
        printf("recv request bytes_read:%d, read_idx_:%u\n", bytes_read, read_idx_);
		if(bytes_read == -1)
		{
			
			if(errno == EAGAIN || errno == EWOULDBLOCK)
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

/*解析HTTP请求，获得请求方法*/
HttpConn::HTTP_CODE HttpConn::parse_request_line(char* text)
{
    printf("parse_request_line text:%s\n", text);
	url_ = strpbrk(text, " \t");
	if(url_ == nullptr)
	{
        printf("bad request \n");
		return BAD_REQUEST;
	}
	*url_++ = '\0';

	char* method = text;

    printf("method:%s url:%s \n", method, url_);
	if(strcasecmp(method, "GET") == 0)
	{
		method_ = GET;
	}
	else
	{
		return BAD_REQUEST;
	}
	url_ += strspn(url_, " \t");
	version_ = strpbrk(url_, " \t");
	if(!version_)
	{
		return BAD_REQUEST;
	}
    *version_++ = '\0';
	version_ += strspn(version_, " \t");
	if(strcasecmp(version_, "HTTP/1.1") !=0)
	{
		return BAD_REQUEST;
	}
	if(strncasecmp(url_, "http://", 7) == 0)
	{
		url_ += 7;
		url_ = strchr(url_, '/');
	}
	if(!url_ || url_[0] != '/')
	{
		return BAD_REQUEST;
	}
	checked_state_ = CHECK_STATE_HEADER;
	return NO_REQUEST;
}

/*解析头部*/
HttpConn::HTTP_CODE HttpConn::parse_headers(char* text)
{
    std::cout<<"parse_headers"<<std::endl;
    if(text[0] == '\0')
    {
        if(content_length_ !=0)
        {
            checked_state_ = CHECK_STATE_CONTENT;
            return NO_REQUEST;
        }
        return GET_REQUEST;
    }
    else if(strncasecmp(text, "Connection:", 11) == 0)
    {
        text += 11;
        text += strspn(text, " \t");
        if(strcasecmp(text, "keep-alive") == 0)
        {
            linger_ = true;
        }
    }
    /*处理content_lenght 头部字段*/
    else if(strncasecmp(text, "Conten-Length:", 15) == 0)
    {
        text += 15;
        text += strspn(text, " \t");
        content_length_ = atol(text);
    }
    /*处理host头部字段*/
    else if(strncasecmp(text, "Host:",5) ==0)
    {
        text += 5;
        text += strspn(text, " \t");
        host_ = text;
    }
    else
    {
        printf("oop! unkonw header %s\n",text);
    }
    return NO_REQUEST;
}

HttpConn::HTTP_CODE HttpConn::parse_content(char *text)
{
    std::cout<<"parse_content"<<std::endl;
    if(read_idx_ >= (content_length_ + checked_idx_))
    {
        text[content_length_] = '\0';
        return GET_REQUEST;
    }
    return NO_REQUEST;
}

/*主机状态机*/
HttpConn::HTTP_CODE HttpConn::process_read()
{
    std::cout<<"process_read"<<std::endl;
    LINE_STATUS line_status = LINE_OK;
    HTTP_CODE ret = NO_REQUEST;
    char* text = nullptr;

    while(((checked_state_ == CHECK_STATE_CONTENT) && (line_status == LINE_OK)) || ((line_status = parse_line()) == LINE_OK))
    {
        printf("line_ok\n");
        text = get_line();
        start_line_ = checked_idx_;
        printf("got 1 http line:%s\n", text);

        switch(checked_state_)
        {
            case CHECK_STATE_REQUESTLINE:
            {
                ret = parse_request_line(text);
                if(ret == BAD_REQUEST)
                {
                    return BAD_REQUEST;
                }
                break;
            }
            case CHECK_STATE_HEADER:
            {
                ret = parse_headers(text);
                if(ret == BAD_REQUEST)
                {
                    return BAD_REQUEST;
                }
                else if (ret == GET_REQUEST)
                {
                    return do_request();
                }
                break;
            }
            case CHECK_STATE_CONTENT:
            {
                ret = parse_content(text);
                if(ret == GET_REQUEST)
                {
                    return do_request();
                }
                line_status = LINE_OPEN;
                break;
            }
            default:
            {
                return INTERNAL_ERROR;
            }
        }
    }
    return NO_REQUEST;
}
HttpConn::HTTP_CODE HttpConn::do_request()
{
    std::cout<<"do_request"<<std::endl;
    strcpy(real_file_, doc_root);
    int len = strlen(doc_root);
    strncpy(real_file_ + len, url_, FILE_NAME_LEN - len -1);
    if(stat(real_file_, &file_stat_) < 0)
    {
        return NO_RESOURCE;
    }
    //文件权限
    if(! (file_stat_.st_mode & S_IROTH))
    {
        return FORBIDDEN_REQUEST;
    }
    if(S_ISDIR(file_stat_.st_mode))
    {
        return BAD_REQUEST;
    }
    int fd = open(real_file_, O_RDONLY);

    file_address_ = (char*)mmap(0, file_stat_.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    close(fd);
    return FILE_REQUEST;
}
 
/*内存映射操作*/
void HttpConn::unmap()
{   
    std::cout<<"unmap"<<std::endl;
    if(file_address_)
    {
        munmap(file_address_, file_stat_.st_size);
        file_address_ = nullptr;
    }
}


bool HttpConn::write()
{
    std::cout<<"write"<<std::endl;
    int temp = 0;
    int bytes_have_send = 0;
    int bytes_to_send = write_idx_;
    if(bytes_to_send == 0)
    {
        modfd(epollfd_, sockfd_, EPOLLIN);
        init();
        return true;
    }
    for(;;)
    {
        temp = writev(sockfd_, iv_, iv_count_);
        {
            if(temp <= -1)
            {
                /*如果TPC写缓冲区没有空间，则等待下一轮EPOLLOUT 事件，虽然在此期间，服务器无法立即接收到同一客户端的请求，但这可以保证链接的完整性*/
                if(errno == EAGAIN)
                {
                    modfd(epollfd_, sockfd_, EPOLLOUT);
                    return true;
                }
                unmap();
                return false;
            }
            bytes_to_send -= temp;
            bytes_have_send += temp;
            if(bytes_to_send <= bytes_have_send)
            {
                unmap();
                if(linger_)
                {
                    init();
                    modfd(epollfd_, sockfd_, EPOLLIN);
                    return true;
                }
                else
                {
                    modfd(epollfd_, sockfd_, EPOLLIN);
                    return false;
                }
            }
        }
    }
}

/*写缓冲区发送数据*/
bool HttpConn::add_response(const char* format, ...)
{
    if(write_idx_ >= WRITE_BUFFER_SIZE)
    {
        return false;
    }
    va_list arg_list;
    va_start(arg_list, format);
    uint32_t len = vsnprintf(write_buf_ + write_idx_, WRITE_BUFFER_SIZE - 1 - write_idx_, format, arg_list);
    if(len >= (WRITE_BUFFER_SIZE -1 - write_idx_))
    {
        return false;
    }
    write_idx_  += len;
    va_end(arg_list);
    return true;
}

bool HttpConn::add_status_line(int status, const char* title)
{
    return add_response("%s %d %s\r\n","HTTP/1.1", status, title);
}

bool HttpConn::add_headers(int content_len)
{
    add_content_length(content_len);
    add_linger();
    add_blank_line();
    return true;
}

bool HttpConn::add_content_length(int content_len)
{
    return add_response("Content-Length:%d\r\n", content_len);
}

bool HttpConn::add_linger()
{
    return add_response("Connection: %s\r\n", (linger_ == true) ? "keep-alive" : "close");
}

bool HttpConn::add_blank_line()
{
    return add_response("%s", "\r\n");
}

bool HttpConn::add_content(const char * content)
{
    return add_response("%s", content);
}


bool HttpConn::process_write(HTTP_CODE ret)
{
    std::cout<<"process_write"<<std::endl;
    switch(ret)
    {
        case INTERNAL_ERROR:
        {
            add_status_line(500, error_500_title);
            add_headers(strlen(error_500_from));
            if(!add_content(error_500_from))
            {
                return false;
            }
            break;
        }
        case BAD_REQUEST:
        {
            add_status_line(400, error_400_title);
            add_headers(strlen(error_400_from));
            if(!add_content(error_400_from))
            {
                return false;
            }
            break;
        }
        case NO_RESOURCE:
        {
            add_status_line(404, error_404_title);
            add_headers(strlen(error_404_from));
            if(!add_content(error_404_from))
            {
                return false;
            }
            break;
        }
        case FORBIDDEN_REQUEST:
        {
            add_status_line(403, error_403_title);
            add_headers(strlen(error_403_from));
            if(!add_content(error_403_from))
            {
                return false;
            }
            break;
        }
        case FILE_REQUEST:
        {
            add_status_line(200, ok_title);
            if(file_stat_.st_size !=0 )
            {
                add_headers(file_stat_.st_size);
                iv_[0].iov_base = write_buf_;
                iv_[0].iov_len = write_idx_;
                iv_[1].iov_base = file_address_;
                iv_[1].iov_len = file_stat_.st_size;
                iv_count_ = 2;
                return true;
            }
            else
            {
                const char* ok_string = "<html><body></body></html>";
                add_headers(strlen(ok_string));
                if(!add_content(ok_string))
                {
                    return false;
                }
            }
            break;
        }
        default:
        {
            return false;
        }
    }
    iv_[0].iov_base = write_buf_;
    iv_[0].iov_len = write_idx_;
    iv_count_ = 1;
    return true;
}
/*处理客户端请求*/
void HttpConn::process()
{
    std::cout<<"process"<<std::endl;
    HTTP_CODE read_ret = process_read();
    if(read_ret == NO_REQUEST)
    {
        modfd(epollfd_, sockfd_, EPOLLIN);
        return;
    }
    bool write_ret = process_write(read_ret);
    if(!write_ret)
    {
        close_conn();
    }
    modfd(epollfd_, sockfd_, EPOLLOUT);
}

