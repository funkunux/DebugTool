#ifndef __SIMPLE_EXCEPTION__
#define __SIMPLE_EXCEPTION__

#include <exception>
#include <iostream>

class SimpleExcept:public std::exception
{
public:
    std::string _errMsg;
    SimpleExcept(const std::string errMsg)
    {
        _errMsg = errMsg;
    }
    const char *what()
    {
        return _errMsg.c_str();
    }
    ~SimpleExcept() throw() {}
};

#endif