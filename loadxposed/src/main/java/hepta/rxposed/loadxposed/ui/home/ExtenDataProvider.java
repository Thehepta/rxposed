package hepta.rxposed.loadxposed.ui.home;

import android.content.pm.ApplicationInfo;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;

import androidx.annotation.Nullable;

import com.chad.library.adapter.base.entity.node.BaseNode;

import java.util.HashMap;
import java.util.List;
import java.util.Map;

import hepta.rxposed.loadxposed.MyApp;

public class ExtenDataProvider extends ModuleInfoProvider<ExtenDataProvider.ExtendInfo> {



    private static ExtenDataProvider _sInstance;


    public static ExtenDataProvider getInstance() {
        if (_sInstance == null) {
            _sInstance = new ExtenDataProvider();
        }
        return _sInstance;
    }


    public ExtenDataProvider() {
        super( MyApp.getInstance().getFilesDir()+"/rxposed_modules");
    }

    public class ExtendInfo extends ModuleInfo {


        public int Dependuid = 0;

        public ExtendInfo(PackageInfo pkg, PackageManager mPm, Map<Integer, AppInfoNode> appInfoMap) {
            super(pkg, mPm, appInfoMap);
        }


        @Override
        public void setEnable(boolean enable) {
            super.setEnable(enable);
            _sInstance.UpdateConfig();
        }

        @Nullable
        @Override
        public List<BaseNode> getChildNode() {
            return super.getChildNode();
        }
    }

    @Override
    public Map<Integer, ExtendInfo>  initModuelList() {
        Map<Integer, ExtendInfo> map_modules = new HashMap<Integer, ExtendInfo> ();
        for (PackageInfo pkg : MyApp.getInstance().getPackageManager().getInstalledPackages(PackageManager.GET_META_DATA)) {
            ApplicationInfo app = pkg.applicationInfo;
            if (!app.enabled)
                continue;
            //xposedmodule
            if (app.metaData != null && app.metaData.containsKey("xposedmodule")) {
                ExtendInfo installed = new ExtendInfo(pkg,MyApp.getInstance().getPackageManager(), AppInfoDataProvider.getInstance().getAllMapApps_module(pkg.packageName,MyApp.getInstance().getPackageManager()));
                map_modules.put(installed.getUID(),installed);
            }
        }
        return map_modules;
    }





}