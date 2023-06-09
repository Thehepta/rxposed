package hepta.rxposed.nativeloader;

import static hepta.rxposed.nativeloader.LoaderApplication.currentName;

import androidx.appcompat.app.AppCompatActivity;

import android.app.ActivityManager;
import android.content.ComponentName;
import android.content.ContentResolver;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageManager;
import android.net.Uri;
import android.os.Binder;
import android.os.Build;
import android.os.Bundle;
import android.os.IBinder;
import android.os.RemoteException;
import android.util.ArrayMap;
import android.util.Log;
import android.view.View;
import android.widget.Button;

import java.lang.ref.WeakReference;
import java.lang.reflect.Field;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;

import dalvik.system.DexClassLoader;
import dalvik.system.PathClassLoader;

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
                    ApplicationInfo applicationInfo = null;
                    try {
                        applicationInfo = getPackageManager().getApplicationInfo("com.hepta.pinedemo",0);
                    } catch (PackageManager.NameNotFoundException e) {
                        throw new RuntimeException(e);
                    }
                    String sourceDir = applicationInfo.sourceDir;
                    String nativelib = applicationInfo.nativeLibraryDir;
                    Log.e("rzx",sourceDir);
                    Log.e("rzx",nativelib);
//                    String sourceapk = "/data/app/~~TdAzolTNGEKIp-LgVPI_GQ==/hepta.rxposed.loadxposed-X06YxjQ6nfbdXzbRgGNq2A==/base.apk";
//                    String nativedir = "/data/app/~~TdAzolTNGEKIp-LgVPI_GQ==/hepta.rxposed.loadxposed-X06YxjQ6nfbdXzbRgGNq2A==/lib/arm64";

                    PathClassLoader classloader = new PathClassLoader(sourceDir,nativelib,MainActivity.class.getClassLoader());

                    Class<?> XposedEntry =  classloader.loadClass("com.hepta.pinedemo.hookEntry");
                    Log.e("Rzx", "dexclassLoader compiler");
                    Method text =  XposedEntry.getMethod("Entry", Context.class);
                    text.invoke(XposedEntry,getApplicationContext());
                } catch ( ClassNotFoundException | NoSuchMethodException | InvocationTargetException | IllegalAccessException e) {
                    Log.e("Rzx", "btn_loadapk error");
                    e.printStackTrace();
                }
            }
        });


        Button btn_nativeloadapk = findViewById(R.id.nativeloadapk);
        btn_nativeloadapk.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                natvieloadapk("hepta.rxposed.manager.Provider|com.hepta.pinedemo");
            }
        });


        Button btn_text_activitymanagle = findViewById(R.id.text_activitymanagle);

        btn_text_activitymanagle.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                String providerName = "";
//                try {
//                    IActivityManager activityManager = ActivityManager.getService();
//                    IBinder token = new Binder();
//                    ContentProviderHolder holder = activityManager.getContentProviderExternal(providerName, this.mUserId, token, "*cmd*");
//                    if (holder == null) {
//                        throw new IllegalStateException("Could not find provider: " + providerName);
//                    }
//                    IContentProvider provider = holder.provider;
//                    onExecute(provider);
//                    if (provider != null) {
//                        activityManager.removeContentProviderExternalAsUser(providerName, token, this.mUserId);
//                    }
//                } catch (Exception e) {
//                    System.err.println("Error while accessing provider:" + providerName);
//                    e.printStackTrace();
//                }
            }
        });
        Button btn_text_rx_provider = findViewById(R.id.text_rx_provider);

        btn_text_rx_provider.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                GetConfigByProvider();
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


        Button btn_getnative = findViewById(R.id.getnative);
        btn_getnative.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                getjavaJniMethod("");
            }
        });
    }



    private  boolean GetConfigByProvider() {

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.Q) {
            Uri uri = Uri.parse("content://hepta.rxposed.manager.Provider");
            ContentResolver contentResolver = getContentResolver();
            Bundle bundle =contentResolver.call(uri,"getConfig",currentName,null);
            String enableUidList_str =  bundle.getString("enableUidList");
            Log.e("Rzx","enableUidList:"+enableUidList_str);
            if(enableUidList_str.equals("null")){
                Log.w("Rzx"," get RxConfigPrvider is null");
                return true;
            }

        }
        Log.w("Rzx"," android version not support rxposed");
        return true;
    }




    private native void natvieloadapk(String appname) ;
    private native void getjavaJniMethod(String appname) ;

    //2023-05-16 23:38:17.194  1774-2045  AppsFilter              system_server                        I  interaction: PackageSetting{425172d hepta.rxposed.nativeloader/10257} -> PackageSetting{b37189f hepta.rxposed.manager/10251} BLOCKED
//2023-05-16 23:38:17.195 29804-29804 ActivityThread          hepta.rxposed.nativeloader           E  Failed to find provider info for hepta.rxposed.manager.Provider

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