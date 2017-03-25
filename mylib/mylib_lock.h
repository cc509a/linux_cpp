#ifndef _H_MYLIB_LOCK_H_
#define _H_MYLIB_LOCK_H_

namespace my_lib
{
template <typename T>
class MylibLock
{
public:
    MylibLock()
    {
        Lock();
    }
    ~MylibLock()
    {
        Unlock();
    }
    int Lock() { return lock_.Lock();}
    int Unlock() { return lock_.Unlock();}
private:
    T lock_;
};
}
#endif