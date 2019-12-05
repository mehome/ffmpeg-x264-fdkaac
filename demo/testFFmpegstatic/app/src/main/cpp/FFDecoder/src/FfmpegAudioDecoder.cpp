//
//  FfmpegAudioDecoder.cpp
//  ffmpegDecoderDemo
//
//  Created by xiaoxiong on 2018/1/20.
//  Copyright © 2018年 xiaoxiong. All rights reserved.
//

#include "FfmpegAudioDecoder.h"
#include "IProtocol.h"
#include "SMDecoderError.h"
#include "SMAudioDecodeCommon.h"
#include <sys/xattr.h>
//#define __ENCRYPT__ 1
#ifdef __cplusplus
extern "C" {
#endif
    
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswresample/swresample.h"
    
#ifdef __cplusplus
};
#endif

//ffmpeg中常用的安全删除
#ifndef FFMPEG_SAFE_FREE
#define FFMPEG_SAFE_FREE(ptr)       \
{                                   \
if(ptr)                         \
{                               \
av_freep(&ptr);             \
}                               \
}
#endif //FFMPEG_SAFE_FREE

//定于一次最大从协议中读取的数据量，参考file协议使用这个
#define FFMPEG_IO_BUFFER_SIZE    32768

//正常的协议读取回调，直接转到协议的具体函数里面去
static int ffmpeg_read_callback(void * opaque, uint8_t * buf, int buf_size)
{
    IProtocol * protocol = (IProtocol * )opaque;
    int num = protocol->read(buf, buf_size);
    if(num <= 0)
    {
        return AVERROR_EOF;
    }
    else
    {
        return num;
    }
}

//正常的协议跳转回调，直接转到协议的具体函数里面去
static int64_t ffmpeg_seek_callback(void *opaque, int64_t offset, int whence)
{
    IProtocol * protocol = (IProtocol * )opaque;
    
    if(AVSEEK_SIZE == whence)
    {
        //也可以返回文件的总长度
        return -1;
    }
    else
    {
        return protocol->seek((int)offset, whence);
    }
}

SMAudioDecoder* SMAudioDecoder::CreateObject(){
    SMAudioDecoder* pObject = new CFfmpegAudioDecoder;
    return pObject;
}
void SMAudioDecoder::DestroyObject(SMAudioDecoder** pObject)
{
    CFfmpegAudioDecoder *decoder = (CFfmpegAudioDecoder *)(*pObject);
    if (decoder) {
        decoder->uninit();
        delete decoder;
        decoder = NULL;
    }
}

CFfmpegAudioDecoder::CFfmpegAudioDecoder()
{
    m_format_context = NULL;
    m_codec_context = NULL;
    m_io_context = NULL;
    m_packet = NULL;
    m_aac_frame = NULL;
    
    m_swr_context = NULL;
    m_swr_buffer = NULL;
    m_swr_bufsize = 0;
    m_out_samplerate = 44100;
    m_out_channels = 2;
    
    m_stream_id = -1;
    
    m_final_buf = NULL;
    m_final_size = 0;
    
    m_time_base = 0;
    m_start_time = 0;
    m_pts = 0;
    
    memset(&m_media_info, 0, sizeof(SMMediaInfo));
    
    m_bInit = false;
}

CFfmpegAudioDecoder::~CFfmpegAudioDecoder()
{
    uninit();
}

int CFfmpegAudioDecoder::init(IProtocol *protocol, int samplerate, int channels)
{
    int errcode = SM_FFMPEG_DECODER_SUCESS;
    
    if(m_bInit)
    {
        return SM_FFMPEG_DECODER_ALREADY_INIT;
    }
    
    //每个具体的类内部是知道Context的实际格式的
    AVStream * audio_stream = NULL;
    AVCodec * codec = NULL;
    m_protocol = protocol;
    double stream_start_time = 0;
    
    SMFileInfo *fileInfo = protocol->getFileInfo();
    int encrypt_method = 0;
    encrypt_method = fileInfo->encrypt_method;
    
    //注册所有的信息
    av_register_all();
    
    //必须通过av_malloc函数来申请空间
    unsigned char * io_buffer = (unsigned char *)av_malloc(FFMPEG_IO_BUFFER_SIZE);
    if(NULL == io_buffer)
    {
        errcode = SM_FFMPEG_DECODER_NO_MEMORY;
        goto init_exit;
    }
    
    //输入的上下文
    m_io_context = avio_alloc_context(
                                      io_buffer,
                                      FFMPEG_IO_BUFFER_SIZE,
                                      0,
                                      protocol,
                                      ffmpeg_read_callback,
                                      NULL,
                                      ffmpeg_seek_callback
                                      );
#if defined(__ENCRYPT__)
    set_decrypt_if_need(encrypt_method);
#else
#endif
    if(NULL == m_io_context)
    {
        errcode = SM_FFMPEG_DECODER_NO_MEMORY;
        goto init_exit;
    }
    
    //格式的上下文
    m_format_context = avformat_alloc_context();
    if(NULL == m_format_context)
    {
        errcode = SM_FFMPEG_DECODER_NO_MEMORY;
        goto init_exit;
    }
    //采用自定义的IO结构体
    m_format_context->pb = m_io_context;
    m_format_context->flags = AVFMT_FLAG_CUSTOM_IO;
    
    //打开一个输入，因为是我们自定义的，因此第二个参数（文件路径）不需要了
    if(0 != avformat_open_input(&m_format_context, "", NULL, NULL))
    {
        errcode = SM_FFMPEG_DECODER_FORMAT;
        goto init_exit;
    }
    
    //查找文件的流信息
    if(avformat_find_stream_info(m_format_context, NULL) < 0)
    {
        errcode = SM_FFMPEG_DECODER_STREAM;
        goto init_exit;
    }
    //查找音频流信息
    m_stream_id = av_find_best_stream(m_format_context, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);
    if(m_stream_id < 0)
    {
        errcode = SM_FFMPEG_DECODER_STREAM;
        goto init_exit;
    }
    
    audio_stream = m_format_context->streams[m_stream_id];
    //获取时间基，方便后面计算时间
    if(audio_stream->time_base.den && audio_stream->time_base.num)
    {
        m_time_base = av_q2d(audio_stream->time_base);
    }
    else
    {
        errcode = SM_FFMPEG_DECODER_STREAM;
        goto init_exit;
    }
    //获取解码开始时间戳
    if(AV_NOPTS_VALUE != audio_stream->start_time)
    {
        stream_start_time = audio_stream->start_time * m_time_base * 1000;
    }
    //获取到解码器上下文
    m_codec_context = audio_stream->codec;
    
    //获取解码器
    codec = avcodec_find_decoder(m_codec_context->codec_id);
    if(NULL == codec)
    {
        errcode = SM_FFMPEG_DECODER_NO_DECODER;
        goto init_exit;
    }
    
    //打开解码器
    if(avcodec_open2(m_codec_context, codec, NULL) < 0)
    {
        errcode = SM_FFMPEG_DECODER_DECODER_OPEN;
        goto init_exit;
    }
    
    //申请解码过程中的编码的一帧数据以及解码的一帧数据空间
    m_packet = (AVPacket * )av_malloc(sizeof(AVPacket));
    if(NULL == m_packet)
    {
        errcode = SM_FFMPEG_DECODER_NO_MEMORY;
        goto init_exit;
    }
    m_aac_frame = av_frame_alloc();
    if(NULL == m_aac_frame)
    {
        errcode = SM_FFMPEG_DECODER_NO_MEMORY;
        goto init_exit;
    }
    
    //判断是否需要进行重采样
    m_out_samplerate = samplerate;
    m_out_channels = channels;
    if((m_out_samplerate != m_codec_context->sample_rate)
       || (m_codec_context->sample_fmt != AV_SAMPLE_FMT_FLT)
       || (m_codec_context->channels != m_out_channels))
    {
        m_swr_context = swr_alloc_set_opts(
                                           NULL,
                                           av_get_default_channel_layout(m_out_channels),
                                           AV_SAMPLE_FMT_FLT,
                                           m_out_samplerate,
                                           av_get_default_channel_layout(m_codec_context->channels),
                                           m_codec_context->sample_fmt,
                                           m_codec_context->sample_rate,
                                           0,
                                           NULL
                                           );
        if((NULL == m_swr_context) || swr_init(m_swr_context))
        {
            errcode = SM_FFMPEG_DECODER_NO_MEMORY;
            goto init_exit;
        }
    }
    
    //这样可以拿第一帧的时间戳来作为起始时间，防止audio_stream中start_time和首帧时间戳不相等导致不对齐
    errcode = decode_one_frame(true);
    if(SM_DECODER_SUCCESS != errcode)
    {
        goto init_exit;
    }
    m_start_time = m_pts;
    //然后将时间戳直接改为0，或者利用seek函数设置为0
    m_pts = 0;
    
    //获取音频流的相关信息，因为在这个时候起始时间才获得
    m_media_info.duration = audio_stream->duration * m_time_base * 1000 - m_start_time + stream_start_time;
    m_media_info.sample_rate = m_codec_context->sample_rate;
    m_media_info.channels = m_codec_context->channels;
    m_media_info.bit_rate = (int)m_codec_context->bit_rate;
    
    m_bInit = true;
    
init_exit:
    if(SM_DECODER_SUCCESS != errcode)
    {
        uninit();
    }
    return errcode;
}

void CFfmpegAudioDecoder::get_media_info(SMMediaInfo *mediaInfo)
{
    if (mediaInfo) {
        mediaInfo->duration = m_media_info.duration;
        mediaInfo->sample_rate = m_media_info.sample_rate;
        mediaInfo->channels = m_media_info.channels;
        mediaInfo->bit_rate = m_media_info.bit_rate;
    }
}

int CFfmpegAudioDecoder::decode(float *buf, int size, double *decode_postion, double *decode_duration)
{
    int errcode = SM_DECODER_SUCCESS;
    double time_stamp = -1;
    
    int buf_size = size;
    //保持给出的数据是整帧连续数据
    while(size > 0)
    {
        if(m_final_size > 0)
        {
            if(time_stamp < 0)
            {
                time_stamp = m_pts;
            }
            
            int copy_size = size > m_final_size ? m_final_size : size;
            memcpy(buf, m_final_buf, copy_size * sizeof(float));
            buf += copy_size;
            size -= copy_size;
            m_final_buf += copy_size;
            m_final_size -= copy_size;
            m_pts += (copy_size / m_out_channels * 1000.0 / m_out_samplerate);
        }
        else
        {
            //这里保证了解码结束的时候就已经退出了循环了
            errcode = decode_one_frame();
            if(SM_DECODER_SUCCESS != errcode)
            {
                //加上这句防止解码失败导致上层通过时间戳为-1，判断为解码结束的问题
                if((SM_FFMPEG_DECODER_END != errcode) && (time_stamp < 0))
                {
                    time_stamp = m_pts;
                }
                break;
            }
        }
    }
    
exit:
    //如果临时时间戳为默认值，说明开始就出错了，因此这个可以直接将时间戳赋给buffer
    //否则就说明确实为这个时间戳，顶多中途出错
    *decode_postion = time_stamp;
    //这里计算出确定的解码出来的有效时长
    *decode_duration = (buf_size - size) / m_out_channels * 1000.0 / m_out_samplerate ;
    //将后面的无效时长的数据置为空，这样算是比较准确的做法了
    if(size > 0)
    {
        memset(buf, 0, size * sizeof(float));
    }
    
    return errcode;
}

int CFfmpegAudioDecoder::seek(double pos)
{
    //先必须flush掉解码器中的部分数据，不然开头会有残留音，好坑，居然不自己内部调用
    avcodec_flush_buffers(m_codec_context);
    if(av_seek_frame(m_format_context,
                     m_stream_id,
                     int64_t((pos + m_start_time) / m_time_base / 1000),
                     AVSEEK_FLAG_BACKWARD) < 0)
    {
        return SM_FFMPEG_DECODER_SEEK;
    }
    
    //清除掉缓存在重采样中的数据
    if(m_swr_context && m_swr_buffer)
    {
        swr_convert(m_swr_context, &m_swr_buffer, m_swr_bufsize, 0, 0);
    }
    
    //将解码相关的数据重新回置一次
    m_final_size = 0;
    while(0 == m_final_size)
    {
        //解码一帧数据，然后跳转到精准位置
        int errcode = decode_one_frame(true);
        if(SM_FFMPEG_DECODER_SUCESS != errcode)
        {
            m_pts = pos;
            return errcode;
        }
        
        int skip_num = ((int)((pos - m_pts) * m_out_samplerate / 1000 + 0.5)) * m_out_channels;
        skip_num = SM_GLOBAL_MID(0, skip_num, m_final_size);
        
        m_final_buf += skip_num;
        m_final_size -= skip_num;
        m_pts += (skip_num / m_out_channels * 1000.0 / m_out_samplerate);
    }
    
    return SM_FFMPEG_DECODER_SUCESS;
}

void CFfmpegAudioDecoder::uninit()
{
    if(m_aac_frame)
    {
        av_frame_free(&m_aac_frame);
    }
    
    FFMPEG_SAFE_FREE(m_packet);
    
    if(m_codec_context)
    {
        //这里不会释放掉m_codec_context内存，使用avcodec_open2对应的释放就好
        avcodec_close(m_codec_context);
        //因为是直接获取的format流里面的codec，因此直接设置为NULL就好了
        m_codec_context = NULL;
    }
    
    //这里也是坑，没办法，io_context会自释放掉buffer，然后重新申请，其实不申请也是OK的
    unsigned char * io_buffer = NULL;
    if(m_io_context)
    {
        io_buffer = m_io_context->buffer;
    }
    FFMPEG_SAFE_FREE(io_buffer);
    
    FFMPEG_SAFE_FREE(m_io_context);
    
    if(m_format_context)
    {
        //内部会直接释放掉context，并置NULL
        avformat_close_input(&m_format_context);
    }
    
    FFMPEG_SAFE_FREE(m_swr_buffer);
    m_swr_bufsize = 0;
    m_out_samplerate = 44100;
    m_out_channels = 2;
    if(m_swr_context)
    {
        swr_free(&m_swr_context);
    }
    
    m_stream_id = -1;
    
    m_final_buf = NULL;
    m_final_size = 0;
    
    m_time_base = 0;
    m_start_time = 0;
    m_pts = 0;
    
    memset(&m_media_info, 0, sizeof(SMMediaInfo));
    
    m_bInit = false;
}

int CFfmpegAudioDecoder::decode_one_frame(bool need_update_pts)
{
    int errcode = SM_FFMPEG_DECODER_SUCESS;
    int got_frame = 0;
    int decode_len = 0;
    
    av_init_packet(m_packet);
    
    //如果读到结尾或者出错，直接等同的返回正常了
    while(true)
    {
        if(av_read_frame(m_format_context, m_packet) < 0)
        {
            errcode = SM_FFMPEG_DECODER_END;
            break;
        }
        //如果是我们想要解码的音频数据流
        if(m_packet->stream_index == m_stream_id)
        {
            //解码了多少的编码的数据
            decode_len = avcodec_decode_audio4(m_codec_context, m_aac_frame, &got_frame, m_packet);
            if(decode_len < 0)
            {
                errcode = SM_FFMPEG_DECODER_CONTINUE;
                break;
            }
            if(got_frame)
            {
                if(m_swr_context)
                {
                    //获取输出最大值得方案有两个，这里随意选一个
                    int max_output = swr_get_out_samples(m_swr_context, m_aac_frame->nb_samples);
                    //看之前申请的内存空间是否足够，不足的话，就进行申请空间
                    if(max_output > m_swr_bufsize)
                    {
                        unsigned char * buffer = NULL;
                        if(av_samples_alloc(&buffer, NULL, m_out_channels, max_output, AV_SAMPLE_FMT_FLT, 0) < 0)
                        {
                            errcode = SM_FFMPEG_DECODER_NO_MEMORY;
                            break;
                        }
                        m_swr_bufsize = max_output;
                        FFMPEG_SAFE_FREE(m_swr_buffer);
                        m_swr_buffer = buffer;
                    }
                    
                    m_final_size = swr_convert(
                                               m_swr_context,
                                               &m_swr_buffer,
                                               m_swr_bufsize,
                                               (const uint8_t **)(&(m_aac_frame->data[0])),
                                               m_aac_frame->nb_samples
                                               );
                    if(m_final_size < 0)
                    {
                        errcode = SM_FFMPEG_DECODER_RESAMPLE;
                        break;
                    }
                    m_final_buf = (float *)(m_swr_buffer);
                }
                else
                {
                    m_final_buf = (float *)(m_aac_frame->data[0]);
                    m_final_size = m_aac_frame->nb_samples;
                }
                
                //更新当前的时间戳
                if(need_update_pts)
                {
                    m_pts = av_frame_get_best_effort_timestamp(m_aac_frame) * m_time_base * 1000 - m_start_time;
                }
                else
                {
                    m_pts += m_final_size / m_out_channels / m_out_samplerate;
                }
                m_final_size = m_final_size * m_out_channels;
                break;
            }
        }
    }
    
    av_free_packet(m_packet);
    
    return errcode;
}
