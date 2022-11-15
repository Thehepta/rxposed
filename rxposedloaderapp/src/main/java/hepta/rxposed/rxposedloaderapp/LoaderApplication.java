package hepta.rxposed.rxposedloaderapp;

import android.app.Application;
import android.content.ComponentName;
import android.content.ContentResolver;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.content.SharedPreferences;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageManager;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.os.IBinder;
import android.os.RemoteException;
import android.util.ArrayMap;
import android.util.Log;

import androidx.annotation.RequiresApi;

import java.lang.reflect.Field;
import java.lang.reflect.Method;

//需要编译一次，会自动生成
import hepta.rxposed.manager.IRxposedService;

public class LoaderApplication extends Application{


    private static final String TAG = "LoaderApplication";
    private static Context SystemContext = null;
    private static  int currentUid ;
    private static  String currentName = "hepta.rxposed.rxposedloaderapp" ;

    @RequiresApi(api = Build.VERSION_CODES.Q)
    @Override
    public void onCreate() {
        super.onCreate();
        //aidl
        callaidl();


    }


    //能不能直接修改context ，然后进行 contentprovider请求
    private void testContentSesolver() {
        Context context = getApplicationContext();
        ContentResolver contentResolver =  getSystemContext().getContentResolver();
        ContentResolver contentResolver1 = context.getContentResolver();
        Log.e("rzx","callpm");
        try {
            Class<?> contextcls = Class.forName("android.content.Context");
            Field field  = contextcls.getDeclaredField("mPackageName");
//            Class<?> mApplicationContentResolverClass = Class.forName("android.app.ContextImpl$ApplicationContentResolver");
//            Field field = mApplicationContentResolverClass.getField("mPackageName");
        } catch (NoSuchFieldException | ClassNotFoundException e) {
            e.printStackTrace();
        }


    }

    public static native Context getApplicationContext(String AppName);
    public static native void NDK_ExceptionCheckTest();

    static {
//        Thread.dumpStack(); //测试代码位置
        System.loadLibrary(BuildConfig.SO_NAME);
//        Context context = Java_getApplicationContext(currentName);
//        Context context = getApplicationContext("android.process.media");
        Context context = getApplicationContext("hepta.rxposed.manager");
        if(GetConfigByProvider(context)){

        }
//        NDK_ExceptionCheckTest();

    }

    private static boolean GetConfigByProvider(Context context) {

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.Q) {
            Uri uri = Uri.parse("content://hepta.rxposed.manager.Provider");
            ContentResolver contentResolver = context.getContentResolver();
            Bundle bundle =contentResolver.call(uri,"getConfig",currentName,null);
            String enableUidList_str =  bundle.getString("enableUidList");
            if(enableUidList_str.equals("null")){
                Log.w(TAG," get RxConfigPrvider is null");
                return true;
            }
            String[] app_vec =   enableUidList_str.split("\\|");
            for(String app :app_vec){
                String[] appinfo_vec = app.split(":");

//                GetAppInfoNative(appinfo_vec)
            }

        }
        Log.w(TAG," android version not support rxposed");
        return true;
    }

    private static void GetAppInfoNative(String[] appinfo_vec) {

    }


    private static Context getSystemContext() {
        Context context = null;
        try {
            Method method = Class.forName("android.app.ActivityThread").getMethod("currentActivityThread");
            method.setAccessible(true);
            Object activityThread = method.invoke(null);
            context = (Context) activityThread.getClass().getMethod("getSystemContext").invoke(activityThread);

        } catch (final Exception e) {
            e.printStackTrace();
            Log.e(TAG, "getSystemContext:"+e.toString());
        }
        return context;
    }

    //经过测试在android 10 可以用
    public static Context Java_getApplicationContext(String currentAppName) {
        //获取 ActivityThread 类
        Class<?> mActivityThreadClass = null;
        try {
            mActivityThreadClass = Class.forName("android.app.ActivityThread");

        //获取 ActivityThread 类
        Class<?> mLoadedApkClass = Class.forName("android.app.LoadedApk");
        //获取 ActivityThread 的 currentActivityThread() 方法
        Method currentActivityThread = mActivityThreadClass.getDeclaredMethod("currentActivityThread");
        currentActivityThread.setAccessible(true);
        //获取 ActivityThread 实例
        Object mActivityThread = currentActivityThread.invoke(null);
        //final ArrayMap<String, WeakReference<LoadedApk>> mPackages = new ArrayMap<>();
        //获取 mPackages 属性
//        Field mPackagesField = mActivityThreadClass.getDeclaredField("mPackages");
//        mPackagesField.setAccessible(true);
        //获取 mPackages 属性的值
//        ArrayMap<String, Object> mPackages = (ArrayMap<String, Object>) mPackagesField.get(mActivityThread);
//        if (mPackages.size() >= 2) {
//            return;
//        }
        //自定义一个 LoadedApk，系统是如何创建的我们就如何创建
        //执行下面的方法会返回一个 LoadedApk，我们就仿照系统执行此方法
        /*
              this.packageInfo = client.getPackageInfoNoCheck(activityInfo.applicationInfo,
                    compatInfo);
              public final LoadedApk getPackageInfo(ApplicationInfo ai, CompatibilityInfo compatInfo,
                    int flags)
         */
        Class<?> mCompatibilityInfoClass = Class.forName("android.content.res.CompatibilityInfo");
        Method getLoadedApkMethod = mActivityThreadClass.getDeclaredMethod("getPackageInfoNoCheck",
                ApplicationInfo.class, mCompatibilityInfoClass);

        /*
             public static final CompatibilityInfo DEFAULT_COMPATIBILITY_INFO = new CompatibilityInfo() {};
         */
        //以上注释是获取默认的 CompatibilityInfo 实例
        Field mCompatibilityInfoDefaultField = mCompatibilityInfoClass.getDeclaredField("DEFAULT_COMPATIBILITY_INFO");
        Object mCompatibilityInfo = mCompatibilityInfoDefaultField.get(null);

        //获取一个 ApplicationInfo实例
        ApplicationInfo applicationInfo = getSystemContext().getPackageManager().getApplicationInfo(currentAppName,0);
        //执行此方法，获取一个 LoadedApk
        Object mLoadedApk = getLoadedApkMethod.invoke(mActivityThread, applicationInfo, mCompatibilityInfo);
        Class<?> mContextImplClass = Class.forName("android.app.ContextImpl");
        Method createAppContext = mContextImplClass.getDeclaredMethod("createAppContext",mActivityThreadClass,mLoadedApkClass);
        createAppContext.setAccessible(true);
        Object context =  createAppContext.invoke(null,mActivityThread,mLoadedApk);
        return (Context) context;

        } catch (PackageManager.NameNotFoundException e) {
            Log.e(TAG,"getApplicationInfoAsUser NOT FOUND,return getSystemContext");
            return getSystemContext();
        }catch (Exception e){
            e.printStackTrace();
            return null;
        }
    }


    private static ApplicationInfo GetAppInfobyUID(int uid) {
        ApplicationInfo applicationInfo = null;
        Context context = getSystemContext();
        String pkgName = context.getPackageManager().getNameForUid(uid);
        try {
            applicationInfo = context.getPackageManager().getApplicationInfo(pkgName,0);
        } catch (PackageManager.NameNotFoundException e) {
            e.printStackTrace();
        }
        return applicationInfo;
    }


    //通过aidl完成进程通讯
    //onServiceConnected 调用时机会延迟，这个方案不太好
    private void callaidl() {
        Intent intent = new Intent();
        intent.setAction("hepta.rxposed.manager.aidl");
        intent.setPackage("hepta.rxposed.manager");
        bindService(intent,mConnection,Context.BIND_AUTO_CREATE);
        Log.e("rzx","bindService before");

    }
    IRxposedService iRxposedService;
    private ServiceConnection mConnection = new ServiceConnection() {
        @Override
        public void onServiceConnected(ComponentName componentName, IBinder iBinder) {
            iRxposedService = IRxposedService.Stub.asInterface(iBinder);
            try {
                String string = iRxposedService.getConfig("wrewrew");
                Log.e("rzx","ffffffffffffffff"+string);
            } catch (RemoteException e) {
                e.printStackTrace();
            }
        }
        @Override
        public void onServiceDisconnected(ComponentName componentName) {

        }
    };


}



