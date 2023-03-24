package hepta.rxposed.loadxposed;

import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.content.pm.ResolveInfo;
import android.util.Log;
import android.widget.Toast;

import java.util.List;

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



}
