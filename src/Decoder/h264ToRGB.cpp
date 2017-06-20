#include <stdio.h>
extern "C"
{
    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
    #include <libavutil/avutil.h>
    #include <libswscale/swscale.h>
};

#pragma pack(push)
#pragma pack(1)

typedef unsigned char  U8;
typedef unsigned short U16;
typedef unsigned int   U32;

// bmp�ļ�ͷ
typedef struct tagBITMAPFILEHEADER{
    U16 bfType;
    U32 bfSize;
    U16 bfReserved1;
    U16 bfReserved2;
    U32 bfOffBits;
} BITMAPFILEHEADER;

// ��Ϣͷ
typedef struct tagBITMAPINFOHEADER{
    U32 biSize;
    U32 biWidth;
    U32 biHeight;
    U16 biPlanes;
    U16 biBitCount;
    U32 biCompression;
    U32 biSizeImage;
    U32 biXPelsPerMeter;
    U32 biYPelsPerMeter;
    U32 biClrUsed;
    U32 biClrImportant;
} BITMAPINFOHEADER;

#pragma pack(pop)

//����BMP�ļ��ĺ���  
void SaveAsBMP(AVFrame *pFrameRGB, int width, int height, int index, int bpp)  
{
    //char buf[5] = {0};  
    BITMAPFILEHEADER bmpheader;  
    BITMAPINFOHEADER bmpinfo;  
    FILE *fp;  
  
    char *filename = new char[255];  
  
    //�ļ����·���������Լ����޸�  
    snprintf(filename, 255, "./outRGB%d.bmp", index);  
    if( (fp = fopen(filename,"wb+")) == NULL ) {  
        printf ("open file failed!\n");  
        return;  
    }  
  
    bmpheader.bfType = 0x4d42;  
    bmpheader.bfReserved1 = 0;  
    bmpheader.bfReserved2 = 0;  
    bmpheader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);  
    // bmpheader.bfSize = bmpheader.bfOffBits + width*height*bpp/8;  
    bmpheader.bfSize = bmpheader.bfOffBits + (width*bpp+31)/32*4*height;  
    
    bmpinfo.biSize = sizeof(BITMAPINFOHEADER);  
    bmpinfo.biWidth = width;  
    bmpinfo.biHeight = height;  
    bmpinfo.biPlanes = 1;  
    bmpinfo.biBitCount = bpp;  
    //bmpinfo.biCompression = BI_RGB;  
    bmpinfo.biCompression = 0;  
    bmpinfo.biSizeImage = (width*bpp+31)/32*4*height;  
    bmpinfo.biXPelsPerMeter = 3780;  
    bmpinfo.biYPelsPerMeter = 3780;  
    bmpinfo.biClrUsed = 0;  
    bmpinfo.biClrImportant = 0;  
  
    fwrite(&bmpheader, sizeof(bmpheader), 1, fp);  
    fwrite(&bmpinfo, sizeof(bmpinfo), 1, fp);  
    //fwrite(pFrameRGB->data[0], width*height*bpp/8, 1, fp);  
    unsigned char* PTR = pFrameRGB->data[0];
    int fixSize = (width*bpp+31)/32*4 - width*bpp/8;
    unsigned char fixData = 0x00;
    for (int i = 0; i < height; ++i) {
        fwrite(PTR, width*bpp/8, 1, fp);  
        for (int j=0; j<fixSize; ++j) {
            fwrite(&fixData, 1, 1, fp); 
        }
        PTR += width*bpp/8;
    }
    
    fclose(fp);  
}

int Work_Save2BMP()  
{  
    int videoStream = -1;  
    AVCodecContext *pCodecCtx;  
    AVFormatContext *pFormatCtx;  
    AVCodec *pCodec;  
    AVFrame *pFrame, *pFrameRGB;  
    struct SwsContext *pSwsCtx;  
    const char *filename = "./out.h264";  
    AVPacket packet;  
    int frameFinished;  
    int PictureSize;  
    uint8_t *outBuff;  
  
    //ע��������  
    av_register_all();  
    // ��ʼ������ģ��  
    avformat_network_init();  
    // ����AVFormatContext  
    pFormatCtx = avformat_alloc_context();  
  
    //����Ƶ�ļ�  
    if( avformat_open_input(&pFormatCtx, filename, NULL, NULL) != 0 ) {  
        printf ("av open input file failed!\n");  
        exit (1);  
    }  
  
    //��ȡ����Ϣ  
    if( avformat_find_stream_info(pFormatCtx, NULL) < 0 ) {  
        printf ("av find stream info failed!\n");  
        exit (1);  
    }  
    //��ȡ��Ƶ��  
    for( int i = 0; i < pFormatCtx->nb_streams; i++ ) {  
        if ( pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO ) {  
            videoStream = i;  
            break;  
        }  
    }  
    if( videoStream == -1 ) {  
        printf ("find video stream failed!\n");  
        exit (1);  
    }  
  
    // Ѱ�ҽ�����  
    pCodecCtx = pFormatCtx->streams[videoStream]->codec;  
    pCodec = avcodec_find_decoder(pCodecCtx->codec_id);  
    if( pCodec == NULL ) {  
        printf ("avcode find decoder failed!\n");  
        exit (1);  
    }  
  
    //�򿪽�����  
    if( avcodec_open2(pCodecCtx, pCodec, NULL) < 0 ) {  
        printf ("avcode open failed!\n");  
        exit (1);  
    }  
  
    //Ϊÿ֡ͼ������ڴ�  
    pFrame = av_frame_alloc();  
    pFrameRGB = av_frame_alloc();  
    if( (pFrame == NULL) || (pFrameRGB == NULL) ) {  
        printf("avcodec alloc frame failed!\n");  
        exit (1);  
    }  
    
    printf("pCodecCtx->width : %d, pCodecCtx->height : %d \n", pCodecCtx->width, pCodecCtx->height);  
    // ȷ��ͼƬ�ߴ�  
    PictureSize = avpicture_get_size(AV_PIX_FMT_BGR24, pCodecCtx->width, pCodecCtx->height);  
    outBuff = (uint8_t*)av_malloc(PictureSize);  
    if( outBuff == NULL ) {  
        printf("av malloc failed!\n");  
        exit(1);  
    }  
    avpicture_fill((AVPicture *)pFrameRGB, outBuff, AV_PIX_FMT_BGR24, pCodecCtx->width, pCodecCtx->height);  
  
    //����ͼ��ת��������  
    // pSwsCtx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt,  
    //    pCodecCtx->width, pCodecCtx->height, AV_PIX_FMT_BGR24,  
    //    SWS_BICUBIC, NULL, NULL, NULL);  
    pSwsCtx = sws_getContext(pCodecCtx->width, pCodecCtx->height, AV_PIX_FMT_YUV420P,
          pCodecCtx->width, pCodecCtx->height, AV_PIX_FMT_BGR24,
          SWS_FAST_BILINEAR, NULL, NULL, NULL);
    
    int i = 0;  
    while( av_read_frame(pFormatCtx, &packet) >= 0 ) {  
        if( packet.stream_index == videoStream ) {  
            avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished, &packet);  
  
            if( frameFinished ) {  
                //��תͼ�� ���������ɵ�ͼ�������µ�����  
                pFrame->data[0] += pFrame->linesize[0] * (pCodecCtx->height - 1);  
                pFrame->linesize[0] *= -1;  
                pFrame->data[1] += pFrame->linesize[1] * (pCodecCtx->height / 2 - 1);  
                pFrame->linesize[1] *= -1;  
                pFrame->data[2] += pFrame->linesize[2] * (pCodecCtx->height / 2 - 1);  
                pFrame->linesize[2] *= -1;  
  
                //ת��ͼ���ʽ������ѹ������YUV420P��ͼ��ת��ΪBRG24��ͼ��  
                sws_scale(pSwsCtx, pFrame->data,  
                    pFrame->linesize, 0, pCodecCtx->height,  
                    pFrameRGB->data, pFrameRGB->linesize);  
  
                SaveAsBMP(pFrameRGB, pCodecCtx->width, pCodecCtx->height, i ++, 24);  
            }  
        } else {  
            int a=2;  
            int b=a;  
        }  
  
        av_free_packet(&packet);  
    }  
  
    sws_freeContext (pSwsCtx);  
    av_free (pFrame);  
    av_free (pFrameRGB);  
    avcodec_close (pCodecCtx);  
    avformat_close_input(&pFormatCtx);  
  
    return 0;  
} 


int main(){
    Work_Save2BMP();
    return 0;
}

