package hepta.rxposed.manager;


import android.app.Service;
import android.content.Intent;
import android.os.IBinder;
import android.os.RemoteException;

import androidx.annotation.Nullable;


public class RxConfigService extends Service {


    @Override
    public void onCreate() {
        super.onCreate();
    }

    @Nullable
    @Override
    public IBinder onBind(Intent intent) {
        return binder;
    }

    private final IRxposedService.Stub binder = new IRxposedService.Stub() {


        @Override
        public String getConfig(String Name) throws RemoteException {
            return "conttext";
        }

        @Override
        public void basicTypes(int anInt, long aLong, boolean aBoolean, float aFloat, double aDouble, String aString) throws RemoteException {

            return;
        }
    };


}
