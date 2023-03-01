# BitmapHandleDemo

使用纯`jni`处理`Bitmap`, 速度会比直接在`java`层快`20倍`
图片越大,速度差值越大,越明显.

> so大小≈5kb

## 使用方式

1. Add it in your root build.gradle at the end of repositories:

```gradle
allprojects {
    repositories {
        ...
        maven { url 'https://jitpack.io' }
    }
}
```

2. Add the dependency

```gradle
dependencies {
    implementation 'com.github.angcyo:BitmapHandleDemo:0.2'
}
```

## BitmapHandle.toBlackWhiteHandle

将图片转成黑白

```kotlin
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
fun toBlackWhiteHandle(
    bitmap: Bitmap,
    result: Bitmap = Bitmap.createBitmap(bitmap.width, bitmap.height, bitmap.config),
    threshold: Int = 128,
    invert: Boolean = false,
    thresholdChannelColor: Int = Color.GRAY,
    alphaBgColor: Int = Color.TRANSPARENT,
    alphaThreshold: Int = 8
): Bitmap?
```

## BitmapHandle.toGrayHandle

将图片转成灰度

```kotlin
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
fun toGrayHandle(
    bitmap: Bitmap,
    result: Bitmap = Bitmap.createBitmap(bitmap.width, bitmap.height, bitmap.config),
    invert: Boolean = false,
    contrast: Float = 0f,
    brightness: Float = 0f,
    alphaBgColor: Int = Color.TRANSPARENT,
    alphaThreshold: Int = 8
): Bitmap?
```

## BitmapHandle.trimEdgeColor

将图片4个边的颜色裁掉

```kotlin
/**
 * 逐行扫描 清除边界空白, 裁边之前建议黑白画处理
 *
 * [alphaThreshold] 透明值小于等于这个值时, 剔除
 * [grayThreshold]  灰度值大于等于这个值时, 剔除
 * [trimColors] 额外需要移除的边界颜色值集合
 * [margin] 边距留多少个像素, 大于0有效
 * @return 清除边界后的Bitmap
 */
fun trimEdgeColor(
    bitmap: Bitmap,
    grayThreshold: Int = 250,
    alphaThreshold: Int = 8,
    trimColors: IntArray = intArrayOf(Color.TRANSPARENT),
    margin: Int = 0
): Bitmap?
```
