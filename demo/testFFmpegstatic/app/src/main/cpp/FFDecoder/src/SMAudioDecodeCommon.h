//
//  SMAudioDecodeCommon.h
//  ffmpegDecoderDemo
//
//  Created by xiaoxiong on 2018/1/22.
//  Copyright © 2018年 xiaoxiong. All rights reserved.
//

#ifndef SMAudioDecodeCommon_h
#define SMAudioDecodeCommon_h

//定义一些常用的宏或者常量等

//录制、播放、解码的缓存buffer的时间长度，单位ms
#define RECORDER_CIRCLE_BUFFER_TIME     ((int)200)

//默认写入文件的人声采样率
#define FILE_VOCAL_SAMPLERATE           ((int)44100)

//跳转在多少ms以内不需要进行跳转
#define SEEK_NO_OPRATE                  (1.01)

//文件读取的基本长度
#define FILE_OPERATE_LEN                ((int)2048)
//人声文件中进行fade的最大长度
#define FILE_FADE_LEN                   ((int)240)
//最大声道数，这个值不能改动，可以改成1
#define MAX_CHANNEL                     ((int)2)

//底层音量的中值
#define DEFAULT_VOLUME                  ((int)50)

//安全关闭文件
#ifndef SM_SAFE_CLOSE_FILE
#define SM_SAFE_CLOSE_FILE(file)       \
{                                   \
if(file)                        \
{                               \
fclose(file);               \
file = NULL;                \
}                               \
}
#endif //SAFE_CLOSE_FILE

//安全释放内存
#ifndef SM_SAFE_FREE
#define SM_SAFE_FREE(ptr)              \
{                                   \
if(ptr)                         \
{                               \
free(ptr);                  \
ptr = NULL;                 \
}                               \
}
#endif //SAFE_FREE

//安全删除对象
#ifndef SM_SAFE_DELETE_OBJ
#define SM_SAFE_DELETE_OBJ(obj)        \
{                                   \
if(obj)                         \
{                               \
delete obj;                 \
obj = NULL;                 \
}                               \
}
#endif //SAFE_DELETE_OBJ

//安全逆初始化并删除对象
#ifndef SM_SAFE_UNINIT_DELETE_OBJ
#define SM_SAFE_UNINIT_DELETE_OBJ(obj) \
{                                   \
if(obj)                         \
{                               \
obj->uninit();              \
delete obj;                 \
obj = NULL;                 \
}                               \
}
#endif //SAFE_UNINIT_DELETE_OBJ

#ifndef SM_SAFE_CLOSE_DELETE_OBJ
#define SM_SAFE_CLOSE_DELETE_OBJ(obj)  \
{                                   \
if(obj)                         \
{                               \
obj->close();               \
delete obj;                 \
obj = NULL;                 \
}                               \
}
#endif //SAFE_CLOSE_DELETE_OBJ

//安全删除数组
#ifndef SM_SAFE_DELETE_ARRAY
#define SM_SAFE_DELETE_ARRAY(array)    \
{                                   \
if(array)                       \
{                               \
delete [] array;            \
array = NULL;               \
}                               \
}
#endif //SAFE_DELETE_ARRAY

//取大值
#ifndef SM_GLOBAL_MAX
#define SM_GLOBAL_MAX(a, b)    (((a) > (b)) ? (a) : (b))
#endif

//取小值
#ifndef SM_GLOBAL_MIN
#define SM_GLOBAL_MIN(a,b)        (((a) < (b)) ? (a) : (b))
#endif

//取中间
#ifndef SM_GLOBAL_MID
#define SM_GLOBAL_MID(a, b, c)    (SM_GLOBAL_MAX(a, SM_GLOBAL_MIN(b, c)))
#endif

//取绝对值
#ifndef SM_GLOBAL_ABS
#define SM_GLOBAL_ABS(a)       ((a) < 0 ? (-(a)) : (a))
#endif

#endif /* SMAudioDecodeCommon_h */
