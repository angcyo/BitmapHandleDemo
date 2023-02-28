package com.angcyo.bitmap.handle

/**
 * Jni图片处理工具
 * @author <a href="mailto:angcyo@126.com">angcyo</a>
 * @since 2023/02/28
 */
object BitmapHandle {

    init {
        System.loadLibrary("BitmapHandle")
    }

    external fun stringFromJNI(): String

}