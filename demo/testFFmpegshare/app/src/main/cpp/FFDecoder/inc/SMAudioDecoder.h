//
//  SMAudioDecoder.h
//  ffmpegDecoderDemo
//
//  Created by xiaoxiong on 2018/1/20.
//  Copyright © 2018年 xiaoxiong. All rights reserved.
//

#ifndef SMAudioDecoder_h
#define SMAudioDecoder_h


//底层获取的音频文件的参数，文件内部的参数，如采样率之类的
typedef struct _SMMediaInfo
{
    //音频的时长 ms
    double duration;
    //音频的采样率
    int sample_rate;
    //音频的通道数
    int channels;
    //码率
    int bit_rate;
}SMMediaInfo, *pSMMediaInfo;


class IProtocol;
class SMAudioDecoder
{
public:
    static SMAudioDecoder* CreateObject();
    static void DestroyObject(SMAudioDecoder** pObject);
public:
    virtual int     init(IProtocol * protocol,int samplerate,int channels) = 0;
    virtual void    get_media_info(SMMediaInfo *mediaInfo) = 0 ;
    virtual int     decode(float * buf,int size,double *decode_postion,double *decode_duration) = 0;
    virtual int     seek(double pos) = 0;
    virtual void    uninit() = 0;
};

#endif /* SMAudioDecoder_h */
