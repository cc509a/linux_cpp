#include <iostream>
#include "curl/curl.h"
#include "thread_pool.h"
#include "http_conn.h"

class A
{
public:
    void process()
    {
    }
};

const int MAX_FD = 65536;
const int MAX_EVENT_NUMBER = 10000;

extern void addfd(int epollfd, int fd, bool one_shot);
extern void removefd(int epollfd, int fd);

int main(int argc, char * argv[])
{
    mylib::ThreadPool<A> pool;
    printf("hello world\n");
    return 0;
}