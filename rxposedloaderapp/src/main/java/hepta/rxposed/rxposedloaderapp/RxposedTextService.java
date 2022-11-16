package hepta.rxposed.rxposedloaderapp;

import android.app.Service;
import android.content.ContentResolver;
import android.content.Intent;
import android.net.Uri;
import android.os.Binder;
import android.os.Bundle;
import android.os.IBinder;
import android.os.RemoteException;

import androidx.annotation.Nullable;

public class RxposedTextService extends Service {



    private final IsoatedService.Stub mBinder = new IsoatedService.Stub() {
        @Override
        public void basicTypes(int anInt, long aLong, boolean aBoolean, float aFloat, double aDouble, String aString) throws RemoteException {
            return;
        }

        @Override
        public String getConfig(String Name) throws RemoteException {
            return "null";
        }
    };

    public RxposedTextService() {
    }

    public void onCreate(){
        super.onCreate();
        /*java.lang.SecurityException: Isolated process not allowed to call getContentProvider

        Uri uri = Uri.parse("content://hepta.rxposed.manager.Provider");
        ContentResolver contentResolver = getApplicationContext().getContentResolver();
        Bundle bundle =contentResolver.call(uri,"getConfig",LoaderApplication.currentName,null);

         */
    }

    public int onStartCommand(Intent intent, int flags, int startId) {
        return START_STICKY;
    }

    @Nullable
    @Override
    public IBinder onBind(Intent intent) {
        return mBinder;
    }

    // Destroy
    @Override
    public void onDestroy() {
        // Release the resources
        super.onDestroy();
    }
}
