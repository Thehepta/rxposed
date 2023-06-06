package hepta.rxposed.manager.fragment.check

import android.content.pm.PackageManager
import android.graphics.drawable.Drawable
import android.os.Bundle
import android.util.Log
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import androidx.fragment.app.Fragment
import com.chad.library.adapter.base.entity.node.BaseNode
import hepta.rxposed.manager.R
import hepta.rxposed.manager.util.InjectTool
import java.lang.reflect.Method
import java.math.BigInteger
import java.util.*


class VpnFragment : Fragment() {
    override fun onCreateView(
        inflater: LayoutInflater,
        container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View? {
        return inflater.inflate(R.layout.fragment_check, container, false)
    }

    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        super.onViewCreated(view, savedInstanceState)


    }



    fun  entity() : Collection<BaseNode>? {
            val processList = InjectTool.rootRun("ps -ef | awk '{print $1, $2, $8}'").split("\n")
            val pm = requireContext().packageManager
            val list = mutableListOf<BaseNode>()
            for (process in processList.drop(1)) {
                if (process.isEmpty()) {
                    continue
                }
                val tmpProcess = process.split(" ")
                val UserName = tmpProcess[0]
                val pid = tmpProcess[1]
                val processName = tmpProcess[2]
                val uid_str = InjectTool.shell("id -u $UserName")
                val bigint = BigInteger(uid_str, 10)
                val uid = bigint.toInt()
                val pkgs = pm.getPackagesForUid(uid)
                var pkgName = ""
                var icon: Drawable? = null
                if (pkgs != null) {
                    pkgName = pkgs[0]
                    val applicationInfo = pm.getApplicationInfo(pkgName, PackageManager.GET_UNINSTALLED_PACKAGES)
                    icon =  applicationInfo.loadIcon(pm)
                }
                var currentRootNode: MutableList<BaseNode>? = null
                list.forEach {
                    var rootNode =  it as RootNode
                    if(rootNode.getUID() == uid){
                        currentRootNode = it.childNode
                    }
                }
                currentRootNode.let {
                    if (it != null) {
                        val seNode = SecondNode(pid, processName)
                        it.add(seNode)
                    }else{
                        val secondNodeList: MutableList<BaseNode> = ArrayList()
                        val seNode = SecondNode(pid, processName)
                        secondNodeList.add(seNode)
                        var entity = RootNode(secondNodeList, UserName, uid)
                        entity.pkgName = pkgName
                        entity.setIcon(icon)
                        list.add(entity)
                    }
                }
            }
            return list
        }
}