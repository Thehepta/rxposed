package hepta.rxposed.manager.util;

import android.util.Log;

public class LogUtil {

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
