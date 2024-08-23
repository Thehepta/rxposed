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

package hepta.rxposed.manager.fragment

import android.os.Bundle
import android.util.Log
import android.view.LayoutInflater
import android.view.Menu
import android.view.MenuInflater
import android.view.MenuItem
import android.view.View
import android.view.ViewGroup
import android.widget.AdapterView
import android.widget.EditText
import android.widget.LinearLayout
import android.widget.Spinner
import android.widget.Toast
import androidx.core.view.MenuHost
import androidx.core.view.MenuProvider
import androidx.fragment.app.Fragment
import androidx.lifecycle.Lifecycle
import hepta.rxposed.manager.R
import hepta.rxposed.manager.RxposedApp
import hepta.rxposed.manager.util.InjectConfig
import hepta.rxposed.manager.util.MmkvManager


/**
 * A simple [Fragment] subclass.
 */
class SettingsFragment : Fragment() {


    var ic: InjectConfig = InjectConfig.getInstance()


    override fun onCreateView(
            inflater: LayoutInflater,
            container: ViewGroup?,
            savedInstanceState: Bundle?
    ): View? {
        // Inflate the layout for this fragment
        return inflater.inflate(R.layout.fragment_settings, container, false)
    }

    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        super.onViewCreated(view, savedInstanceState)
        init_ui()
        val menuHost: MenuHost = requireActivity()
        menuHost.addMenuProvider(object : MenuProvider {
            override fun onCreateMenu(menu: Menu, menuInflater: MenuInflater) {
                menuInflater.inflate(R.menu.setting_menu, menu)

            }

            override fun onMenuItemSelected(menuItem: MenuItem): Boolean {
                when(menuItem.itemId){
                    R.id.set_save->{

                        val initinject_type = view?.findViewById<Spinner>(R.id.sp_initInject)
                        if(initinject_type?.selectedItemId?.toInt() == 0){
                            MmkvManager.putInjectConfigBoolean("injectInit", true)
                        }else{
                            MmkvManager.putInjectConfigBoolean("injectInit", false)
                        }

                        val inject_type = view?.findViewById<Spinner>(R.id.inject_type)
                        if(inject_type?.selectedItemId?.toInt() == InjectConfig.HIED_MAPS){
                            MmkvManager.putInjectConfigInt("injectType", InjectConfig.HIED_MAPS)
                            ic.updateConfigSave()
                            parentFragmentManager.popBackStack()
                        }else{
                            MmkvManager.putInjectConfigInt("injectType", InjectConfig.MOUNT_TMP)

                            val mountPath = view?.findViewById<EditText>(R.id.et_mount_path)
                            if(mountPath?.text.toString().isEmpty()){
                                Toast.makeText(requireContext(), "mountWorkDir is not null", Toast.LENGTH_LONG).show()

                            }else{
                                MmkvManager.putInjectConfigString("mountWorkDir", mountPath?.text.toString())
                                ic.updateConfigSave()
                                parentFragmentManager.popBackStack()
                            }
                        }
                    }
                }
                return false
            }

        },viewLifecycleOwner, Lifecycle.State.RESUMED)

    }

    private fun init_ui() {
        val ll_mount = view?.findViewById<LinearLayout>(R.id.ll_mount)
        val inject_type = view?.findViewById<Spinner>(R.id.inject_type)
        val mountPath = view?.findViewById<EditText>(R.id.et_mount_path)
        val suPath = view?.findViewById<EditText>(R.id.et_supath)
        val remarks = view?.findViewById<EditText>(R.id.et_remarks)
        val libPath = view?.findViewById<EditText>(R.id.et_lib_path)
        val lib64Path = view?.findViewById<EditText>(R.id.et_lib64_path)
        val injectArg = view?.findViewById<EditText>(R.id.et_inject_arg)
        remarks?.setText(ic.config_name)
        suPath?.setText(ic.su_path)
        injectArg?.setText(InjectConfig.InjectArg)

        if (ic.injectType == InjectConfig.HIED_MAPS){
            inject_type?.setSelection(InjectConfig.HIED_MAPS)
            ll_mount?.visibility = View.GONE
            libPath?.setText( ic.arm32_InjectSo)
            lib64Path?.setText( ic.arm32_InjectSo)
        }else{
            inject_type?.setSelection(InjectConfig.MOUNT_TMP)
            mountPath?.setText(ic.mountWorkDir)
            ll_mount?.visibility = View.VISIBLE
            libPath?.setText( ic.arm32_InjectSo)
            lib64Path?.setText( ic.arm32_InjectSo)
        }

        val initinject_type = view?.findViewById<Spinner>(R.id.sp_initInject)
        if (ic.injectInit){
            initinject_type?.setSelection(0)
        }else{
            initinject_type?.setSelection(1)
        }

        inject_type?.onItemSelectedListener = object : AdapterView.OnItemSelectedListener {
            override fun onItemSelected(parent: AdapterView<*>, view: View?, position: Int, id: Long) {
                // 获取选择的项
                if(position == InjectConfig.HIED_MAPS){
                    ll_mount?.visibility = View.GONE
                }else{
                    mountPath?.setText(ic.mountWorkDir)
                    ll_mount?.visibility = View.VISIBLE
                }
            }
            override fun onNothingSelected(parent: AdapterView<*>) {
                // 当没有选择项时的处理（一般情况很少用到）
            }
        }
        return
    }

}
