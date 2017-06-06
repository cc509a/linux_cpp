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
        :buffer_(nullptr), file_(file) {}

    ~MylibFile()
    {
     Clear();
    }

    void set_file(const std::string& file)
    {
        file_ = file;
        Clear();
    }

    int Read(std::string& text)
    {
      if(Read() != 0)
      {
        return -1;
      }
      text = buffer_;
      return 0;
    }
    int Read(std::vector<std::string>& text)
    {
        if(Read() != 0)
        {
            return -1;
        }
        char *p = strtok(buffer_, "\r\n");
        while (p != nullptr)
        {
            text.push_back(p);
            p = strtok(NULL,"\r\n");
        }
        return 0;
    }

private:
    int Read()
    {
      int ret = 0;

      //MylibLock<MylibPthreadMutex> lock;

      struct stat file_stat;
      ret = stat(file_.c_str(), &file_stat);
      if(ret != 0)
      {
          return ERR_SYS_STAT;
      }
      Clear();
      size_t length = file_stat.st_size == 0 ? 1024 : file_stat.st_size + 1;
      
      buffer_ = (char*)malloc(length);
      if(buffer_ == nullptr)
      {
        return -2;
      }
      memset(buffer_, 0, length);

      FILE* fp = fopen(file_.c_str(), "rb");
      if (fp == nullptr)
      {
        return ERR_SYS_FOPEN;
      }
      size_t read_ret = fread(buffer_, 1, length -1, fp);
      if( read_ret < 0)
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