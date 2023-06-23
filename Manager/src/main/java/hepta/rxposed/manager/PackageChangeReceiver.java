package hepta.rxposed.manager;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageManager;
import android.util.Log;

import java.util.Map;

import hepta.rxposed.manager.util.MmkvManager;

public class PackageChangeReceiver extends BroadcastReceiver {

    @Override
    public void onReceive(Context context, Intent intent){
        //接收安装广播
        if (intent.getAction().equals("android.intent.action.ACTION_PACKAGE_CHANGED")) {
            String packageName = intent.getDataString();
            Log.e("rzx","ACTION_PACKAGE_CHANGED:" +packageName);

        }
        if (intent.getAction().equals("android.intent.action.PACKAGE_ADDED")) {
            String packageName = intent.getDataString();
            Log.e("rzx","安装了:" +packageName + "包名的程序");
            try {
                ApplicationInfo app = context.getPackageManager().getApplicationInfo(packageName, PackageManager.GET_META_DATA);
                if (app.metaData != null && app.metaData.containsKey("rxmodule")) {
                    Map<String,Boolean> Modules =  MmkvManager.INSTANCE.getModuleList();
                    Modules.put(app.packageName,false);
                }
            } catch (PackageManager.NameNotFoundException e) {
                throw new RuntimeException(e);
            }
        }
        //接收卸载广播
        if (intent.getAction().equals("android.intent.action.PACKAGE_REMOVED")) {
//            String packageName = intent.getDataString();
//            Map<String,Boolean> Modules =  MmkvManager.INSTANCE.getModuleList();
//            if(Modules.containsKey(packageName)){
//                Modules.remove(packageName);
//                MmkvManager.INSTANCE.setModuleList(Modules);
//            }
        }
    }
}
