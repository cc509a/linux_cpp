#include <iostream>
#include "curl/curl.h"
#include "thread_pool.h"

class A
{
public:
    void process()
    {
    }
};

int main(int argc, char * argv[])
{
    mylib::ThreadPool<A> pool;
    printf("hello world\n");
    return 0;
}