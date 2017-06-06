#ifndef _H_MYLIB_COMMON_H_
#define _H_MYLIB_COMMON_H_
#include <vector>
#include <map>

//static const int SUCCESS = 0;

#define SUCCESS 0

#define IFNOT_RET(cmd, ret)                                                    \
    do{                                                                        \
        int iRet = (cmd);                                                      \
        if(iRet != ret)                                                        \
        {                                                                      \
            return iRet;                                                       \
        }                                                                      \
    } while(0)

enum ERROR_CODE
{
	ERR_SYS_STAT = -101,
	ERR_SYS_FOPEN = -102,
	ERR_SYS_FREAD = -103,
};

#endif