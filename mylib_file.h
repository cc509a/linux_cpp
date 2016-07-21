/********************************************************************
 @Author: cc509a
 @Created Time : 四  7/21 20:31:17 2016

 @File Name: mylib_file.h
 @Description:

*********************************************************************/
#ifndef _H_MYLIB_FILE_H_
#define _H_MYLIB_FILE_H_

namespace mylib
{
class MylibFile
{
    public:
        explicit MylibFile(const std::string& file)
            :buffer_(NULL), file_(file) {}
    private:
        char* buffer_;
        std::string file_;
};
}
