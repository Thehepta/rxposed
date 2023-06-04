package hepta.rxposed.manager.fragment.LoadExten;

import android.content.pm.ApplicationInfo;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;

import androidx.annotation.Nullable;

import com.chad.library.adapter.base.entity.node.BaseNode;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import hepta.rxposed.manager.RxposedApp;
import hepta.rxposed.manager.fragment.base.AppInfoDataProvider;
import hepta.rxposed.manager.fragment.base.AppInfoNode;
import hepta.rxposed.manager.fragment.base.ModuleInfo;
import hepta.rxposed.manager.fragment.base.ModuleInfoProvider;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.ListIterator;
import java.util.Map;

public class ExtenInfoProvider extends ModuleInfoProvider<ExtenInfoProvider.ExtendInfo> {



    private static ExtenInfoProvider _sInstance;


    public static ExtenInfoProvider getInstance() {
        if (_sInstance == null) {
            _sInstance = new ExtenInfoProvider();
        }
        return _sInstance;
    }


    public ExtenInfoProvider() {
        super( RxposedApp.getInstance().getFilesDir()+"/rxposed_modules");
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
        for (PackageInfo pkg : RxposedApp.getInstance().getPackageManager().getInstalledPackages(PackageManager.GET_META_DATA)) {
            ApplicationInfo app = pkg.applicationInfo;
            if (!app.enabled)
                continue;
            //xposedmodule
            if (app.metaData != null && app.metaData.containsKey("rxmodule")) {
                ExtendInfo installed = new ExtendInfo(pkg,RxposedApp.getInstance().getPackageManager(), AppInfoDataProvider.getInstance().getAllMapApps_module(pkg.packageName,RxposedApp.getInstance().getPackageManager()));
                map_modules.put(installed.getUID(),installed);
            }
        }
        return map_modules;
    }

    public List<Integer>  getConfigToUidList(String ProcessName,PackageManager pm){


        String json = readerJson();
        List<Integer> uidlist = new ArrayList<>();
        try {
            ApplicationInfo info = pm.getApplicationInfo(ProcessName, PackageManager.GET_UNINSTALLED_PACKAGES);
            if(json !=null) {
                try {
                    JSONObject parseobj = new JSONObject(json);
                    Iterator<String> iterator = parseobj.keys();
                    while(iterator.hasNext()){
                        String key =  iterator.next();
                        JSONObject value = parseobj.getJSONObject(key);
                        boolean enable = value.getBoolean("enable");
                        if(!enable){
                            continue;
                        }
                        JSONArray EnableProcUidList = value.getJSONArray("EnableProcUid");
                        for(int i = 0; i < EnableProcUidList.length(); i ++) {
                            int enableAppUid= EnableProcUidList.getInt(i);
                            if (enableAppUid == info.uid){
                                uidlist.add(Integer.valueOf(key));
                            }
                        }
                    }
                } catch (JSONException e) {
                    e.printStackTrace();
                }
            }
        } catch (PackageManager.NameNotFoundException e) {
            e.printStackTrace();
        }

        return uidlist;
    }



}