package hepta.rxposed.manager.util;


import android.content.Context;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.nio.charset.Charset;
import java.util.Enumeration;
import java.util.zip.ZipEntry;
import java.util.zip.ZipFile;

import hepta.rxposed.manager.BuildConfig;
import hepta.rxposed.manager.RxposedApp;

public class InjectConfig {
        public  static String config_name;
        public  static String su_path;
        public  static String arm64_InjectSo ;
        public  static String arm32_InjectSo;
        public static String mountWorkDir;
        //注入参数
        public static String InjectArg;
        //修改selinux策略工具路径
        public static  String policy_tool_path;
        //要修改的selinux策略
        public static  String policy_te_path;
        // 自定一个的mount工具
        public static  String mntSh32_tool_path;
        public static  String mntSh64_tool_path;
        public static  String shell_script_path;
        //是否隐藏注入到zygote中的so的 maps（隐藏是不需要本地路径的）
        public static  boolean hidemaps;
        //注入1号进程init，开启server注入功能，开发中
        public static  boolean injectInit;
        //注入工具的路径，app目录下，没必要移动到别的路径中
        public  static String arm64_InjectTool ;
        public  static String arm32_InjectTool;


        //原始so路径，不会删除，用作初始化和修改后备份 (app files目录下)
        public  static String appfiles_arm64_InjectSo ;
        public  static String appfiles_arm32_InjectSo;





        public final static String soName = "lib"+BuildConfig.Rxposed_Inject_So+".so";
        public final static String HostProviderName = BuildConfig.APPLICATION_ID+".Provider";
        public final static String assets_mntSh64_tool = "assets/arm64_mntSh";
        public final static String assets_mntSh32_tool = "assets/armv7_mntSh";
        public final static String assets_shell_script = "assets/Inject.sh";
        public final static String assets_policy_tool = "assets/magiskpolicy";
        public final static String assets_policy_te = "assets/rxposed.te";
        public final static String assets_arm64_InjectTool = "assets/arm64_generalInjectTool";
        public final static String assets_arm32_InjectTool = "assets/armv7_generalInjectTool";


        static public  void Init(){


                Context context = RxposedApp.getRxposedContext();
                int App_Uid = context.getApplicationInfo().uid;
                InjectConfig.InjectArg = App_Uid+":"+BuildConfig.APPLICATION_ID+":"+ InjectConfig.HostProviderName;;

                String AppFilePath = context.getFilesDir().getAbsolutePath()+ File.separator;
                unziplib(context.getApplicationInfo().sourceDir,AppFilePath);
                InjectConfig.policy_tool_path = AppFilePath+InjectConfig.assets_policy_tool;
                InjectConfig.policy_te_path = AppFilePath+InjectConfig.assets_policy_te;
                InjectConfig.mntSh32_tool_path = AppFilePath+InjectConfig.assets_mntSh32_tool;
                InjectConfig.mntSh64_tool_path = AppFilePath+InjectConfig.assets_mntSh64_tool;
                InjectConfig.shell_script_path = AppFilePath+InjectConfig.assets_shell_script;

                InjectConfig.arm64_InjectTool = AppFilePath+InjectConfig.assets_arm64_InjectTool;
                InjectConfig.arm32_InjectTool = AppFilePath+InjectConfig.assets_arm32_InjectTool;

                InjectConfig.appfiles_arm64_InjectSo = AppFilePath+"lib/arm64-v8a/"+InjectConfig.soName;
                InjectConfig.appfiles_arm32_InjectSo = AppFilePath+"lib/armeabi-v7a/"+InjectConfig.soName;

                try {
                        Runtime.getRuntime().exec("chmod +x "+InjectConfig.arm64_InjectTool);
                        Runtime.getRuntime().exec("chmod +x "+InjectConfig.arm32_InjectTool);
                        Runtime.getRuntime().exec("chmod +x "+InjectConfig.policy_tool_path);
                        Runtime.getRuntime().exec("chmod +x "+InjectConfig.mntSh32_tool_path);
                        Runtime.getRuntime().exec("chmod +x "+InjectConfig.mntSh64_tool_path);
                        Runtime.getRuntime().exec("chmod +x "+InjectConfig.assets_shell_script);

                } catch (IOException e) {
                        throw new RuntimeException(e);
                }
        }



        static void updateConfigSave() {

                InjectConfig.config_name = MmkvManager.INSTANCE.getInjectConfigString("config_name","default");
                InjectConfig.su_path = MmkvManager.INSTANCE.getInjectConfigString("supath","su");
//                InjectConfig.hidemaps = MmkvManager.INSTANCE.getBoolean("hidemaps",true);
                InjectConfig.hidemaps = MmkvManager.INSTANCE.getInjectConfigBoolean("hidemaps",false);
                InjectConfig.injectInit = MmkvManager.INSTANCE.getInjectConfigBoolean("injectInit",false);
                if(hidemaps){
                        InjectConfig.arm32_InjectSo = InjectConfig.appfiles_arm32_InjectSo;
                        InjectConfig.arm64_InjectSo = InjectConfig.appfiles_arm64_InjectSo;
                }else {
                        InjectConfig.mountWorkDir = MmkvManager.INSTANCE.getInjectConfigString("mountWorkDir","/apex/com.android.i18nrxp");
                        InjectConfig.arm32_InjectSo = mountWorkDir+"/lib/"+InjectConfig.soName;
                        InjectConfig.arm64_InjectSo = mountWorkDir+"/lib64/"+InjectConfig.soName;
                }

        }

        //解压lib 和assets 目录到 filee 目录下
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

}