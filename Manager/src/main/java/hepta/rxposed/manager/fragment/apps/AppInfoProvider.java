package hepta.rxposed.manager.fragment.apps;

import android.content.Context;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.graphics.drawable.Drawable;


import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import hepta.rxposed.manager.RxposedApp;

public class AppInfoProvider {

    private static AppInfoProvider _sInstance;

    private Map<Integer, AppInfo> map_AppInfos = new HashMap<Integer, AppInfo>();
    private PackageManager packageManager;
    //获取一个包管理器
    public AppInfoProvider(){
        packageManager = RxposedApp.getInstance().getPackageManager();
        IninApps();
    }
    /**
     *获取系统中所有应用信息，
     *并将应用软件信息保存到list列表中。
     **/
    public void IninApps(){
        List<AppInfo> list = new ArrayList<AppInfo>();
        AppInfo myAppInfo;
        List<PackageInfo> packageInfos = packageManager.getInstalledPackages(0);
        for(PackageInfo info:packageInfos){
            myAppInfo = new AppInfo();
            //拿到包名
            String packageName = info.packageName;
            //拿到应用程序的信息
            ApplicationInfo appInfo = info.applicationInfo;
            //拿到应用程序的图标
            Drawable icon = appInfo.loadIcon(packageManager);
            //拿到应用程序的大小
            //long codesize = packageStats.codeSize;
            //Log.i("info", "-->"+codesize);
            //拿到应用程序的程序名
            String appName = appInfo.loadLabel(packageManager).toString();
            myAppInfo.setPackageName(packageName);
            myAppInfo.setAppName(appName);
            myAppInfo.setIcon(icon);
            myAppInfo.setAppInfo(appInfo);

            if(filterApp(appInfo)){
                myAppInfo.setSystemApp(false);
                map_AppInfos.put(myAppInfo.getUID(),myAppInfo);
                list.add(myAppInfo);
            }else{
                myAppInfo.setSystemApp(true);
            }
        }
    }

    public List<AppInfo> getAllListApps(){
        List<AppInfo> result = new ArrayList(map_AppInfos.values());
        return result;
    }


    public Map<Integer, AppInfo> getAllMapApps_module(String module_packageName){

        Map<Integer,AppInfo> mapAppInfos = new HashMap<>();
        AppInfo myAppInfo;
        List<PackageInfo> packageInfos = packageManager.getInstalledPackages(0);
        for(PackageInfo info:packageInfos){
            myAppInfo = new AppInfo();
            //拿到包名
            String packageName = info.packageName;
            //拿到应用程序的信息
            ApplicationInfo appInfo = info.applicationInfo;
            //拿到应用程序的图标
            Drawable icon = appInfo.loadIcon(packageManager);
            //拿到应用程序的大小
            //long codesize = packageStats.codeSize;
            //Log.i("info", "-->"+codesize);
            //拿到应用程序的程序名
            String appName = appInfo.loadLabel(packageManager).toString();
            myAppInfo.setPackageName(packageName);
            myAppInfo.setAppName(appName);
            myAppInfo.setIcon(icon);
            myAppInfo.setAppInfo(appInfo);
            myAppInfo.setModuleName(module_packageName);
            if(filterApp(appInfo)){
                myAppInfo.setSystemApp(false);

            }else{
                myAppInfo.setSystemApp(true);
            }
            mapAppInfos.put(myAppInfo.getUID(),myAppInfo);
        }

        return mapAppInfos;
    }

    public List<AppInfo> getSystemApps(){
        List<AppInfo> list = new ArrayList<AppInfo>();
        AppInfo myAppInfo;
        List<PackageInfo> packageInfos = packageManager.getInstalledPackages(0);
        for(PackageInfo info:packageInfos){
            ApplicationInfo appInfo = info.applicationInfo;
            if(!appInfo.enabled)
                continue;
            myAppInfo = new AppInfo();
            //拿到包名
            String packageName = info.packageName;
            //拿到应用程序的图标
            Drawable icon = appInfo.loadIcon(packageManager);
            //拿到应用程序的大小
            //long codesize = packageStats.codeSize;
            //Log.i("info", "-->"+codesize);
            //拿到应用程序的程序名
            String appName = appInfo.loadLabel(packageManager).toString();
            myAppInfo.setPackageName(packageName);
            myAppInfo.setAppName(appName);
            myAppInfo.setIcon(icon);
            myAppInfo.setAppInfo(appInfo);

            if(filterApp(appInfo)){
                myAppInfo.setSystemApp(true);
            }else{
                myAppInfo.setSystemApp(false);
            }
            list.add(myAppInfo);
        }
        return list;
    }


    /**
     *判断某一个应用程序是不是系统的应用程序，
     *如果是返回true，否则返回false。
     */
    public boolean filterApp(ApplicationInfo info){
        //有些系统应用是可以更新的，如果用户自己下载了一个系统的应用来更新了原来的，它还是系统应用，这个就是判断这种情况的
        if((info.flags & ApplicationInfo.FLAG_UPDATED_SYSTEM_APP) != 0){
            return true;
        }else if((info.flags & ApplicationInfo.FLAG_SYSTEM) != 0){//判断是不是系统应用
            return true;
        }
        return false;
    }


    public  boolean isSystemApplication(String packageName) {
        try {
            final PackageInfo packageInfo = packageManager.getPackageInfo(packageName, PackageManager.GET_CONFIGURATIONS);
            if ((packageInfo.applicationInfo.flags & ApplicationInfo.FLAG_SYSTEM) != 0) {
                return true;
            }
        } catch (PackageManager.NameNotFoundException e) {
            e.printStackTrace();
        }
        return false;

    }

    public static AppInfoProvider getInstance() {
        if (_sInstance == null) {
            _sInstance = new AppInfoProvider();
        }
        return _sInstance;
    }
}