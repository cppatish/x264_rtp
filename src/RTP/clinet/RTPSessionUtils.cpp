#include "RTPSessionUtils.h"

namespace jrtplib{
    RTPSessionUtils::RTPSessionUtils()
    {
#ifdef WIN32
        WSADATA dat;
        WSAStartup(MAKEWORD(2,2),&dat);
#endif // WIN32
    }

    RTPSessionUtils::~RTPSessionUtils()
    {
#ifdef WIN32
        WSACleanup();
#endif // WIN32
    }

    int RTPSessionUtils::CreateDefault(uint16_t port)
    {
        RTPUDPv4TransmissionParams transparams;
        RTPSessionParams sessparams;
        sessparams.SetOwnTimestampUnit(1.0/10.0);//必须设置
        transparams.SetPortbase(port);//port必须是偶数
        return base_type::Create(sessparams, &transparams);

        base_type::SetDefaultPayloadType(0);
        base_type::SetDefaultTimestampIncrement(0);
        base_type::SetDefaultMark(false);
    }

    int RTPSessionUtils::AddDestination(const std::string& ip, uint16_t port)
    {
        return base_type::AddDestination(RTPIPv4Address(ntohl(inet_addr(ip.c_str())), port));
    }

    int RTPSessionUtils::DeleteDestination(const std::string& ip, uint16_t port)
    {
        return base_type::DeleteDestination(RTPIPv4Address(ntohl(inet_addr(ip.c_str())), port));
    }

    int RTPSessionUtils::GetAddrFromSource(RTPSourceData *dat, uint32_t& ip, uint16_t& port)
    {
        if (dat->IsOwnSSRC())
            return -1;

        if (dat->GetRTPDataAddress() != 0)
        {
            const RTPIPv4Address *addr = (const RTPIPv4Address *)(dat->GetRTPDataAddress());
            ip = addr->GetIP();
            port = addr->GetPort();
        }
        else if (dat->GetRTCPDataAddress() != 0)
        {
            const RTPIPv4Address *addr = (const RTPIPv4Address *)(dat->GetRTCPDataAddress());
            ip = addr->GetIP();
            port = addr->GetPort()-1;
        }

        return 0;
    }

    void RTPSessionUtils::OnNewSource(RTPSourceData *dat)
    {
        uint32_t ip;
        uint16_t port;

        if (GetAddrFromSource(dat, ip, port))
            return;
        

        RTPIPv4Address dest(ip,port);
        base_type::AddDestination(dest);

        std::cout << "OnNewSource Adding destination " << IPToString(ip) << ":" << port << std::endl;
    }

    void RTPSessionUtils::OnRemoveSource(RTPSourceData *dat)
    {
        if (dat->ReceivedBYE())
            return;

        uint32_t ip;
        uint16_t port;

        if (GetAddrFromSource(dat, ip, port))
            return;

        RTPIPv4Address dest(ip,port);
        base_type::DeleteDestination(dest);

        std::cout << "OnRemoveSource Deleting destination " << IPToString(ip) << ":" << port << std::endl;
    }

    void RTPSessionUtils::OnBYEPacket(RTPSourceData *dat)
    {
        uint32_t ip;
        uint16_t port;

        if (GetAddrFromSource(dat, ip, port))
            return;

        RTPIPv4Address dest(ip,port);
        base_type::DeleteDestination(dest);

        std::cout << "OnBYEPacket Deleting destination " << IPToString(ip) << ":" << port << std::endl;
    }

    //只要有rtp包就会触发
    void RTPSessionUtils::OnRTPPacket(RTPPacket *pack,const RTPTime &receivetime,
        const RTPAddress *senderaddress)
    {
        std::cout << "OnRTPPacket: data:" << pack->GetPayloadData() << std::endl;
    }

    //收到rtcp包触发
    void RTPSessionUtils::OnRTCPCompoundPacket(RTCPCompoundPacket *pack,const RTPTime &receivetime,
        const RTPAddress *senderaddress)
    {
        std::cout << "OnRTCPCompoundPacket: data:" << pack->GetCompoundPacketData() << std::endl;
    }

    //隔段时间就会触发,也可以用于收包回调函数
    //void RTPSessionUtils::OnPollThreadStep()
    //{
    //  BeginDataAccess();

    //  // check incoming packets
    //  if (GotoFirstSourceWithData())
    //  {
    //      do
    //      {
    //          RTPPacket *pack;
    //          RTPSourceData *srcdat;

    //          srcdat = GetCurrentSourceInfo();

    //          while ((pack = GetNextPacket()) != NULL)
    //          {
    //              std::cout << "Got packet " << pack->GetExtendedSequenceNumber() << " from SSRC " << srcdat->GetSSRC() << std::endl;
    //              DeletePacket(pack);
    //          }
    //      } while (GotoNextSourceWithData());
    //  }

    //  EndDataAccess();
    //}
}
