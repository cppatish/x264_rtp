#include <zmq.h>
#include <string.h>
#include "ZMQSocket.h"

void* ZMQSocket::_context=NULL;

ZMQSocket::ZMQSocket(ROLE role){
    _responder=NULL;
    _buffer=NULL;
    _role=role;
    _isInit=false;
    bufferSize=MAXBUFFERSIZE;
    //for send 
    _sendLoop=0;
    _sendLen=0;
    _sendCnt=0;
    //for receive
    _receivePTR=NULL;
    _receiveLen=0;
    _receiveCnt=0;
    _rcvmore=0;
    _sz=sizeof(_rcvmore);
}

ZMQSocket::~ZMQSocket(){
    if(_responder!=NULL){
        zmq_close(_responder);
        _responder=NULL;
    }
    if(_buffer!=NULL){
        delete _buffer;
        _buffer=NULL;
    }
}

int ZMQSocket::init(std::string host, int port, int type, int queueSize, int linger, int sendTimeOut, int receiveTimeOut){
    if(_isInit){
        return 0;
    }
    if(_context==NULL){
        _context=zmq_ctx_new();
        if(_context==NULL){
            return -1;
        }
    }
    _responder=zmq_socket(_context,type);
    if(_responder==NULL){
        return -1;
    }
    int rc=-1;
    //linger
    if(linger>=0){
        rc = zmq_setsockopt(_responder, ZMQ_LINGER, &(linger), sizeof(linger));
        if (rc != 0) {
            return -1;
        }
    }
    //queue size
    if(queueSize>0 && queueSize!=1000){
        rc = zmq_setsockopt(_responder, ZMQ_SNDHWM, &(queueSize), sizeof(queueSize));
        if (rc != 0) {
            return -1;
        }
        rc = zmq_setsockopt(_responder, ZMQ_RCVHWM, &(queueSize), sizeof(queueSize));
        if (rc != 0) {
            return -1;
        }
    }
    //send time out
    if(sendTimeOut>0){
        rc = zmq_setsockopt(_responder, ZMQ_SNDTIMEO, &(sendTimeOut), sizeof(sendTimeOut));
        if (rc != 0) {
            return -1;
        }
    }
    //receive time out
    if(receiveTimeOut>0){
        rc = zmq_setsockopt(_responder, ZMQ_RCVTIMEO, &(receiveTimeOut), sizeof(receiveTimeOut));
        if (rc != 0) {
            return -1;
        }
    }
    //connect   
    char address[512]={0};
    snprintf(address,sizeof(address),"tcp://%s:%d",host.c_str(),port);
    switch(_role){
        case SERVER:
            rc=zmq_bind(_responder, address);
            if(rc!=0){
                return -1;
            }
            break;
        case CLIENT:
            rc=zmq_connect(_responder, address);
            if(rc!=0){
                return -1;
            }
            break;
        default:
            return -1;
            break;
    }
    //init buffer
    _buffer=new char[bufferSize];
    if(_buffer==NULL){
        return -1;
    }
    memset(_buffer,0x00,bufferSize);
    //set flag
    _isInit=true;
    return 0;
}

int ZMQSocket::send(const char* buffer, int size){
    if(size<=0){
        return 0;
    }
    if(buffer==NULL){
        return -1;
    }
    _sendCnt=0;
    //计算分包数
    _sendLoop=size/MAXBUFFERSIZE;
    for(int i=0;i<_sendLoop;i++){
        _sendLen=zmq_send(_responder, buffer ,MAXBUFFERSIZE, ZMQ_SNDMORE);
        if(_sendLen<0){//failed
            return -1;
        }else{
            _sendCnt+=_sendLen;
        }
        buffer+=MAXBUFFERSIZE;
    }
    //发送剩余字节
    _sendLen=zmq_send(_responder, buffer ,size-_sendCnt, 0);
    if(_sendLen<0){
        return -1;
    }else{
        _sendCnt+=_sendLen;
    }
    return _sendCnt;
}

int ZMQSocket::receive(char* &buffer){
    //重置接收地址到起始位置
    memset(_buffer,0x00,bufferSize);
    _receivePTR=_buffer;
    _receiveCnt=0;
    while(1){
        //接收客户端消息
        _receiveLen=zmq_recv(_responder, _receivePTR ,MAXBUFFERSIZE, 0);
        if(_receiveLen<0){//failed
            return -1;
        }else{
            _receiveCnt+=_receiveLen;
        }
        //检查是否有分包
        if(zmq_getsockopt(_responder, ZMQ_RCVMORE, &_rcvmore, &_sz)<0){
            return -1;
        }
        //偏移缓存的可用地址
        _receivePTR+=_receiveLen;
        if(_rcvmore){//有分包
            //检查缓存是否溢出
            if((_receiveCnt+MAXBUFFERSIZE) >= bufferSize){
                char *tmp=new char[bufferSize*2];
                memcpy(tmp,_buffer,bufferSize);
                //修正缓存可用地址
                _receivePTR=tmp+(_receivePTR - _buffer);
                //释放旧的缓存块，替换为大的。
                delete _buffer;
                _buffer=tmp;
                tmp=NULL;
                bufferSize=bufferSize*2;
            }
        }else{//没有分包
            break;
        }
    }
    buffer = _buffer;
    return _receiveCnt;
}
    
    