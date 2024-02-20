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

import android.annotation.SuppressLint
import android.content.Context
import android.os.Bundle
import android.text.InputType
import android.view.*
import android.widget.*
import androidx.core.view.MenuHost
import androidx.core.view.MenuProvider
import androidx.fragment.app.Fragment
import androidx.lifecycle.Lifecycle
import androidx.navigation.fragment.findNavController
import com.afollestad.materialdialogs.LayoutMode
import com.afollestad.materialdialogs.MaterialDialog
import com.afollestad.materialdialogs.bottomsheets.BottomSheet
import com.afollestad.materialdialogs.input.input
import com.afollestad.materialdialogs.list.listItems
import hepta.rxposed.manager.R
import hepta.rxposed.manager.util.InjectTool
import hepta.rxposed.manager.util.Util
import hepta.rxposed.manager.widget.DialogUtil


/**
 * Fragment used to show how to navigate to another destination
 */
class HomeFragment : Fragment() {
    override fun onCreateView(
        inflater: LayoutInflater,
        container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View? {

        Util.LogD("onCreateView")
        return inflater.inflate(R.layout.fragment_home, container, false)
    }

    @SuppressLint("CheckResult")
    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        super.onViewCreated(view, savedInstanceState)
        rxposed_activity_ui_init()
        val btn_modules = view.findViewById<Button>(R.id.btn_modules)
        btn_modules?.setOnClickListener {
            findNavController().navigate(R.id.modules_dest, null)
        }

        val btn_root = view.findViewById<Button>(R.id.btn_root)
        btn_root?.setOnClickListener {

            DialogUtil.DidalogSimple(requireContext(),"当前设置:"+ InjectTool.su_path,{
                var search: CharSequence? = null
                val dialog = MaterialDialog(requireContext(), BottomSheet(LayoutMode.WRAP_CONTENT)).show {
                    title(R.string.setRootTips)
                    input(hint = "当前root设置："+InjectTool.su_path, inputType = InputType.TYPE_CLASS_TEXT or InputType.TYPE_TEXT_FLAG_CAP_WORDS) { _, text ->
                        search = text
                    }
                    negativeButton(android.R.string.cancel)
                    positiveButton(android.R.string.ok)
                }
                dialog.positiveButton {
                    InjectTool.su_path = search.toString()
                    requireContext().getSharedPreferences("rxposed", Context.MODE_PRIVATE).edit().putString("supath",InjectTool.su_path).commit()
                }
            }, Tips = R.string.RootShowTips, Ok = R.string.Edit)

            true
        }
        val btn_framework = view.findViewById<Button>(R.id.btn_framework)
        btn_framework?.setOnClickListener {
            findNavController().navigate(R.id.pluginject_dest, null)

        }


        val menuHost: MenuHost = requireActivity()
        menuHost.addMenuProvider(object : MenuProvider {
            override fun onCreateMenu(menu: Menu, menuInflater: MenuInflater) {
                menuInflater.inflate(R.menu.main_menu, menu)

            }

            override fun onMenuItemSelected(menuItem: MenuItem): Boolean {
                when(menuItem.itemId){
                    R.id.id_toolbar_option->{
                        MaterialDialog(requireContext(), BottomSheet(LayoutMode.WRAP_CONTENT)).show {
                            listItems(R.array.rxposetOptions, waitForPositiveButton = false) { _, index, text ->
                                Util.LogD("$text")

                                when(index){
                                    0 -> DialogUtil.DidalogSimple(requireContext(),R.string.zygoteMessage, {
                                        InjectTool.zygote_reboot()
                                    })
                                    1 -> DialogUtil.DidalogSimple(requireContext(),R.string.rebootMessage, {
                                        InjectTool.Application_reboot()
                                    })
                                    2 -> DialogUtil.DidalogSimple(requireContext(),R.string.injectTestMessage, {
                                        InjectTool.inject_text()
                                    })
                                }

                            }
                            positiveButton(android.R.string.cancel)
                        }
                    }
                }
                return false
            }

        },viewLifecycleOwner, Lifecycle.State.RESUMED)
    }


    private fun rxposed_activity_ui_init() {

        val btn_activity = view?.findViewById<Button>(R.id.btn_activity)
        val Image_icon = view?.findViewById<ImageView>(R.id.status_icon)
        val Text_status = view?.findViewById<TextView>(R.id.status_text)

        var activity = get_rxposed_status();

        if (activity){
            btn_activity?.visibility=View.INVISIBLE
            Text_status?.visibility=View.INVISIBLE
        }else{
            Image_icon?.setImageResource(R.drawable.ic_error)
            btn_activity?.setOnClickListener {

                DialogUtil.DidalogSimple(requireContext(),R.string.activityMessage, {
                    InjectTool.Start()
                    DialogUtil.DidalogSimple(requireContext(),R.string.rxrebootMessage, {
                        InjectTool.Application_reboot()
                    })
                })
            }
        }
    }
    private fun get_rxposed_status(): Boolean {

        val zygote_host_uid = android.os.Process.getUidForName(InjectTool.getStatusAuthority());
        if(zygote_host_uid!=-1){
            return true;
        }
        return false;
    }

//        external fun get_rxposed_activity():Boolean

}
