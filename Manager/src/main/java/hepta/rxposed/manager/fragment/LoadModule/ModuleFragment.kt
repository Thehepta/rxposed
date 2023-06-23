package hepta.rxposed.manager.fragment.LoadModule

import android.os.Bundle
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import androidx.databinding.DataBindingUtil
import androidx.fragment.app.Fragment
import androidx.navigation.NavController
import androidx.navigation.fragment.findNavController
import androidx.recyclerview.widget.LinearLayoutManager
import com.chad.library.adapter.base.listener.OnItemClickListener
import hepta.rxposed.manager.R
import hepta.rxposed.manager.databinding.FragmentModulesBinding
import hepta.rxposed.manager.util.MmkvManager

class ModuleFragment : Fragment() {


    private lateinit var binding: FragmentModulesBinding
    private var appListAdapter: ModuleListAdapter? = null

    override fun onCreateView(
        inflater: LayoutInflater,
        container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View? {
        binding = DataBindingUtil.inflate(inflater, R.layout.fragment_modules, container, false)
        return binding.root
    }


    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        super.onViewCreated(view, savedInstanceState)
        initData()
        initRecycleView()
    }

    private fun initRecycleView() {

        appListAdapter = ModuleListAdapter(R.layout.item_module)
        val layoutManager = LinearLayoutManager(requireContext())
        layoutManager.orientation = LinearLayoutManager.VERTICAL
        binding.recyclerView.layoutManager = layoutManager
        binding.recyclerView.adapter = appListAdapter
        appListAdapter!!.setList(initData())
        appListAdapter!!.setOnItemClickListener(OnItemClickListener { adapter, view, position ->

            var tmpappinfo = adapter.data.get(position) as ItemInfo
            val controller: NavController = findNavController()
            val bundle1:Bundle  =  Bundle();
            bundle1.putInt("uid",tmpappinfo.applicationInfo.uid);
            bundle1.putString("appname",tmpappinfo.appName);
            bundle1.putString("packageName",tmpappinfo.packageName);
            controller.navigate(R.id.extend_apps_dest, bundle1)
        })
    }

    private fun initData(): MutableList<ItemInfo> {
        var launcherIconPackageList = mutableListOf<ItemInfo>()
        var pKgList =  MmkvManager.getModuleList()
        var mPm = requireContext().packageManager;
        for(name in pKgList.keys){
            var pkgInfo = mPm.getApplicationInfo(name,0)
            var tmpappinfo: ItemInfo = ItemInfo(pkgInfo, mPm)
            launcherIconPackageList.add(tmpappinfo)
        }
        return launcherIconPackageList
    }

    fun updateData(){
        var launcherIconPackageList = mutableListOf<ItemInfo>()
        var pKgList =  MmkvManager.updataModuleList()
        appListAdapter!!.setList(launcherIconPackageList)
    }
}

