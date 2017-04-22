#ifndef MY_HTTP_DEMO_CONN_H
#define MY_HTTP_DEMO_CONN_H

#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <sys/stat.h>
#include <string.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <stdarg.h>
#include <errno.h>
#include "mylib_lock.h"
#include "mylib_sem.h"

class HttpConn
{
public:
    static const uint32_t FILE_NAME_LEN = 200;

    static const uint32_t READ_BUFFER_SIZE = 2048;

    static const uint32_t WRITE_BUFFER_SIZE = 1024;

    enum METHOD
    {
        GET = 0,
        POST,
        HEAD,
        PUT,
        DELETE,
        TRACE,
        OPTIONS,
        CONNECT,
        PAHT
    };
    enum CHECK_STATE
    {
        CHECK_STATE_REQUESTLINE = 0,
        CHECK_STATE_HEADER,
        CHECK_STATE_CONTENT
    };

    enum HTTP_CODE
    {
        NO_REQUEST,
        GET_REQEST,
        BAD_REQUEST,
        NO_RESOURCE,
        FORBIDDEN_REQUEST,
        FILE_REQUEST,
        INTERNAL_ERROR,
        CLOSED_CONNECTION
    };

    enum LINE_STATUS
    {
        LINE_OK = 0,
        LINE_BAD,
        LINE_OPEN
    };

public:
    HttpConn() = default;
    ~HttpConn() = default;

public:
    void init(int sockfd, const sockaddr_in& addr);
    /*关闭连接*/
    void close_conn(bool real_close = true);
    /*处理请求*/
    void process();
    /*非阻塞读*/
    bool read();
    /*非阻塞写*/
    bool write();
private:
    void init();
    /*解析HTTP请求*/
    HTTP_CODE process_read();
    /*填充http response*/
    bool process_write(HTTP_CODE ret);

    HTTP_CODE parse_request_line(char* text);
    HTTP_CODE parse_headers(char* text);
    HTTP_CODE parse_content(char* text);
    HTTP_CODE do_request();
    char* get_line() { return read_buf_ + start_line_;}
    LINE_STATUS parse_line();
    /*处理*/
    void unmap();
    bool add_response(const char * format, ...);
    bool add_content(const char* content);
    bool add_status_line(int status, const char* title);
    bool add_content_length(int content_length);
    bool add_headers(int header_lenght);
    bool add_linger();
    bool add_blank_line();

public:
    static int epollfd_;
    static int user_count_;

private:
    int          sockfd_;
    sockaddr_in  address_;

    char    read_buf_[READ_BUFFER_SIZE];
    int     read_idx_;
    int     checked_idx;
    int     start_line_;

    char    write_buf_[WRITE_BUFFER_SIZE];
    int     write_idx;

    CHECK_STATE check_state_;
    METHOD      method_;

    /*请求文件路径, doc_root + url*/
    char  read_file_[FILE_NAME_LEN];
    
    char* url_;
    /*协议版本号*/
    char  version_;
    char* host_;
    int   content_length_;
    /*是否保持连接*/
    bool  linger_;
    char* file_address_;
    struct stat file_stat_;
    struct iovec iv_[2]; 
    int    iv_count_;
};


#endif