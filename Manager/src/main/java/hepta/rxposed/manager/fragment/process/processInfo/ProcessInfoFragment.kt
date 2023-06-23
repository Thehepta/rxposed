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

package hepta.rxposed.manager.fragment.process.processInfo

import android.annotation.SuppressLint
import android.os.Bundle
import android.text.InputType
import android.util.Log
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import androidx.appcompat.widget.Toolbar
import androidx.fragment.app.Fragment
import androidx.navigation.fragment.findNavController
import androidx.recyclerview.widget.LinearLayoutManager
import androidx.recyclerview.widget.RecyclerView
import com.afollestad.materialdialogs.LayoutMode
import com.afollestad.materialdialogs.MaterialDialog
import com.afollestad.materialdialogs.bottomsheets.BottomSheet
import com.afollestad.materialdialogs.input.input
import com.afollestad.materialdialogs.list.listItemsSingleChoice
import hepta.rxposed.manager.MainActivity
import hepta.rxposed.manager.R
import hepta.rxposed.manager.fragment.base.AppItemInfo
import hepta.rxposed.manager.fragment.process.UIDPorcessItem
import hepta.rxposed.manager.util.Consts.INJECTLIST_FRAGMENT_ARGE
import hepta.rxposed.manager.util.Consts.INJECT_FRAGMENT_ARGE
import hepta.rxposed.manager.util.InjectTool
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch


/**
 * A simple [Fragment] subclass.
 */
class ProcessInfoFragment : Fragment() {

    private var appsAdapter: usprocInfoAdapter? = null
    private val filterListApp: MutableList<AppItemInfo> = mutableListOf()
    val Datalist: MutableList<UIDPorcessItem.subprocessItem> = ArrayList()

    private var toolbar: Toolbar? = null
    private var recyclerView:RecyclerView? = null

    private var inject_pid = "-1"
    override fun onCreateView(
        inflater: LayoutInflater,
        container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View? {
        parentFragmentManager.setFragmentResultListener(INJECT_FRAGMENT_ARGE,this) { requestKey, result ->
            val apkNaemList = result.getStringArrayList(INJECTLIST_FRAGMENT_ARGE);
            if (apkNaemList != null) {
                ProcessInject_Apk(apkNaemList)
            }


        }
        var binding = inflater.inflate(R.layout.fragment_userprocess, container, false)
        var mainActivity = requireActivity() as MainActivity
        mainActivity.DisableToolBar();  //主动调用activity的方法，隐藏toolbar
        return binding
    }

    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        super.onViewCreated(view, savedInstanceState)
        val parcelable = arguments?.getSerializable("item") as UIDPorcessItem
        toolbar = view.findViewById<Toolbar>(R.id.toolbar)
        recyclerView = view.findViewById(R.id.recyclerView)
        toolbar?.setTitle(parcelable.appName)
        toolbar?.setSubtitle(parcelable.pkgName)
        initToolbar();
        appsAdapter = usprocInfoAdapter(R.layout.item_inject_usproc)
        val layoutManager = LinearLayoutManager(requireContext())
        layoutManager.orientation = LinearLayoutManager.VERTICAL
        recyclerView?.setLayoutManager(layoutManager)
        recyclerView?.setAdapter(appsAdapter)
        appsAdapter?.setList(parcelable.childNode)
        appsAdapter?.setOnClickInjectListener(object :usprocInfoAdapter.OnClickInjectListener{
            override fun onClick(item: UIDPorcessItem.subprocessItem?) {
                if (item != null) {
                    inject_pid = item.pid;
                    val bundle1:Bundle  =  Bundle();
                    bundle1.putBoolean(INJECT_FRAGMENT_ARGE,true);
                    findNavController().navigate(R.id.pluginject_dest, bundle1)

                };
            }

        })

    }


    fun ProcessInject_Apk( apkNaemList :ArrayList<String>){
        CoroutineScope(Dispatchers.IO).launch {
            val retString = StringBuilder()
            val it: MutableListIterator<String> = apkNaemList.listIterator()
            while (it.hasNext()){
                var apkName = it.next()
                retString.append(apkName)
                if (it.hasNext()) {
                    retString.append(":")
                }
            }
            InjectTool.Inject_Process(inject_pid.toInt(),retString.toString())
            Log.e("Rzx","ProcessInject_Apk:"+retString.toString())
        }
    }

    private fun initSwitchBar() {
        // recycleview 添加toolbar
//        val headerView: View = layoutInflater.inflate(R.layout.recycle_head_switchbar, null)
//        headerView.layoutParams = ViewGroup.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.WRAP_CONTENT)
//        var switchCompat = headerView.findViewById<SwitchCompat>(R.id.switch_enable)
//        switchCompat.isChecked = moduleInfo!!.getEnable()
//        switchCompat.setOnCheckedChangeListener(CompoundButton.OnCheckedChangeListener { buttonView, isChecked ->
//            Util.LogE("check:",isChecked)
//            moduleInfo?.setEnable(isChecked)
//        })
//        appsAdapter?.addHeaderView(headerView);
    }



    @SuppressLint("CheckResult")
    private fun initToolbar() {
        toolbar?.let {

            it.setNavigationIcon(R.drawable.toolbar_back)

            it.setNavigationOnClickListener {
                requireActivity().onBackPressed()
            }

            it.menu.findItem(R.id.id_toolbar_option).setOnMenuItemClickListener {
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

//                moduleInfo?.getAppInfoList()?.forEach {
//
//                    if(current_index == 0){
//                        filterListApp.add(it)
//                    }
//
//                    if (it.isSystemApp){
//                        if(current_index == 1){
//                            filterListApp.add(it)
//                        }
//                    }else{
//                        if(current_index == 2){
//                            filterListApp.add(it)
//                        }
//                    }
//                }
//                applistAdapter?.setList(filterListApp)
                }

                dialog.positiveButton {
                }

                false
            }
            it.menu.findItem(R.id.id_toolbar_search).setOnMenuItemClickListener {
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
//                moduleInfo?.getAppInfoList()?.forEach {
//
//                    if (it.appName.contains(search.toString())||it.packageName.contains(search.toString())){
//                        filterListApp.add(it)
//                    }
//                }
//                applistAdapter?.setList(filterListApp)
                }
                dialog.negativeButton {

                }

                true
            }

        }
    }

    override fun onDestroy() {
        super.onDestroy()
        var mainActivity = requireActivity() as MainActivity
        mainActivity.EnableToolBar()
    }

}
