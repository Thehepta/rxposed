package hepta.rxposed.manager.fragment.PlugInject;

import android.content.pm.ApplicationInfo;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.util.ArrayMap;

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

public class SupportInfoProvider extends ModuleInfoProvider<SupportInfoProvider.SupportInfo> {



    private static SupportInfoProvider _sInstance;


    public static SupportInfoProvider getInstance() {
        if (_sInstance == null) {
            _sInstance = new SupportInfoProvider();
        }
        return _sInstance;
    }


    public SupportInfoProvider() {
        super( RxposedApp.getInstance().getFilesDir()+"/rxposed_support");
    }

    public class SupportInfo extends ModuleInfo {

        public Map<Integer, ModuleInfo> depondsList = new ArrayMap<Integer, ModuleInfo>();

        public SupportInfo(PackageInfo pkg, PackageManager mPm, Map<Integer, AppInfoNode> appInfoMap) {
            super(pkg, mPm, appInfoMap);
        }

        public void setAppDepend(boolean enable, int uid, ModuleInfo moduleInfo) {
            if(enable){
                depondsList.put(uid,moduleInfo);
            }else {
                depondsList.remove(uid);
            }
            _sInstance.UpdateConfig();
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