package hepta.rxposed.manager.fragment.check;

import android.content.AttributionSource;
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
import java.lang.reflect.Method;
import java.lang.reflect.Parameter;
import java.util.ArrayList;

import hepta.rxposed.manager.R;

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
        chekc_java_method();

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

    public native boolean  chekc_android_os_Process_setArgV0();
    public native boolean  chekc_PreGetenv();



    public native boolean chekc_GetArtmethodNative_init();
    public native boolean chekc_android_os_Process_getUidForName();

}
