package hepta.rxposed.manager.service;

import android.content.pm.ApplicationInfo;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.graphics.drawable.Drawable;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import hepta.rxposed.manager.RxposedApp;
import hepta.rxposed.manager.fragment.base.AppInfoNode;

public class AppInfoSingleProvider {

    private static AppInfoSingleProvider _sInstance;

    private Map<Integer, PackageInfo> map_user_AppInfos = new HashMap<Integer, PackageInfo>();
    private Map<Integer, PackageInfo> map_System_AppInfos = new HashMap<Integer, PackageInfo>();
    private PackageManager packageManager;
    //获取一个包管理器
    public AppInfoSingleProvider(){
        initAppList(RxposedApp.getInstance().getPackageManager());
    }

    private void initAppList(PackageManager packageManager) {
        List<PackageInfo> packageInfos = packageManager.getInstalledPackages(0);
        for(PackageInfo info:packageInfos) {
            if(filterApp(info.applicationInfo)){
                map_user_AppInfos.put(info.applicationInfo.uid,info);
            }else{
                map_System_AppInfos.put(info.applicationInfo.uid,info);
            }
        }
    }

    public List<PackageInfo> getAllListApps(){
        List<PackageInfo> result = new ArrayList(map_user_AppInfos.values());
        result.addAll(map_System_AppInfos.values());
        return result;
    }

    public List<PackageInfo> getSystemApps(){
        List<PackageInfo> result = new ArrayList(map_System_AppInfos.values());
        return result;
    }
    public List<PackageInfo> getUsertApps(){
        List<PackageInfo> result = new ArrayList(map_user_AppInfos.values());
        return result;
    }

    public void UpdateAppList(PackageManager packageManager) {
        map_user_AppInfos.clear();
        map_System_AppInfos.clear();
        List<PackageInfo> packageInfos = packageManager.getInstalledPackages(0);
        for(PackageInfo info:packageInfos) {
            if(filterApp(info.applicationInfo)){
                map_user_AppInfos.put(info.applicationInfo.uid,info);

            }else{
                map_System_AppInfos.put(info.applicationInfo.uid,info);
            }
        }
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



    public static AppInfoSingleProvider getInstance() {
        if (_sInstance == null) {
            _sInstance = new AppInfoSingleProvider();
        }
        return _sInstance;
    }
}