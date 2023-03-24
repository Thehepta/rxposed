package hepta.rxposed.loadxposed.ui.home

import android.os.Bundle
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.TextView
import androidx.fragment.app.Fragment
import androidx.lifecycle.ViewModelProvider
import androidx.navigation.NavController
import androidx.navigation.fragment.findNavController
import androidx.recyclerview.widget.LinearLayoutManager
import hepta.rxposed.loadxposed.R
import hepta.rxposed.loadxposed.databinding.FragmentHomeBinding

class HomeFragment : Fragment() {

    private var _binding: FragmentHomeBinding? = null

    // This property is only valid between onCreateView and
    // onDestroyView.
    private val binding get() = _binding!!
    private var moduleListAdapter: ExtenListAdapter? = null

    override fun onCreateView(
        inflater: LayoutInflater,
        container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View {
        val homeViewModel =
            ViewModelProvider(this).get(HomeViewModel::class.java)

        _binding = FragmentHomeBinding.inflate(inflater, container, false)
        val root: View = binding.root
        return root
    }

    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {

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

    override fun onDestroyView() {
        super.onDestroyView()
        _binding = null
    }
}