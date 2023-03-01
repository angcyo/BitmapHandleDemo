package com.angcyo.bitmap.handle.demo

import android.graphics.Bitmap
import android.os.Bundle
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import androidx.fragment.app.Fragment
import com.angcyo.bitmap.handle.BitmapHandle
import com.angcyo.bitmap.handle.demo.databinding.FragmentFirstBinding
import com.angcyo.component.getPhoto
import com.angcyo.library.L
import com.angcyo.library.LTime
import com.angcyo.library.ex.fileSizeString

/**
 * A simple [Fragment] subclass as the default destination in the navigation.
 */
class FirstFragment : Fragment() {

    private var _binding: FragmentFirstBinding? = null

    // This property is only valid between onCreateView and
    // onDestroyView.
    private val binding get() = _binding!!

    private var targetBitmap: Bitmap? = null

    val bitmapInfo: String
        get() = buildString {
            targetBitmap?.let { bitmap ->
                append("图片宽:${bitmap.width} ")
                append("高:${bitmap.height} ")
                append("占用内存:")
                append(bitmap.allocationByteCount.toLong().fileSizeString())
            }
        }

    override fun onCreateView(
        inflater: LayoutInflater, container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View? {
        _binding = FragmentFirstBinding.inflate(inflater, container, false)
        return binding.root
    }

    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        super.onViewCreated(view, savedInstanceState)

        binding.imageView.setOnClickListener {
            getPhoto { bitmap ->
                binding.imageView.setImageBitmap(bitmap)
                targetBitmap = bitmap
                binding.textviewFirst.text = bitmapInfo
            }
        }

        binding.bwButton.setOnClickListener {
            wrapAction { bitmap ->
                BitmapHandle.toBlackWhiteHandle(bitmap)
            }
        }

        binding.grayButton.setOnClickListener {
            wrapAction { bitmap ->
                BitmapHandle.toGrayHandle(bitmap)
            }
        }

        binding.trimButton.setOnClickListener {
            wrapAction { bitmap ->
                BitmapHandle.trimEdgeColor(bitmap)
            }
        }
    }

    override fun onDestroyView() {
        super.onDestroyView()
        _binding = null
    }

    fun wrapAction(action: (Bitmap) -> Bitmap?) {
        LTime.tick()
        val bitmap = targetBitmap ?: return
        var result: Bitmap? = null
        try {
            result = action(bitmap)
            binding.imageView.setImageBitmap(result)
        } catch (e: Exception) {
            e.printStackTrace()
        }
        L.i("${bitmapInfo}\n耗时:${LTime.time()} ${bitmap == result}".apply {
            binding.textviewFirst.text = this
        })
    }
}