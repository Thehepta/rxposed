package hepta.rxposed.nativeloader;

import androidx.appcompat.app.AppCompatActivity;

import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.os.IBinder;
import android.os.RemoteException;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;

import dalvik.system.DexClassLoader;

public class MainActivity extends AppCompatActivity {

    RxposedTextService localService;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        Button btn = findViewById(R.id.isolatedProcess);
        btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent myIntent = new Intent(MainActivity.this, RxposedTextService.class);
                bindService(myIntent, connection, Context.BIND_AUTO_CREATE);
            }
        });
        Button btn_loadapk = findViewById(R.id.loadapk);
        btn_loadapk.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                try {
                    Log.e("RZX", "btn_loadapk");
                    ApplicationInfo applicationInfo = getApplicationContext().getPackageManager().getApplicationInfo("hepta.rxposed.loadxposed",0);
                    String sourceDir = applicationInfo.sourceDir;
                    String nativelib = applicationInfo.nativeLibraryDir;
                    ClassLoader classLoader =  ClassLoader.getSystemClassLoader();
                    DexClassLoader dexClassLoader = new DexClassLoader(applicationInfo.sourceDir,null,nativelib,classLoader);

                    Class<?> XposedEntry =  dexClassLoader.loadClass("hepta.rxposed.loadxposed.XposedEntry");
                    Log.e("Rzx", "dexclassLoader compiler");
                    Method text =  XposedEntry.getMethod("text", Context.class);
                    text.invoke(XposedEntry,getApplicationContext());
                } catch (PackageManager.NameNotFoundException | ClassNotFoundException | NoSuchMethodException | InvocationTargetException | IllegalAccessException e) {
                    Log.e("Rzx", "btn_loadapk error");
                    e.printStackTrace();
                }
            }
        });

        Button btn_startact = findViewById(R.id.startact);
        btn_startact.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent intent = new Intent();
                intent.setClass (MainActivity.this, textActivity.class);
                startActivity(intent);
            }
        });

    }




    //Isoated 进程只能使用远程进程通讯的方式
    IsoatedService isoatedService;
    private ServiceConnection connection = new ServiceConnection() {
        @Override
        public void onServiceConnected(ComponentName className, IBinder service) {
            isoatedService = IsoatedService.Stub.asInterface(service);
            try {
                String string = isoatedService.getConfig("wrewrew");
                Log.e("rzx","ffffffffffffffff"+string);
            } catch (RemoteException e) {
                e.printStackTrace();
            }
        }
        @Override
        public void onServiceDisconnected(ComponentName arg0) {
        }
    };
}