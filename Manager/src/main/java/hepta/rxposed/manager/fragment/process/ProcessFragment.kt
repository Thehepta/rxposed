/*
 * Copyright (C) 2018 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package hepta.rxposed.manager.fragment.process

import android.content.pm.PackageManager
import android.graphics.drawable.Drawable
import android.os.Bundle
import android.view.*
import android.widget.Toast
import androidx.core.view.MenuHost
import androidx.core.view.MenuProvider
import androidx.fragment.app.Fragment
import androidx.lifecycle.Lifecycle
import androidx.navigation.NavController
import androidx.navigation.fragment.findNavController
import androidx.recyclerview.widget.LinearLayoutManager
import androidx.recyclerview.widget.RecyclerView
import hepta.rxposed.manager.R
import hepta.rxposed.manager.RxposedApp
import hepta.rxposed.manager.util.InjectTool
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch
import kotlinx.coroutines.withContext
import java.math.BigInteger

/**
 * Fragment used to show how to deep link to a destination
 */
class ProcessFragment : Fragment() {



    var processListAdapter :ProcessListAdapter? =null;
    override fun onCreateView(
        inflater: LayoutInflater,
        container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View? {
        return inflater.inflate(R.layout.processlist_fragment, container, false)
    }

    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        super.onViewCreated(view, savedInstanceState)

        processListAdapter = ProcessListAdapter(R.layout.item_inject_user)
        val recyclerView = view.findViewById<RecyclerView>(R.id.recyclerView)
        recyclerView.layoutManager = LinearLayoutManager(requireContext())
        recyclerView.adapter = processListAdapter
        processListAdapter?.setOnClickListener(object :ProcessListAdapter.OnClickListener{
            override fun onClick( item: UIDPorcessNode) {
                val controller: NavController = findNavController()
                val bundle1:Bundle  =  Bundle();
                bundle1.putSerializable("item",item);
                controller.navigate(R.id.process_info_dest,bundle1)
            }
        })



        if(list != null){
            processListAdapter?.setList(list)
        }else{
            updateProcess()
        }



        val menuHost: MenuHost = requireActivity()
        menuHost.addMenuProvider(object : MenuProvider {
            override fun onCreateMenu(menu: Menu, menuInflater: MenuInflater) {
                menuInflater.inflate(R.menu.injetc_menu, menu)
            }

            override fun onMenuItemSelected(menuItem: MenuItem): Boolean {
                when(menuItem.itemId){
                    R.id.id_toolbar_sync->{
                        updateProcess()
                    }
                }



                return false
            }
        },viewLifecycleOwner, Lifecycle.State.RESUMED)

    }


    fun updateProcess(){
        CoroutineScope(Dispatchers.IO).launch {
            list = entity();
            withContext(Dispatchers.Main) {
                processListAdapter?.setList(list)
                Toast.makeText(RxposedApp.getRxposedContext(), "加载完成", Toast.LENGTH_LONG).show()

            }
        }
        Toast.makeText(RxposedApp.getRxposedContext(), "加载时间较慢，请等待", Toast.LENGTH_LONG).show()
    }

    fun  entity() : MutableCollection<UIDPorcessNode>? {
        val processList = InjectTool.rootRun("ps -ef | awk '{print $1, $2, $8}'").split("\n")
        val pm = RxposedApp.getRxposedContext().packageManager
        val list = mutableListOf<UIDPorcessNode>()
        for (process in processList.drop(1)) {
            if (process.isEmpty()) {
                continue
            }
            val tmpProcess = process.split(" ")
            val UserName = tmpProcess[0]
            val pid = tmpProcess[1]
            val processName = tmpProcess[2]
            val uid_str = InjectTool.shell("id -u $UserName")
            val bigint = BigInteger(uid_str, 10)
            val uid = bigint.toInt()
            val pkgs = pm.getPackagesForUid(uid)
            var pkgName = ""
            var AppName = ""
            var icon: Drawable? = null
            if (pkgs != null) {
                pkgName = pkgs[0]
                val applicationInfo = pm.getApplicationInfo(pkgName, PackageManager.GET_UNINSTALLED_PACKAGES)
                icon =  applicationInfo.loadIcon(pm)
                if(applicationInfo.name != null){
                    AppName =  applicationInfo.name

                }

            }
            var currentRootNode: MutableList<UIDPorcessNode.subprocess>? = null
            list.forEach {
                var porcessNode =  it as UIDPorcessNode
                if(porcessNode.uid == uid){
                    currentRootNode = it.childNode
                }
            }
            currentRootNode.let {
                if (it != null) {
                    val seNode = UIDPorcessNode.subprocess(pid, processName)
                    it.add(seNode)
                }else{
                    val secondNodeList: MutableList<UIDPorcessNode.subprocess> = ArrayList()
                    val seNode = UIDPorcessNode.subprocess(pid, processName)
                    secondNodeList.add(seNode)
                    if(icon == null){
                        icon = RxposedApp.getRxposedContext().getResources().getDrawable(R.drawable.ic_settings);
                    }
                    var entity = UIDPorcessNode(secondNodeList, UserName, uid,AppName,icon)
                    list.add(entity)
                }
            }
        }
        return list
    }


    companion object{
        var list :MutableCollection<UIDPorcessNode>?= null
    }

}
