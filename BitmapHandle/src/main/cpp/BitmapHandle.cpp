#include <jni.h>
#include <string>
#include "android-log.h"
#include <android/bitmap.h>

//region ---init---

extern "C" JNIEXPORT JNICALL
jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    LOGD("So初始化:%s %s", vm, reserved);
    return JNI_VERSION_1_6;
}

/**创建一个图片*/
jobject generateBitmap(JNIEnv *env, AndroidBitmapInfo info) {
    int width = info.width;
    int height = info.height;

    jclass bitmapCls = env->FindClass("android/graphics/Bitmap");
    jmethodID createBitmapFunction = env->GetStaticMethodID(bitmapCls,
                                                            "createBitmap",
                                                            "(IILandroid/graphics/Bitmap$Config;)Landroid/graphics/Bitmap;");
    jstring configName = env->NewStringUTF("ARGB_8888");
    jclass bitmapConfigClass = env->FindClass("android/graphics/Bitmap$Config");
    jmethodID valueOfBitmapConfigFunction = env->GetStaticMethodID(
            bitmapConfigClass,
            "valueOf",
            "(Ljava/lang/String;)Landroid/graphics/Bitmap$Config;");

    jobject bitmapConfig = env->CallStaticObjectMethod(bitmapConfigClass,
                                                       valueOfBitmapConfigFunction,
                                                       configName);

    jobject newBitmap = env->CallStaticObjectMethod(bitmapCls,
                                                    createBitmapFunction,
                                                    width,
                                                    height,
                                                    bitmapConfig);
    return newBitmap;
}

//endregion ---init---

//region ---method---

extern "C"
JNIEXPORT jobject JNICALL
Java_com_angcyo_bitmap_handle_BitmapHandle_toBlackWhiteHandle(JNIEnv *env, jobject thiz,
                                                              jobject bitmap,
                                                              jobject result,
                                                              jint threshold,
                                                              jboolean invert,
                                                              jint threshold_channel_color,
                                                              jint alpha_bg_color,
                                                              jint alpha_threshold) {
    //原来的图片信息
    AndroidBitmapInfo info;
    int ref;
    ref = AndroidBitmap_getInfo(env, bitmap, &info);//获取图片信息

    if (ref != 0) {
        LOGE("获取图片信息失败[error=%d]", ref);
        return nullptr;
    } else {
        //LOGE("w:%d h:%d", info.width, info.height);

        // 读取 bitmap 的像素内容到 native 内存
        void *bitmapPixels;
        if ((ref = AndroidBitmap_lockPixels(env, bitmap, &bitmapPixels)) != 0) {
            LOGE("获取图片像素失败[error=%d]", ref);
            return nullptr;
        }
        // 读取返回的 bitmap 的像素内容到 native 内存
        void *resultPixels;
        if ((ref = AndroidBitmap_lockPixels(env, result, &resultPixels)) != 0) {
            LOGE("获取图片像素失败[error=%d]", ref);
            return nullptr;
        }
        // 创建一个新的数组指针，把这个新的数组指针填充像素值
        uint32_t width = info.width;
        uint32_t height = info.height;
        int index = 0;
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                uint32_t pixel = ((uint32_t *) bitmapPixels)[index];
                //newBitmapPixels[width * y + x] = pixel;

                uint8_t alpha = (pixel >> 24) & 0xff;
                uint8_t red = (pixel >> 16) & 0xff;
                uint8_t green = (pixel >> 8) & 0xff;
                uint8_t blue = pixel & 0xff;

                if (alpha < alpha_threshold) {
                    //当前的透明值, 小于透明阈值, 则当前颜色当做全透明处理
                    pixel = alpha_bg_color;
                }

                if (pixel == 0) {
                    ((uint32_t *) resultPixels)[index] = 0;
                } else {
                    switch ((uint32_t) threshold_channel_color) {
                        case 0xffff0000: //Color.RED
                            pixel = red;
                            break;
                        case 0xFF00FF00://Color.GREEN
                            pixel = green;
                            break;
                        case 0xFF0000FF://Color.BLUE
                            pixel = blue;
                            break;
                        default://灰度
                            pixel = (red + green + blue) / 3;
                            break;
                    }
                    //std::max((uint32_t) 0, pixel);
                    //LOGW("pixel1:%d %d", pixel, threshold);
                    if (pixel >= threshold) {//大于等于阈值, 则是白色
                        //白色
                        if (invert) {
                            pixel = 0x0;
                        } else {
                            pixel = 0xff;
                        }
                    } else {
                        //黑色
                        if (invert) {
                            pixel = 0xff;
                        } else {
                            pixel = 0x0;
                        }
                    }

                    //LOGW("pixel1:%d %d %d %d", alpha << 24, pixel << 16, pixel << 8, pixel);
                    uint32_t color = (alpha << 24) | (pixel << 16) | (pixel << 8) | pixel;
                    ((uint32_t *) resultPixels)[index] = color;

                    //LOGW("pixel2:%d %d %d %d", alpha, color);
                }
                index++;
            }
        }
        AndroidBitmap_unlockPixels(env, bitmap);
        AndroidBitmap_unlockPixels(env, result);
    }
    return result;
}

//endregion ---method---
extern "C"
JNIEXPORT jobject JNICALL
Java_com_angcyo_bitmap_handle_BitmapHandle_toGrayHandle(JNIEnv *env, jobject thiz,
                                                        jobject bitmap,
                                                        jobject result,
                                                        jboolean invert,
                                                        jfloat contrast,
                                                        jfloat brightness,
                                                        jint alpha_bg_color,
                                                        jint alpha_threshold) {
//原来的图片信息
    AndroidBitmapInfo info;
    int ref;
    ref = AndroidBitmap_getInfo(env, bitmap, &info);//获取图片信息

    if (ref != 0) {
        LOGE("获取图片信息失败[error=%d]", ref);
        return nullptr;
    } else {
        //LOGE("w:%d h:%d", info.width, info.height);

        // 读取 bitmap 的像素内容到 native 内存
        void *bitmapPixels;
        if ((ref = AndroidBitmap_lockPixels(env, bitmap, &bitmapPixels)) != 0) {
            LOGE("获取图片像素失败[error=%d]", ref);
            return nullptr;
        }
        // 读取返回的 bitmap 的像素内容到 native 内存
        void *resultPixels;
        if ((ref = AndroidBitmap_lockPixels(env, result, &resultPixels)) != 0) {
            LOGE("获取图片像素失败[error=%d]", ref);
            return nullptr;
        }
        // 创建一个新的数组指针，把这个新的数组指针填充像素值
        uint32_t width = info.width;
        uint32_t height = info.height;
        int index = 0;
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                uint32_t pixel = ((uint32_t *) bitmapPixels)[index];
                //newBitmapPixels[width * y + x] = pixel;

                uint8_t alpha = (pixel >> 24) & 0xff;
                uint8_t red = (pixel >> 16) & 0xff;
                uint8_t green = (pixel >> 8) & 0xff;
                uint8_t blue = pixel & 0xff;

                if (alpha < alpha_threshold) {
                    //透明颜色, 则使用alpha_bg_color颜色代替
                    ((uint32_t *) resultPixels)[index] = alpha_bg_color;
                } else {
                    //非透明
                    uint8_t gray = (red + green + blue) / 3;//灰度值
                    gray = (contrast + 1) * gray + brightness * 255;//亮度 对比度
                    if (invert) {
                        gray = 255 - gray;
                    }
                    uint32_t color = (alpha << 24) | (gray << 16) | (gray << 8) | gray;
                    ((uint32_t *) resultPixels)[index] = color;
                }
                index++;
            }
        }
        AndroidBitmap_unlockPixels(env, bitmap);
        AndroidBitmap_unlockPixels(env, result);
    }
    return result;
}