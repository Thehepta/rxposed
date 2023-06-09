package hepta.rxposed.manager.util;

import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.content.pm.ResolveInfo;
import android.util.Log;
import android.widget.Toast;

import java.util.List;

import hepta.rxposed.manager.RxposedApp;

public class Util {

    public static void LogE(Object... args){
        String tmp = "";
        for(Object arg:args)
            tmp = tmp +" "+arg;
        Log.e("rxposde",tmp);
   }
    public static void LogD(Object... args){
        String tmp = "";
        for(Object arg:args)
            tmp = tmp +" "+arg;
        Log.d("rxposde",tmp);
   }


   public static void StartRxmoduleApplication(String packageName){
       Intent resolveIntent = new Intent(Intent.ACTION_MAIN, null);
       resolveIntent.addCategory(Intent.CATEGORY_LAUNCHER);
       resolveIntent.setPackage(packageName);

       Context context = RxposedApp.getRxposedContext();
       PackageManager pm =  context.getPackageManager();
       List<ResolveInfo> apps = pm.queryIntentActivities(resolveIntent, 0);
       if (apps.iterator().hasNext()) {
           ResolveInfo ri = apps.iterator().next();
           String className = ri.activityInfo.name;
           Intent intent = new Intent(Intent.ACTION_MAIN);
           intent.addCategory(Intent.CATEGORY_LAUNCHER);
           intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
           ComponentName cn = new ComponentName(packageName, className);
           intent.setComponent(cn);
           context.startActivity(intent);
       }else {
           Toast.makeText(context,"Can't open Application", Toast.LENGTH_LONG).show();
       }

   }

}
