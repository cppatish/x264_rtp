#ifndef _ZEROMQSOCKET_H_
#define _ZEROMQSOCKET_H_
#include <zmq.h>
#include <string>

#define MAXBUFFERSIZE 256

enum ROLE{
    SERVER = 0,
    CLIENT = 1
};

class ZMQSocket{
public:
    /*
    constructure
    param:
        role : SERVER or CLIENT
    return :
        void
    */
    ZMQSocket(ROLE role);
    ~ZMQSocket();
    
    /*
    function init
    param:
        host : host IP or host name
        port : 
        type : message send or receive type (ZMQ_PUSH/ZMQ_PULL etc)
    return :
        success 0
        failed -1
    */
    int init(std::string host, int port, int type, int queueSize=1000, int linger=-1, int sendTimeOut=-1, int receiveTimeOut=-1);
    
    /*
    function send
    param:
        buffer : pointer of string buffer
        size : size of string buffer
    return :
        success >=0( byte sended )
        failed -1
    */
    int send(const char* buffer, int size);
    
    /*
    function receive
    param:
        buffer : pointer of received string
    return :
        success >=0( byte received)
        failed -1
    */
    int receive(char* &buffer);
private:
    static void *_context;
    void *_responder;
    char *_buffer;
    int bufferSize;
    ROLE _role;
    bool _isInit;
    //for send 
    int _sendLoop;
    int _sendLen;
    int _sendCnt;
    //for receive
    char *_receivePTR;
    int _receiveLen;
    int _receiveCnt;
    int _rcvmore;
    size_t _sz;
};

#endif
