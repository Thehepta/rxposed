package hepta.rxposed.manager.fragment.extend;

import android.content.pm.ApplicationInfo;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;


import hepta.rxposed.manager.RxposedApp;
import hepta.rxposed.manager.fragment.apps.AppInfo;
import hepta.rxposed.manager.fragment.apps.AppInfoDataProvider;
import hepta.rxposed.manager.util.LogUtil;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileWriter;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.UnsupportedEncodingException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;


public class ModuleInfoProvider {

    private static ModuleInfoProvider _sInstance;
    private PackageManager mPm;
    private Map<Integer, ModuleInfo> map_modules = new HashMap<Integer, ModuleInfo>();

    public static ModuleInfoProvider getInstance() {
        if (_sInstance == null) {
            _sInstance = new ModuleInfoProvider();
        }
        return _sInstance;
    }
    private ModuleInfoProvider(){
        mPm = RxposedApp.getInstance().getPackageManager();
        initModuelList();
        readConfig();
    }

    private void initModuelList() {
        for (PackageInfo pkg : mPm.getInstalledPackages(PackageManager.GET_META_DATA)) {
            ApplicationInfo app = pkg.applicationInfo;
            if (!app.enabled)
                continue;
            if (app.metaData != null && app.metaData.containsKey("xposedmodule")) {
                ModuleInfo installed = new ModuleInfo(pkg,mPm, AppInfoDataProvider.getInstance().getAllMapApps_module(pkg.packageName));
                map_modules.put(installed.getUID(),installed);
            }
        }
    }

    public List<ModuleInfo> getModuleList(){
        List<ModuleInfo> result = new ArrayList(map_modules.values());
        return result;
    }

    public ModuleInfo ByUidGetModuleInfo(int uid){

        ModuleInfo moduleInfo =  map_modules.get(uid);
        return moduleInfo;
    }

    private JSONObject rxposedModulejson = new JSONObject();
    private static String ConfigPath = RxposedApp.getInstance().getFilesDir()+"/rxposed_modules";

    public void readConfig() {
        try {
            String json = readerJson();
            if(json !=null){
                JSONObject parseobj = new JSONObject(json);
                for (Map.Entry<Integer, ModuleInfo> entry : map_modules.entrySet()) {
                    ModuleInfo moduleInfo = entry.getValue();
                    JSONObject json_moduleInfo = parseobj.getJSONObject(String.valueOf(moduleInfo.getUID()));
                    JSONArray enabelAppUidList =  json_moduleInfo.getJSONArray("EnableProcUid");
                    moduleInfo.enable = json_moduleInfo.getBoolean("enable");

                    for(int i = 0; i < enabelAppUidList.length(); i ++) {
                        int enableAppUid= (int) enabelAppUidList.get(i);
                        moduleInfo.getAppInfoMaps().get(enableAppUid).enable = true;
                    }
                }
            }else{
                LogUtil.LogD("readConfig content == null");
            }
        } catch (JSONException e) {
            e.printStackTrace();
        }

    }

    public static String readerJson() {
        try {
            File file = getConfigFile();
            if (file.isFile() && file.exists()) {
                InputStreamReader read = new InputStreamReader(new FileInputStream(file), "UTF-8");
                BufferedReader bufferedReader = new BufferedReader(read);
                String lineTxt = bufferedReader.readLine();
                while (lineTxt != null) {
                    return lineTxt;
                }
            }
        } catch (UnsupportedEncodingException | FileNotFoundException e) {
            LogUtil.LogE("Cannot find the file specified!");
            e.printStackTrace();
        } catch (IOException e) {
            LogUtil.LogE("Error reading file content!");
            e.printStackTrace();
        }
        return null;
    }



    public void UpdateConfig() {
        for (Map.Entry<Integer,ModuleInfo> entry : map_modules.entrySet()) {
            ModuleInfo moduleInfo = entry.getValue();
            JSONArray appList = addmodule(moduleInfo.getUID(),moduleInfo.getEnable());
            for(AppInfo appInfo : moduleInfo.getAppInfoList()){
                if(appInfo.getEnable()){
                    appList.put(appInfo.getUID());
                }
            }
        }
        LogUtil.LogD(rxposedModulejson.toString());
        File config = getConfigFile();
        try {
            FileWriter fw = new FileWriter(config);
            fw.write("");
            fw.flush();
            fw.write(rxposedModulejson.toString());
            fw.flush();
            fw.close();
        } catch (IOException e) {
            e.printStackTrace();
        }




    }



    public static File getConfigFile() {
        File config = new File (ConfigPath);
        try {
            if (!config.exists()) { //用来测试此路径名表示的文件或目录是否存在
                if (!config.getParentFile().exists()) {
                    //创建上级目录
                    config.getParentFile().mkdirs();
                }
                //在上级目录里创建文件
                config.createNewFile();
            }
        } catch (IOException e) {
            e.printStackTrace();
        }

        return config;
    }

    public JSONArray addmodule(int uid, boolean enable){

        JSONArray appobject = new JSONArray();
        try {
            JSONObject moduleInfo = new JSONObject();
            moduleInfo.put("enable",enable);
            moduleInfo.put("EnableProcUid",appobject);
            rxposedModulejson.put(String.valueOf(uid),moduleInfo);
        } catch (JSONException e) {
            e.printStackTrace();
        }
        return appobject;
    }




}
