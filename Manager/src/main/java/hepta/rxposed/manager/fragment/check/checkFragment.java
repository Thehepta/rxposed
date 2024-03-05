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

import java.lang.reflect.Constructor;
import java.lang.reflect.Field;
import java.lang.reflect.Method;
import java.lang.reflect.Parameter;
import java.util.ArrayList;
import hepta.rxposed.manager.util.CheckTool;
import hepta.rxposed.manager.R;

public class checkFragment extends Fragment {




    ArrayList<ItemBean> itemBeans = new ArrayList<>();
    CheckTool checkTool;
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
        checkTool = new CheckTool();
        if(checkTool.chekc_GetArtmethodNative_init()){

            itemBeans.add(new ItemBean("chekc_GetArtmethodNative_init",true));

            itemBeans.add(new ItemBean("chekc_android_os_Process_getUidForName",checkTool.chekc_android_os_Process_getUidForName()));

        }else {
            itemBeans.add(new ItemBean("chekc_GetArtmethodNative_init",false));
        }

        itemBeans.add(new ItemBean("chekc_PreGetenv", checkTool.chekcPreGetenv()));
        itemBeans.add(new ItemBean("linkerResolveElfInternalSymbol",checkTool.ELFresolveSymbol()));
        itemBeans.add(new ItemBean("check_artmethod_jni_hook",checkTool.check_jni_hook()));
        chekc_java_method();

    }



    private void chekc_java_method(){

        Class<?>[] nativeSpecializeAppProcess_parameter={int.class,int.class,int[].class,int.class,int[][].class,int.class,String.class, String.class,
                boolean.class,String.class,String.class,boolean.class,String[].class,String[].class,boolean.class,boolean.class};

        itemBeans.add(checkTool.Found_javaMethod("com.android.internal.os.Zygote","nativeSpecializeAppProcess",nativeSpecializeAppProcess_parameter));
        if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.S) {
            Class<?>[] AttributionSource_parameter= {int.class,String.class,String.class};
            itemBeans.add(checkTool.Found_getConstructorsMethod("android.content.AttributionSource","<init>",AttributionSource_parameter));
            Class<?>[] IContentProvider_call_parameter= {AttributionSource.class,String.class,String.class,String.class,Bundle.class};
            itemBeans.add(checkTool.Found_javaMethod("android.content.IContentProvider","call",IContentProvider_call_parameter));
        }else {
            Class<?>[] IContentProvider_call_parameter= {String.class,String.class,String.class,String.class,String.class,Bundle.class};
            itemBeans.add(checkTool.Found_javaMethod("android.content.IContentProvider","call",IContentProvider_call_parameter));
        }
        Class<?>[] getContentProviderExternal_parameter= {String.class,int.class,IBinder.class,String.class};
        itemBeans.add(checkTool.Found_javaMethod("android.app.IActivityManager","getContentProviderExternal",getContentProviderExternal_parameter));
        Class<?>[] setArgV0Native_parameter= {String.class};

        //Process.setArgV0Native 这个函数隐藏了
        itemBeans.add(checkTool.Found_javaMethod("android.os.Process","setArgV0Native",getContentProviderExternal_parameter));
        itemBeans.add(checkTool.Java_CreateApplicationContext());


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














}
