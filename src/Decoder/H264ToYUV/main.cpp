/*
 * =====================================================================================
 *
 *       Filename:  main.cpp
 *
 *    Description:  i
 *
 *        Version:  1.0
 *        Created:  2017年06月20日 14时43分32秒
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  guanzhixiang
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdio.h>      // FILE
#include <string.h>     // memset, memcpy
#include <stdlib.h>     // atexit
#include <sys/stat.h>   // struct stat, stat(...)
#include <unistd.h>
#include <fcntl.h>
#include "decoder.h"


int ReadFile2Buf(const char *fileName, char **outBuf, int *outLen)
{
    // check param
    if ((NULL == fileName) || (NULL == outBuf) || NULL == (outLen)) {
        printf("ErrorOccurred: invalid param!\n"); return 1;
    }

    /* open input file and read */
    FILE *fp = fopen(fileName, "rb");
    if (NULL == fp) {
        printf("Err_in_fopen: %s\n", fileName); return 1;
    }   

    struct stat stbuf;
    if(-1 == stat(fileName, &stbuf)) {
        printf("Err_in_stat\n"); return 1;
    }   

    *outLen = stbuf.st_size;
    printf("<<< fileName=%s size =%u\n", fileName, *outLen);
    *outBuf = (char *)malloc(*outLen);
    if(NULL == *outBuf) {
        printf("Err_in_malloc\n"); return 1;
    }

    memset(*outBuf, 0, *outLen);
    if(1 != fread(*outBuf, *outLen, 1, fp)) {
        printf("Err_in_fread: %s\n", fileName); return 1;
    }   

    fclose(fp);
    return 0;
}

int main()
{
    char* video_src_buf = NULL;
    int len             = 0;
    if (0 != ReadFile2Buf("../../../res/video/Ref.mp4", &video_src_buf, &len)) {
        return 1;
    }

    decoder dec;
    dec.initialize();

    printf("file_len:%d\n", len);
    int parsed_len = 0;
    while (len > 0) {
        parsed_len = dec.decodeFrame(video_src_buf, len);
        printf("len:%d, parsed_len:%d\n", len, parsed_len);
        len -= parsed_len;
        video_src_buf += parsed_len;
    }
    dec.close();

    return 0;
}
