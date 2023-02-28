#include <jni.h>
#include <string>
#include "android-log.h"

extern "C" JNIEXPORT jstring JNICALL
Java_com_angcyo_bitmap_handle_BitmapHandle_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++ 123";

    LOGW("Test...");

    return env->NewStringUTF(hello.c_str());
}

//region ---init---

extern "C" JNIEXPORT JNICALL
jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    LOGD("So初始化:%s %s", vm, reserved);
    return JNI_VERSION_1_6;
}

//endregion ---init---