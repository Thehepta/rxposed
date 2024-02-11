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

import com.tencent.mmkv.MMKV;

import java.io.File;
import java.io.FileNotFoundException;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;

import hepta.rxposed.manager.RxposedApp;
import hepta.rxposed.manager.util.MmkvManager;

public class RxConfigProvider extends ContentProvider {

    private String configName = "rxposed_config";
    String TAG = RxConfigProvider.class.getName();
    @Override
    public boolean onCreate() {
        MMKV.initialize(this.getContext());
        Log.e(TAG, "onCreate");
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
    public Bundle call(@NonNull String method, @Nullable String uid, @Nullable Bundle extras) {
        Bundle bundle = new Bundle();
        Log.e("getRxConfig","method:"+method);
        Log.e("getRxConfig","ProcessName:"+uid);

        String req_packageName = getContext().getPackageManager().getNameForUid(new Integer(uid));
        List<String> enableModuleList = MmkvManager.INSTANCE.getAppEnableModuleList(req_packageName);
        ArrayList<String> stringList = new ArrayList<>();
        PackageManager pm = RxposedApp.getInstance().getBaseContext().getPackageManager();
        for(String pkgName:enableModuleList ){
            try {
                ApplicationInfo applicationInfo = pm.getApplicationInfo(pkgName,PackageManager.GET_META_DATA);
                String apk_path = applicationInfo.sourceDir;

                String entry_class = applicationInfo.metaData.getString("rxposed_clsentry");
                String entry_method = applicationInfo.metaData.getString("rxposed_mtdentry");
                boolean hide = applicationInfo.metaData.getBoolean("rxposed_hide",false);
                stringList.add(apk_path+":"+entry_class+":"+entry_method+":"+hide);
            } catch (PackageManager.NameNotFoundException e) {
                Log.e("getRxConfig","PackageManager$NameNotFoundException ");
            }
        }
        bundle.putStringArrayList("ModuleList", stringList);
        return bundle;
    }

}
