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
            try {
                Thread.sleep(5000);
            } catch (InterruptedException e) {
                throw new RuntimeException(e);
            }
            String packageName = intent.getDataString();
            Log.e("rzx","安装了:" +packageName + "包名的程序");
            try {
                ApplicationInfo app = context.getPackageManager().getApplicationInfo(packageName, PackageManager.GET_META_DATA);
//                Log.e("rzx",app.sourceDir);
//                if (app.metaData != null && app.metaData.containsKey("rxmodule")) {
//                    Map<String,Boolean> Modules =  MmkvManager.INSTANCE.getModuleList();
//                    Modules.put(app.packageName,false);
//                }
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
//class AppUninstalledBroadcastReceiver : BroadcastReceiver() {
//
//private var appUninstallCallbacks: AppUninstallCallbacks? = null
//
//        override fun onReceive(context: Context?, intent: Intent?) {
//        Log.d("AppUninstalled", "onReceive: ${intent?.action}")
//
//        when (intent?.action) {
//        Intent.ACTION_PACKAGE_REMOVED,
//        Intent.ACTION_PACKAGE_FULLY_REMOVED,
//        IntentConstants.ACTION_APP_UNINSTALLED -> {
//        appUninstallCallbacks?.onAppUninstalled(intent.data?.schemeSpecificPart)
//        Log.d("AppUninstalled", "onReceive: app removed ${intent.data?.schemeSpecificPart}")
//        }
//
//        Intent.ACTION_PACKAGE_ADDED -> {
//        Log.d("AppUninstalled", "onReceive: app added ${intent.data?.schemeSpecificPart}")
//        appUninstallCallbacks?.onAppInstalled(intent.data?.schemeSpecificPart)
//        }
//
//        Intent.ACTION_PACKAGE_REPLACED -> {
//        Log.d("AppUninstalled", "onReceive: New app added ${intent.data?.schemeSpecificPart}")
//        appUninstallCallbacks?.onAppReplaced(intent.data?.schemeSpecificPart)
//        }
//        }
//        }
//
//        fun setAppUninstallCallbacks(appUninstallCallbacks: AppUninstallCallbacks) {
//        this.appUninstallCallbacks = appUninstallCallbacks
//        }
//        }