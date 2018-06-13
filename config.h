#ifndef __DEBUGTOOL_CONFIG__
#define __DEBUGTOOL_CONFIG__

#include <stdio.h>
#include <iostream>
#include <iomanip>

#ifdef DEBUG
#define debug(format, args...) printf("[%s:%u] " format, __PRETTY_FUNCTION__, __LINE__, ##args)
#else
#define debug(format, args...) 
#endif

#define SHOWKEY(key, value) std::cout << std::setw(20) << std::setiosflags(std::ios::left) << key << value << std::endl

#endif