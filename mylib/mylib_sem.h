#ifndef MYLIB_SEM_H
#define MYLIB_SEM_H
#include <semaphore.h>
#include <exception>
namespace mylib
{
class MylibSem
{
public:
    MylibSem()
    {
        if(sem_init(&sem_, 0, 0) != 0)
        {
            throw std::exception();
        }
    }
    ~MylibSem()
    {
        sem_destroy(&sem_);
    }

    bool wait()
    {
        return sem_wait(&sem_) == 0;
    }
    bool post()
    {
        return sem_post(&sem_) == 0;
    }

private:
    sem_t sem_;
};
}

#endif