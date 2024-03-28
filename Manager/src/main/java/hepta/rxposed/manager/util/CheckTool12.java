package hepta.rxposed.manager.util;

import android.content.AttributionSource;
import android.content.Context;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.os.IBinder;
import android.os.Process;
import android.util.Log;

import java.lang.reflect.Field;
import java.lang.reflect.Method;
import java.util.ArrayList;

import hepta.rxposed.manager.RxposedApp;
import hepta.rxposed.manager.fragment.check.ItemBean;

public class CheckTool12 extends CheckTool{

    public CheckTool12(){
        System.loadLibrary("check");
    }


    public void addCheckItem(ArrayList<ItemBean> itemBeans) {

        if(chekc_GetArtmethodNative_init()){
            itemBeans.add(new ItemBean("chekc_GetArtmethodNative_init",true));
        }else {
            itemBeans.add(new ItemBean("chekc_GetArtmethodNative_init",false));
        }

        if(CheckTool11.get_rxposed_status()) {
            itemBeans.add(new ItemBean("chekc_android_os_Process_getUidForName",true));
        }else {
            itemBeans.add(new ItemBean("chekc_android_os_Process_getUidForName",chekc_android_os_Process_getUidForName()));
        }

        itemBeans.add(new ItemBean("chekc_PreGetenv", chekcPreGetenv()));
        itemBeans.add(new ItemBean("linkerResolveElfInternalSymbol",ELFresolveSymbol()));
        itemBeans.add(new ItemBean("check_artmethod_jni_hook",check_jni_hook()));
        chekc_java_method(itemBeans);
    }


    public void chekc_java_method(ArrayList<ItemBean> itemBeans){

        Class<?>[] nativeSpecializeAppProcess_parameter={int.class,int.class,int[].class,int.class,int[][].class,int.class,String.class, String.class,
                boolean.class,String.class,String.class,boolean.class,String[].class,String[].class,boolean.class,boolean.class};
        itemBeans.add(Found_javaMethod("com.android.internal.os.Zygote","nativeSpecializeAppProcess",nativeSpecializeAppProcess_parameter));


        Class<?>[] nativeForkAndSpecialize_parameter={int.class, int.class, int[].class,
                int.class, int[][].class, int.class, String.class, String.class,
                int[].class, int[].class, boolean.class, String.class,
                String.class, boolean.class, String[].class,
                String[].class, boolean.class,
                boolean.class};

        itemBeans.add(Found_javaMethod("com.android.internal.os.Zygote","nativeForkAndSpecialize",nativeForkAndSpecialize_parameter));


        Class<?>[] getContentProviderExternal_parameter= {String.class,int.class, IBinder.class,String.class};
        itemBeans.add(Found_javaMethod("android.app.IActivityManager","getContentProviderExternal",getContentProviderExternal_parameter));

        Class<?>[] setArgV0Native_parameter= {String.class};
        itemBeans.add(Found_javaMethod("android.os.Process","setArgV0",setArgV0Native_parameter));

        itemBeans.add(Java_CreateApplicationContext());

        if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.S) {
            Class<?>[] AttributionSource_parameter= {int.class,String.class,String.class};
            itemBeans.add(Found_getConstructorsMethod("android.content.AttributionSource","<init>",AttributionSource_parameter));
            Class<?>[] IContentProvider_call_parameter= {AttributionSource.class,String.class,String.class,String.class,Bundle.class};
            itemBeans.add(Found_javaMethod("android.content.IContentProvider","call",IContentProvider_call_parameter));
        }


    }

}
