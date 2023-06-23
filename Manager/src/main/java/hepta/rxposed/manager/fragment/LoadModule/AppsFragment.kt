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

package hepta.rxposed.manager.fragment.LoadModule

import android.annotation.SuppressLint
import android.os.Bundle
import android.text.InputType
import android.view.LayoutInflater
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
import hepta.rxposed.manager.MainActivity
import hepta.rxposed.manager.R
import hepta.rxposed.manager.databinding.FragmentApplistBinding
import hepta.rxposed.manager.fragment.base.AppItemInfo
import hepta.rxposed.manager.fragment.base.SingApplist
import hepta.rxposed.manager.util.MmkvManager


/**
 * A simple [Fragment] subclass.
 */
class AppsFragment : Fragment() {

    private var appsAdapter: AppInfoAdapter? = null
    private val filterListApp: MutableList<AppItemInfo> = mutableListOf()
    val Datalist: MutableList<BaseNode> = ArrayList()
    lateinit var binding: FragmentApplistBinding

    lateinit var moduleName:String


    override fun onCreateView(
        inflater: LayoutInflater,
        container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View? {
        binding = FragmentApplistBinding.inflate(getLayoutInflater())
        var mainActivity = requireActivity() as MainActivity
        mainActivity.DisableToolBar();  //主动调用activity的方法，隐藏toolbar
        return binding.root
    }


    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        super.onViewCreated(view, savedInstanceState)
        moduleName = arguments?.getString("packageName")!!

        initRecycleView();
        initSwitchBar()
        initToolbar();



    }


//初始化多级列表数据


    private fun initSwitchBar() {
        var pkgName = arguments?.getString("packageName")
        requireContext().packageManager.getApplicationInfo(pkgName!!,0)
        val headerView: View = layoutInflater.inflate(R.layout.recycle_head_switchbar, null)
        headerView.layoutParams = ViewGroup.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.WRAP_CONTENT)
        var switchCompat = headerView.findViewById<SwitchCompat>(R.id.switch_enable)
        switchCompat.isChecked = MmkvManager.getModuleStatus(pkgName)
        switchCompat.setOnCheckedChangeListener(CompoundButton.OnCheckedChangeListener { buttonView, isChecked ->
            MmkvManager.setModuleStatus(pkgName,isChecked)
        })
        appsAdapter?.addHeaderView(headerView);
    }

    @SuppressLint("CheckResult")
    private fun initRecycleView() {
        appsAdapter = AppInfoAdapter(R.layout.item_application)
        val layoutManager = LinearLayoutManager(requireContext())
        layoutManager.orientation = LinearLayoutManager.VERTICAL
        binding.recyclerView.layoutManager = layoutManager
        binding.recyclerView.adapter = appsAdapter
        appsAdapter!!.setList(initData())
        appsAdapter!!.setOnCheckedChangeListener(AppInfoAdapter.onListener { status, AppName ->
            MmkvManager.setAppEnableModuleStatus(AppName,moduleName,status)
        })
    }
    fun initData(): Collection<AppItemInfo>? {
        var applist: MutableList<AppItemInfo> = mutableListOf()
        SingApplist.get().global_applist?.forEach{
            it.setEnable(MmkvManager.getAppEnableModuleStatus(it.packageName,moduleName))
            applist.add(it)
        }
        return applist
    }

    fun updateData(){
        SingApplist.get().updateApps();
        appsAdapter!!.setList(initData())
    }


    @SuppressLint("CheckResult")
    private fun initToolbar() {
        binding.toolbar.setNavigationIcon(R.drawable.toolbar_back)  //添加返回键
        //返回键调动函数
        binding.toolbar.setNavigationOnClickListener {
            requireActivity().onBackPressed()
        }
        var appname = arguments?.getString("appname")
        binding.toolbar.subtitle = moduleName
        binding.toolbar.title = appname

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

                appsAdapter?.data?.forEach {

                    if(current_index == 0){
                        filterListApp.add(it)
                    }

//                    if (it.isSystemApp){
//                        if(current_index == 1){
//                            filterListApp.add(it)
//                        }
//                    }else{
//                        if(current_index == 2){
//                            filterListApp.add(it)
//                        }
//                    }
                }
//                appsAdapter?.setList(filterListApp)
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
                appsAdapter?.data?.forEach {

                    if (it.appName.contains(search.toString())||it.packageName.contains(search.toString())){
                        filterListApp.add(it)
                    }
                }
//                appsAdapter?.setList(filterListApp)
            }
            dialog.negativeButton {

            }

            true
        }
    }


    override fun onDestroy() {
        super.onDestroy()
        var mainActivity = requireActivity() as MainActivity
        mainActivity.EnableToolBar()
    }

}
