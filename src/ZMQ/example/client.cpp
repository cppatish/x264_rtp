#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <time.h>
#include <vector>
#include "ZMQSocket.h"
using namespace std;

void* svc(void* ptr){
    int num=*((int*)ptr);
    ZMQSocket client(CLIENT);
    if(client.init("172.26.181.127",5230,ZMQ_PUSH)<0){
        cout<<"init socket failed!\n";
        return NULL;
    }
    char filename[20]={0};
    snprintf(filename,sizeof(filename),"./log.client.%d",num);
    ofstream fout(filename,ios::app);
    
    char str_time[64];
    memset(str_time,0x00,sizeof(str_time));
    int size=0;
    while (1) {
        time_t t=time(NULL);
        struct tm now;
        localtime_r(&t,&now);
        switch(num){
            case 1:
                sprintf(str_time,"abcdefghijklmnopqrstuvwxyz%04d-%02d-%02d-%02d-%02d-%02d",now.tm_year+1900,now.tm_mon+1,now.tm_mday,now.tm_hour,now.tm_min,now.tm_sec);
                break;
            case 2:
                sprintf(str_time,"ABCDEFGHIJKLMNOPQRSTUVWXYZ%04d|%02d|%02d|%02d|%02d|%02d",now.tm_year+1900,now.tm_mon+1,now.tm_mday,now.tm_hour,now.tm_min,now.tm_sec);
                break;
            case 3:
                sprintf(str_time,"abcdefghijklmnopqrstuvwxyz%04d/%02d/%02d/%02d/%02d/%02d",now.tm_year+1900,now.tm_mon+1,now.tm_mday,now.tm_hour,now.tm_min,now.tm_sec);
                break;
            case 4:
                sprintf(str_time,"ABCDEFGHIJKLMNOPQRSTUVWXYZ%04d\\%02d\\%02d\\%02d\\%02d\\%02d",now.tm_year+1900,now.tm_mon+1,now.tm_mday,now.tm_hour,now.tm_min,now.tm_sec);
                break;
            default:
                sprintf(str_time,"abcdefghijklmnopqrstuvwxyz%04d%02d%02d%02d%02d%02d",now.tm_year+1900,now.tm_mon+1,now.tm_mday,now.tm_hour,now.tm_min,now.tm_sec);
                break;
        }
        size=strlen(str_time);
        if(client.send(str_time,size)!=size){
            cout<<"send error !\n";
            break;
        }
        sleep(2);
    }
    fout<<"send to server end \n";
    fout.close();
    return NULL;
}

int main (){
    std::vector<pthread_t> threadVec;
    pthread_t tid;
    for(int i=0;i<5;i++){
        if(pthread_create(&tid,NULL,svc,&i)<0){
            cout<<"create thread failed!\n";
            exit(-1);
        }else{
            threadVec.push_back(tid);
        }
    }
    std::vector<pthread_t>::iterator it;
    for(it=threadVec.begin();it!=threadVec.end();it++){
        pthread_join(tid,NULL);
    }
    return 0;
}
