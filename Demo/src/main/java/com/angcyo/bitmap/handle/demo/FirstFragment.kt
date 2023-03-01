package com.angcyo.bitmap.handle.demo

import android.graphics.Bitmap
import android.os.Bundle
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import androidx.fragment.app.Fragment
import com.angcyo.bitmap.handle.BitmapHandle
import com.angcyo.bitmap.handle.demo.databinding.FragmentFirstBinding
import com.angcyo.library.L
import com.angcyo.library.LTime
import com.angcyo.library.ex.toBitmap

/**
 * A simple [Fragment] subclass as the default destination in the navigation.
 */
class FirstFragment : Fragment() {

    private var _binding: FragmentFirstBinding? = null

    // This property is only valid between onCreateView and
    // onDestroyView.
    private val binding get() = _binding!!

    override fun onCreateView(
        inflater: LayoutInflater, container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View? {

        _binding = FragmentFirstBinding.inflate(inflater, container, false)
        return binding.root

    }

    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        super.onViewCreated(view, savedInstanceState)

        binding.buttonFirst.setOnClickListener {
            //findNavController().navigate(R.id.action_FirstFragment_to_SecondFragment)
            //binding.textviewFirst.text = BitmapHandle.stringFromJNI()

            LTime.tick()
            //val bitmap = Bitmap.createBitmap(10000, 10000, Bitmap.Config.ARGB_8888)
            val bitmap = resources.assets.open("jpg1.jpg").readBytes().toBitmap()
            var result: Bitmap? = null
            try {
                result = BitmapHandle.toBlackWhiteHandle(bitmap)
                binding.imageView.setImageBitmap(result)
                /*binding.imageView.setImageBitmap(
                    bitmap.toBlackWhiteHandle(
                        128,
                        false,
                        Color.GRAY,
                        Color.TRANSPARENT,
                        8
                    )
                )*/
            } catch (e: Exception) {
                e.printStackTrace()
            }
            L.i("耗时:${LTime.time()} ${bitmap == result}".apply {
                binding.textviewFirst.text = this
            })
        }
    }

    override fun onDestroyView() {
        super.onDestroyView()
        _binding = null
    }
}