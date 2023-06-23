package hepta.rxposed.manager.fragment.LoadModule

import android.content.pm.PackageManager
import android.os.Bundle
import android.util.Log
import android.view.*
import androidx.core.view.MenuHost
import androidx.core.view.MenuProvider
import androidx.databinding.DataBindingUtil
import androidx.fragment.app.Fragment
import androidx.lifecycle.Lifecycle
import androidx.navigation.NavController
import androidx.navigation.fragment.findNavController
import androidx.recyclerview.widget.LinearLayoutManager
import com.chad.library.adapter.base.listener.OnItemClickListener
import com.tencent.mmkv.MMKV
import hepta.rxposed.manager.R
import hepta.rxposed.manager.databinding.FragmentModulesBinding
import hepta.rxposed.manager.fragment.base.AppItemInfo
import hepta.rxposed.manager.util.Consts
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
        initRecycleView()

        val menuHost: MenuHost = requireActivity()
        menuHost.addMenuProvider(object : MenuProvider {
            override fun onCreateMenu(menu: Menu, menuInflater: MenuInflater) {
                menuInflater.inflate(R.menu.sync, menu)
            }

            override fun onMenuItemSelected(menuItem: MenuItem): Boolean {
                when(menuItem.itemId){
                    R.id.id_toolbar_sync->{
                        updateData()
                    }
                }
                return false
            }
        },viewLifecycleOwner, Lifecycle.State.RESUMED)



    }

    private fun initRecycleView() {

        appListAdapter = ModuleListAdapter(R.layout.item_module)
        val layoutManager = LinearLayoutManager(requireContext())
        layoutManager.orientation = LinearLayoutManager.VERTICAL
        binding.recyclerView.layoutManager = layoutManager
        binding.recyclerView.adapter = appListAdapter
        appListAdapter!!.setList(initData())
        appListAdapter!!.setOnItemClickListener(OnItemClickListener { adapter, view, position ->

            var tmpappinfo = adapter.data.get(position) as AppItemInfo
            val controller: NavController = findNavController()
            val bundle1:Bundle  =  Bundle();
            bundle1.putInt("uid",tmpappinfo.applicationInfo.uid);
            bundle1.putString("appname",tmpappinfo.appName);
            bundle1.putString("packageName",tmpappinfo.packageName);
            controller.navigate(R.id.extend_apps_dest, bundle1)
        })
    }

    private fun initData(): MutableList<AppItemInfo> {
        var PackageList = mutableListOf<AppItemInfo>()
        var pKgList =  MmkvManager.getModuleList()
        var mPm = requireContext().packageManager;
        for(name in pKgList.keys){
            try {
                var pkgInfo = mPm.getApplicationInfo(name,0)
                var tmpappinfo: AppItemInfo = AppItemInfo(pkgInfo, mPm)
                PackageList.add(tmpappinfo)
            }catch (e: PackageManager.NameNotFoundException){
                Log.e("Rzx",name+": not found")
                continue
            }
        }
        return PackageList
    }

    fun updateData(){
        MmkvManager.updataModuleList()
        appListAdapter!!.setList(initData())
        appListAdapter!!.notifyDataSetChanged()
    }
}

