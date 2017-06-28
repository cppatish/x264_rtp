#include "rtpsession.h"
#include "rtppacket.h"
#include "rtpudpv4transmitter.h"
#include "rtpipv4address.h"
#include "rtpsessionparams.h"
#include "rtperrors.h"
#ifndef WIN32
#include <netinet/in.h>
#include <arpa/inet.h>
#else
#include <winsock2.h>
#endif // WIN32
#include "rtpsourcedata.h"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>

//jrtplib应用需链接的lib
#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib, "jrtplib_d.lib")
#pragma comment(lib,"jthread_d.lib")

namespace jrtplib
{
    class RTPSessionUtils : public RTPSession
    {
        typedef RTPSession base_type;
    public:
        RTPSessionUtils();
        ~RTPSessionUtils();

        int AddDestination(const std::string& ip, uint16_t port);
        int DeleteDestination(const std::string& ip, uint16_t port);
        int CreateDefault(uint16_t port);
    protected:
        void OnNewSource(RTPSourceData *dat);
        void OnBYEPacket(RTPSourceData *dat);
        void OnRemoveSource(RTPSourceData *dat);
        void OnRTPPacket(RTPPacket *pack,const RTPTime &receivetime,
            const RTPAddress *senderaddress);
        void OnRTCPCompoundPacket(RTCPCompoundPacket *pack,const RTPTime &receivetime,
            const RTPAddress *senderaddress);
        void OnPollThreadStep();
    private:
        int GetAddrFromSource(RTPSourceData *dat, uint32_t& ip, uint16_t& port);
    };
}

//整形的ip转成字符串ip
static std::string IPToString(const unsigned int iIP)
{
    struct in_addr inaddr;
    inaddr.s_addr = htonl(iIP);
    return std::string(inet_ntoa(inaddr));
}

//字符串ip转成整形ip
static unsigned int IPToInt(const std::string& sIP)
{
    return inet_addr(sIP.c_str());
}
