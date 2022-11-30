package hepta.rxposed.loadxposed;

import android.app.Activity;
import android.app.Application;
import android.content.Context;
import android.os.Bundle;
import android.util.Log;

import com.swift.sandhook.xposedcompat.XposedCompat;

import de.robv.android.xposed.XC_MethodHook;
import de.robv.android.xposed.XposedHelpers;

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
        XposedEntry.LoadEntry();
//        XposedEntry.Entry(this);
//        XposedEntry.hook(this);

    }




    private void sandhooktext(Context context) {

        XposedCompat.cacheDir = context.getCacheDir();
//for load xp module(sandvxp)
        XposedCompat.context = context;
        XposedCompat.classLoader = context.getClassLoader();
        XposedCompat.isFirstApplication= true;
        XposedHelpers.findAndHookMethod(Activity.class, "onCreate", Bundle.class,new XC_MethodHook() {
            @Override
            protected void beforeHookedMethod(XC_MethodHook.MethodHookParam param) throws Throwable {
                super.beforeHookedMethod(param);
                Log.e("XposedCompat", "beforeHookedMethod: " + param.method.getName());
            }

            @Override
            protected void afterHookedMethod(MethodHookParam param) throws Throwable {
                super.afterHookedMethod(param);
                Log.e("XposedCompat", "afterHookedMethod: " + param.method.getName());
            }
        });
    }

    static {
        Thread.dumpStack(); //测试代码位置
    }

}
