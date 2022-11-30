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

package hepta.rxposed.manager.fragment.PlugExten

import android.os.Bundle
import android.view.*
import androidx.databinding.DataBindingUtil
import androidx.fragment.app.Fragment
import androidx.navigation.NavController
import androidx.navigation.fragment.findNavController
import androidx.recyclerview.widget.LinearLayoutManager
import hepta.rxposed.manager.MainActivity
import hepta.rxposed.manager.R
import hepta.rxposed.manager.databinding.FragmentModulesBinding


/**
 * A simple [Fragment] subclass.
 */
class ExtenFragment : Fragment() {


    private var moduleListAdapter: ExtenListAdapter? = null
    private val filterModuleInfo: List<ExtenDataProvider> = ArrayList()
    private lateinit var binding: FragmentModulesBinding


    override fun onCreateView(
            inflater: LayoutInflater,
            container: ViewGroup?,
            savedInstanceState: Bundle?
    ): View? {
        binding = DataBindingUtil.inflate(inflater, R.layout.fragment_modules, container, false)
        setHasOptionsMenu(true)

        // Inflate the layout for this fragment
        return binding.root
    }

    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        super.onViewCreated(view, savedInstanceState)
        initRecycleView();

//        view.findViewById<View>(R.id.next_button).setOnClickListener(
//            Navigation.createNavigateOnClickListener(R.id.next_action)
//        )
    }

    private fun initRecycleView() {
        moduleListAdapter = ExtenListAdapter(R.layout.item_module)
        val layoutManager = LinearLayoutManager(requireContext())
        layoutManager.orientation = LinearLayoutManager.VERTICAL
        binding.recyclerView.setLayoutManager(layoutManager)
        binding.recyclerView.setAdapter(moduleListAdapter)
        moduleListAdapter!!.setOnItemClickListener { adapter, view, position ->
            val moduleInfo = adapter.data[position] as ExtenDataProvider.ExtendInfo
            val controller: NavController = findNavController()
            val bundle1:Bundle  =  Bundle();
            bundle1.putInt("Key",moduleInfo.uid);
//            bundle1.putInt("type", START_FRAGMENT_MODULE);
            // 把Persion数据放入到bundle中
            controller.navigate(R.id.extend_apps_dest, bundle1)
        }

        moduleListAdapter!!.setList(ExtenDataProvider.getInstance().moduleList)

    }



    override fun onCreateOptionsMenu(menu: Menu, inflater: MenuInflater) {
        inflater.inflate(R.menu.main_menu, menu)
    }

}
