//
// Created by xiaoxiong on 2019-12-04.
//

#ifndef TESTFFMPEG_FFMPEGVIDEODECODER_H
#define TESTFFMPEG_FFMPEGVIDEODECODER_H


#include "SMAudioDecoder.h"
#include "SMDecoderError.h"
#include "SMAudioDecodeType.h"

struct AVFormatContext;
struct AVCodecContext;
struct AVIOContext;
struct AVFrame;
struct AVPacket;
struct SwrContext;

class FfmpegVideoDecoder : public SMAudioDecoder
{
public:
    FfmpegVideoDecoder();
    ~FfmpegVideoDecoder();

public:
    virtual int     init(IProtocol * protocol,int samplerate,int channels);
    virtual void    get_media_info(SMMediaInfo *mediaInfo);
    virtual int     decode(float * buf,int size,double *decode_postion,double *decode_duration);
    virtual int     seek(double pos);
    virtual void    uninit();
    int    test();


private:
    int decode_one_frame(bool need_update_pts = false);
    IProtocol * m_protocol;
private:
    //ffmpeg解码五剑客
    AVFormatContext * m_format_context;
    AVCodecContext * m_codec_context;
    AVIOContext * m_io_context;
    AVPacket * m_packet;
    AVFrame * m_video_frame;

    //ffmpeg重采样单剑客
    SwrContext * m_swr_context;
    //重采样的buffer和长度
    unsigned char * m_swr_buffer;
    int m_swr_bufsize;
    int m_out_samplerate;
    int m_out_channels;

    //音频流的ffmpeg内部id
    int m_stream_id;

    //解码重采样之后的数据，这个指针不需要释放
    float * m_final_buf;
    //解码重采样之后的数据长度
    int m_final_size;

    //时间基准，ffmpeg内部的东西，进行时间的转换
    double m_time_base;
    //解码的开始时间戳，ms
    double m_start_time;
    //当前时间戳，为重采样之后的时间戳，ms
    double m_pts;

    //音频文件的相关信息
    SMMediaInfo m_media_info;

    //是否已经初始化
    bool m_bInit;
};
#endif //TESTFFMPEG_FFMPEGVIDEODECODER_H
