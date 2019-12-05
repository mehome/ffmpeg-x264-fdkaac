//
// Created by 杨将 on 2017/6/27.
//

#ifndef __SMDECODER_ERROR_H__
#define __SMDECODER_ERROR_H__

//设置返回的错误码
enum {
    //成功
    SM_DECODER_SUCCESS                              = 0,
    
    //解码部分产生的错误码
    SM_FFMPEG_DECODER_SUCESS                       = SM_DECODER_SUCCESS,
    SM_FFMPEG_DECODER_NO_MEMORY                    = 30000,
    SM_FFMPEG_DECODER_FORMAT,
    SM_FFMPEG_DECODER_STREAM,
    SM_FFMPEG_DECODER_NO_DECODER,
    SM_FFMPEG_DECODER_DECODER_OPEN,
    SM_FFMPEG_DECODER_ALREADY_INIT,
    SM_FFMPEG_DECODER_RESAMPLE,
    SM_FFMPEG_DECODER_CONTINUE,
    SM_FFMPEG_DECODER_SEEK,
    SM_FFMPEG_DECODER_END,
    
    //协议部分产生的错误码
    SM_PROTOCOL_SUCESS                       = SM_DECODER_SUCCESS,
    SM_PROTOCOL_PATH_OPEN                    = 40000,
};




#endif //__SMDECODER_ERROR_H__
