package hepta.rxposed.manager.util;

import static android.content.Context.MODE_PRIVATE;

import android.content.Context;
import android.content.Intent;
import android.system.Os;
import android.util.Log;

import java.io.BufferedReader;
import java.io.DataOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.nio.charset.Charset;
import java.util.Enumeration;
import java.util.zip.ZipEntry;
import java.util.zip.ZipFile;

import hepta.rxposed.manager.BuildConfig;
import hepta.rxposed.manager.RxposedApp;

public  class InjectTool {

    // Used to load the 'injecttool' library on application startup.

    /**
     * A native method that is implemented by the 'injecttool' native library,
     * which is packaged with this application.
     */

    public static String TAG = "InjectTool";
    public static String su_path;
    public static String arm64_InjectTool = "assets/arm64_InjectTool";
    public static String armv7_InjectTool = "assets/armv7_InjectTool";
    public static String HostProviderName = BuildConfig.APPLICATION_ID+".Provider";

    public static int arch_armv7=32;
    public static int arch_armv8=64;
    public static String InjectArg = BuildConfig.APPLICATION_ID+":"+HostProviderName;
    public static Context context = RxposedApp.getRxposedContext();
    private static String InjectTool_arm64_path;
    private static String InjectTool_armv7_path;
    private static String policy_path;
    private static String policy_te;

    public static String policy_tool = "magiskpolicy";

    private static String InjectSo_arm64_path;
    private static String InjectSo_armv7_path;
    private static String InjectTestSo_arm64_path;
    private static String InjectTestSo_armv7_path;

    public static void zygote_reboot() throws IOException {
        rootRun("killall zygote");
    }

    static {
        InjectTool.su_path = context.getSharedPreferences("rxposed",MODE_PRIVATE).getString("supath","su");
    }
    public static void init(){

        unziplib(context.getApplicationInfo().sourceDir,context.getFilesDir().getAbsolutePath()+ File.separator);

        policy_path = context.getFilesDir().getAbsolutePath()+ File.separator+"assets/"+policy_tool;
        policy_te = context.getFilesDir().getAbsolutePath()+ File.separator+"assets/rxposed.te";


        InjectTool_arm64_path = context.getFilesDir().getAbsolutePath()+ File.separator+arm64_InjectTool;
        InjectSo_arm64_path = context.getFilesDir().getAbsolutePath()+ File.separator+"lib/arm64-v8a/lib"+BuildConfig.Rxposed_Inject_So+".so";
        InjectTool_armv7_path = context.getFilesDir().getAbsolutePath()+ File.separator+armv7_InjectTool;
        InjectSo_armv7_path = context.getFilesDir().getAbsolutePath()+ File.separator+"lib/armeabi-v7a/lib"+BuildConfig.Rxposed_Inject_So+".so";

        InjectTestSo_arm64_path = context.getFilesDir().getAbsolutePath()+ File.separator+"lib/arm64-v8a/lib"+BuildConfig.Rxposed_InjectTest_So+".so";
        InjectTestSo_armv7_path = context.getFilesDir().getAbsolutePath()+ File.separator+"lib/armeabi-v7a/lib"+BuildConfig.Rxposed_InjectTest_So+".so";
        try {
            Runtime.getRuntime().exec("chmod +x "+InjectTool_arm64_path);
            Runtime.getRuntime().exec("chmod +x "+InjectTool_armv7_path);
            Runtime.getRuntime().exec("chmod +x "+policy_path);

        } catch (IOException e) {
            throw new RuntimeException(e);
        }
//        Log.e(TAG,InjectTool_arm64_path);
//        Log.e(TAG,InjectTool_armv7_path);
//        Log.e(TAG,InjectSo_armv7_path);
//        Log.e(TAG,InjectSo_arm64_path);

    }

    public static String getStatusAuthority(){
        int uid = context.getApplicationInfo().uid;
        return uid+":"+InjectArg;
    }
    public static void Application_reboot() throws IOException {
        Intent intent = context.getPackageManager().getLaunchIntentForPackage(context.getPackageName());
        intent.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP); //与正常页面跳转一样可传递序列化数据,在Launch页面内获得
        intent.putExtra("REBOOT", "reboot");
        context.startActivity(intent);
        android.os.Process.killProcess(android.os.Process.myPid());
    }

    // /data/user/0/hepta.rxposed.manager/files/arm64_InjectTool  -n zygote64 -hidemaps 1 -so /data/user/0/hepta.rxposed.manager/files/arm64_libnativeloader.so -symbols _Z14Ptrace_ZygotesPKc hepta.rxposed.manager.Provider;com.hep>                                                     <

    public static boolean Start()  {
        init();

        //修改selinux 规则
        set_selinux_context();
        //ptrace zygote
        zygote_ptrace_hide_so_maps();
        return  true;
    }

    private static void set_selinux_context() {
//
        String selinux_policy = policy_path +" --apply "+policy_te+" --live ";
        // magiskpolicy  --live "allow zygote unstrsfe binder {  call  transfer }"


        rootRun(selinux_policy);
        String selinux_domain = getDomain();
        String selinux_policy_domain = policy_path +  " --live \"allow zygote "+selinux_domain +" binder { call transfer }\"";
        rootRun(selinux_policy_domain);


    }

    private static String getDomain() {

        String selinux = InjectTool.shell("id -Z");
        String user_domain = selinux.split(":")[2];
        return user_domain;
    }

    public static void inject_text(){
        init();
        int uid = context.getApplicationInfo().uid;
        String cmd_arm64 = InjectTool_arm64_path+" -n zygote64 -hidemaps 1 -so "+ InjectTestSo_arm64_path+" -symbols _Z14Ptrace_ZygotesPKc "+uid+":"+InjectArg;
        String cmd_armv7 = InjectTool_armv7_path+" -n zygote -hidemaps 1 -so "  + InjectTestSo_armv7_path+" -symbols _Z14Ptrace_ZygotesPKc "+uid+":"+InjectArg;

        rootRun(cmd_arm64);
        rootRun(cmd_armv7);
    }
    ///data/user/0/hepta.rxposed.manager/files/assets/armv7_InjectTool -n zygote -hidemaps 1 -so /data/user/0/hepta.rxposed.manager/files/lib/armeabi-v7a/librxposed.so -symbols _Z14Ptrace_ZygotesPKc 10288:hepta.rxposed.manager:hepta.rxposed.manager.Provider
    public static  void zygote_ptrace_hide_so_maps()  {
        //zygote 附加
        int uid = context.getApplicationInfo().uid;
        String cmd_arm64 = InjectTool_arm64_path+" -n zygote64 -hidemaps 1 -so "+ InjectSo_arm64_path+" -symbols _Z14Ptrace_ZygotesPKc "+uid+":"+InjectArg;
        String cmd_armv7 = InjectTool_armv7_path+" -n zygote -hidemaps 1 -so "  + InjectSo_armv7_path+" -symbols _Z14Ptrace_ZygotesPKc "+uid+":"+InjectArg;

        LogFileHelper.writeLog(cmd_arm64);
        String ret_cmd_64 = rootRun(cmd_arm64);
        LogFileHelper.writeLog(ret_cmd_64);

        LogFileHelper.writeLog(cmd_armv7);
        String ret_cmd_32 = rootRun(cmd_armv7);
        LogFileHelper.writeLog(ret_cmd_32);

    }

    public static  void zygote_ptrace()  {
        //zygote 附加
        int uid = context.getApplicationInfo().uid;
        String cmd_arm64 = InjectTool_arm64_path+" -n zygote64 -so "+ InjectSo_arm64_path+" -symbols _Z14Ptrace_ZygotesPKc "+uid+":"+InjectArg;
        String cmd_armv7 = InjectTool_armv7_path+" -n zygote -so "  + InjectSo_armv7_path+" -symbols _Z14Ptrace_ZygotesPKc "+uid+":"+InjectArg;

        rootRun(cmd_arm64);
        rootRun(cmd_armv7);
    }


        //用于指定进程注入，目前用的不多
    // /data/user/0/hepta.rxposed.manager/files/arm64_InjectTool -p 4903  -so /data/user/0/hepta.rxposed.manager/files/arm64_libnativeloader.so -symbols _Z9Inject_ProcessPKc hepta.rxposed.manager.Provider;com.hep>                                                     <
    public static void Inject_Process(int Pid,String package_list)  {

        String Inject_Arg = HostProviderName+":"+package_list;
        int arch = ByPidGetProcessArch(Pid);
        if(arch == arch_armv8){

            String cmd_arm64 = InjectTool_arm64_path+" -p "+Pid+" -so "+ InjectSo_arm64_path+" -symbols _Z14Inject_PorcessPKc "+Inject_Arg;;
            Log.e(TAG,"arch 64:"+cmd_arm64);
            rootRun(cmd_arm64);
        }else {

            String cmd_armv7 = InjectTool_armv7_path+" -p "+Pid+" -so "+ InjectSo_armv7_path+" -symbols _Z14Inject_PorcessPKc "+Inject_Arg;
            Log.e(TAG,"arch 32:"+cmd_armv7);
            rootRun(cmd_armv7);
        }

    }

    private static int ByPidGetProcessArch(int pid) {
        String exe = rootRun("file -L /proc/"+pid+"/exe");
        if(exe.contains("linker64")){
            return 64;
        }else {
            return 32;
        }
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



    public static void unziplib(String zippath,String resourcepath){
        //判断生成目录是否生成，如果没有就创建
        File pathFile=new File(resourcepath);
        if(!pathFile.exists()){
            pathFile.mkdirs();
        }
        ZipFile zp=null;
        try{
            //指定编码，否则压缩包里面不能有中文目录
            zp=new ZipFile(zippath, Charset.forName("gbk"));
            //遍历里面的文件及文件夹
            Enumeration entries=zp.entries();
            while(entries.hasMoreElements()){
                ZipEntry entry= (ZipEntry) entries.nextElement();
                String zipEntryName=entry.getName();
                if(zipEntryName.contains("lib/") || zipEntryName.contains("assets/")){
                    InputStream in=zp.getInputStream(entry);
                    String outpath=(resourcepath+zipEntryName).replace("/",File.separator);
                    //判断路径是否存在，不存在则创建文件路径
                    File file = new  File(outpath.substring(0,outpath.lastIndexOf(File.separator)));
                    if(!file.exists()){
                        file.mkdirs();
                    }
                    //判断文件全路径是否为文件夹,如果是,不需要解压
                    if(new File(outpath).isDirectory())
                        continue;
                    OutputStream out=new FileOutputStream(outpath);
                    byte[] bf=new byte[2048];
                    int len;
                    while ((len=in.read(bf))>0){
                        out.write(bf,0,len);
                    }
                    in.close();
                    out.close();
                }

            }
            zp.close();
        }catch ( Exception e){
            e.printStackTrace();
        }
    }


        public static String shell(String cmd){
        try {
            Process process = Runtime.getRuntime().exec(cmd);
            BufferedReader reader = new BufferedReader(new InputStreamReader(process.getInputStream()));
            String line = "";
            while (true) {
                if ((line = reader.readLine()) == null) break;
                return line;
            }
        } catch (IOException e) {
            throw new RuntimeException(e);
        }

        return null;
    }


    public static String rootRun(String cmd)
    {
        Log.e(TAG,cmd);
        String Result  ="";
        try {
            // 申请获取root权限
            Process process = Runtime.getRuntime().exec(su_path); //"/system/xbin/su"
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
            String is_line = null;
            String es_line = null;
//            Log.d(TAG, "Run:\"" + cmd +"\", "+"process.waitFor() = " + code);
            BufferedReader br;
            br = new BufferedReader(new InputStreamReader(is, "UTF-8"));
            while ((is_line = br.readLine()) != null) {
                Log.d(TAG, "cmd > "+is_line);
                Result = Result+is_line+"\n";
            }

            br = new BufferedReader(new InputStreamReader(es, "UTF-8"));
            while ((es_line = br.readLine()) != null) {
//                Log.d(TAG, "cmd > "+es_line);
//                Result += es_line;
            }
        } catch (Throwable t) {
            t.printStackTrace();
        }
        return Result;
    }

}