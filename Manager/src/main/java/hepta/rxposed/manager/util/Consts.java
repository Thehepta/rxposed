package hepta.rxposed.manager.util;

import hepta.rxposed.manager.BuildConfig;
import hepta.rxposed.manager.RxposedApp;

public class Consts {

    public static final int START_FRAGMENT_MODULE = 1 ;
    public static final int START_FRAGMENT_FRAMEWORK = 2 ;



    //   -------------------------------------------
    //注入参数
    public  static String InjectArg;
    //修改selinux策略工具路径
    public  static String policy_tool_path;
    //要修改的selinux策略
    public  static String policy_te_path;
    //注入工具的路径，app目录下，没必要移动到别的路径中
    public  static String arm64_InjectTool ;
    public  static String arm32_InjectTool;
    // 执行挂载脚本
    public  static String mntSh32_tool_path;
    public  static String mntSh64_tool_path;
    public  static String shell_script_path;

    //原始so路径，不会删除，用作初始化和修改后备份 (app files目录下)
    public  static String appfiles_arm64_InjectSo ;
    public  static String appfiles_arm32_InjectSo;




    public final static String SO_NAME = "lib"+ BuildConfig.Rxposed_Inject_So+".so";
    public final static String HOST_PROVIDER_NAME = BuildConfig.APPLICATION_ID+".Provider";
    public final static String ASSETS_MNT_SH64_TOOL = "assets/arm64_mntSh";
    public final static String ASSETS_MNT_SH32_TOOL = "assets/armv7_mntSh";
    public final static String ASSETS_SHELL_SCRIPT = "assets/Inject.sh";
    public final static String ASSETS_POLICY_TOOL = "assets/magiskpolicy";
    public final static String ASSETS_POLICY_TE = "assets/rxposed.te";
    public final static String ASSETS_ARM_64_INJECT_TOOL = "assets/arm64_generalInjectTool";
    public final static String ASSETS_ARM_32_INJECT_TOOL = "assets/armv7_generalInjectTool";

    public static final int MOUNT_TMP = 1;
    public static final int HIDE_MAPS = 0;


    public static final String INJECT_FRAGMENT_ARGE = "injectProcess";
    public static final String INJECTLIST_FRAGMENT_ARGE = "injectList";

    public static final String LOG_FILE_NAME = RxposedApp.getInstance().getFilesDir()+"/inject_log.txt";


}
