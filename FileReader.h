#ifndef __FILE_READER__
#define __FILE_READER__

#include <iostream>
#include <fstream>
#include "SimpleExcept.h"
#include "config.h"

class FileReader
{
public:
    FileReader(){}
    ~FileReader()
    {
        if(_infile->is_open())
        {
            debug("CLOSING...\n");
            _infile->close();
        }
    }
    FileReader(const char* filePath)
    {
        _infile = new std::ifstream(filePath);
        if(!_infile->is_open())
        {
            perror("Open File Error");
            throw SimpleExcept("open error!");
        }
        debug("_infile:%x\n", _infile);
    }

    void open(const char* filePath)
    {
        std::ifstream infile(filePath);
        if(!infile.is_open())
        {
            perror("Open File Error");
            throw SimpleExcept("open error!");
        }
    }

    int read(char* buff, int len, std::streamoff offset = 0, std::ios_base::seekdir from = std::ios::beg)
    {
        int readSize;
        _infile->seekg(offset, from);
        _infile->read(buff, len);
        readSize = _infile->gcount();
        if(_infile->eof())
        {
            debug("Touch the end of file!\n");
        }
        debug("buff:%x, off:%lld, len:%d readSize:%d\n", buff, offset, len, readSize);
        return readSize;
    }

public:
    std::ifstream *_infile;
};

#endif