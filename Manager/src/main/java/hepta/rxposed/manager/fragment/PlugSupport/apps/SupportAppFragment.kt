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

package hepta.rxposed.manager.fragment.PlugSupport.apps

import android.annotation.SuppressLint
import android.os.Bundle
import android.text.InputType
import android.view.View
import android.view.ViewGroup
import android.widget.CompoundButton
import androidx.appcompat.widget.SwitchCompat
import androidx.fragment.app.Fragment
import androidx.recyclerview.widget.LinearLayoutManager
import com.afollestad.materialdialogs.LayoutMode
import com.afollestad.materialdialogs.MaterialDialog
import com.afollestad.materialdialogs.bottomsheets.BottomSheet
import com.afollestad.materialdialogs.input.input
import com.afollestad.materialdialogs.list.listItemsSingleChoice
import com.chad.library.adapter.base.entity.node.BaseNode
import hepta.rxposed.manager.R
import hepta.rxposed.manager.fragment.PlugSupport.SupportData
import hepta.rxposed.manager.fragment.base.AppInfoNode
import hepta.rxposed.manager.fragment.base.ModuleInfo
import hepta.rxposed.manager.fragment.base.SectionBarNode
import hepta.rxposed.manager.fragment.base.baseCollToolbarFragment
import hepta.rxposed.manager.util.LogUtil


/**
 * A simple [Fragment] subclass.
 */
class ApplistFragment : baseCollToolbarFragment() {

    private var appsAdapter: AppInfoAdapter? = null
    private val filterListApp: MutableList<AppInfoNode> = mutableListOf()
    val Datalist: MutableList<BaseNode> = ArrayList()




    override fun getModuleInfo(): ModuleInfo {
        return SupportData.getInstance().ByUidGetModuleInfo(arguments?.getInt("Key")!!)
    }

    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        super.onViewCreated(view, savedInstanceState)
        var supportinfo:SupportData.SupportInfo = moduleInfo as SupportData.SupportInfo

        val result: java.util.ArrayList<Any?> = java.util.ArrayList<Any?>(supportinfo.depondsList.values)

        val firstSectionBar = SectionBarNode(result as List<BaseNode>? , "依赖应用")
        Datalist.add(firstSectionBar)
        val secondSectionBar = SectionBarNode(moduleInfo?.appInfoList as List<BaseNode>?, "应用列表")
        Datalist.add(secondSectionBar)

        initRecycleView();
//        initSwitchBar()
        initToolbar();
    }

    private fun initSwitchBar() {
        // recycleview 添加toolbar
        val headerView: View = layoutInflater.inflate(R.layout.recycle_head_switchbar, null)
        headerView.layoutParams = ViewGroup.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.WRAP_CONTENT)
        var switchCompat = headerView.findViewById<SwitchCompat>(R.id.switch_enable)
        switchCompat.isChecked = moduleInfo!!.getEnable()
        switchCompat.setOnCheckedChangeListener(CompoundButton.OnCheckedChangeListener { buttonView, isChecked ->
            LogUtil.LogE("check:",isChecked)
            moduleInfo?.setEnable(isChecked)
        })
        appsAdapter?.addHeaderView(headerView);
    }

    @SuppressLint("CheckResult")
    private fun initRecycleView() {
        appsAdapter = AppInfoAdapter()
        val layoutManager = LinearLayoutManager(requireContext())
        layoutManager.orientation = LinearLayoutManager.VERTICAL
        binding.recyclerView.setLayoutManager(layoutManager)
        binding.recyclerView.setAdapter(appsAdapter)

//        appsAdapter!!.setOnItemClickListener { adapter, view, position ->
//            val appInfo = adapter.data[position] as AppModuleInfo
//            LogUtil.LogD(appInfo.appName)
//        }
        appsAdapter?.setList(Datalist)
        //绑定数据
        binding.modInfo = moduleInfo
    }

    @SuppressLint("CheckResult")
    private fun initToolbar() {
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
//                applistAdapter?.setList(filterListApp)
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
//                applistAdapter?.setList(filterListApp)
            }
            dialog.negativeButton {

            }

            true
        }
    }

    override fun onDestroy() {
        super.onDestroy()
        LogUtil.LogE("onDestroy")
    }

    override fun onDestroyView() {
        super.onDestroyView()
        LogUtil.LogE("onDestroyView")

    }
}
