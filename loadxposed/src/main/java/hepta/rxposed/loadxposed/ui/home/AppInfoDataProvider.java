package hepta.rxposed.loadxposed.ui.home;

import android.content.pm.ApplicationInfo;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.graphics.drawable.Drawable;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class AppInfoDataProvider {

    private static AppInfoDataProvider _sInstance;

    private Map<Integer, AppInfoNode> map_AppInfos = new HashMap<Integer, AppInfoNode>();
    private PackageManager packageManager;
    //获取一个包管理器
    public AppInfoDataProvider(){

    }


    public List<AppInfoNode> getAllListApps(){
        List<AppInfoNode> result = new ArrayList(map_AppInfos.values());
        return result;
    }


    public Map<Integer, AppInfoNode> getAllMapApps_module(String module_packageName, PackageManager packageManager){

        Map<Integer, AppInfoNode> mapAppInfos = new HashMap<>();
        AppInfoNode myAppInfo;
        List<PackageInfo> packageInfos = packageManager.getInstalledPackages(0);
        for(PackageInfo info:packageInfos){
            myAppInfo = new AppInfoNode();
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



    public static AppInfoDataProvider getInstance() {
        if (_sInstance == null) {
            _sInstance = new AppInfoDataProvider();
        }
        return _sInstance;
    }
}