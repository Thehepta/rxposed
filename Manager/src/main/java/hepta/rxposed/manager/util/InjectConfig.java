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
        public   String config_name;
        public   String su_path;
        public   String arm64_InjectSo ;
        public   String arm32_InjectSo;
        public   String mountWorkDir;
        //是否隐藏注入到zygote中的so的 maps（隐藏是不需要本地路径的）
        public   int injectType;
        //注入1号进程init，开启server注入功能，开发中
        public   boolean injectInit;

        InjectConfig(){
                Context context = RxposedApp.getRxposedContext();
                int App_Uid = context.getApplicationInfo().uid;
                Consts.InjectArg = App_Uid+":"+BuildConfig.APPLICATION_ID+":"+ Consts.HOST_PROVIDER_NAME;;

                String AppFilePath = context.getFilesDir().getAbsolutePath()+ File.separator;
                unziplib(context.getApplicationInfo().sourceDir,AppFilePath);
                Consts.policy_tool_path = AppFilePath+Consts.ASSETS_POLICY_TOOL;
                Consts.policy_te_path = AppFilePath+Consts.ASSETS_POLICY_TE;
                Consts.mntSh32_tool_path = AppFilePath+Consts.ASSETS_MNT_SH32_TOOL;
                Consts.mntSh64_tool_path = AppFilePath+Consts.ASSETS_MNT_SH64_TOOL;
                Consts.shell_script_path = AppFilePath+Consts.ASSETS_SHELL_SCRIPT;

                Consts.arm64_InjectTool = AppFilePath+Consts.ASSETS_ARM_64_INJECT_TOOL;
                Consts.arm32_InjectTool = AppFilePath+Consts.ASSETS_ARM_32_INJECT_TOOL;

                Consts.appfiles_arm64_InjectSo = AppFilePath+"lib/arm64-v8a/"+Consts.SO_NAME;
                Consts.appfiles_arm32_InjectSo = AppFilePath+"lib/armeabi-v7a/"+Consts.SO_NAME;

                try {
                        Runtime.getRuntime().exec("chmod +x "+Consts.arm64_InjectTool);
                        Runtime.getRuntime().exec("chmod +x "+Consts.arm32_InjectTool);
                        Runtime.getRuntime().exec("chmod +x "+Consts.policy_tool_path);
                        Runtime.getRuntime().exec("chmod +x "+Consts.mntSh32_tool_path);
                        Runtime.getRuntime().exec("chmod +x "+Consts.mntSh64_tool_path);
                        Runtime.getRuntime().exec("chmod +x "+Consts.ASSETS_SHELL_SCRIPT);

                } catch (IOException e) {
                        throw new RuntimeException(e);
                }
                updateConfigSave();
        }



        public void updateConfigSave() {

                this.config_name = MmkvManager.INSTANCE.getInjectConfigString("config_name","default");
                this.su_path = MmkvManager.INSTANCE.getInjectConfigString("supath","su");
                this.injectType = MmkvManager.INSTANCE.getInjectConfigInt("injectType",Consts.HIDE_MAPS);
                this.injectInit = MmkvManager.INSTANCE.getInjectConfigBoolean("injectInit",false);
                this.mountWorkDir = MmkvManager.INSTANCE.getInjectConfigString("mountWorkDir","/apex/com.android.i18nrxp");
                if(injectType == Consts.HIDE_MAPS){
                        this.arm32_InjectSo = Consts.appfiles_arm32_InjectSo;
                        this.arm64_InjectSo = Consts.appfiles_arm64_InjectSo;
                }else {
                        this.arm32_InjectSo = mountWorkDir+"/lib/"+Consts.SO_NAME;
                        this.arm64_InjectSo = mountWorkDir+"/lib64/"+Consts.SO_NAME;
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

        public static InjectConfig instance = null;

        public static  InjectConfig getInstance(){
                if(instance == null){
                        instance = new InjectConfig();
                }
                return instance;
        }
}