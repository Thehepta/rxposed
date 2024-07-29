package hepta.rxposed.manager.util;


import hepta.rxposed.manager.BuildConfig;

public class InjectConfig {

    public  static String su_path;
    public  static String arm64_InjectTool ;
    public  static String arm64_InjectSo ;
    public  static String appfiles_arm64_InjectSo ;

    public  static String arm32_InjectTool;
    public  static String arm32_InjectSo;
    public  static String appfiles_arm32_InjectSo;

    public  static String InjectArg;
    public static  String policy_path;
    public static  String policy_te;
    public static  boolean hidemaps;
    public static  boolean injectInit;



    public final static String HostProviderName = BuildConfig.APPLICATION_ID+".Provider";
    public final static String assets_policy_tool = "assets/magiskpolicy";
    public final static String assets_policy_te = "assets/rxposed.te";
    public final static String assets_arm64_InjectTool = "assets/arm64_InjectTool";
    public final static String assets_arm32_InjectTool = "assets/armv7_InjectTool";


}