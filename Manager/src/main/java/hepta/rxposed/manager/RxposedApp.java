package hepta.rxposed.manager;


import android.app.Application;

import hepta.rxposed.manager.fragment.PlugExten.ExtenDataProvider;
import hepta.rxposed.manager.fragment.PlugSupport.SupportData;
import hepta.rxposed.manager.util.InjectTool;


public class RxposedApp extends Application {


    private static RxposedApp instance;

    public static  RxposedApp getInstance(){
        return instance;
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
                SupportData.getInstance();
                ExtenDataProvider.getInstance();
                InjectTool.init();
            }
        }.start();
    }

    static {
            System.loadLibrary("status");
    }

}
