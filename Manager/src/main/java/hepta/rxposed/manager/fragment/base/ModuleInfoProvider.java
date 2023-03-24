package hepta.rxposed.manager.fragment.base;

import android.content.pm.ApplicationInfo;
import android.content.pm.PackageManager;
import android.util.Log;

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
import java.util.Iterator;
import java.util.List;
import java.util.ListIterator;
import java.util.Map;
import java.util.Objects;

import hepta.rxposed.manager.RxposedApp;
import hepta.rxposed.manager.util.Util;


public abstract class ModuleInfoProvider<T extends ModuleInfo> {

    private String TAG = "ModuleInfoProvider";
    private Map<Integer, T> map_modules ;
    private String ConfigPath ;
    private JSONObject rxposedModulejson = new JSONObject();

    public ModuleInfoProvider(String ConfigPath){
        this.ConfigPath = ConfigPath;
    }
    public void init(){
        map_modules = initModuelList();
        readConfig();
    }

    public abstract Map<Integer, T>  initModuelList();

    public List<T> getModuleList(){
        List<T> result = new ArrayList(map_modules.values());
        return result;
    }

    public T ByUidGetModuleInfo(int uid){
        T moduleInfo =  map_modules.get(uid);
        return moduleInfo;
    }


    public void readConfig() {
        try {
            String json = readerJson();
            if(json !=null){
                JSONObject parseobj = new JSONObject(json);
                for (Map.Entry<Integer, T> entry : map_modules.entrySet()) {
                    T moduleInfo = entry.getValue();
                    Log.e(TAG,"module Name:"+moduleInfo.getAppName()+" uid:"+moduleInfo.getUID());
                    String uid_str = String.valueOf(moduleInfo.getUID());
                    if(parseobj.has(uid_str)){
                        JSONObject json_moduleInfo = parseobj.getJSONObject(uid_str);
                        JSONArray enabelAppUidList =  json_moduleInfo.getJSONArray("EnableProcUid");
                        moduleInfo.enable = json_moduleInfo.getBoolean("enable");
                        for(int i = 0; i < enabelAppUidList.length(); i ++) {
                            int enableAppUid= (int) enabelAppUidList.get(i);
                            AppInfoNode enableAppUid_AppInfoNode = moduleInfo.getAppInfoMaps().get(enableAppUid);
                            if(enableAppUid_AppInfoNode == null){
                                continue;
                            }else {
                                enableAppUid_AppInfoNode.enable = true;
                            }
                        }
                    }else {
                        continue;
                    }
                }
            }else{
                Util.LogD("readConfig content == null");
            }
        } catch (JSONException e) {
            e.printStackTrace();
        }

    }

    public String readerJson() {
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
            Util.LogE("Cannot find the file specified!");
            e.printStackTrace();
        } catch (IOException e) {
            Util.LogE("Error reading file content!");
            e.printStackTrace();
        }
        return null;
    }


    public File getConfigFile() {
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


    public void UpdateConfig() {
        for (Map.Entry<Integer, T> entry : map_modules.entrySet()) {
            T moduleInfo = entry.getValue();
            JSONArray appList = addmodule(moduleInfo.getUID(),moduleInfo.getEnable());
            for(AppInfoNode appInfo : moduleInfo.getAppInfoList()){
                if(appInfo.getEnable()){
                    appList.put(appInfo.getUID());
                }
            }
        }
        Util.LogD(rxposedModulejson.toString());
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

    public String  getConfigToString(String ProcessName,PackageManager pm){


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
        ListIterator<Integer> it = uidlist.listIterator();
        StringBuilder retString = new StringBuilder();

        while(it.hasNext()) {
            String pkgName = pm.getNameForUid(it.next());
            try {
                ApplicationInfo applicationInfo = pm.getApplicationInfo(pkgName,PackageManager.GET_UNINSTALLED_PACKAGES|PackageManager.GET_META_DATA);
                String entry_class  = applicationInfo.metaData.getString("rxposed_clsentry");
                String entry_method = applicationInfo.metaData.getString("rxposed_mtdentry");
                retString.append(applicationInfo.packageName+":"+entry_class+":"+entry_method);
                if(it.hasNext()){
                    retString.append("|");
                }
            } catch (PackageManager.NameNotFoundException e) {
                e.printStackTrace();
            }
        }
        if(retString.toString()==""){
            return "null";
        }else {

            return retString.toString();
        }
    }


}
