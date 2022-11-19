package hepta.rxposed.manager.fragment.PlugSupport;

import android.content.pm.ApplicationInfo;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;

import androidx.annotation.Nullable;

import com.chad.library.adapter.base.entity.node.BaseNode;

import java.util.HashMap;
import java.util.List;
import java.util.Map;

import hepta.rxposed.manager.RxposedApp;
import hepta.rxposed.manager.fragment.base.AppInfoDataProvider;
import hepta.rxposed.manager.fragment.base.AppInfoNode;
import hepta.rxposed.manager.fragment.base.ModuleInfo;
import hepta.rxposed.manager.fragment.base.ModuleInfoProvider;

public class SupportData extends ModuleInfoProvider<SupportData.SupportInfo> {



    private static SupportData _sInstance;


    public static SupportData getInstance() {
        if (_sInstance == null) {
            _sInstance = new SupportData();
        }
        return _sInstance;
    }


    public SupportData() {
        super( RxposedApp.getInstance().getFilesDir()+"/rxposed_support");
    }

    public class SupportInfo extends ModuleInfo {
        public SupportInfo(PackageInfo pkg, PackageManager mPm, Map<Integer, AppInfoNode> appInfoMap) {
            super(pkg, mPm, appInfoMap);
        }


        @Nullable
        @Override
        public List<BaseNode> getChildNode() {
            return null;
        }
    }



    @Override
    public Map<Integer, SupportInfo>  initModuelList() {
        Map<Integer, SupportInfo> map_modules = new HashMap<Integer, SupportInfo> ();
        for (PackageInfo pkg : RxposedApp.getInstance().getPackageManager().getInstalledPackages(PackageManager.GET_META_DATA)) {
            ApplicationInfo app = pkg.applicationInfo;
            if (!app.enabled)
                continue;
            //xposedmodule
            if (app.metaData != null && app.metaData.containsKey("rxposed_support")) {
                SupportInfo installed = new SupportInfo(pkg,RxposedApp.getInstance().getPackageManager(), AppInfoDataProvider.getInstance().getAllMapApps_module(pkg.packageName,RxposedApp.getInstance().getPackageManager()));
                map_modules.put(installed.getUID(),installed);
            }
        }
        return map_modules;
    }



    //    ByUidGetModuleInfo



}