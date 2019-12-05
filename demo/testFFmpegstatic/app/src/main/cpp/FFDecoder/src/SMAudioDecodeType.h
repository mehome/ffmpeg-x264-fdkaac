//
//  SMAudioDecodeType.h
//  ffmpegDecoderDemo
//
//  Created by xiaoxiong on 2018/1/22.
//  Copyright © 2018年 xiaoxiong. All rights reserved.
//

#ifndef SMAudioDecodeType_h
#define SMAudioDecodeType_h
#include "SMDecoderError.h"
#include <stdio.h>

//自定义的一帧音频数据
template <class T>
class SMAudioFrameBuffer
{
public:
    SMAudioFrameBuffer()
    : m_buffer(NULL)
    , m_size(0)
    , m_postion(0)
    , m_duration(0) { }
    
    ~SMAudioFrameBuffer() { uninit(); }
    
public:
    inline int init(int size) { m_buffer = new T [size]; m_size = size; return SM_DECODER_SUCCESS; }
    inline T * get_buffer() { return m_buffer; }
    inline int get_size() { return m_size; }
    inline void set_postion(double postion) { m_postion = postion; }
    inline double get_postion() { return m_postion; }
    inline void set_duration(double duration) { m_duration = duration; }
    inline double get_duration() { return m_duration; }
    inline void uninit() {if(m_buffer){delete [] m_buffer;m_buffer = NULL;}; m_size = 0; m_postion = 0; m_duration = 0; }
    
private:
    //音频数据
    T * m_buffer;
    //音频数据长度
    int m_size;
    //音频数据的开始时间点，ms
    double m_postion;
    //音频的持续时间，ms
    double m_duration;
};
#endif /* SMAudioDecodeType_h */
