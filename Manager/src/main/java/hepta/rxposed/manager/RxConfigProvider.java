package hepta.rxposed.manager;

import android.content.ContentProvider;
import android.content.ContentValues;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageManager;
import android.database.Cursor;
import android.net.Uri;
import android.os.Bundle;
import android.os.ParcelFileDescriptor;
import android.util.Log;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.io.File;
import java.io.FileNotFoundException;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import java.util.ListIterator;

import hepta.rxposed.manager.fragment.PlugSupport.FrameData;

public class RxConfigProvider extends ContentProvider {

    private String configName = "rxposed_config";

    @Override
    public boolean onCreate() {
        Log.e("com.rxposed.android", "RxConfigProvider : onCreate");
        return true;
    }




    @Nullable
    @Override
    public Cursor query(@NonNull Uri uri, @Nullable String[] projection, @Nullable String selection, @Nullable String[] selectionArgs, @Nullable String sortOrder) {
        return null;
    }

    @Nullable
    @Override
    public String getType(@NonNull Uri uri) {
        return null;
    }

    @Nullable
    @Override
    public Uri insert(@NonNull Uri uri, @Nullable ContentValues values) {
        return null;
    }

    @Override
    public int delete(@NonNull Uri uri, @Nullable String selection, @Nullable String[] selectionArgs) {
        return 0;
    }

    @Override
    public int update(@NonNull Uri uri, @Nullable ContentValues values, @Nullable String selection, @Nullable String[] selectionArgs) {
        return 0;
    }

    @Nullable
    @Override
    public ParcelFileDescriptor openFile(@NonNull Uri uri, @NonNull String mode) throws FileNotFoundException {
        File configNameFile =new File(getContext().getFilesDir(),configName);

        return ParcelFileDescriptor.open(configNameFile,
                ParcelFileDescriptor.MODE_READ_ONLY);
    }

    @Nullable
    @Override
    public Bundle call(@NonNull String method, @Nullable String ProcessName, @Nullable Bundle extras) {
        Bundle bundle = new Bundle();
        PackageManager pm =  RxposedApp.getInstance().getPackageManager();
        String json = FrameData.getInstance().readerJson();
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
            bundle.putString("enableUidList", "null");
        }else {
            bundle.putString("enableUidList", retString.toString());
        }

        Log.e("getRxConfig", "pkgName: "+ProcessName);
        Log.e("getRxConfig", "return: " + bundle.getString("enableUidList"));
        return bundle;
    }

}
