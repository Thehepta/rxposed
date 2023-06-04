package hepta.rxposed.loadxposed;

import android.app.Activity;
import android.app.Application;
import android.app.LoadedApk;
import android.content.ContentResolver;
import android.content.Context;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageManager;
import android.net.Uri;
import android.os.Bundle;
import android.util.Log;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.util.zip.ZipEntry;
import java.util.zip.ZipFile;

import dalvik.system.DexClassLoader;
import top.canyie.pine.Pine;
import top.canyie.pine.callback.MethodHook;


public class XposedEntry {

    private static final String TAG = "XposedEntry";

    static void Entry(Context context){
//        XposedCompat.context = context;
//        XposedCompat.classLoader = context.getClassLoader();
//        Log.e(TAG, "loadxposed Entry");
//        Uri uri = Uri.parse("content://hepta.rxposed.loadxposed.Provider");
//        ContentResolver contentResolver = context.getContentResolver();
//        Bundle bundle =contentResolver.call(uri,"getConfig",context.getPackageName(),null);
//        String enableUidList_str =  bundle.getString("enableUidList");
//        if(enableUidList_str.equals("null")){
//            Log.w(TAG,"getloadxposedPrvider is null");
//            return ;
//        }
//        String[] app_vec = enableUidList_str.split("\\|");
//        for(String app :app_vec){
//            try {
//                ApplicationInfo applicationInfo =  context.getPackageManager().getApplicationInfo(app,0);
//                LoadApk(applicationInfo,context);
//            } catch (PackageManager.NameNotFoundException | IOException | ClassNotFoundException e) {
//                e.printStackTrace();
//            }
//        }
//        try {
//            XposedCompat.callXposedModuleInit();
//        } catch (Throwable e) {
//            e.printStackTrace();
//        }
    }

    static Boolean LoadApk(ApplicationInfo applicationInfo, Context context) throws IOException, ClassNotFoundException
    {
        Log.e(TAG, "LoadApk:"+applicationInfo.packageName);
        ZipFile zipFile = null;
        InputStream is;

        zipFile = new ZipFile(applicationInfo.sourceDir);
        ZipEntry zipEntry = zipFile.getEntry("assets/xposed_init");
        if (zipEntry == null) {
            Log.e(TAG, "  assets/xposed_init not found in the APK");
            zipFile.close();
            return false;
        }
        is = zipFile.getInputStream(zipEntry);
        BufferedReader moduleClassesReader = new BufferedReader(new InputStreamReader(is));
        String moduleClassName;

        ClassLoader mcl = XposedEntry.class.getClassLoader();  //需要用当前的classloader，不能用context 的classloader
        DexClassLoader XpApkClassLoader = new DexClassLoader(applicationInfo.sourceDir,null,applicationInfo.nativeLibraryDir,mcl);
//        while ((moduleClassName = moduleClassesReader.readLine()) != null) {
//            moduleClassName = moduleClassName.trim();
//            if (moduleClassName.isEmpty() || moduleClassName.startsWith("#"))
//                continue;
//            try {
//                Log.i(TAG, "Loading class " + moduleClassName);
//                Class<?> moduleClass = XpApkClassLoader.loadClass(moduleClassName);
//                final Object moduleInstance = moduleClass.newInstance();
//                if (moduleInstance instanceof IXposedHookLoadPackage) {
//                    Log.i(TAG, "XposedCompat addXposedModuleCallback " + moduleClassName);
//                    XposedCompat.addXposedModuleCallback((IXposedHookLoadPackage) moduleInstance);
//                }
//            }catch (Throwable t) {
//                Log.e(TAG, "    Failed to load class " + moduleClassName, t);
//            }
//        }
        return true;
    }

    static void hook(Context context){
//        XposedCompat.cacheDir = context.getCacheDir();
////for load xp module(sandvxp)
//        XposedCompat.context = context;
//        XposedCompat.classLoader = context.getClassLoader();
////        XposedCompat.isFirstApplication= true;
//        XposedCompat.addXposedModuleCallback(new HookMain());
//        try {
//            Log.e("XposedCompat", "callXposedModuleInit:");
//            XposedCompat.callXposedModuleInit();
//        } catch (Throwable e) {
//            e.printStackTrace();
//        }
    }

    //nativeloader 工程 的activity会加载这个项目的apk，然后调用这个函数测试java hook  建议public，否则java层调用比较麻烦
    public static void LoadApk_text(Context context){
        Log.e("XposedCompat", "XposedEntry text");

        try {
            Pine.hook(Activity.class.getDeclaredMethod("onCreate", Bundle.class),new MethodHook() {
                @Override public void beforeCall(Pine.CallFrame callFrame) {
                    Log.i("TAG", "Before " + callFrame.thisObject + " onCreate()");
                }

                @Override public void afterCall(Pine.CallFrame callFrame) {
                    Log.i("TAG", "After " + callFrame.thisObject + " onCreate()");
                }
            });
        } catch (NoSuchMethodException e) {
            throw new RuntimeException(e);
        }
    }


    public static void hook_app_class_text(Context context){
        Log.e("XposedCompat", "entry XposedEntry hook_app_class_text");

//        XposedCompat.cacheDir = context.getCacheDir();
////for load xp module(sandvxp)
//        XposedCompat.context = context;
//        XposedCompat.classLoader = context.getClassLoader();
//        XposedCompat.isFirstApplication= true;
//
//        XposedHelpers.findAndHookMethod("com.hepta.theptavpn.TheptaVapApp",context.getClassLoader(), "onCreate", new XC_MethodHook() {
//
////        XposedHelpers.findAndHookMethod(Application.class, "onCreate",new XC_MethodHook() {
//            @Override
//            protected void beforeHookedMethod(XC_MethodHook.MethodHookParam param) throws Throwable {
//                super.beforeHookedMethod(param);
//                Log.e("XposedCompat", "XposedEntry beforeHookedMethod: " + param.method.getName());
//            }
//
//            @Override
//            protected void afterHookedMethod(MethodHookParam param) throws Throwable {
//                super.afterHookedMethod(param);
//                Log.e("XposedCompat", "XposedEntry afterHookedMethod: " + param.method.getName());
//            }
//        });
    }
}
//D/SandHook: method <public void com.hepta.theptavpn.TheptaVapApp.onCreate()> hook <replacement> success!
//        D/SandXposed: hook method <public void com.hepta.theptavpn.TheptaVapApp.onCreate()> cost 27 ms, by internal stub
//        E/TheptaVapApp: androidx.core.app.CoreComponentFactory.CoreComponentFactory.java:instantiateApplication(52)
//        android.app.Instrumentation.Instrumentation.java:newApplication(1155)
//        android.app.LoadedApk.LoadedApk.java:makeApplication(1218)
//        android.app.ActivityThread.ActivityThread.java:handleBindApplication(6431)
//        android.app.ActivityThread.ActivityThread.java:access$1300(219)
//        android.app.ActivityThread$H.ActivityThread.java:handleMessage(1859)
//        android.os.Handler.Handler.java:dispatchMessage(107)
//        android.os.Looper.Looper.java:loop(214)
//        android.app.ActivityThread.ActivityThread.java:main(7356)
//        java.lang.reflect.Method.Method.java:invoke(-2)
//        com.android.internal.os.RuntimeInit$MethodAndArgsCaller.RuntimeInit.java:run(492)
//        com.android.internal.os.ZygoteInit.ZygoteInit.java:main(930)
//        D/SandXposed: method <public void com.hepta.theptavpn.TheptaVapApp.onCreate()> hook in
//        E/XposedCompat: XposedEntry beforeHookedMethod: onCreate
//        E/TheptaVapApp onCreate: com.swift.sandhook.SandHook.SandHook.java:callOriginMethod(163)
//        com.swift.sandhook.xposedcompat.hookstub.HookStubManager.HookStubManager.java:hookBridge(299)
//        com.swift.sandhook.xposedcompat.hookstub.MethodHookerStubs64.MethodHookerStubs64.java:stub_hook_0(80)
//        android.app.Instrumentation.Instrumentation.java:callApplicationOnCreate(1189)
//        android.app.ActivityThread.ActivityThread.java:handleBindApplication(6460)
//        android.app.ActivityThread.ActivityThread.java:access$1300(219)
//        android.app.ActivityThread$H.ActivityThread.java:handleMessage(1859)
//        android.os.Handler.Handler.java:dispatchMessage(107)
//        android.os.Looper.Looper.java:loop(214)
//        android.app.ActivityThread.ActivityThread.java:main(7356)
//        java.lang.reflect.Method.Method.java:invoke(-2)
//        com.android.internal.os.RuntimeInit$MethodAndArgsCaller.RuntimeInit.java:run(492)
//        com.android.internal.os.ZygoteInit.ZygoteInit.java:main(930)
//        E/XposedCompat: XposedEntry afterHookedMethod: onCreate