#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include "ZMQSocket.h"
using namespace std;

int main (){
    ZMQSocket server(SERVER);
    if(server.init("172.26.181.127",5230,ZMQ_PULL)<0){
        cout<<"init socket failed!\n";
        return -1;
    }
    char *p=NULL;
    int size=0;
    while (1) {
        size=server.receive(p);
        //打印客户端消息
        cout<<"get "<<size<<" byte(s) : "<<p<<endl;
        cout<<"************************\n";
    }
    return 0;
}
