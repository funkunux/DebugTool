#ifndef __MSG_QUEUE__
#define __MSG_QUEUE__

#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <mqueue.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#ifdef DEBUG
#define debug(format, args...) printf("[%s:%u] " format, __FUNCTION__, __LINE__, ##args)
#else
#define debug(format, args...) 
#endif

#define MQ_OFLAG O_RDWR

class CheckParam
{
    class Nullpointer{};
public:
    static void NullPointer(void *pointer)
    {
        if(NULL == pointer)
        {
            throw Nullpointer();
        }
    }
};

class msgQueue
{
public:
    class MqNameToLong{};
    class CantOpenMsgQue{};
    class CantCloseMsgQue{};
    class UnlinkFailed{};
    class GetMqAttrFailed{};
    class MsgLenError{};
    msgQueue(const char* msgQueName, unsigned char prio, bool isCreate = false)
    {
        int o_flag = MQ_OFLAG;
        CheckParam::NullPointer((void *)msgQueName);
        
        unsigned char mqNameLen = strnlen(msgQueName, 128);
        if(sizeof(mq_name) < mqNameLen || 0 == mqNameLen)
        {
            throw MsgLenError();
        }
        strncpy(mq_name, msgQueName, sizeof(mq_name) - 1);
        mq_name[sizeof(mq_name) - 1] = 0;

        if(isCreate)
        {
            o_flag |= O_EXCL|O_CREAT;
            if(-1 == mq_unlink(mq_name) && ENOENT != errno)
            {
                debug("mq_unlink(\"%s\"): %m\n", mq_name);
                throw UnlinkFailed();
            }
            else if(0 == errno)
            {
                printf("Warnning: %s is already exist! Unlink now!\n", mq_name);
            }
        }

        mq_id = mq_open(mq_name, o_flag, S_IRWXU, NULL);
        if(0 > (int)mq_id)
        {
            debug("mq_open(%s)\n", mq_name);
            throw CantOpenMsgQue();
        }

        mq_attr *msgQ_attr = new mq_attr;
        if(-1 == mq_getattr(mq_id, msgQ_attr))
        {
            debug("mq_getattr(%lu): %m\n", mq_id);
            this->~msgQueue();
            throw GetMqAttrFailed();
        }
        mq_maxmsg = msgQ_attr->mq_maxmsg;
        mq_maxsize = msgQ_attr->mq_msgsize;
        mq_prio = prio;
        _isCreate = isCreate;
    }
    ~msgQueue()
    {
        //debug("exit\n");
        if(-1 == mq_close(mq_id))
        {
            debug("mq_close(%lu): %m\n", mq_id);
            throw CantCloseMsgQue();
        }
        if(_isCreate)
        {
            if(-1 == mq_unlink(mq_name))
            {
                debug("mq_unlink(%s): %m\n", mq_name);
                throw UnlinkFailed();
            }
        }
    }

    void send(const char* buff, unsigned long len)
    {
        CheckParam::NullPointer((void *)buff);
        if(len > mq_maxsize)
        {
            throw MsgLenError();
        }
        if(-1 == mq_send(mq_id, buff, len, mq_prio))
        {
            debug("Failed to send: %m\n");
            return;
        }
    }

    void recv(char *container, unsigned long len)
    {   
        CheckParam::NullPointer(container);

        char *buff = new char[mq_maxsize];
        CheckParam::NullPointer(buff);
        memset(buff, 0, mq_maxsize);
        if(-1 == mq_receive(mq_id, buff, mq_maxsize, NULL))
        {
            debug("Failed to recv: %m\n");
        }
        else
        {
            memcpy(container, buff, len);
        }

        delete [] buff;
    }

private:
    char mq_name[64];
    unsigned long mq_maxsize;
    unsigned long mq_maxmsg;
    unsigned long mq_id;
    unsigned char mq_prio;
    bool _isCreate;
};

#endif