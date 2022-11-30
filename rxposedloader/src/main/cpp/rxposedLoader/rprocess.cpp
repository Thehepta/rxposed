//
// Created by Intel on 2022/4/5.
//


#include "include/rprocess.h"


jobject LoadModuleClassLoader = nullptr;


rprocess::rprocess() {

}

rprocess * rprocess::instance_ =nullptr;

bool rprocess::Init(JNIEnv *env, jstring name) {
    currentUid = getuid();
    string currentName = env->GetStringUTFChars(name, nullptr);
    RxposedContext = getRxposedContext(env);
    if(RxposedContext != nullptr){
        LoadFramework(env);
        return true;
    }
    return false;

}

bool rprocess::is_providerHostProcess(JNIEnv *env, jstring pJstring) {
    processName= env->GetStringUTFChars(pJstring, nullptr);
//    LOGE("pkg compare provider:%s  processName:%s",providerHost_pkgName.c_str(),processName.c_str());
    if(!strncmp(providerHost_pkgName.c_str(), processName.c_str(), providerHost_pkgName.size())){
        return true;
    }
    return false;
}


vector<string> string_split(string str,string pattern)
{
    string::size_type pos;
    vector<string> result;
    str += pattern;//扩展字符串以方便操作
    int size = str.size();
    for (int i = 0; i < size; i++)
    {
        pos = str.find(pattern, i);
        if (pos < size)
        {
            string s = str.substr(i, pos - i);
            result.push_back(s);
            i = pos + pattern.size() - 1;
        }
    }
    return result;
}


bool rprocess::GetConfigByProvider(JNIEnv *env) {
    string err_message="";
    jclass Context_cls = env->FindClass("android/content/Context");
    jclass Uri_cls = env->FindClass("android/net/Uri");
    jclass ContentResolver_cls = env->FindClass("android/content/ContentResolver");
    jclass Bundle_cls = env->FindClass("android/os/Bundle");
    jmethodID context_getContentResolver_method = env->GetMethodID(Context_cls,
                                                                   "getContentResolver",
                                                                   "()Landroid/content/ContentResolver;");
    jmethodID ContentResolver_call_method = env->GetMethodID(ContentResolver_cls, "call",
                                                             "(Landroid/net/Uri;Ljava/lang/String;Ljava/lang/String;Landroid/os/Bundle;)Landroid/os/Bundle;");
    AUTHORITY = "content://"+AUTHORITY;
    jmethodID Uri_parse_method = env->GetStaticMethodID(Uri_cls, "parse",
                                                        "(Ljava/lang/String;)Landroid/net/Uri;");
    jmethodID Bundle_getString_method = env->GetMethodID(Bundle_cls, "getString",
                                                         "(Ljava/lang/String;)Ljava/lang/String;");
    jobject extras = nullptr;

    jstring method = env->NewStringUTF("getConfig");
    jstring method_arg = env->NewStringUTF(processName.c_str());
    jobject URI = env->CallStaticObjectMethod(Uri_cls, Uri_parse_method,env->NewStringUTF(AUTHORITY.c_str()));

    jobject ContentResolver_obj = env->CallObjectMethod(RxposedContext,context_getContentResolver_method);

    jobject bundle_obj = env->CallObjectMethod(ContentResolver_obj, ContentResolver_call_method,
                                               URI, method, method_arg, extras);

    err_message = "processName:"+processName+"get config privider failed AUTHORITY:"+ AUTHORITY;
    if(NDK_ExceptionCheck(env,(char *)err_message.c_str())){
        return false;
    }
    jstring key = env->NewStringUTF("enableUidList");
    jstring config = static_cast<jstring>(env->CallObjectMethod(bundle_obj, Bundle_getString_method,key));
    string enableUidList_str = env->GetStringUTFChars(config, nullptr);

    if(!strncmp(enableUidList_str.c_str(),"null", strlen("null"))){
        LOGE("processName: %s get RxConfigPrvider is null",processName.c_str());
        return false;
    }

    vector<string> app_vec = string_split(enableUidList_str, "|");
    for (auto i : app_vec)
    {
        vector<string> appinfo_vec = string_split(i, ":");

        AppinfoNative* appinfoNative = GetAppInfoNative(env,appinfo_vec[0].c_str(),appinfo_vec[1],appinfo_vec[2]);
        if(appinfoNative != nullptr){
            AppinfoNative_vec.push_back(appinfoNative);
        } else{
            LOGE("GetAppInfoNative ret==ull");
            delete appinfoNative;
        }
    }
    return true;

}

rprocess::AppinfoNative* rprocess::GetAppInfoNative(JNIEnv *env, string pkgName, string Entry_class, string Entry_method){

    jclass Context_cls = env->FindClass("android/content/Context");
    jmethodID getPackageManager_method = env->GetMethodID(Context_cls, "getPackageManager", "()Landroid/content/pm/PackageManager;");
    jclass PackageManager_cls = env->FindClass("android/content/pm/PackageManager");
    jmethodID pm_getApplicationInfo_method = env->GetMethodID(PackageManager_cls, "getApplicationInfo", "(Ljava/lang/String;I)Landroid/content/pm/ApplicationInfo;");
    jobject PackageManager = env->CallObjectMethod(RxposedContext, getPackageManager_method);
    jobject java_pkaName = env->NewStringUTF(pkgName.c_str());

    jobject appinfo = env->CallObjectMethod(PackageManager,pm_getApplicationInfo_method,java_pkaName,0);
    jclass ApplicationInfo_cls = env->FindClass("android/content/pm/ApplicationInfo");
    jfieldID sourceDir_field = env->GetFieldID(ApplicationInfo_cls,"sourceDir","Ljava/lang/String;");
    jfieldID nativeLibraryDir_field = env->GetFieldID(ApplicationInfo_cls,"nativeLibraryDir","Ljava/lang/String;");

    if(appinfo == nullptr){
        LOGE("appinfo == nullptr");
        return nullptr;
    }
    if(NDK_ExceptionCheck(env,"GetAppInfoNative error")){
        LOGE("ndk error");
        return nullptr;
    }
    jstring tmp_sourceDir = static_cast<jstring>(env->GetObjectField(appinfo,sourceDir_field));
    const char * sourceDir = env->GetStringUTFChars(tmp_sourceDir, nullptr);
    env->DeleteLocalRef(tmp_sourceDir);

    jstring tmp_nativeLibraryDir = static_cast<jstring>(env->GetObjectField(appinfo,nativeLibraryDir_field));
    const char * nativeLibraryDir = env->GetStringUTFChars(tmp_nativeLibraryDir, nullptr);
    env->DeleteLocalRef(tmp_nativeLibraryDir);

    AppinfoNative * tmp = new AppinfoNative(pkgName, sourceDir, nativeLibraryDir, Entry_class, Entry_method);

    env->DeleteLocalRef(appinfo);
    env->DeleteLocalRef(PackageManager);
    env->DeleteLocalRef(java_pkaName);
    return tmp ;

}

void rprocess::setAUTHORITY(char* tmp){

    vector<string> a = string_split(tmp,":");
    providerHost_pkgName=a[0];
    AUTHORITY =a[1];
//    这个打印不出来
//    LOGE("get config pkgName:%s AUTHORITY=%s",providerHost_pkgName.c_str(),AUTHORITY.c_str());
}

bool rprocess::LoadFramework(JNIEnv *env){

    if(GetConfigByProvider(env)){
        for (auto appinfoNativeVec : AppinfoNative_vec)
        {
            load_apk_And_exe_Class_Method(env, appinfoNativeVec);
        }

    }
    return true;
}

jobject rprocess::getRxposedContext(JNIEnv *env)
{
    char sdk[128] = "0";
    __system_property_get("ro.system.build.version.release", sdk);
    int sdk_verison = atoi(sdk);
    switch (sdk_verison) {
        case 9:
            return getSystemContext(env);
        case 10:
            return getApplicationContext(env,processName);

    };

}


void rprocess::load_apk_And_exe_Class_Method(JNIEnv *pEnv, AppinfoNative *appinfoNativeVec) {

    LOGE("enbale pkgName:%s ",appinfoNativeVec->pkgName.c_str());
    jstring apk = pEnv->NewStringUTF(appinfoNativeVec->source.c_str());
    jstring dexOutput = nullptr;
    jstring nativelib = pEnv->NewStringUTF(appinfoNativeVec->NativelibPath.c_str());
    jstring class_name = pEnv->NewStringUTF(appinfoNativeVec->Entry_class.c_str());
    jstring method_name = pEnv->NewStringUTF(appinfoNativeVec->Entry_method.c_str());
    jobjectArray JAAR = nullptr;
    jclass DexClassLoader_cls = pEnv->FindClass("dalvik/system/DexClassLoader");
    jmethodID init = pEnv->GetMethodID(DexClassLoader_cls, "<init>", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/ClassLoader;)V");
    jclass ClassLoader_cls = pEnv->FindClass("java/lang/ClassLoader");
    jmethodID ClassLoader_getSystemClassLoader_cls = pEnv->GetStaticMethodID(ClassLoader_cls, "getSystemClassLoader", "()Ljava/lang/ClassLoader;");
    jmethodID Class_loadClass_method = pEnv->GetMethodID(ClassLoader_cls, "loadClass", "(Ljava/lang/String;)Ljava/lang/Class;");
    jclass Class_cls = pEnv->FindClass("java/lang/Class");
    jclass Method_cls = pEnv->FindClass("java/lang/reflect/Method");
    jmethodID invoke_met =  pEnv->GetMethodID(Method_cls,"invoke","(Ljava/lang/Object;[Ljava/lang/Object;)Ljava/lang/Object;");
    jmethodID clsmethod_method = pEnv->GetMethodID(Class_cls,"getMethod","(Ljava/lang/String;[Ljava/lang/Class;)Ljava/lang/reflect/Method;");
    LOGE("new classload ");
    jobject entryMethod_obj = nullptr;
    jobject entryClass_obj = nullptr;

    if(LoadModuleClassLoader == nullptr) {
        jobject classload = pEnv->CallStaticObjectMethod(ClassLoader_cls,
                                                             ClassLoader_getSystemClassLoader_cls);
        LoadModuleClassLoader = pEnv->NewGlobalRef(classload);
    }
    jobject dexClassLoader = pEnv->NewObject(DexClassLoader_cls,init,apk,dexOutput,nativelib,LoadModuleClassLoader);
    pEnv->DeleteGlobalRef(LoadModuleClassLoader);
    LoadModuleClassLoader = pEnv->NewGlobalRef(dexClassLoader);
//    LoadModuleClassLoader = pEnv->NewGlobalRef(dexClassLoader);
    if(pEnv->ExceptionCheck()){
        goto out2;
    }
//    Class clazz = dexClassLoader.loadClass(name);
    LOGE("loadclass Entry_class=%s ",appinfoNativeVec->Entry_class.c_str());
    entryClass_obj = pEnv->CallObjectMethod(dexClassLoader, Class_loadClass_method, class_name);
    if(pEnv->ExceptionCheck()){
        goto out2;
    }
//    Method native_hook = clazz.getMethod("native_hook");
    LOGE("invoke method_name=%s ",appinfoNativeVec->Entry_method.c_str());

    entryMethod_obj = pEnv->CallObjectMethod(entryClass_obj, clsmethod_method, method_name, JAAR);
    if(pEnv->ExceptionCheck()){
        goto out2;
    }
//    native_hook.invoke(clazz);
    pEnv->CallObjectMethod(entryMethod_obj, invoke_met, entryClass_obj, JAAR);
    if(pEnv->ExceptionCheck()){

    }
    out2:
    pEnv->ExceptionDescribe();
    pEnv->ExceptionClear();//清除引发的异常，在Java层不会打印异常堆栈信息，如果不清除，后面的调用ThrowNew抛出的异常堆栈信息会
    pEnv->DeleteLocalRef(entryMethod_obj);
    pEnv->DeleteLocalRef(dexClassLoader);
    pEnv->DeleteLocalRef(entryClass_obj);
}

jobject rprocess::getApplicationContext(JNIEnv *env,string pkgName) {

    jobject ApplicationContext = nullptr;
    //获取Activity Thread的实例对象
    jclass mActivityThreadClass = env->FindClass("android/app/ActivityThread");
    jclass mLoadedApkClass = env->FindClass("android/app/LoadedApk");
    jclass mContextImplClass = env->FindClass("android/app/ContextImpl");
    jclass mCompatibilityInfoClass = env->FindClass("android/content/res/CompatibilityInfo");
    jmethodID getLoadedApkMethod = env->GetMethodID(mActivityThreadClass,"getPackageInfoNoCheck","(Landroid/content/pm/ApplicationInfo;Landroid/content/res/CompatibilityInfo;)Landroid/app/LoadedApk;");

    if(NDK_ExceptionCheck(env,"find class android/app/ActivityThread failed")){
        return nullptr;
    }

    jmethodID currentActivityThread = env->GetStaticMethodID(mActivityThreadClass, "currentActivityThread", "()Landroid/app/ActivityThread;");
    jobject at = env->CallStaticObjectMethod(mActivityThreadClass, currentActivityThread);
    if(at == nullptr){
//        LOGE("Failed to call currentActivityThread,at is null");
        return nullptr;
    }

    //ContextImpl，getSystemContext
    jmethodID getApplication = env->GetMethodID(mActivityThreadClass, "getSystemContext", "()Landroid/app/ContextImpl;");
    jobject SystemContext = env->CallObjectMethod(at, getApplication);
    if(SystemContext == nullptr){
        LOGE("Failed to call getRxposedContext,SystemContext is null");
        return nullptr;
    }
    jmethodID getPackageManager_method = env->GetMethodID(mContextImplClass, "getPackageManager", "()Landroid/content/pm/PackageManager;");
    jclass PackageManager_cls = env->FindClass("android/content/pm/PackageManager");
    jmethodID pm_getApplicationInfo_method = env->GetMethodID(PackageManager_cls, "getApplicationInfo", "(Ljava/lang/String;I)Landroid/content/pm/ApplicationInfo;");
    jobject PackageManager = env->CallObjectMethod(SystemContext, getPackageManager_method);
    jobject java_pkaName = env->NewStringUTF(pkgName.c_str());
    jobject applicationInfo = env->CallObjectMethod(PackageManager,pm_getApplicationInfo_method,java_pkaName,0);
    string err_message = "processName:"+pkgName+" getApplicationInfo not found,return SystemContext";
    if(NDK_ExceptionCheck(env,(char *)err_message.c_str())){
        ApplicationContext = SystemContext;
        return ApplicationContext;
    }

    jfieldID mCompatibilityInfoDefaultField = env->GetStaticFieldID(mCompatibilityInfoClass,"DEFAULT_COMPATIBILITY_INFO","Landroid/content/res/CompatibilityInfo;");
    jobject mCompatibilityInfo = env->GetStaticObjectField(mCompatibilityInfoClass,mCompatibilityInfoDefaultField);
    jmethodID createAppContext_method = env->GetStaticMethodID(mContextImplClass, "createAppContext", "(Landroid/app/ActivityThread;Landroid/app/LoadedApk;)Landroid/app/ContextImpl;");
    jobject  mLoadedApk = env->CallObjectMethod(at,getLoadedApkMethod,applicationInfo,mCompatibilityInfo);
    ApplicationContext = env->CallStaticObjectMethod(mContextImplClass,createAppContext_method,at,mLoadedApk);

    env->DeleteLocalRef(at);
    return ApplicationContext;
}

void rprocess::setProcessName(char *tmp) {
    processName = tmp;
}

void rprocess::setRxposedContext(jobject RxposedContext) {

}

bool rprocess::is_isIsolatedProcess() {
    int uid = getuid();
    return (uid >= 99000 && uid <= 99999)|| (uid >= 90000 && uid <= 98999);
}
