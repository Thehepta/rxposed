package hepta.rxposed.manager.fragment.extend;

import android.content.pm.ApplicationInfo;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;

import hepta.rxposed.manager.RxposedApp;
import hepta.rxposed.manager.fragment.apps.AppInfo;
import hepta.rxposed.manager.fragment.apps.AppInfoDataProvider;
import hepta.rxposed.manager.fragment.base.AppModuleInfo;
import hepta.rxposed.manager.fragment.base.AppModuleInfoProvider;

import java.util.HashMap;
import java.util.Map;

public class ModuleData extends AppModuleInfoProvider<ModuleData.Modules> {



    private static ModuleData _sInstance;


    public static ModuleData getInstance() {
        if (_sInstance == null) {
            _sInstance = new ModuleData();
        }
        return _sInstance;
    }


    public ModuleData() {
        super( RxposedApp.getInstance().getFilesDir()+"/rxposed_modules");
    }

    public class Modules extends AppModuleInfo {
        public Modules(PackageInfo pkg, PackageManager mPm, Map<Integer, AppInfo> appInfoMap) {
            super(pkg, mPm, appInfoMap);
        }
    }

    @Override
    public Map<Integer, Modules>  initModuelList() {
        Map<Integer, Modules> map_modules = new HashMap<Integer, Modules> ();
        for (PackageInfo pkg : RxposedApp.getInstance().getPackageManager().getInstalledPackages(PackageManager.GET_META_DATA)) {
            ApplicationInfo app = pkg.applicationInfo;
            if (!app.enabled)
                continue;
            //xposedmodule
            if (app.metaData != null && app.metaData.containsKey("xposedmodule")) {
                Modules installed = new Modules(pkg,RxposedApp.getInstance().getPackageManager(), AppInfoDataProvider.getInstance().getAllMapApps_module(pkg.packageName,RxposedApp.getInstance().getPackageManager()));
                map_modules.put(installed.getUID(),installed);
            }
        }
        return map_modules;
    }



    //    ByUidGetModuleInfo



}