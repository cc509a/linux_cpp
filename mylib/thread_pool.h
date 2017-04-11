#ifndef MYLIB_THREAD_POOL_H
#define MYLIB_THREAD_POOL_H

#include <list>
#include <cstdio>
#include <exception>
#include <pthread.h>

namespace mylib
{
template<typename T>
class ThreadPool
{
public:
    ThreadPool(int thread_num = 8, int max_req = 10000);
    ~ThreadPool();
    bool append(T* request);

private:
    static void* worker(void* arg);
private:
    int             thread_num_;    //线程数量
    int             max_req_;       //队列中最大请求数
    pthread_t*      threads_;       //描述线程池的数组
    std::list<T*>   worker_queue_;  //请求队列
    MylibLocker     locker_;        //保护队列锁
    MylibSem        sem_;           //是否有任务需要处理
    bool            stop_;          //是否結束线程
};

template<typename T>
ThreadPool<T>::ThreadPool(int thread_num, int max_req):thread_num_(thread_num), max_req_(max_req), threads_(NULL), stop_(false)
{
    threads_ = new pthread_t[thread_num_];
    for(int i = 0; i < thread_num_; ++i)
    {
    
    }
}


}


#endif 