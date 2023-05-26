package hepta.rxposed.loadxposed;

import android.app.Activity;
import android.app.Application;
import android.content.Context;
import android.os.Bundle;
import android.util.Log;
import hepta.rxposed.loadxposed.ui.home.ExtenDataProvider;

public class MyApp extends Application {



    private static MyApp instance;

    public static  MyApp getInstance(){
        return instance;
    }


    @Override
    public void onCreate() {
        Log.e("XposedCompat", "MyApp:");
        super.onCreate();
        instance = this;
        ExtenDataProvider.getInstance().init();
        XposedEntry.LoadApk_text(this);
//        XposedEntry.Entry(this);
//        XposedEntry.hook(this);

    }


    static {
//        Thread.dumpStack(); //测试代码位置
    }

}
