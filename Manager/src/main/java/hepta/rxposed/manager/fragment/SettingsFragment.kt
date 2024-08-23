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
import android.widget.EditText
import android.widget.Spinner
import androidx.core.view.MenuHost
import androidx.core.view.MenuProvider
import androidx.fragment.app.Fragment
import androidx.lifecycle.Lifecycle
import hepta.rxposed.manager.R
import hepta.rxposed.manager.util.InjectConfig
import hepta.rxposed.manager.util.MmkvManager


/**
 * A simple [Fragment] subclass.
 */
class SettingsFragment : Fragment() {

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
                        val inject_type = view?.findViewById<Spinner>(R.id.inject_type)
                        if(inject_type?.selectedItemId?.toInt() == 0){
                            MmkvManager.putInjectConfigBoolean("hidemaps", true)
                        }else{
                            MmkvManager.putInjectConfigBoolean("hidemaps", false)
                        }

                        val initinject_type = view?.findViewById<Spinner>(R.id.sp_initInject)
                        if(initinject_type?.selectedItemId?.toInt() == 0){
                            MmkvManager.putInjectConfigBoolean("injectInit", true)
                        }else{
                            MmkvManager.putInjectConfigBoolean("injectInit", false)
                        }
                        val mountPath = view?.findViewById<EditText>(R.id.et_mount_path)
                        MmkvManager.putInjectConfigString("mountWorkDir", mountPath?.text.toString())
                        val remarks = view?.findViewById<EditText>(R.id.et_remarks)
                        MmkvManager.putInjectConfigString("config_name", remarks?.text.toString())
                        parentFragmentManager.popBackStack()

                    }
                }
                return false
            }

        },viewLifecycleOwner, Lifecycle.State.RESUMED)

    }

    private fun init_ui() {

        var config_name = MmkvManager.getInjectConfigString("config_name", "default")
        var su_path = MmkvManager.getInjectConfigString("supath", "su")
        var mountWorkDir = MmkvManager.getInjectConfigString("mountWorkDir", "/apex/com.android.i18nrxp")
        var hidemaps = MmkvManager.getInjectConfigBoolean("hidemaps", false)
        var injectInit = MmkvManager.getInjectConfigBoolean("injectInit", false)

        val inject_type = view?.findViewById<Spinner>(R.id.inject_type)
        if (hidemaps){
            inject_type?.setSelection(0)
        }else{
            inject_type?.setSelection(1)
        }
        val initinject_type = view?.findViewById<Spinner>(R.id.sp_initInject)
        if (injectInit){
            initinject_type?.setSelection(0)
        }else{
            initinject_type?.setSelection(1)
        }
        val mountPath = view?.findViewById<EditText>(R.id.et_mount_path)
        val remarks = view?.findViewById<EditText>(R.id.et_remarks)
        val libPath = view?.findViewById<EditText>(R.id.et_lib_path)
        val lib64Path = view?.findViewById<EditText>(R.id.et_lib64_path)
        val injectArg = view?.findViewById<EditText>(R.id.et_inject_arg)
        remarks?.setText(config_name)
        mountPath?.setText(mountWorkDir)

        libPath?.setText(mountWorkDir+ "/lib/" + InjectConfig.soName)
        lib64Path?.setText(mountWorkDir+ "/lib64/" + InjectConfig.soName)
        injectArg?.setText(InjectConfig.InjectArg)

        return
    }

}
