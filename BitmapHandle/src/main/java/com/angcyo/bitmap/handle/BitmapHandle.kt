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

    /**将图片转黑白
     * [bitmap] 需要处理的图片
     * [result] 返回值
     * [threshold] 阈值, [0~255] [黑色~白色] 大于这个值的都是白色
     * [invert] 反色, 是否要将黑白颜色颠倒
     * [alphaBgColor] 透明像素时的替换颜色
     * [thresholdChannelColor] 颜色阈值判断的通道支持[Color.RED] [Color.GREEN] [Color.BLUE] [Color.GRAY]
     * [alphaBgColor] 如果是透明颜色, 则使用此值替代
     * [alphaThreshold] 透明颜色的阈值, 当颜色的透明值小于此值时, 视为透明
     *
     * @return 返回null表示处理失败
     * */
    external fun toBlackWhiteHandle(
        bitmap: Bitmap,
        result: Bitmap = Bitmap.createBitmap(bitmap.width, bitmap.height, bitmap.config),
        threshold: Int = 128,
        invert: Boolean = false,
        thresholdChannelColor: Int = Color.GRAY,
        alphaBgColor: Int = Color.TRANSPARENT,
        alphaThreshold: Int = 8
    ): Bitmap?

    /**将图片转灰度
     * [bitmap] 需要处理的图片
     * [result] 返回值
     * [invert] 是否反色
     * [contrast] 对比度 [-1~1]
     * [brightness] 亮度 [-1~1]
     * [alphaBgColor] 如果是透明颜色, 则使用此值替代
     * [alphaThreshold] 透明颜色的阈值, 当颜色的透明值小于此值时, 视为透明
     * @return 返回null表示处理失败
     * */
    external fun toGrayHandle(
        bitmap: Bitmap,
        result: Bitmap = Bitmap.createBitmap(bitmap.width, bitmap.height, bitmap.config),
        invert: Boolean = false,
        contrast: Float = 0f,
        brightness: Float = 0f,
        alphaBgColor: Int = Color.TRANSPARENT,
        alphaThreshold: Int = 8
    ): Bitmap?
}