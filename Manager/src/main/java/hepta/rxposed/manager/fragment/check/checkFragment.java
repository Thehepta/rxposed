package hepta.rxposed.manager.fragment.check;

import android.content.AttributionSource;
import android.content.Context;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.os.IBinder;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.fragment.app.Fragment;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import com.chad.library.adapter.base.BaseQuickAdapter;
import com.chad.library.adapter.base.BaseSectionQuickAdapter;

import java.lang.reflect.Constructor;
import java.lang.reflect.Field;
import java.lang.reflect.Method;
import java.lang.reflect.Parameter;
import java.util.ArrayList;

import hepta.rxposed.manager.R;
import hepta.rxposed.manager.fragment.LoadExten.ExtenInfoProvider;
import hepta.rxposed.manager.fragment.PlugInject.SupportInfoProvider;
import hepta.rxposed.manager.util.InjectTool;

public class checkFragment extends Fragment {



    static {
        System.loadLibrary("nativeloader");
    }

    ArrayList<ItemBean> itemBeans = new ArrayList<>();
    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        return inflater.inflate(R.layout.fragment_check, container, false);
    }

    @Override
    public void onViewCreated(@NonNull View view, @Nullable Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);
        all_check();
        RecyclerView recyclerView = view.findViewById(R.id.rv_list);
        recyclerView.setLayoutManager(new LinearLayoutManager(getContext()));
        recyclerView.setAdapter(new RecyclerViewAdapter(R.layout.item_check_status,itemBeans));
    }

    private void all_check() {

        if(chekc_GetArtmethodNative_init()){

            itemBeans.add(new ItemBean("chekc_GetArtmethodNative_init",true));

            itemBeans.add(new ItemBean("chekc_android_os_Process_getUidForName",chekc_android_os_Process_getUidForName()));

            itemBeans.add(new ItemBean("chekc_android_os_Process_setArgV0",chekc_android_os_Process_setArgV0()));
        }else {
            itemBeans.add(new ItemBean("chekc_GetArtmethodNative_init",false));
        }

        itemBeans.add(new ItemBean("chekc_PreGetenv",chekc_PreGetenv()));
        itemBeans.add(new ItemBean("check_inline_hook",check_inline_hook()));
        chekc_java_method();
        check_file();
    }

    private boolean check_file(){
        new Thread(){
            @Override
            public void run() {

                InjectTool.init();
            }
        }.start();

        return true;
    }

    private void chekc_java_method(){

        Class<?>[] nativeSpecializeAppProcess_parameter={
                int.class,int.class,int[].class,int.class,int[][].class,int.class,String.class, String.class,
                boolean.class,String.class,String.class,boolean.class,String[].class,String[].class,boolean.class,boolean.class
        };

        itemBeans.add(Found_javaMethod("com.android.internal.os.Zygote","nativeSpecializeAppProcess",nativeSpecializeAppProcess_parameter));

        if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.S) {
            Class<?>[] AttributionSource_parameter= {
                    int.class,String.class,String.class
            };
            itemBeans.add(Found_getConstructorsMethod("android.content.AttributionSource","<init>",AttributionSource_parameter));

            Class<?>[] IContentProvider_call_parameter= {
                    AttributionSource.class,String.class,String.class,String.class,Bundle.class
            };
            itemBeans.add(Found_javaMethod("android.content.IContentProvider","call",IContentProvider_call_parameter));

        }else {

            Class<?>[] IContentProvider_call_parameter= {
                    String.class,String.class,String.class,String.class,String.class,Bundle.class
            };

            itemBeans.add(Found_javaMethod("android.content.IContentProvider","call",IContentProvider_call_parameter));
        }


        Class<?>[] getContentProviderExternal_parameter= {
                String.class,int.class,IBinder.class,String.class
        };
        itemBeans.add(Found_javaMethod("android.app.IActivityManager","getContentProviderExternal",getContentProviderExternal_parameter));


        itemBeans.add(Java_CreateApplicationContext());






    }



    private ItemBean Found_javaMethod(String className, String Method,Class<?>[] cmp_parameter){

        ItemBean itemBean = new ItemBean(className+":"+className,true);
        try {

            Class<?> cls_zygote = Class.forName(className);
            cls_zygote.getDeclaredMethod(Method,cmp_parameter);

        } catch (ClassNotFoundException e) {
            itemBean.setMsg(className+":ClassNotFoundException");
            itemBean.setStatus(false);
            return itemBean;
        } catch (NoSuchMethodException e) {
            itemBean.setMsg(className+":"+className+":NoSuchMethodException");
            itemBean.setStatus(false);
            return itemBean;
        }

        return itemBean;
    }

    private ItemBean Found_getConstructorsMethod(String className, String Method,Class<?>[] cmp_parameter){

        ItemBean itemBean = new ItemBean(className+":"+className,true);
        try {

            Class<?> cls_zygote = Class.forName(className);
            cls_zygote.getConstructor(cmp_parameter);

        } catch (ClassNotFoundException e) {
            itemBean.setMsg(className+":ClassNotFoundException");
            itemBean.setStatus(false);
            return itemBean;
        } catch (NoSuchMethodException e) {
            itemBean.setMsg(className+":"+className+":NoSuchMethodException");
            itemBean.setStatus(false);
            return itemBean;
        }

        return itemBean;
    }


    private boolean method_cmp(String className, String Method,Class<?>[] cmd_parameter ,Class<?> retType,String retString){

        boolean is_find = false;

        try {
            Class<?> cls_zygote = Class.forName(className);

            if(Method.equals("<init>")){
                Constructor[] constructor = cls_zygote.getConstructors();

            }
            {
                Method[] methods = cls_zygote.getDeclaredMethods();
                for (Method method1 : methods) {
                    if (method1.getName().equals(Method)) {
                        is_find = true;
                        Parameter[] parameter = method1.getParameters();
                        if (!method1.getReturnType().getTypeName().equals(retType.getTypeName())) {
                            retString = "return type is not equal";
                            return false;
                        }

                        if (parameter.length == cmd_parameter.length) {
                            for (int i = 0; i < parameter.length; i++) {
                                if (!parameter[i].getType().getTypeName().equals(cmd_parameter[i].getTypeName())) {
                                    retString = "arg type is not equal";
                                    return false;
                                }
                            }
                        } else {
                            retString = "arg  length is not equal";
                            return false;
                        }
                    }
                }
            }
        } catch (ClassNotFoundException e) {
            retString = "class is not found:"+className;
            return false;
        }

        if(is_find){
            retString = "successful";
            return true;
        }
        retString = "method is not found:"+Method+"->"+Method;
        return false;

    }


    public  ItemBean Java_CreateApplicationContext() {
        //获取 ActivityThread 类
        Class<?> mActivityThreadClass = null;
        String currentAppName = getContext().getPackageName();
        ItemBean itemBean = new ItemBean("Java_CreateApplicationContext",true);
        try {
            mActivityThreadClass = Class.forName("android.app.ActivityThread");

            //获取 ActivityThread 类
            Class<?> mLoadedApkClass = Class.forName("android.app.LoadedApk");
            //获取 ActivityThread 的 currentActivityThread() 方法
            Method currentActivityThread = mActivityThreadClass.getDeclaredMethod("currentActivityThread");
            currentActivityThread.setAccessible(true);
            //获取 ActivityThread 实例
            Object mActivityThread = currentActivityThread.invoke(null);

            Class<?> mCompatibilityInfoClass = Class.forName("android.content.res.CompatibilityInfo");
            Method getLoadedApkMethod = mActivityThreadClass.getDeclaredMethod("getPackageInfoNoCheck", ApplicationInfo.class, mCompatibilityInfoClass);

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

            return itemBean;

        } catch (PackageManager.NameNotFoundException e) {
            Log.e("check","getApplicationInfoAsUser NOT FOUND,return getSystemContext");
            itemBean.setMsg(e.getMessage());
            itemBean.setStatus(false);
            return itemBean;
        }catch (Exception e){
            itemBean.setMsg(e.getMessage());
            itemBean.setStatus(false);
            return itemBean;
        }
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
            Log.e("check", "getSystemContext:"+e.toString());
        }
        return context;
    }


    public native boolean  chekc_android_os_Process_setArgV0();
    public native boolean  chekc_PreGetenv();
    public native boolean  check_inline_hook();



    public native boolean chekc_GetArtmethodNative_init();
    public native boolean chekc_android_os_Process_getUidForName();

}
