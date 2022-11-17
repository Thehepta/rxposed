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

package hepta.rxposed.manager.fragment.vpn
import android.os.Bundle
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import androidx.fragment.app.Fragment
import androidx.recyclerview.widget.LinearLayoutManager
import androidx.recyclerview.widget.RecyclerView
import com.chad.library.adapter.base.entity.node.BaseNode
import hepta.rxposed.manager.R

/**
 * A simple [Fragment] subclass.
 */
class VpnFragment : Fragment() {

    override fun onCreateView(
            inflater: LayoutInflater,
            container: ViewGroup?,
            savedInstanceState: Bundle?
    ): View? {
        // Inflate the layout for this fragment
        return inflater.inflate(R.layout.fragment_vpn, container, false)
    }


    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        super.onViewCreated(view, savedInstanceState)

        var nodeAdapter = NodeAdapter()
        var rv_list = view.findViewById<RecyclerView>(R.id.rv_list)
        rv_list.layoutManager = LinearLayoutManager(requireContext())
        rv_list.adapter = nodeAdapter
        nodeAdapter.setList(getEntity())

    }

    private fun getEntity(): List<BaseNode>? {
        //总的 list，里面放的是 FirstNode
        val list: MutableList<BaseNode> = ArrayList()
        for (i in 0..7) {

            //SecondNode 的 list
            val secondNodeList: MutableList<BaseNode> = ArrayList()
            for (n in 0..5) {
                val seNode = SecondNode("Second Node $n")
                secondNodeList.add(seNode)
            }
            val entity = RootNode(secondNodeList, "Root Node $i")
            list.add(entity)
        }
        val firstNodeList: MutableList<BaseNode> = ArrayList()

        for (n in 0..5) {
            val seNode = FirstNode("First Node $n")
            firstNodeList.add(seNode)
        }


        val entityfirst = RootNode(firstNodeList, "Root Node $100")
        list.add(entityfirst)
        return list
    }

}
