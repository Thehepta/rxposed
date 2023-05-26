package hepta.rxposed.loadxposed;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
public class HookMain {
}

//public class HookMain implements IXposedHookLoadPackage {
//    @Override
//    public void handleLoadPackage(XC_LoadPackage.LoadPackageParam lpparam) throws Throwable {
//        Log.e("XposedCompat", "handleLoadPackage:");
//
//        XposedHelpers.findAndHookMethod(Activity.class, "onCreate", Bundle.class,new XC_MethodHook() {
//            @Override
//            protected void beforeHookedMethod(XC_MethodHook.MethodHookParam param) throws Throwable {
//                super.beforeHookedMethod(param);
//                Log.e("XposedCompat", "beforeHookedMethod: " + param.method.getName());
//            }
//
//            @Override
//            protected void afterHookedMethod(MethodHookParam param) throws Throwable {
//                super.afterHookedMethod(param);
//                Log.e("XposedCompat", "afterHookedMethod: " + param.method.getName());
//            }
//        });
//    }
//}
