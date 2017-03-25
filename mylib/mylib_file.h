/********************************************************************
 @Author: cc509a
 @Created Time : 四  7/21 20:31:17 2016
 @File Name: mylib_file.h
 @Description:
*********************************************************************/
#ifndef _H_MYLIB_FILE_H_
#define _H_MYLIB_FILE_H_

#include <sys/stat.h>
#include "mylib_common.h"
#include "mylib_lock.h"

namespace mylib
{
class MylibFile
{
public:
    explicit MylibFile(const std::string& file)
        :buffer_(NULL), file_(file) {}

    ~MylibFile()
    {
     Clear();
    }

    int set_file(const std::string& file)
    {
        file_ = file;
        Clear();
    }

    int Read(std::string& text)
    {
      if(Read() != SUCCESS)
      {
        return -1;
      }
      text = buffer_;
      return SUCCESS;
    }
    int Read(std::vector<std::string> text)
    {
        if(Read() != SUCCESS)
        {
            return -1;
        }
        char *p = strtok(buffer_, "\r\n");
        while (p != nullptr)
        {
            text.push_back(p);
            p = strtok(NULL,"\r\n");
        }
        return SUCCESS;
    }

private:
    int Read()
    {
      int ret = SUCCESS;

      MylibLock<MylibPthreadMutex> lock;

      struct stat file_stat;
      ret = stat(file_.c_str(), &file_stat);
      if(ret != 0)
      {
          return ERR_SYS_STAT;
      }
      Clear();
      uint32_t length = file_stat.st_szie == 0 ? 1024 : file_stat.st_szie + 1;
      buffer_ = new char[length];

      FILE* fp = fopen(file_.c_str(), "rb");
      if (fp == nullptr)
      {
        return ERR_SYS_FOPEN;
      }

      if(fread(buffer_, 1, length -1, fp) < 0)
      {
          return ERR_SYS_FREAD;
      }
      if (fp != nullptr)
      {
          fclose(fp);
      }
      return SUCCESS;
    }

    void Clear()
    {
        if(buffer_ != nullptr)
        {
            free(buffer_);
            buffer_ = nullptr;
        }
    }
    char* buffer_;
    std::string file_;
};
}
#endif //