#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <x264.h>
extern "C"{
    #include <libswscale/swscale.h>
    #include <libavutil/pixfmt.h>
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

struct SwsContext* convertCtx = NULL;
x264_t* encoder = NULL;
U32 width, height = 0; 
x264_picture_t pic_in, pic_out;

U8* GetBmpData(U8 *bitCountPerPix, U32 *width, U32 *height, const char* filename) { 
    FILE *pf = fopen(filename, "rb"); 
    if(!pf) { 
        printf("fopen failed : %s, %d\n", __FILE__, __LINE__); 
        return NULL; 
    }
    BITMAPFILEHEADER fileHeader;
    BITMAPINFOHEADER infoHeader;
    fread(&fileHeader, sizeof(BITMAPFILEHEADER), 1, pf); 
    fread(&infoHeader, sizeof(BITMAPINFOHEADER), 1, pf); 
    printf("size : %ld, %ld\n",sizeof(BITMAPFILEHEADER), sizeof(BITMAPINFOHEADER)); 
    if(infoHeader.biBitCount!=24 && infoHeader.biBitCount!=32){  // Ŀǰֻ֧��24λ��32λ
        fprintf( stderr, "unsupport bitCountPerPix %d \n", infoHeader.biBitCount );
        return NULL;
    } 
    if(infoHeader.biWidth%2 != 0 || infoHeader.biHeight%2 != 0){  //  ���صĳ��Ϳ���ҪΪ2�ı���(����������Ҫ��)
        fprintf( stderr, "unsupport width or height not divisible by 2 (%dx%d) \n", infoHeader.biWidth, infoHeader.biHeight);
        return NULL;
    }
    if(bitCountPerPix) { 
        *bitCountPerPix = infoHeader.biBitCount; 
    } 
    if(width) { 
        *width = infoHeader.biWidth; 
    } 
    if(height) { 
        *height = infoHeader.biHeight; 
    } 
    fprintf( stderr, "GetBmpData bitCountPerPix %d \n", *bitCountPerPix );
    U32 imgLineSize = (((*width)*(*bitCountPerPix) + 31) >> 5) << 2;  // ͼƬÿ�е��ֽ�
    U8 *pEachLinBuf = (U8*)malloc(imgLineSize); 
    memset(pEachLinBuf, 0, imgLineSize); 
    U32 buffLineSize = (((*width)*24 + 31) >> 5) << 2;   // ���buffer��ÿ���ֽ�
    U8 *pdata = (U8*)malloc((*height)*buffLineSize); 
    memset(pdata, 0, (*height)*buffLineSize); 
    U8 *bufPoniter = NULL;
    //U8 BytePerPix = (*bitCountPerPix) >> 3; // 4
    U8 BytePerPix = 3;         //  ���buffer��ÿ�����ص�ռ�õ��ֽ�
    if(pdata && pEachLinBuf) { 
        int w, h, j; 
        for(h = (*height) - 1; h >= 0; --h) { // bmp�ļ�ÿ���ǵ��ŷŵ�
            fread(pEachLinBuf, imgLineSize, 1, pf); 
            bufPoniter = pdata + (*width)*h*BytePerPix ;
            for(w = 0 ; w < (int)(*width); ++w) { 
                for(j=0; j<BytePerPix; ++j){
                    if((*bitCountPerPix)==32){  // 32λʱ��ԭͼ��һ������ռ��4���ֽ�
                        bufPoniter[w*BytePerPix + j ] = pEachLinBuf[w*4 + j ]; 
                    }else{
                        bufPoniter[w*BytePerPix + j ] = pEachLinBuf[w*BytePerPix + j ]; 
                    }
                }
            }
        }
        free(pEachLinBuf);
    }
    fclose(pf); 
    return pdata; 
} 

void FreeBmpData(U8 *pdata) { 
    if(pdata) { 
        free(pdata); 
        pdata = NULL; 
    } 
}

void Init_Encoder(U32 width, U32 height)
{
    x264_param_t param;
    x264_param_default_preset(&param, "veryfast", "zerolatency");
    // ���ò���
    param.i_width = width;
    param.i_height = height;
    param.i_threads  = X264_SYNC_LOOKAHEAD_AUTO;//* ȡ�ջ���������ʹ�ò������ı�֤.  
    param.i_frame_total = 0; //* ������֡��.��֪����0.  
    param.i_keyint_max = 10;   
    //* ������  
    // param.i_bframe  = 5;  
    param.b_open_gop  = 0;  
    // param.i_bframe_pyramid = 0;  
    // param.i_bframe_adaptive = X264_B_ADAPT_TRELLIS;  
    //* Log����������Ҫ��ӡ������Ϣʱֱ��ע�͵�����  
    param.i_log_level  = X264_LOG_DEBUG;  
    //* ���ʿ��Ʋ���  
    param.rc.i_bitrate = 1024 * 10;//* ����(������,��λKbps)  
    //* muxing parameters  
    param.i_fps_den  = 1; //* ֡�ʷ�ĸ  
    param.i_fps_num  = 10;//* ֡�ʷ���  
    param.i_timebase_den = param.i_fps_num;  
    param.i_timebase_num = param.i_fps_den;  
    // ִ�����ò���
    x264_param_apply_profile(&param, "baseline");
    // ������
    encoder = x264_encoder_open(&param);
    x264_picture_alloc(&pic_in, X264_CSP_I420, width, height);
    pic_in.img.i_csp = X264_CSP_I420;  
    pic_in.img.i_plane = 3;
    // ת������
    convertCtx = sws_getContext(width, height, AV_PIX_FMT_BGR24, width, height, AV_PIX_FMT_YUV420P, SWS_FAST_BILINEAR, NULL, NULL, NULL);
    /*
    switch(bitCountPerPix){
        case 32:
            fprintf( stderr, "32 bit image\n" );
            convertCtx = sws_getContext(width, height, AV_PIX_FMT_0RGB, width, height, AV_PIX_FMT_YUV420P, SWS_FAST_BILINEAR, NULL, NULL, NULL);
            break;
        case 24:
            fprintf( stderr, "24 bit image\n" );
            //AV_PIX_FMT_BGR24 AV_PIX_FMT_RGB24
            convertCtx = sws_getContext(width, height, AV_PIX_FMT_BGR24, width, height, AV_PIX_FMT_YUV420P, SWS_FAST_BILINEAR, NULL, NULL, NULL);
            break;
        default:
            convertCtx = sws_getContext(width, height, AV_PIX_FMT_RGB24, width, height, AV_PIX_FMT_YUV420P, SWS_FAST_BILINEAR, NULL, NULL, NULL);
            break;
    }
    */
}

void DestroyEncoder()
{
    x264_encoder_close( encoder );
    x264_picture_clean( &pic_in );
}

int rgbToH264(U8 *pdata){
    if (NULL == convertCtx) {
        return 0;
    }

    fprintf( stderr, "begin convert\n" );
    int srcstride = width*3;
    sws_scale(convertCtx, &pdata, &srcstride, 0, height, pic_in.img.plane, pic_in.img.i_stride);
    // h264����ļ�
    FILE* fout=fopen("out.mp4","a+");
    if(fout==NULL){
        fprintf( stderr, "open out.h264 failed\n" );
        return -1;
    }
    // h264����
    x264_nal_t* nals;
    int i_nals;
    int frame_size = x264_encoder_encode(encoder, &nals, &i_nals, &pic_in, &pic_out);
    // ���h264��
    if (frame_size >= 0){
        fwrite( nals->p_payload, frame_size, 1, fout );
    }
    
    fclose(fout);
    return 0;
}

int main(int argc, char *argv[]) {
    // if( argc != 2){ 
    //     fprintf( stderr,"Example usage: example xxxx.bmp\n");
    //     return -1; 
    // };
    U8 bitCountPerPix; 
    const char* file_fat = "../../res/pic/1280x720-%d.bmp";
    char file_name[20];
    for (int i = 0; i < 30; ++i) {
        memset(file_name, 0, sizeof(file_name));
        sprintf(file_name, file_fat, (i % 9) + 1);
        U32 _width, _height = 0; 
        U8 *pdata = GetBmpData(&bitCountPerPix, &_width, &_height, file_name); 
        if (NULL == pdata) continue;
        if (width != _width || height != _height) {
            width = _width;
            height = _height;
            Init_Encoder(width, height);
        }

        if(pdata){
            // rgbToH264(pdata, width, height);
            rgbToH264(pdata);
            FreeBmpData(pdata); 
        }
    }

    DestroyEncoder();

    return 0;
}
