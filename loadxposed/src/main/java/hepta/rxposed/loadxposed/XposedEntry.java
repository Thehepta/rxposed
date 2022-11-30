package hepta.rxposed.loadxposed;

import android.app.LoadedApk;
import android.content.ContentResolver;
import android.content.Context;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageManager;
import android.net.Uri;
import android.os.Bundle;
import android.util.Log;

import com.swift.sandhook.xposedcompat.XposedCompat;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.util.zip.ZipEntry;
import java.util.zip.ZipFile;

import dalvik.system.DexClassLoader;
import de.robv.android.xposed.IXposedHookLoadPackage;
import de.robv.android.xposed.XC_MethodHook;
import de.robv.android.xposed.XposedHelpers;

public class XposedEntry {

    private static final String TAG = "XposedEntry";

    static void Entry(Context context){
        Log.e(TAG, "loadxposed Entry");
        Uri uri = Uri.parse("content://hepta.rxposed.loadxposed.Provider");
        ContentResolver contentResolver = context.getContentResolver();
        Bundle bundle =contentResolver.call(uri,"getConfig","hepta.rxposed.loadxposed",null);
        String enableUidList_str =  bundle.getString("enableUidList");
        if(enableUidList_str.equals("null")){
            Log.w(TAG," get RxConfigPrvider is null");
            return ;
        }
        String[] app_vec = enableUidList_str.split("\\|");
        for(String app :app_vec){
            try {
                ApplicationInfo applicationInfo =  context.getPackageManager().getApplicationInfo(app,0);
                LoadApk(applicationInfo,context);
            } catch (PackageManager.NameNotFoundException e) {
                e.printStackTrace();
            } catch (IOException e) {
                e.printStackTrace();
            } catch (ClassNotFoundException e) {
                e.printStackTrace();
            }
//                GetAppInfoNative(appinfo_vec)
        }

    }

    static void LoadEntry() {
        Log.e(TAG, "loadxposed LoadEntry");
    }
        static Boolean LoadApk(ApplicationInfo applicationInfo, Context context) throws IOException, ClassNotFoundException {
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
        ClassLoader mcl = context.getClassLoader();
        while ((moduleClassName = moduleClassesReader.readLine()) != null) {
            moduleClassName = moduleClassName.trim();
            if (moduleClassName.isEmpty() || moduleClassName.startsWith("#"))
                continue;
            try {
                Log.i(TAG, "  Loading class " + moduleClassName);
                Class<?> moduleClass = mcl.loadClass(moduleClassName);
                if (!IXposedHookLoadPackage.class.isAssignableFrom(moduleClass)) {
                    Log.e(TAG, "    This class doesn't implement any sub-interface of IXposedMod, skipping it");
                    continue;
                }
                final Object moduleInstance = moduleClass.newInstance();

            }catch (Throwable t) {
                Log.e(TAG, "    Failed to load class " + moduleClassName, t);
            }
        }


        XposedHelpers.findAndHookConstructor(LoadedApk.class,new XC_MethodHook(){
            @Override
            protected void afterHookedMethod(MethodHookParam param) throws Throwable {
                super.afterHookedMethod(param);
                Log.i(TAG, "LoadedApk.class findAndHookConstructor");
            }
        });







        return true;
    }

    static void hook(Context context){
        XposedCompat.cacheDir = context.getCacheDir();
//for load xp module(sandvxp)
        XposedCompat.context = context;
        XposedCompat.classLoader = context.getClassLoader();
//        XposedCompat.isFirstApplication= true;
        XposedCompat.addXposedModuleCallback(new HookMain());
        try {
            Log.e("XposedCompat", "callXposedModuleInit:");
            XposedCompat.callXposedModuleInit();
        } catch (Throwable e) {
            e.printStackTrace();
        }
    }
}
