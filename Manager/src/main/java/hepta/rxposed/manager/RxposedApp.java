package hepta.rxposed.manager;


import android.app.Application;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.util.Log;

import com.tencent.mmkv.MMKV;

import hepta.rxposed.manager.service.RxConfigProvider;


public class RxposedApp extends Application {

    String TAG = RxposedApp.class.getName();
    private static RxposedApp instance;

    public static  RxposedApp getInstance(){
        return instance;
    }


    public static Context getRxposedContext(){
        return instance.getApplicationContext();
    }

    @Override
    public void onCreate() {
        super.onCreate();
        instance = this;

        IntentFilter filter = new IntentFilter();
        filter.addAction(Intent.ACTION_PACKAGE_REMOVED);
        filter.addAction(Intent.ACTION_PACKAGE_REPLACED);
        filter.addAction(Intent.ACTION_PACKAGE_ADDED);
        filter.addAction(Intent.ACTION_PACKAGE_CHANGED);
        filter.addDataScheme("package");
        PackageChangeReceiver packageChangeReceiver = new PackageChangeReceiver();
        registerReceiver(packageChangeReceiver, filter);
    }

    @Override
    protected void attachBaseContext(Context base) {
        super.attachBaseContext(base);
        Log.e(TAG, "attachBaseContext");
        MMKV.initialize(this);

    }

}
