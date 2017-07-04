#include <iostream>
#include "RTPSessionUtils.h"
using namespace jrtplib;

#if 0
int  main()
{
    int status;
    RTPSessionUtils sess;
    status = sess.CreateDefault(8888);
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
#endif

void checkerror(int rtperr)
{
	if (rtperr < 0)
	{
		std::cout << "ERROR: " << RTPGetErrorString(rtperr) << std::endl;
		exit(-1);
	}
}

int main(void)
{	
	RTPSession sess;
	uint16_t portbase = 8888;
	int status;
	bool done = false;

	RTPUDPv4TransmissionParams transparams;
	RTPSessionParams sessparams;
	sessparams.SetOwnTimestampUnit(1.0/10.0);		
	
	sessparams.SetAcceptOwnPackets(true);

	transparams.SetPortbase(portbase);
	status = sess.Create(sessparams,&transparams);	
	checkerror(status);

	sess.BeginDataAccess();
	RTPTime delay(0.001);
	RTPTime starttime = RTPTime::CurrentTime();
	
	FILE *fd;
	size_t len;
	uint8_t *loaddata;
	RTPPacket *pack;
	uint8_t* buff = new uint8_t[1024 * 1000];
	int pos = 0;

    int file_size = 0;
	
	fd = fopen("./test_recv.h264","wb+");
	while (!done)
	{
		status = sess.Poll();
		checkerror(status);
			
		if (sess.GotoFirstSourceWithData())
		{
			do
			{
				while ((pack = sess.GetNextPacket()) != NULL)
				{
					
					loaddata = pack->GetPayloadData();
					len		 = pack->GetPayloadLength();

                    if (pack->HasMarker()) {
                        printf(">> rcv data size:%d [Y]\n", len);
                    }
                    else {
                        printf(">> rcv data size:%d [N]\n", len);
                    }
					
					if(pack->GetPayloadType() == 96) //H264
					{
                        file_size += len;
                        // fwrite(loaddata, 1, len, fd);
                        // fflush(fd);
#if 0
						if(pack->HasMarker()) // the last packet
						{
							memcpy(&buff[pos],loaddata,len);	
							fwrite(buff, 1, pos + len, fd);
                            printf("<< write data size:%d\n", pos + len);
                            fflush(fd);
							pos = 0;
						}
						else
						{
							memcpy(&buff[pos],loaddata,len);
							pos = pos + len;	
						}
#endif
					}else
					{
						printf("!!!  GetPayloadType = %d !!!! \n ",pack->GetPayloadType());
					}
                    printf("Total rcv data size:%d\n", file_size);

					sess.DeletePacket(pack);
				}
                // fflush(fd);
			} while (sess.GotoNextSourceWithData());
		}
				
		RTPTime::Wait(delay);
		RTPTime t = RTPTime::CurrentTime();
		t -= starttime;
		if (t > RTPTime(40.0))
			done = true;
	}
	fclose(fd);
    delete [] buff;
	
	sess.EndDataAccess();
	delay = RTPTime(10.0);
	sess.BYEDestroy(delay,0,0);
    
	return 0;
}
