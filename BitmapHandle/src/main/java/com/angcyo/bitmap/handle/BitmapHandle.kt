package com.angcyo.bitmap.handle

import android.graphics.Bitmap
import android.graphics.Color

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

    /**将图片转黑白
     * [threshold] 阈值, [0~255] [黑色~白色] 大于这个值的都是白色
     * [invert] 反色, 是否要将黑白颜色颠倒
     * [alphaBgColor] 透明像素时的替换颜色
     * [thresholdChannelColor] 颜色阈值判断的通道支持[Color.RED] [Color.GREEN] [Color.BLUE] [Color.GRAY]
     * [alphaThreshold] 透明颜色的阈值, 当颜色的透明值小于此值时, 视为透明
     * [alphaBgColor] 如果是透明颜色, 则使用此值替代
     * */
    external fun toBlackWhiteHandle(
        bitmap: Bitmap,
        threshold: Int = 128,
        invert: Boolean = false,
        thresholdChannelColor: Int = Color.GRAY,
        alphaBgColor: Int = Color.TRANSPARENT,
        alphaThreshold: Int = 8
    ): Bitmap?
}