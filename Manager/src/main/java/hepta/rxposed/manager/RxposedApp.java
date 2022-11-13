package hepta.rxposed.manager;


import android.app.Application;



import java.io.File;

import hepta.rxposed.manager.fragment.modules.ModuleInfo;
import hepta.rxposed.manager.fragment.modules.ModuleInfoProvider;
import hepta.rxposed.manager.util.InjectTool;
import hepta.rxposed.manager.util.LogUtil;


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
                ModuleInfoProvider.getInstance();
                InjectTool.su_path = getSharedPreferences("rxposed",MODE_PRIVATE).getString("supath","su");
            }
        }.start();
    }

    static {
            System.loadLibrary("status");
    }

}
