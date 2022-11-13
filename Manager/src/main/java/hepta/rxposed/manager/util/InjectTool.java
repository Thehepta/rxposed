package hepta.rxposed.manager.util;

import android.content.Context;
import android.content.Intent;
import android.util.Log;

import java.io.BufferedReader;
import java.io.DataOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;

public  class InjectTool {

    // Used to load the 'injecttool' library on application startup.

    /**
     * A native method that is implemented by the 'injecttool' native library,
     * which is packaged with this application.
     */

    public static String su_path;


    public static void zygote_reboot() throws IOException {
        rootRun(su_path,"killall zygote");
    }
    public static void zygote_reboot(Context context) throws IOException {
        Intent intent = context.getPackageManager().getLaunchIntentForPackage(context.getPackageName());
        intent.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP); //与正常页面跳转一样可传递序列化数据,在Launch页面内获得
        intent.putExtra("REBOOT", "reboot");
        context.startActivity(intent);
        android.os.Process.killProcess(android.os.Process.myPid());
    }



    public static boolean zygote_patrace(Context context) throws IOException {

        String InjectTool_arm64_path = context.getFilesDir().getAbsolutePath()+ File.separator+"InjectTool_arm64";
        String InjectTool_armv7_path = context.getFilesDir().getAbsolutePath()+ File.separator+"InjectTool_armv7";
        String InjectSo_arm64_path   = context.getFilesDir().getAbsolutePath()+ File.separator+"libhookbase_arm64.so";
        String InjectSo_armv7_path   = context.getFilesDir().getAbsolutePath()+ File.separator+"libhookbase_armv7.so";

        InjectTool.copyAssetToDst(context,"InjectTool_arm64",InjectTool_arm64_path);
        Runtime.getRuntime().exec("chmod +x "+InjectTool_arm64_path);
        InjectTool.copyAssetToDst(context,"InjectTool_armv7",InjectTool_armv7_path);
        Runtime.getRuntime().exec("chmod +x "+InjectTool_armv7_path);
        InjectTool.copyAssetToDst(context,"libhookbase_arm64.so",InjectSo_arm64_path);
        InjectTool.copyAssetToDst(context,"libhookbase_armv7.so",InjectSo_armv7_path);

        //修改函数或者参数，记得修改符号
//        String cmd = InjectTool_arm64_path+" -n zygote64 -so "+ injectSo+" -symbols _Z9dobby_strPKc com.rxposed.qmulkt:com.rxposed.qmulkt.Provider";

        String cmd_arm64 = InjectTool_arm64_path+" -n zygote64 -so "+ InjectSo_arm64_path+" -symbols _Z9dobby_strPKc com.example.android.codelabs.navigation:com.rxposed.android.Provider";
        String cmd_armv7 = InjectTool_armv7_path+" -n zygote -so "  + InjectSo_armv7_path+" -symbols _Z9dobby_strPKc com.example.android.codelabs.navigation:com.rxposed.android.Provider";
        Log.d("rzx",cmd_arm64);
        Log.d("rzx",cmd_armv7);
//        Runtime.getRuntime().exec("su "+cmd_arm64);

//        rootRun("su","top");
        rootRun(su_path,cmd_arm64);
        rootRun(su_path,cmd_armv7);
        return  true;
    }





    public static boolean copyAssetToDst(Context context, String fileName,String dstFilePath){
        try {
            File outFile =new File(dstFilePath);
            if (!outFile.exists()){
                boolean res=outFile.createNewFile();
                if (!res){
                    return false;
                }
            }else {
                if (outFile.length()>10){//表示已经写入一次
                    outFile.delete();
                }
            }
            InputStream is=context.getAssets().open(fileName);
            FileOutputStream fos = new FileOutputStream(outFile);
            byte[] buffer = new byte[1024];
            int byteCount;
            while ((byteCount = is.read(buffer)) != -1) {
                fos.write(buffer, 0, byteCount);
            }
            fos.flush();
            is.close();
            fos.close();
            return true;
        } catch (IOException e) {
            e.printStackTrace();
        }

        return false;
    }





    private static void rootRun(String su_tool,  String cmd)
    {
        try {
            // 申请获取root权限
            Process process = Runtime.getRuntime().exec(su_tool); //"/system/xbin/su"
            // 获取输出流
            OutputStream outputStream = process.getOutputStream();
            InputStream is = process.getInputStream();
            InputStream es = process.getErrorStream();
            DataOutputStream dataOutputStream = new DataOutputStream(outputStream);
            dataOutputStream.writeBytes(cmd);
            dataOutputStream.flush();
            dataOutputStream.close();
            outputStream.close();
            int code = process.waitFor();
            Log.d("InjectTool", "Run:\"" + cmd +"\", "+"process.waitFor() = " + code);
            String line;
            BufferedReader br;
            br = new BufferedReader(new InputStreamReader(is, "UTF-8"));
            while ((line = br.readLine()) != null) {
                Log.d("InjectTool is ", line);
            }

            br = new BufferedReader(new InputStreamReader(es, "UTF-8"));
            while ((line = br.readLine()) != null) {
                Log.e("InjectTool es", line);
            }

        } catch (Throwable t) {
            Log.e("InjectTool", "Throwable = " + t.getMessage());
            t.printStackTrace();
        }
    }







}