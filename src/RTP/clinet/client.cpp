#include <iostream>
#include "RTPSessionUtils.h"
using namespace jrtplib;

int main()
{
    int status;
    RTPSessionUtils sess;
    status = sess.CreateDefault(6666);
    if(status)
    {
        std::cout << "RTP error:" << RTPGetErrorString(status) << std::endl;
        return -1;
    }
    status = sess.AddDestination("127.0.0.1", 8888);
    if(status)
    {
        std::cout << "RTP error:" << RTPGetErrorString(status) << std::endl;
        return -1;
    }

    while (1)
    {
        std::string buf;
        std::cout << "Input send data:" ;
        std::cin >> buf;

        sess.SendPacket((void*)buf.c_str(), buf.length(), 0, false, 0);
        if(status)
        {
            std::cout << "RTP error:" << RTPGetErrorString(status) << std::endl;
            continue;
        }
    }

    return 0;
}
