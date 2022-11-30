package hepta.rxposed.manager.service;


import android.app.Service;
import android.content.Intent;
import android.os.IBinder;
import android.os.RemoteException;

import androidx.annotation.Nullable;

import hepta.rxposed.manager.IRxposedService;

//aidl服务，进行进程通讯，目前弃用，android service启动时机不可控，不是立即启动
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
