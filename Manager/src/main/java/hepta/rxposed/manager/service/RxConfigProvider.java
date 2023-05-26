package hepta.rxposed.manager.service;

import android.content.ContentProvider;
import android.content.ContentValues;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageManager;
import android.database.Cursor;
import android.database.MatrixCursor;
import android.net.Uri;
import android.os.Bundle;
import android.os.ParcelFileDescriptor;
import android.util.Log;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import java.io.File;
import java.io.FileNotFoundException;
import java.util.ArrayList;
import java.util.List;

import hepta.rxposed.manager.RxposedApp;
import hepta.rxposed.manager.fragment.LoadExten.ExtenInfoProvider;

public class RxConfigProvider extends ContentProvider {

    private String configName = "rxposed_config";

    @Override
    public boolean onCreate() {
        Log.e("hepta.rxposed.manager", "RxConfigProvider : onCreate");
        return true;
    }


    @Nullable
    @Override
    public Cursor query(@NonNull Uri uri, @Nullable String[] projection, @Nullable String selection, @Nullable String[] selectionArgs, @Nullable String sortOrder) {
        String[] columnNames = {"_id", "name", "age"};

// 创建MatrixCursor对象，并指定列名数组
        MatrixCursor cursor = new MatrixCursor(columnNames);

// 添加数据行
// 请注意，"_id"列是一个必需的，它在每一行中标识唯一的行ID
        cursor.addRow(new Object[]{1, "John Doe", 25});
        cursor.addRow(new Object[]{2, "Jane Smith", 30});
        cursor.addRow(new Object[]{3, "Bob Johnson", 40});
        return cursor;
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
        Log.e("getRxConfig","method:"+method);
        Log.e("getRxConfig","ProcessName:"+ProcessName);
        Log.e("getCallingPackage","getCallingPackage:"+getCallingPackage());


        bundle.putString("enableUidList", "fewfewfewfewfewfewfew");
//        if(method.equals("appinfo")){
//
//            try {
//                 ApplicationInfo applicationInfo =  RxposedApp.getInstance().getBaseContext().getPackageManager().getApplicationInfo(ProcessName, PackageManager.GET_META_DATA);
//                 String apk_path = applicationInfo.sourceDir;
//                 String apk_native_lib = applicationInfo.nativeLibraryDir;
//                 String entry_class = applicationInfo.metaData.getString("rxposed_clsentry");
//                 String entry_method = applicationInfo.metaData.getString("rxposed_mtdentry");
//                bundle.putString("apk_path", apk_path);
//                bundle.putString("apk_native_lib", apk_native_lib);
//                bundle.putString("rxposed_clsentry", entry_class);
//                bundle.putString("rxposed_mtdentry", entry_method);
//                return bundle;
//            } catch (PackageManager.NameNotFoundException e) {
//                throw new RuntimeException(e);
//            }
//
//        }else {
        String callName = getCallingPackage();
        List<Integer> uidList = ExtenInfoProvider.getInstance().getConfigToUidList(callName,getContext().getPackageManager());
        ArrayList<String> stringList = new ArrayList<>();
        int i = 0;
        for(int uid:uidList ){
            i++;
            try {
                ApplicationInfo applicationInfo =  RxposedApp.getInstance().getBaseContext().getPackageManager().getApplicationInfo(ProcessName, PackageManager.GET_META_DATA);
                String apk_path = applicationInfo.sourceDir;
                String entry_class = applicationInfo.metaData.getString("rxposed_clsentry");
                String entry_method = applicationInfo.metaData.getString("rxposed_mtdentry");
                stringList.add(apk_path+":"+entry_class+":"+entry_method);
            } catch (PackageManager.NameNotFoundException e) {
                throw new RuntimeException(e);
            }
        }
        bundle.putStringArrayList("ModuleList", stringList);
        return bundle;
    }

}
