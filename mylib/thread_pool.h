#ifndef MYLIB_THREAD_POOL_H
#define MYLIB_THREAD_POOL_H

#include <list>
#include <cstdio>
#include <exception>
#include <pthread.h>
#include "mylib_sem.h"
#include "mylib_lock.h"

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
    void run();
private:
    int             thread_num_;    //线程数量
    int             max_req_;       //队列中最大请求数
    pthread_t*      threads_;       //描述线程池的数组
    std::list<T*>   worker_queue_;  //请求队列
    MylibLocker     locker_;        //保护队列锁
    MylibSem        queue_stat_;    //是否有任务需要处理
    bool            stop_;          //是否結束线程
};

template<typename T>
ThreadPool<T>::ThreadPool(int thread_num, int max_req):thread_num_(thread_num), max_req_(max_req), threads_(NULL), stop_(false)
{
    threads_ = new pthread_t[thread_num_];
    for(int i = 0; i < thread_num_; ++i)
    {
        printf("create the %dth thread\n",i);
        if(pthread_create(threads_ +i, NULL, worker, this) != 0)
        {
            delete [] threads_;
        }
        if(pthread_detach(threads_[i]))
        {
            delete [] threads_;
        }
    }
}

template<typename T>
ThreadPool<T>::~ThreadPool()
{
    delete [] threads_;
    stop_ = true;
}

template<typename T>
bool ThreadPool<T>::append(T* request)
{
    locker_.Lock();
    if(worker_queue_.size() > max_req_)
    {
        locker_.Unlock();
        return false;
    }
    worker_queue_.push_back(request);
    locker_.Unlock();
    queue_stat_.post();
    return true;
}

template<typename T>
void* ThreadPool<T>::worker(void *arg)
{
    ThreadPool *pool = reinterpret_cast<ThreadPool*>(arg);
    pool->run();
    return pool;
}

template<typename T>
void ThreadPool<T>::run()
{
    while(!stop_)
    {
        queue_stat_.wait();
        locker_.Lock();
        if(worker_queue_.empty())
        {
             locker_.Unlock();
             continue;
        }
        T* request = worker_queue_.front();
        worker_queue_.pop_front();
        locker_.Unlock();
        if(!request)
        {
            continue;
        }
        request->process();
    }
}


}
#endif 