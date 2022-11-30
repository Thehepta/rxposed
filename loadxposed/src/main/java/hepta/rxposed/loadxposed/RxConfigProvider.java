package hepta.rxposed.loadxposed;

import android.content.ContentProvider;
import android.content.ContentValues;
import android.database.Cursor;
import android.net.Uri;
import android.os.Bundle;
import android.os.ParcelFileDescriptor;
import android.util.Log;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import java.io.File;
import java.io.FileNotFoundException;
import java.lang.reflect.Method;

import hepta.rxposed.loadxposed.ui.home.ExtenDataProvider;

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

        String callName = getCallingPackage();



        Bundle bundle = new Bundle();
        String reString = ExtenDataProvider.getInstance().getConfigToString(ProcessName);
        bundle.putString("enableUidList", reString);
        Log.e("getRxConfig", "pkgName: "+ProcessName+" callName:"+callName);
        Log.e("getRxConfig", "return: " + bundle.getString("enableUidList"));
        return bundle;
    }

}
