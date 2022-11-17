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

package hepta.rxposed.manager.fragment.apps

import android.annotation.SuppressLint
import android.os.Bundle
import android.text.InputType
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.CompoundButton
import android.widget.ExpandableListAdapter
import androidx.appcompat.widget.SwitchCompat
import androidx.databinding.DataBindingUtil
import androidx.fragment.app.Fragment
import androidx.recyclerview.widget.LinearLayoutManager
import com.afollestad.materialdialogs.LayoutMode
import com.afollestad.materialdialogs.MaterialDialog
import com.afollestad.materialdialogs.bottomsheets.BottomSheet
import com.afollestad.materialdialogs.input.input
import com.afollestad.materialdialogs.list.listItemsSingleChoice
import com.chad.library.adapter.base.entity.node.BaseNode
import hepta.rxposed.manager.MainActivity
import hepta.rxposed.manager.R
import hepta.rxposed.manager.databinding.FragmentApplistBinding
import hepta.rxposed.manager.fragment.extend.ModuleData
import hepta.rxposed.manager.util.LogUtil


/**
 * A simple [Fragment] subclass.
 */
class ApplistFragment : Fragment() {


    private var applistAdapter: AppListAdapter? = null
    private var appsAdapter: AppsAdapter? = null
    private var expandableListAdapter : ExpandableListAdapter? = null

    private val filterListApp: MutableList<AppInfo> = mutableListOf()
    private lateinit var binding: FragmentApplistBinding
    var moduleInfo: ModuleData.Modules? = null


    override fun onCreateView(
            inflater: LayoutInflater,
            container: ViewGroup?,
            savedInstanceState: Bundle?
    ): View? {
        binding = DataBindingUtil.inflate(inflater, R.layout.fragment_applist, container, false)
        setHasOptionsMenu(true)
        var mainActivity = requireActivity() as MainActivity
        mainActivity.DisableToolBar();
        // Inflate the layout for this fragment
        return binding.root
    }


//    fun ApplistFragment(moduleInfo: ModuleInfo) {
//        moduleInfo = moduleInfo
//    }


    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        super.onViewCreated(view, savedInstanceState)
        initUi();
    }

    @SuppressLint("CheckResult")
    private fun initUi() {
        moduleInfo = ModuleData.getInstance().ByUidGetModuleInfo(arguments?.getInt("Key")!!)
        val headerView: View = layoutInflater.inflate(R.layout.recycle_head_switchbar, null)
        headerView.layoutParams = ViewGroup.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.WRAP_CONTENT)


        var switchCompat = headerView.findViewById<SwitchCompat>(R.id.switch_enable)
        switchCompat.isChecked = moduleInfo!!.getEnable()
        switchCompat.setOnCheckedChangeListener(CompoundButton.OnCheckedChangeListener { buttonView, isChecked ->
            LogUtil.LogE("check:",isChecked)
            moduleInfo?.setEnable(isChecked)
        })

        appsAdapter = AppsAdapter()
        appsAdapter?.addHeaderView(headerView);
//        applistAdapter = AppListAdapter(R.layout.item_application)
//        applistAdapter?.addHeaderView(headerView)


        val layoutManager = LinearLayoutManager(requireContext())
        layoutManager.orientation = LinearLayoutManager.VERTICAL
        binding.recyclerView.setLayoutManager(layoutManager)
//        binding.recyclerView.setAdapter(applistAdapter)
        binding.recyclerView.setAdapter(appsAdapter)

//        applistAdapter!!.setOnItemClickListener { adapter, view, position ->
//            val appInfo = adapter.data[position] as AppInfo
//            LogUtil.LogD(appInfo.appName)
//        }

        appsAdapter?.setList(getEntity())
//        applistAdapter?.setList(moduleInfo.appInfoList)

        binding.modInfo = moduleInfo
        binding.toolbar.setNavigationIcon(R.drawable.toolbar_back)

        binding.toolbar.setNavigationOnClickListener {
            requireActivity().onBackPressed()
        }



        binding.toolbar.menu.findItem(R.id.id_toolbar_option).setOnMenuItemClickListener {
            var current_index: Int = 0
            val dialog = MaterialDialog(requireContext(), BottomSheet(LayoutMode.WRAP_CONTENT)).show {
                listItemsSingleChoice(R.array.selectAppType, waitForPositiveButton = false, initialSelection = 0) { _, index, text ->
                    current_index = index
                }
                negativeButton(android.R.string.ok)
                positiveButton(android.R.string.cancel)

            }
            dialog.negativeButton {
                filterListApp.clear()

                moduleInfo?.getAppInfoList()?.forEach {

                    if(current_index == 0){
                        filterListApp.add(it)
                    }

                    if (it.isSystemApp){
                        if(current_index == 1){
                            filterListApp.add(it)
                        }
                    }else{
                        if(current_index == 2){
                            filterListApp.add(it)
                        }
                    }
                }
                applistAdapter?.setList(filterListApp)
            }

            dialog.positiveButton {
            }

            false
        }
        binding.toolbar.menu.findItem(R.id.id_toolbar_search).setOnMenuItemClickListener {
            var search: CharSequence? = null
            val dialog = MaterialDialog(requireContext(), BottomSheet(LayoutMode.WRAP_CONTENT)).show {
                message(R.string.applist_searchTips)
                input(
                    hint = "请输入搜索关键字",
                    inputType = InputType.TYPE_CLASS_TEXT or InputType.TYPE_TEXT_FLAG_CAP_WORDS
                ) { _, text ->
                    search = text
                }
                negativeButton(android.R.string.cancel)
                positiveButton(android.R.string.ok)
            }
            dialog.positiveButton {
                filterListApp.clear()
                moduleInfo?.getAppInfoList()?.forEach {

                    if (it.appName.contains(search.toString())||it.packageName.contains(search.toString())){
                        filterListApp.add(it)
                    }
                }
                applistAdapter?.setList(filterListApp)
            }
            dialog.negativeButton {

            }

            true
        }

    }


    private fun getEntity(): List<BaseNode>? {
        //总的 list，里面放的是 FirstNode
        val list: MutableList<BaseNode> = ArrayList()
        val firstNodeList: MutableList<BaseNode> = ArrayList()
        val fiNode = SecondNode("fiNode Node 1")
        firstNodeList.add(fiNode)
        val firstentity = RootNode(firstNodeList, "框架列表")

        list.add(firstentity)
        val appNodeList: MutableList<AppInfo> = ArrayList()
//        for (i in 1..5) {
//            var appin = moduleInfo?.appInfoList?.get(i)
//            appin?.let { appNodeList.add(it) }
//        }


        val secondentity = RootNode(moduleInfo?.appInfoList as List<BaseNode>?, "应用列表")
        list.add(secondentity)

        return list
    }


}
