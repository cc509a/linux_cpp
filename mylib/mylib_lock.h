#ifndef _H_MYLIB_LOCK_H_
#define _H_MYLIB_LOCK_H_
#include <exception>
#include <pthread.h>

namespace mylib
{

class MylibLocker
{
public:
    MylibLocker()
    {
        if(pthread_mutex_init(&mutex_, NULL) != 0)
        {
            throw std::exception();
        }
    }
    ~MylibLocker()
    {
        pthread_mutex_destroy(&mutex_);
    }
    bool Lock()
    { 
        return pthread_mutex_lock(mutex_) == 0;
    }
    bool Unlock() 
    { 
        return pthread_mutex_unlock(mutex_) == 0;
    }
private:
    pthread_mutex_t mutex_;
};
}
#endif