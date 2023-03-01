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
jobject createBitmap(JNIEnv *env, AndroidBitmapInfo info) {
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

/**创建一个图片
 * [android.graphics.Bitmap.createBitmap(android.graphics.Bitmap, int, int, int, int)]
 * */
jobject createBitmap(JNIEnv *env, jobject source, int x, int y, int width, int height) {
    jclass bitmapCls = env->FindClass("android/graphics/Bitmap");
    jmethodID createBitmapFunction = env->GetStaticMethodID(bitmapCls,
                                                            "createBitmap",
                                                            "(Landroid/graphics/Bitmap;IIII)Landroid/graphics/Bitmap;");
    jobject newBitmap = env->CallStaticObjectMethod(bitmapCls,
                                                    createBitmapFunction,
                                                    source,
                                                    x,
                                                    y,
                                                    width,
                                                    height);
    return newBitmap;
}

/**判断当前的颜色[color], 是否是需要剔除的颜色*/
jboolean isTrimColor(JNIEnv *env, uint32_t color,
                     jint alpha_threshold,
                     jint gray_threshold,
                     jintArray array) {
    uint8_t alpha = (color >> 24) & 0xff;
    uint8_t red = (color >> 16) & 0xff;
    uint8_t green = (color >> 8) & 0xff;
    uint8_t blue = color & 0xff;
    uint8_t gray = (red + green + blue) / 3;//灰度值

    if (gray >= gray_threshold || alpha <= alpha_threshold) {
        return true;
    }
    if (isInIntArray(env, color, array)) {
        return true;
    }
    return false;
}

/**当前元素是否在数组中*/
jboolean isInIntArray(JNIEnv *env, uint32_t element, jintArray array) {
    jboolean result = false;
    int length = env->GetArrayLength(array);
    jint *e = env->GetIntArrayElements(array, nullptr);
    for (int i = 0; i < length; ++i) {
        if (e[i] == element) {
            result = true;
            break;
        }
    }
    env->ReleaseIntArrayElements(array, e, JNI_ABORT);
    return result;
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

extern "C"
JNIEXPORT jobject JNICALL
Java_com_angcyo_bitmap_handle_BitmapHandle_trimEdgeColor(JNIEnv *env, jobject thiz,
                                                         jobject bitmap,
                                                         jint gray_threshold,
                                                         jint alpha_threshold,
                                                         jintArray trim_colors,
                                                         jint margin) {
    //获取图片信息
    AndroidBitmapInfo info;
    int ref;
    ref = AndroidBitmap_getInfo(env, bitmap, &info);//获取图片信息

    if (ref != 0) {
        LOGE("获取图片信息失败[error=%d]", ref);
        return nullptr;
    }

    // 读取 bitmap 的像素内容到 native 内存
    uint32_t *bitmapPixels;
    if ((ref = AndroidBitmap_lockPixels(env, bitmap, (void **) &bitmapPixels)) != 0) {
        LOGE("获取图片像素失败[error=%d]", ref);
        return nullptr;
    }

    //图片遍历的宽高
    int width = info.width;
    int height = info.height;

    //需要截取的图片位置
    int top = 0;
    int left = 0;
    int right = 0;
    int bottom = 0;

    bool interrupt = false;

    //遍历top, 横向扫描
    for (int i = 0; i < height; ++i) {
        uint32_t *pixels = &bitmapPixels[i * width];
        interrupt = false;
        top = i;
        for (int j = 0; j < width; ++j) { //列
            uint32_t color = pixels[j];//当前行列对应的颜色
            if (isTrimColor(env, color, alpha_threshold, gray_threshold, trim_colors)) {
                //需要剔除的颜色
            } else {
                interrupt = true;
                break;
            }
        }
        if (interrupt) {
            break;
        }
    }

    //遍历bottom, 横向扫描
    for (int i = height - 1; i >= 0; --i) {
        uint32_t *pixels = &bitmapPixels[i * width];
        interrupt = false;
        bottom = i;
        for (int j = 0; j < width; ++j) { //列
            uint32_t color = pixels[j];//当前行列对应的颜色
            if (isTrimColor(env, color, alpha_threshold, gray_threshold, trim_colors)) {
                //需要剔除的颜色
            } else {
                interrupt = true;
                break;
            }
        }
        if (interrupt) {
            break;
        }
    }

    //遍历left, 纵向扫描
    for (int i = 0; i < width; ++i) {//列
        interrupt = false;
        left = i;
        for (int j = 0; j < height; ++j) { //行
            uint32_t color = bitmapPixels[j * width + i];//当前行列对应的颜色
            if (isTrimColor(env, color, alpha_threshold, gray_threshold, trim_colors)) {
                //需要剔除的颜色
            } else {
                interrupt = true;
                break;
            }
        }
        if (interrupt) {
            break;
        }
    }

    //遍历right, 纵向扫描
    for (int i = width - 1; i >= 0; --i) {//列
        interrupt = false;
        right = i;
        for (int j = 0; j < height; ++j) { //行
            uint32_t color = bitmapPixels[j * width + i];//当前行列对应的颜色
            if (isTrimColor(env, color, alpha_threshold, gray_threshold, trim_colors)) {
                //需要剔除的颜色
            } else {
                interrupt = true;
                break;
            }
        }
        if (interrupt) {
            break;
        }
    }

    left = left - margin;
    top = top - margin;
    right = right + margin;
    bottom = bottom + margin;

    if (left < 0) {
        left = 0;
    }
    if (top < 0) {
        top = 0;
    }
    if (right >= width) {
        right = width - 1;
    }
    if (bottom >= height) {
        bottom = height - 1;
    }

    AndroidBitmap_unlockPixels(env, bitmap);

    int newWidth = right - left;
    int newHeight = bottom - top;
    if (newWidth <= 0 || newHeight <= 0) {
        return nullptr;
    }

    return createBitmap(env, bitmap, left, top, newWidth, newHeight);
}

//endregion ---method---
