package hepta.rxposed.manager;


import android.app.Application;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;

import com.tencent.mmkv.MMKV;


public class RxposedApp extends Application {


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
        MMKV.initialize(this);
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


    static {
            System.loadLibrary("status");
    }

}
