#include <iostream>
#include "msgQueue.hpp"

using namespace std;

int main(int argc, char **argv)
{
    cout << "Let's getting start!" << endl;
    char msgQueName[64] = {0};
    char buff[256];
    snprintf(msgQueName, sizeof(msgQueName), "/dbtMsgQue_%u", getpid());
    msgQueue myMsgQue(msgQueName, 99, true);

    while(string(buff) != "shutdown")
    {
        memset(buff, 0, sizeof(buff));
        myMsgQue.recv(buff, sizeof(buff));
        cout << "app> Recv: " << buff << endl;
        usleep(100);
    }
    
    return 0;
}
