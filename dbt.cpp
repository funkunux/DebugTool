#include <iostream>
#include <stdio.h>
#include "msgQueue.hpp"

#define BUFF_SIZE 1024
using namespace std;

void showUsage(char *name)
{
    cout << "Usage: " << name << " attach PID" << endl;
}

bool isNumStr(string str)
{
    for(int i = 0; i < str.size(); i++)
    {
        if('9' < str[i] || '0' > str[i])
        {
            return false;
        }
    }
    return true;
}

int main(int argc, char** argv)
{
    char buff[BUFF_SIZE];
    char msgQueName[64];
    msgQueue *myMsgQ;

    if(3 != argc || string(argv[1]) != "attach" || !isNumStr(argv[2]))
    {
        showUsage(argv[0]);
        return 1;
    }
    snprintf(msgQueName, sizeof(msgQueName), "/dbtMsgQue_%s", argv[2]);
    try
    {
        myMsgQ = new msgQueue(msgQueName, 99);
    }
    catch(...)
    {
        cout << "Can't open msgQueue: "<< msgQueName << endl;
        return 1;
    }

    while(string(buff) != "exit")
    {
        char recv_buff[256];
        memset(buff, 0, sizeof(buff));
        cout << "dbt> " << flush;
        cin.getline(buff, sizeof(buff));
        myMsgQ->send(buff, strlen(buff));
        usleep(100);
    }
    
}
