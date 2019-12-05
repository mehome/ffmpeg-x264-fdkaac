#include <jni.h>
#include <string>
#include "FfmpegVideoDecoder.h"
#include "FileProtocol.h"
#include "FfmpegAudioDecoder.h"
#include <android/log.h>

#define LOGE(TAG, ...) __android_log_print(ANDROID_LOG_ERROR  , TAG, __VA_ARGS__)


extern "C" JNIEXPORT jstring JNICALL
Java_com_example_testffmpeg_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";

    FfmpegVideoDecoder *videoDecoder = new FfmpegVideoDecoder();
    videoDecoder->test();

//    CFileProtocol *protocol = new CFileProtocol();
//    protocol->open("/sdcard/Alan/record/qwe.mp4", nullptr);
//    int code = videoDecoder->init(protocol,0,0);
//    LOGE("native-lib   ","init  code: %d\n",code);
//    SMMediaInfo *mediaInfo = new SMMediaInfo;
//    videoDecoder->get_media_info(mediaInfo);
//    mediaInfo->sample_rate;
//
//    LOGE("native-lib   ","duration: %f\n",mediaInfo->duration);
//    LOGE("native-lib   ","bit_rate: %d\n",mediaInfo->bit_rate);
//    LOGE("native-lib   ","channels: %d\n",mediaInfo->channels);
//    LOGE("native-lib   ","sample_rate: %d\n",mediaInfo->sample_rate);

//    CFileProtocol *protocol = new CFileProtocol();
//    protocol->open("/sdcard/Alan/record/piano.m4a", nullptr);
//    CFfmpegAudioDecoder *audioDecoder = new CFfmpegAudioDecoder();
//    audioDecoder->init(protocol,44100,1);
//    SMMediaInfo *mediaInfo = new SMMediaInfo;
//    audioDecoder->get_media_info(mediaInfo);
//    mediaInfo->sample_rate;
//
//    LOGE("native-lib   ","duration: %f\n",mediaInfo->duration);
//    LOGE("native-lib   ","bit_rate: %d\n",mediaInfo->bit_rate);
//    LOGE("native-lib   ","channels: %d\n",mediaInfo->channels);
//    LOGE("native-lib   ","sample_rate: %d\n",mediaInfo->sample_rate);

    return env->NewStringUTF(hello.c_str());
}
