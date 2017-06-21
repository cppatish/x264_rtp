//frameEncoder

#ifndef X264_EXAMPLE_DECODER_H
#define X264_EXAMPLE_DECODER_H

#include <inttypes.h>
#include <stdio.h>
#include <iostream>
#include <stdint.h>
#include <stdlib.h>
#include <sstream>
#include <string>
#include <cstring>
#include <fstream>
#include <boost/date_time/posix_time/posix_time.hpp>
// #include "media_player.h"

extern "C" {
#include <x264.h>
// #include <libavcodec/vdpau.h>
    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
    #include <libavutil/avutil.h>
    #include <libswscale/swscale.h>
}

namespace pt = boost::posix_time;
#define INBUF_SIZE 4096

class decoder {

private:

    int                   frame_count;
    uint8_t               inbuf[INBUF_SIZE + FF_INPUT_BUFFER_PADDING_SIZE];
    long                  recordOldTime;
    AVFrame*              frame;
    AVPacket              avpkt;
    AVCodecParserContext* parser;
    AVCodec*              codec;
    AVCodecContext*       codecContext;
    // media_player       mPlayer;

public:

    decoder();
    void initialize();
    int decodeFrame(const char *data, int length);
    void close();
};

#endif
