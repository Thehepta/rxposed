package hepta.rxposed.manager.fragment

import android.os.Bundle
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import androidx.appcompat.widget.Toolbar
import androidx.fragment.app.Fragment
import hepta.rxposed.manager.MainActivity

abstract class BaseFragment : Fragment() {

    private lateinit var toolbar: Toolbar


    override fun onCreateView(
        inflater: LayoutInflater,
        container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View? {
        setHasOptionsMenu(true)
        var view = inflater.inflate(getLanytout(), container, false)
        var mainActivity = requireActivity() as MainActivity
        mainActivity.EnableToolBar()
        return view
    }

    abstract fun getLanytout(): Int

}