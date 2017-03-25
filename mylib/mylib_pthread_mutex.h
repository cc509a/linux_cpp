/********************************************************************
 @Author: cc509a
 @Created Time : 四  7/21 20:22:21 2016

 @File Name: mylib_pthread_mutex.h
 @Description:

*********************************************************************/
#ifndef _H_MYLIB_PTHREAD_MUTEX_H_
#define _H_MYLIB_PTHREAD_MUTEX_H_

#include <pthread.h>

namespace mylib
{

class MylibPthreadMutex()
{
    public:
    MylibPthreadMutex()
    {
        memset(&mutex_, 0, sizeof(mutex_));
        pthread_mutex_init(&mutex_, NULL);
    }
    ~MylibPthreadMutex()
    {
        pthread_mutex_destory(&mutex_);
    }
    int Lock() {return pthread_mutex_lock(&mutex_);}
    int Unlokc() {return pthread_mutex_unlock(&mutex_);}

    private:
    pthread_mutex_t mutex_;
};
}

#endif
