package hepta.rxposed.manager.fragment.LoadExten

import android.content.Intent
import android.content.pm.ApplicationInfo
import android.content.pm.PackageManager
import android.content.pm.ResolveInfo
import hepta.rxposed.manager.RxposedApp

class SingApplist {




    companion object {


        private var instance: SingApplist? = null
            //这里使用的是自定义访问器
            get() {
                if (field == null) {
                    field = SingApplist()
                }
                return field
            }

        fun get(): SingApplist {
            return instance!!
        }
    }


    public var global_applist:List<ApplicationInfo>? = null
        get(){
            if (field == null) {
                field = getallapp()
            }
            return field
        }

    private fun getallapp(): List<ApplicationInfo>? {
        var applist = mutableListOf<ApplicationInfo>()
        var mPm = RxposedApp.getInstance().packageManager
        val intent = Intent()
        intent.action = Intent.ACTION_MAIN
        intent.addCategory(Intent.CATEGORY_LAUNCHER)

        val resolveInfos: List<ResolveInfo> = mPm.queryIntentActivities(intent, PackageManager.MATCH_ALL)
        for (info in resolveInfos) {
            applist.add(info.activityInfo.applicationInfo)
        }
        return applist

    }


}