package hepta.rxposed.manager.fragment.base;

import android.content.pm.ApplicationInfo;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;

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

import hepta.rxposed.manager.RxposedApp;
import hepta.rxposed.manager.fragment.apps.AppInfo;
import hepta.rxposed.manager.fragment.apps.AppInfoDataProvider;
import hepta.rxposed.manager.fragment.extend.ModuleData;
import hepta.rxposed.manager.util.LogUtil;


public abstract class AppModuleInfoProvider<T extends AppModuleInfo> {

    private Map<Integer, T> map_modules ;
    private String ConfigPath ;


    public AppModuleInfoProvider(String ConfigPath){
        this.ConfigPath = ConfigPath;
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
            LogUtil.LogE("Cannot find the file specified!");
            e.printStackTrace();
        } catch (IOException e) {
            LogUtil.LogE("Error reading file content!");
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

}
