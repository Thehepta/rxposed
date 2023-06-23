package hepta.rxposed.manager.fragment.base

import android.os.Bundle
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import androidx.databinding.DataBindingUtil
import androidx.fragment.app.Fragment
import hepta.rxposed.manager.MainActivity
import hepta.rxposed.manager.R
import hepta.rxposed.manager.databinding.FragmentApplistBinding

abstract class baseCollToolbarFragment : Fragment() {


    companion object{
        var moduleInfo: ModuleInfo? = null

        init {
        }
    }

    lateinit var binding: FragmentApplistBinding

    override fun onCreateView(
        inflater: LayoutInflater,
        container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View? {
        binding = FragmentApplistBinding.inflate(getLayoutInflater())
        var mainActivity = requireActivity() as MainActivity
        mainActivity.DisableToolBar();  //主动调用activity的方法，隐藏toolbar
//        baseCollToolbarFragment.moduleInfo = getModuleInfo()
        return binding.root
    }

    //    abstract fun getModuleInfo(): ModuleInfo;
    override fun onDestroy() {
        super.onDestroy()
        var mainActivity = requireActivity() as MainActivity
        mainActivity.EnableToolBar()
    }


}