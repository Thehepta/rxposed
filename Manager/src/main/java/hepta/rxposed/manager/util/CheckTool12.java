package hepta.rxposed.manager.util;

import android.content.AttributionSource;
import android.os.Bundle;
import android.os.IBinder;

import java.util.ArrayList;

import hepta.rxposed.manager.fragment.check.ItemBean;

public class CheckTool12 extends CheckTool{

    public CheckTool12(){
        System.loadLibrary("check");
    }


    public void addExternCheckItem(ArrayList<ItemBean> itemBeans) {

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

        itemBeans.add(Java_CreateApplicationContext());

        if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.S) {
            Class<?>[] AttributionSource_parameter= {int.class,String.class,String.class};
            itemBeans.add(Found_getConstructorsMethod("android.content.AttributionSource","<init>",AttributionSource_parameter));
            Class<?>[] IContentProvider_call_parameter= {AttributionSource.class,String.class,String.class,String.class,Bundle.class};
            itemBeans.add(Found_javaMethod("android.content.IContentProvider","call",IContentProvider_call_parameter));
        }


    }

}
