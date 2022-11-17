package hepta.rxposed.manager.fragment.depends;

import android.content.pm.ApplicationInfo;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;

import java.util.HashMap;
import java.util.Map;

import hepta.rxposed.manager.RxposedApp;
import hepta.rxposed.manager.fragment.apps.AppInfo;
import hepta.rxposed.manager.fragment.apps.AppInfoDataProvider;
import hepta.rxposed.manager.fragment.base.AppModuleInfo;
import hepta.rxposed.manager.fragment.base.AppModuleInfoProvider;

public class FrameData extends AppModuleInfoProvider<FrameData.Frames> {



    private static FrameData _sInstance;


    public static FrameData getInstance() {
        if (_sInstance == null) {
            _sInstance = new FrameData();
        }
        return _sInstance;
    }


    public FrameData() {
        super( RxposedApp.getInstance().getFilesDir()+"/rxposed_modules");
    }

    public class Frames extends AppModuleInfo {
        public Frames(PackageInfo pkg, PackageManager mPm, Map<Integer, AppInfo> appInfoMap) {
            super(pkg, mPm, appInfoMap);
        }
    }

    @Override
    public Map<Integer, Frames>  initModuelList() {
        Map<Integer, Frames> map_modules = new HashMap<Integer, Frames> ();
        for (PackageInfo pkg : RxposedApp.getInstance().getPackageManager().getInstalledPackages(PackageManager.GET_META_DATA)) {
            ApplicationInfo app = pkg.applicationInfo;
            if (!app.enabled)
                continue;
            //xposedmodule
            if (app.metaData != null && app.metaData.containsKey("xposedmodule")) {
                Frames installed = new Frames(pkg,RxposedApp.getInstance().getPackageManager(), AppInfoDataProvider.getInstance().getAllMapApps_module(pkg.packageName,RxposedApp.getInstance().getPackageManager()));
                map_modules.put(installed.getUID(),installed);
            }
        }
        return map_modules;
    }



    //    ByUidGetModuleInfo



}