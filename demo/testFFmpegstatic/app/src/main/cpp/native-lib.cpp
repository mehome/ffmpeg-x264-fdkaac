#include <jni.h>
#include <string>
#include "FfmpegVideoDecoder.h"

extern "C" JNIEXPORT jstring JNICALL
Java_com_example_testffmpeg_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    FfmpegVideoDecoder *decoder = new FfmpegVideoDecoder();
    decoder->test();
    return env->NewStringUTF(hello.c_str());
}
