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

package hepta.rxposed.manager.fragment.PlugInject

import android.os.Bundle
import android.view.*
import androidx.core.view.MenuHost
import androidx.core.view.MenuProvider
import androidx.databinding.DataBindingUtil
import androidx.fragment.app.Fragment
import androidx.lifecycle.Lifecycle
import androidx.recyclerview.widget.LinearLayoutManager
import hepta.rxposed.manager.MainActivity
import hepta.rxposed.manager.R
import hepta.rxposed.manager.databinding.FragmentFrameworksBinding
import hepta.rxposed.manager.util.Consts.INJECTLIST_FRAGMENT_ARGE
import hepta.rxposed.manager.util.Consts.INJECT_FRAGMENT_ARGE


/**
 * A simple [Fragment] subclass.
 */
class SupportFragment : Fragment() {


    private var moduleListAdapter: SupportListAdapter? = null
    private val filterModuleInfo: List<SupportInfoProvider.SupportInfo> = ArrayList()
    private lateinit var binding: FragmentFrameworksBinding
    private lateinit var listPlug:  List<SupportInfoProvider.SupportInfo>
    override fun onCreateView(
        inflater: LayoutInflater,
        container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View? {
        binding = DataBindingUtil.inflate(inflater, R.layout.fragment_frameworks, container, false)
        var mainActivity = requireActivity() as MainActivity
        mainActivity.EnableToolBar()
        return binding.root
    }

    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        var inject = if(arguments!=null) arguments?.getBoolean(INJECT_FRAGMENT_ARGE,false) else false
        super.onViewCreated(view, savedInstanceState)
        if(inject == true){
            val menuHost: MenuHost = requireActivity()
            menuHost.addMenuProvider(object : MenuProvider {
                override fun onCreateMenu(menu: Menu, menuInflater: MenuInflater) {
                    menuInflater.inflate(R.menu.plug_inject, menu)
                }

                override fun onMenuItemSelected(menuItem: MenuItem): Boolean {
                    val tmplistPlug = ArrayList<String>()
                    when(menuItem.itemId){
                        R.id.id_toolbar_option->{
                            val result = Bundle()
                            for(plug in listPlug){
                                if(plug.enable){
                                    tmplistPlug.add(plug.packageName)
                                }
                            }
                            result.putStringArrayList(INJECTLIST_FRAGMENT_ARGE
                                , tmplistPlug)
                            parentFragmentManager.setFragmentResult(INJECT_FRAGMENT_ARGE, result);
                            parentFragmentManager.popBackStack()
                        }
                    }
                    return false
                }
            },viewLifecycleOwner, Lifecycle.State.RESUMED)

        }
        moduleListAdapter = SupportListAdapter(R.layout.item_module,inject)
        val layoutManager = LinearLayoutManager(requireContext())
        layoutManager.orientation = LinearLayoutManager.VERTICAL
        binding.recyclerView.setLayoutManager(layoutManager)
        binding.recyclerView.setAdapter(moduleListAdapter)
        listPlug = SupportInfoProvider.getInstance().moduleList
        moduleListAdapter!!.setList(listPlug)

    }



//    override fun onCreateOptionsMenu(menu: Menu, inflater: MenuInflater) {
//        inflater.inflate(R.menu.main_menu, menu)
//    }

}
