package hepta.rxposed.rxposedloaderapp;

import androidx.appcompat.app.AppCompatActivity;

import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.Bundle;
import android.os.IBinder;
import android.os.RemoteException;
import android.util.Log;
import android.view.View;
import android.widget.Button;

import hepta.rxposed.manager.IRxposedService;

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