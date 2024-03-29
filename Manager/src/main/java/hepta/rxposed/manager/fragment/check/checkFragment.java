package hepta.rxposed.manager.fragment.check;

import android.content.AttributionSource;
import android.os.Build;
import android.os.Bundle;
import android.os.IBinder;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.fragment.app.Fragment;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import java.lang.reflect.Constructor;
import java.lang.reflect.Method;
import java.lang.reflect.Parameter;
import java.util.ArrayList;

import hepta.rxposed.manager.util.CheckTool;
import hepta.rxposed.manager.util.CheckTool11;
import hepta.rxposed.manager.R;
import hepta.rxposed.manager.util.CheckTool12;
import hepta.rxposed.manager.util.CheckTool13;

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

        if (android.os.Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
            checkTool = new CheckTool12();
        }else if(android.os.Build.VERSION.SDK_INT >= Build.VERSION_CODES.R){
            checkTool = new CheckTool11();
        }
        checkTool.addCheckItem(itemBeans);
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
