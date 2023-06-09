package hepta.rxposed.manager;


import android.app.Application;
import android.content.Context;

import hepta.rxposed.manager.fragment.LoadExten.ExtenInfoProvider;
import hepta.rxposed.manager.fragment.PlugInject.SupportInfoProvider;
import hepta.rxposed.manager.util.InjectTool;


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

        instance = this;
        initConfig();
//        IntentFilter filter = new IntentFilter();
//        filter.addAction(Intent.ACTION_PACKAGE_REMOVED);
//        filter.addAction(Intent.ACTION_PACKAGE_REPLACED);
//        filter.addAction(Intent.ACTION_PACKAGE_ADDED);
//        filter.addAction(Intent.ACTION_PACKAGE_CHANGED);
//        filter.addDataScheme("package");
//        PackageChangeReceiver packageChangeReceiver = new PackageChangeReceiver();
//        registerReceiver(packageChangeReceiver, filter);
    }

    private void initConfig() {
        new Thread(){
            @Override
            public void run() {
                SupportInfoProvider.getInstance().init();
                ExtenInfoProvider.getInstance().init();
            }
        }.start();
    }

    static {
        System.loadLibrary("status");


    }

}
