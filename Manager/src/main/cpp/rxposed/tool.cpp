//
// Created by chic on 2023/5/15.
//


#include <dlfcn.h>
#include "tool.h"
#include "artmethod_native_hook.h"
#include "FunHook.h"
#include "linker.h"
#include "elf_symbol_resolver.h"

using namespace std;




android_namespace_t* g_default_namespace = static_cast<android_namespace_t *>(linker_resolve_elf_internal_symbol(
        get_android_linker_path(), "__dl_g_default_namespace"));

soinfo* (*soinf_alloc_fun)(android_namespace_t* , const char* ,const struct stat* , off64_t ,uint32_t ) = (soinfo* (*)(android_namespace_t* , const char* ,const struct stat* , off64_t ,uint32_t )) linker_resolve_elf_internal_symbol(
        get_android_linker_path(), "__dl__Z12soinfo_allocP19android_namespace_tPKcPK4statlj");

soinfo* (*solist_get_head)() = (soinfo* (*)()) linker_resolve_elf_internal_symbol(
        get_android_linker_path(), "__dl__Z15solist_get_headv");

soinfo* (*solist_get_somain)() = (soinfo* (*)()) linker_resolve_elf_internal_symbol(
        get_android_linker_path(), "__dl__Z17solist_get_somainv");

char* (*soinfo_get_soname)(soinfo*) = (char* (*)(soinfo*)) linker_resolve_elf_internal_symbol(
        get_android_linker_path(), "__dl__ZNK6soinfo10get_sonameEv");

bool (*solist_remove_soinfo)(soinfo*) = (bool  (*)(soinfo*)) linker_resolve_elf_internal_symbol(
        get_android_linker_path(), "__dl__Z20solist_remove_soinfoP6soinfo");

ElfW(Addr) (*call_ifunc_resolver)(ElfW(Addr) resolver_addr) =(ElfW(Addr) (*)(ElfW(Addr) resolver_addr)) linker_resolve_elf_internal_symbol(
        get_android_linker_path(), "__dl__Z19call_ifunc_resolvery");


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


//清除异常，打印java堆栈好用户输入，继续执行
bool NDK_ExceptionCheck(JNIEnv *env,const char* message){

    if(env->ExceptionCheck()){
        LOGD("%s",message);
        env->ExceptionDescribe();
        env->ExceptionClear();
        return true;
    }
    return false;
}

//请求转接，获取应用信息
AppinfoNative* GetRxAppInfoNative(JNIEnv *env, jobject android_Context,string AUTHORITY,string pkgName) {

    string Provider_call_method = "appinfo";
    auto arg_apk_path = env->NewStringUTF(RXPOSED_APK_PATH);
    auto arg_entry_class = env->NewStringUTF(RXPOSED_CLS_ENTRY);
    auto arg_entry_method = env->NewStringUTF(RXPOSED_MTD_CLASS);
    auto arg_apk_native_lib = env->NewStringUTF(RXPOSED_APK_NATIVE_LIB);

    jobject obj_bundle = GetRxposedProvider(env, android_Context, AUTHORITY, Provider_call_method,pkgName);
    jclass Bundle_cls = env->FindClass("android/os/Bundle");
    jmethodID Bundle_getString_method = env->GetMethodID(Bundle_cls, "getString","(Ljava/lang/String;)Ljava/lang/String;");
    auto japk_path = env->CallObjectMethod(obj_bundle, Bundle_getString_method,arg_apk_path);
    auto japk_native_lib = env->CallObjectMethod(obj_bundle, Bundle_getString_method,arg_apk_native_lib);
    auto jentry_class = env->CallObjectMethod(obj_bundle, Bundle_getString_method,arg_entry_class);
    auto jentry_method =env->CallObjectMethod(obj_bundle, Bundle_getString_method,arg_entry_method);

    string apk_path = env->GetStringUTFChars(static_cast<jstring>(japk_path), nullptr);
    string apk_native_lib = env->GetStringUTFChars(static_cast<jstring>(japk_native_lib), nullptr);
    string entry_class = env->GetStringUTFChars(static_cast<jstring>(jentry_class), nullptr);
    string entry_method = env->GetStringUTFChars(static_cast<jstring>(jentry_method), nullptr);

    auto * tmp = new AppinfoNative(pkgName, apk_path, apk_native_lib, entry_class, entry_method,"false");
    return tmp;
}

//andorid 11以后，app应用有可见性的问题，getApplicationInfo 需要权限，所以弃用
AppinfoNative* GetPmAppInfoNative(JNIEnv *env, jobject android_Context, string pkgName){

    jclass Context_cls = env->FindClass("android/content/Context");
    jmethodID getPackageManager_method = env->GetMethodID(Context_cls, "getPackageManager", "()Landroid/content/pm/PackageManager;");
    jclass PackageManager_cls = env->FindClass("android/content/pm/PackageManager");
    jmethodID pm_getApplicationInfo_method = env->GetMethodID(PackageManager_cls, "getApplicationInfo", "(Ljava/lang/String;I)Landroid/content/pm/ApplicationInfo;");
    jobject PackageManager = env->CallObjectMethod(android_Context, getPackageManager_method);
    jobject java_pkaName = env->NewStringUTF(pkgName.c_str());

    jobject appinfo = env->CallObjectMethod(PackageManager,pm_getApplicationInfo_method,java_pkaName,GET_META_DATA);
    jclass ApplicationInfo_cls = env->FindClass("android/content/pm/ApplicationInfo");
    jfieldID sourceDir_field = env->GetFieldID(ApplicationInfo_cls,"sourceDir","Ljava/lang/String;");
    jfieldID nativeLibraryDir_field = env->GetFieldID(ApplicationInfo_cls,"nativeLibraryDir","Ljava/lang/String;");
    jfieldID metaData_field = env->GetFieldID(ApplicationInfo_cls,"metaData","Landroid/os/Bundle;");
    jclass Bundle_cls = env->FindClass("android/os/Bundle");
    jmethodID mateData_getString_method = env->GetMethodID(Bundle_cls, "getString", "(Ljava/lang/String;)Ljava/lang/String;");

    if(appinfo == nullptr){
        LOGE("appinfo == nullptr");
        return nullptr;
    }
    if(NDK_ExceptionCheck(env,"GetPmAppInfoNative error")){
        LOGE("ndk error");
        return nullptr;
    }
    jstring tmp_sourceDir = static_cast<jstring>(env->GetObjectField(appinfo,sourceDir_field));
    const char * sourceDir = env->GetStringUTFChars(tmp_sourceDir, nullptr);
    env->DeleteLocalRef(tmp_sourceDir);

    jstring tmp_nativeLibraryDir = static_cast<jstring>(env->GetObjectField(appinfo,nativeLibraryDir_field));
    const char * nativeLibraryDir = env->GetStringUTFChars(tmp_nativeLibraryDir, nullptr);
    env->DeleteLocalRef(tmp_nativeLibraryDir);



    jobject tmp_metaData = static_cast<jstring>(env->GetObjectField(appinfo,metaData_field));
    jstring rxposed_clsentry = env->NewStringUTF(RXPOSED_CLS_ENTRY);
    jstring rxposed_mtdentry = env->NewStringUTF(RXPOSED_MTD_CLASS);

    jstring js_entry_class = static_cast<jstring>(env->CallObjectMethod(tmp_metaData,mateData_getString_method,rxposed_clsentry));
    jstring js_entry_method = static_cast<jstring>(env->CallObjectMethod(tmp_metaData,mateData_getString_method,rxposed_mtdentry));

    const char *  entry_class = env->GetStringUTFChars(js_entry_class, nullptr);
    const char *  entry_method = env->GetStringUTFChars(js_entry_method, nullptr);


    AppinfoNative * tmp = new AppinfoNative(pkgName, sourceDir, nativeLibraryDir, entry_class, entry_method,"false");


    env->DeleteLocalRef(js_entry_method);
    env->DeleteLocalRef(js_entry_class);
    env->DeleteLocalRef(rxposed_mtdentry);
    env->DeleteLocalRef(rxposed_clsentry);
    env->DeleteLocalRef(tmp_metaData);


    env->DeleteLocalRef(appinfo);
    env->DeleteLocalRef(PackageManager);
    env->DeleteLocalRef(java_pkaName);
    return tmp ;

}
jobject PathClassLoaderLoadAPK(JNIEnv *pEnv,jstring apkSource,jstring nativelib){

    jclass PathClassLoader_cls = pEnv->FindClass("dalvik/system/PathClassLoader");
    jmethodID PathClassLoader_init_mth = pEnv->GetMethodID(PathClassLoader_cls, "<init>", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/ClassLoader;)V");
    jclass ClassLoader_cls = pEnv->FindClass("java/lang/ClassLoader");
    jmethodID ClassLoader_getSystemClassLoader_cls = pEnv->GetStaticMethodID(ClassLoader_cls, "getSystemClassLoader", "()Ljava/lang/ClassLoader;");
    jobject SystemClassLoader_obj = pEnv->CallStaticObjectMethod(ClassLoader_cls,ClassLoader_getSystemClassLoader_cls);
    jobject ApkClassLoader = pEnv->NewObject(PathClassLoader_cls,PathClassLoader_init_mth,apkSource,nativelib,SystemClassLoader_obj);
    pEnv->DeleteLocalRef(SystemClassLoader_obj);
    return ApkClassLoader;
}

void load_apk_And_Call_Class_Entry_Method(JNIEnv *pEnv, jobject android_context, AppinfoNative *appinfoNativeVec) {

    LOGE("enbale pkgName:%s ",appinfoNativeVec->source.c_str());
    jobject  ApkClassLoader = nullptr;
    jobject entryClass_obj = nullptr;
    jstring apkSource = pEnv->NewStringUTF(appinfoNativeVec->source.c_str());
    if(strncmp(appinfoNativeVec->hide.c_str(),"true", strlen("true"))==0){
//        ApkClassLoader = hideLoadApkModule(pEnv, (char*)appinfoNativeVec->source.c_str());
    } else{
        jstring nativelib = pEnv->NewStringUTF(appinfoNativeVec->NativelibPath.c_str());
        ApkClassLoader = PathClassLoaderLoadAPK(pEnv, apkSource, nativelib);
    }

    jclass ClassLoader_cls = pEnv->FindClass("java/lang/ClassLoader");
    jmethodID Class_loadClass_method = pEnv->GetMethodID(ClassLoader_cls, "loadClass", "(Ljava/lang/String;)Ljava/lang/Class;");
    jstring class_name = pEnv->NewStringUTF(appinfoNativeVec->Entry_class.c_str());

    if(pEnv->ExceptionCheck()){
//        goto out2;
    }
//    Class clazz = dexClassLoader.loadClass(name);
    LOGE("loadclass Entry_class=%s ",appinfoNativeVec->Entry_class.c_str());
    entryClass_obj = pEnv->CallObjectMethod(ApkClassLoader, Class_loadClass_method, class_name);
    if(pEnv->ExceptionCheck()){
//        goto out2;
    }
//    Method native_hook = clazz.getMethod("native_hook");
    LOGE("invoke method_name=%s ",appinfoNativeVec->Entry_method.c_str());
    jmethodID call_method_mth = pEnv->GetStaticMethodID(static_cast<jclass>(entryClass_obj), appinfoNativeVec->Entry_method.c_str(), "(Landroid/content/Context;Ljava/lang/String;)V");
    DEBUG();
    if(pEnv->ExceptionCheck()){
        goto out2;
    }
    DEBUG();
    pEnv->CallStaticVoidMethod(static_cast<jclass>(entryClass_obj), call_method_mth, android_context, apkSource);
    DEBUG();
    if(pEnv->ExceptionCheck()){

    }
    DEBUG();
    out2:
    pEnv->ExceptionDescribe();
    pEnv->ExceptionClear();//清除引发的异常，在Java层不会打印异常堆栈信息，如果不清除，后面的调用ThrowNew抛出的异常堆栈信息会
    pEnv->DeleteLocalRef(ApkClassLoader);
    pEnv->DeleteLocalRef(entryClass_obj);
}



jobject GetRxposedProvider(JNIEnv *env, jobject android_Context , string& AUTHORITY, const string& Provider_call_method, const string& Provider_call_arg) {
    LOGD("CALL rprocess GetRxposedProvider");
    string err_message = "";

    string Provider_AUTHORITY = "content://" + AUTHORITY;
    //请求provider的参数，目前没有区分
    DEBUG("")

    jclass Context_cls = env->FindClass("android/content/Context");
    jclass Uri_cls = env->FindClass("android/net/Uri");
    jclass ContentResolver_cls = env->FindClass("android/content/ContentResolver");
    jmethodID context_getContentResolver_method = env->GetMethodID(Context_cls,"getContentResolver",
                                                                   "()Landroid/content/ContentResolver;");
    jmethodID ContentResolver_call_method = env->GetMethodID(ContentResolver_cls, "call",
                                                             "(Landroid/net/Uri;Ljava/lang/String;Ljava/lang/String;Landroid/os/Bundle;)Landroid/os/Bundle;");
    jmethodID Uri_parse_method = env->GetStaticMethodID(Uri_cls, "parse",
                                                        "(Ljava/lang/String;)Landroid/net/Uri;");

    jobject extras = nullptr;
    DEBUG("")
    jstring method = env->NewStringUTF(Provider_call_method.c_str());
    jstring method_arg = env->NewStringUTF(Provider_call_arg.c_str());
    jobject URI = env->CallStaticObjectMethod(Uri_cls, Uri_parse_method,
                                              env->NewStringUTF(Provider_AUTHORITY.c_str()));
    DEBUG("")

    jobject ContentResolver_obj = env->CallObjectMethod(android_Context,
                                                        context_getContentResolver_method);
    DEBUG("")

    jobject bundle_obj = env->CallObjectMethod(ContentResolver_obj, ContentResolver_call_method,
                                               URI, method, method_arg, extras);
    DEBUG("")

    err_message ="processName:" + Provider_call_arg + "get config privider failed AUTHORITY:" + Provider_AUTHORITY;
    if (NDK_ExceptionCheck(env, (char *) err_message.c_str())) {
        return nullptr;
    }
    DEBUG("")

    return bundle_obj;
}


JNIEnv *Pre_GetEnv() {
    void * libandroid_runtime =  dlopen("libandroid_runtime.so",RTLD_NOW);
    void *getAndroidRuntimeEnv = reinterpret_cast<void *>(dlsym(libandroid_runtime,"_ZN7android14AndroidRuntime9getJNIEnvEv"));
    dlclose(libandroid_runtime);
    return ((JNIEnv*(*)())getAndroidRuntimeEnv)();
}


jobject getSystemContext(JNIEnv *env)
{
    //获取Activity Thread的实例对象
    jclass activityThreadClass = env->FindClass("android/app/ActivityThread");
    if(NDK_ExceptionCheck(env,"find class android/app/ActivityThread failed")){
        LOGE("find class android/app/ActivityThread failed");
        return nullptr;
    }
    jmethodID currentActivityThread = env->GetStaticMethodID(activityThreadClass, "currentActivityThread", "()Landroid/app/ActivityThread;");
    jobject at = env->CallStaticObjectMethod(activityThreadClass, currentActivityThread);
    if(at == nullptr){
        LOGE("Failed to call currentActivityThread,at is null");
        return nullptr;
    }
    //ContextImpl，RxposedContext
    jmethodID getApplication = env->GetMethodID(activityThreadClass, "getSystemContext", "()Landroid/app/ContextImpl;");
    jobject context = env->CallObjectMethod(at, getApplication);
    if(context == nullptr){
        LOGE("Failed to call getRxposedContext,context is null");
        return nullptr;
    }
    env->DeleteLocalRef(at);
    return context;
}


jobject getContext(JNIEnv *env)
{
    //获取Activity Thread的实例对象
    jclass activityThreadClass = env->FindClass("android/app/ActivityThread");
    if(NDK_ExceptionCheck(env,"find class android/app/ActivityThread failed")){
        return nullptr;
    }
    jmethodID currentActivityThread = env->GetStaticMethodID(activityThreadClass, "currentActivityThread", "()Landroid/app/ActivityThread;");
    jobject at = env->CallStaticObjectMethod(activityThreadClass, currentActivityThread);
    if(at == nullptr){
//        LOGE("Failed to call currentActivityThread,at is null");
        return nullptr;
    }
    //ContextImpl，RxposedContext
    jmethodID getApplication = env->GetMethodID(activityThreadClass, "getApplication", "()Landroid/app/Application;");
    jobject context = env->CallObjectMethod(at, getApplication);
    if(context == nullptr){
        LOGE("Failed to call getRxposedContext,context is null");
        return nullptr;
    }
    env->DeleteLocalRef(at);
    return context;
}





jobject CreateApplicationContext(JNIEnv *env, string pkgName,uid_t currentUid) {

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
    jmethodID pm_getPackagesForUid_method = env->GetMethodID(PackageManager_cls, "getPackagesForUid", "(I)[Ljava/lang/String;");
    jobject PackageManager = env->CallObjectMethod(SystemContext, getPackageManager_method);

    jobjectArray packageNameArray = static_cast<jobjectArray>(env->CallObjectMethod(PackageManager,pm_getPackagesForUid_method,(jint) currentUid));
    jstring jstrPackageName;
    if (packageNameArray != nullptr && env->GetArrayLength(packageNameArray) > 0) {
        jstrPackageName = (jstring) env->GetObjectArrayElement(packageNameArray, 0);
    } else{
        return nullptr;
    }
    jobject applicationInfo = env->CallObjectMethod(PackageManager,pm_getApplicationInfo_method,jstrPackageName,0);
    string err_message = " getApplicationInfo not found,return SystemContext,uid = "+currentUid;
    if(NDK_ExceptionCheck(env,(char *)err_message.c_str())){
//        int version = android_get_device_api_level()
        return nullptr;
    }
    jfieldID mCompatibilityInfoDefaultField = env->GetStaticFieldID(mCompatibilityInfoClass,"DEFAULT_COMPATIBILITY_INFO","Landroid/content/res/CompatibilityInfo;");
    jobject mCompatibilityInfo = env->GetStaticObjectField(mCompatibilityInfoClass,mCompatibilityInfoDefaultField);
    jmethodID createAppContext_method = env->GetStaticMethodID(mContextImplClass, "createAppContext", "(Landroid/app/ActivityThread;Landroid/app/LoadedApk;)Landroid/app/ContextImpl;");
    jobject  mLoadedApk = env->CallObjectMethod(at,getLoadedApkMethod,applicationInfo,mCompatibilityInfo);
    ApplicationContext = env->CallStaticObjectMethod(mContextImplClass,createAppContext_method,at,mLoadedApk);

    env->DeleteLocalRef(at);
    return ApplicationContext;
}

bool art_method_hook_init(JNIEnv* env){

    jclass Zygote_cls =  env->FindClass("com/android/internal/os/Zygote");

    jclass  Process_cls = env->FindClass("android/os/Process");
    jmethodID javamethod = env->GetStaticMethodID(Process_cls,"getUidForName", "(Ljava/lang/String;)I");
    void * libandroid_runtime =  dlopen("libandroid_runtime.so",RTLD_NOW);
    uintptr_t getUidForName = reinterpret_cast<uintptr_t>(dlsym(libandroid_runtime,"_Z32android_os_Process_getUidForNameP7_JNIEnvP8_jobjectP8_jstring"));


    INIT_HOOK_PlatformABI(env, nullptr,javamethod,(uintptr_t*)getUidForName,0x109);

    uintptr_t  art_javamethod_method = GetArtMethod(env, Zygote_cls,javamethod);
    uintptr_t native_art_art_javamethod_method = GetOriginalNativeFunction((uintptr_t *)art_javamethod_method);

    if(native_art_art_javamethod_method == getUidForName){
        return true;
    } else{
        return false;
    }

}

uintptr_t getJmethod_JniFunction(JNIEnv* env,jclass jclass1,jmethodID jmethodId){


    uintptr_t  jmethodArtmethod = GetArtMethod(env, jclass1, jmethodId);
    uintptr_t native_art_javamethod_method = GetOriginalNativeFunction((uintptr_t *)jmethodArtmethod);
    return native_art_javamethod_method;
}
void unHookJmethod_JniFunction(JNIEnv* env,jclass jclass1,jmethodID jmethodId) {

    uintptr_t  jmethodArtmethod = GetArtMethod(env, jclass1,jmethodId);
    unHookJniNativeFunction((uintptr_t *)jmethodArtmethod);
}
uintptr_t HookJmethod_JniFunction(JNIEnv* env,jclass jclass1,jmethodID jmethodId,uintptr_t fun_addr) {

    uintptr_t jmethodArtmethod = GetArtMethod(env, jclass1,jmethodId);
    if(jmethodArtmethod == NULL){
        LOGE("jmethodArtmethod is null");
        return NULL;
    }
    return HookJniNativeFunction( (uintptr_t*)jmethodArtmethod,fun_addr);
}

void find_class_method(JNIEnv* env){
    jclass Class_cls =  env->FindClass("java/lang/Class");
    jclass Zygote_cls =  env->FindClass("com/android/internal/os/Zygote");

    jmethodID get_declared_method_id = env->GetMethodID(Class_cls, "getDeclaredMethods","()[Ljava/lang/reflect/Method;");

    jobjectArray methodsArray  = static_cast<jobjectArray>(env->CallObjectMethod(Zygote_cls,get_declared_method_id));

    // 调用 getDeclaredMethods 方法
    if (methodsArray == NULL) {
        LOGE("Failed to get methods array");
    }
    jmethodID nativeForkAndSpecialize_Method = nullptr;
    // 获取数组长度
    jsize arrayLength = env->GetArrayLength(methodsArray);
    LOGE("get methods array = %d",arrayLength);

    // 遍历数组获取每个方法的信息
    for (int i = 0; i < arrayLength; i++) {
        jobject methodObj = env->GetObjectArrayElement(methodsArray, i);

        // 获取方法信息
        jclass methodClass = env->GetObjectClass(methodObj);
        jmethodID getNameMethod = env->GetMethodID(methodClass, "getName", "()Ljava/lang/String;");
        jstring methodName = (jstring)env->CallObjectMethod(methodObj, getNameMethod);

        // 将方法名转换为 C 字符串
        const char* methodNameCStr = env->GetStringUTFChars(methodName, NULL);
        int re = strncmp(methodNameCStr,"nativeForkAndSpecialize", strlen(methodNameCStr));

        LOGE("Method name: %s %d", methodNameCStr,re);
        nativeForkAndSpecialize_Method = getNameMethod;

        // 打印方法名

        // 释放局部引用
        env->DeleteLocalRef(methodObj);
        env->DeleteLocalRef(methodClass);
        env->DeleteLocalRef(methodName);
    }
}
//这部分需要权限，在第一次注入zygote的时候，会通过root权限修改selinux规则
string getCurrentAppRxposedConfig(JNIEnv* env,string AUTHORITY , string callName,string method ,uid_t currentUid){
    DEBUG();
    jstring key = env->NewStringUTF("ModuleList");
    DEBUG();
    string uid_str = std::to_string(currentUid);
    DEBUG();
    jobject obj_bundle = getConfigByProvider(env,  AUTHORITY,callName  ,method,uid_str);
    DEBUG();
    jclass Bundle_cls = env->FindClass("android/os/Bundle");
    jmethodID Bundle_getStringArrayList_method = env->GetMethodID(Bundle_cls, "getStringArrayList","(Ljava/lang/String;)Ljava/util/ArrayList;");
    jclass ArrayList_cls = env->FindClass("java/util/ArrayList");
    jmethodID ArrayList_size_method = env->GetMethodID(ArrayList_cls, "size","()I");
    jmethodID ArrayList_get_method = env->GetMethodID(ArrayList_cls, "get","(I)Ljava/lang/Object;");
    jobject config = env->CallObjectMethod(obj_bundle, Bundle_getStringArrayList_method,key);
    if(config == nullptr){
        return "";
    }
//    string bask = "base.apk";
    jint size = env->CallIntMethod(config,ArrayList_size_method);
    string appinfoList;
    for(int i=0;i<size;i++){
        jstring element = static_cast<jstring>(env->CallObjectMethod(config, ArrayList_get_method,i));
        string appinfo = env->GetStringUTFChars(element,0);
        appinfoList = appinfoList +"|"+appinfo;
    }
    return appinfoList;
}

jobject getConfigByProvider(JNIEnv* env,string AUTHORITY , string callName,string method ,string uid_str){
    DEBUG()
    jclass ServiceManager_cls = env->FindClass("android/app/ActivityManager");
    auto IActivityManager_class = env->FindClass("android/app/IActivityManager");
    auto IActivityManager_getContentProviderExternal_method = env->GetMethodID(IActivityManager_class,"getContentProviderExternal","(Ljava/lang/String;ILandroid/os/IBinder;Ljava/lang/String;)Landroid/app/ContentProviderHolder;");
    auto IActivityManager_removeContentProviderExternalAsUser_method = env->GetMethodID(IActivityManager_class,"removeContentProviderExternalAsUser","(Ljava/lang/String;Landroid/os/IBinder;I)V");
    auto ContentProviderHolder_class = env->FindClass("android/app/ContentProviderHolder");
    auto Binder_class = env->FindClass("android/os/Binder");
    auto Bundle_class = env->FindClass("android/os/Bundle");
    jmethodID Binder_init = env->GetMethodID(Binder_class, "<init>","()V");
    jmethodID Binder_getCallingUid = env->GetStaticMethodID(Binder_class, "getCallingUid", "()I");
    jmethodID Bundle_init = env->GetMethodID(Bundle_class, "<init>","()V");
    jmethodID Bundle_getString_method = env->GetMethodID(Bundle_class, "getString","(Ljava/lang/String;)Ljava/lang/String;");
    auto IContentProvider_class = env->FindClass("android/content/IContentProvider");
    auto ContentProviderHolder_provider_filed = env->GetFieldID(ContentProviderHolder_class,"provider","Landroid/content/IContentProvider;");
    jmethodID ActivityManager_getservice_method_ = env->GetStaticMethodID(ServiceManager_cls, "getService", "()Landroid/app/IActivityManager;");
    jobject IActivityManager_Obj = env->CallStaticObjectMethod(ServiceManager_cls, ActivityManager_getservice_method_);
    if(IActivityManager_Obj == nullptr){
        NDK_ExceptionCheck(env,"ActivityManager_getservice_method_ is null");
    }
//    jstring AUTHORITY_jstring = env->NewStringUTF("hepta.rxposed.manager.Provider");
    jstring j_AUTHORITY = env->NewStringUTF(AUTHORITY.c_str());
    jstring tag_jstring = env->NewStringUTF("*cmd*");
    jstring j_callingPkg = env->NewStringUTF(callName.c_str());
    jstring j_method = env->NewStringUTF(method.c_str());
    jstring j_uid = env->NewStringUTF(uid_str.c_str());

    auto token_ibinderObj = env->NewObject(Binder_class,Binder_init);
    auto mExtras_BundleObj = env->NewObject(Bundle_class,Bundle_init);

    DEBUG()
    LOGE("j_AUTHORITY : %s",AUTHORITY.c_str());
    jobject holder_ContentProviderHolderObj = env->CallObjectMethod(IActivityManager_Obj,IActivityManager_getContentProviderExternal_method,j_AUTHORITY,0,token_ibinderObj,tag_jstring);
    DEBUG()
    jobject  provider_IContentProviderObj = env->GetObjectField(holder_ContentProviderHolderObj,ContentProviderHolder_provider_filed);
    DEBUG()
    jobject ret_bundle;
    if(android_get_device_api_level() == 33){
        jstring root_jstring = env->NewStringUTF("root");
        auto AttributionSource_class = env->FindClass("android/content/AttributionSource");
        jmethodID AttributionSource_init = env->GetMethodID(AttributionSource_class, "<init>","(ILjava/lang/String;Ljava/lang/String;)V");
        jint uid = env->CallStaticIntMethod(Binder_class,Binder_getCallingUid);
        auto  attributionSourceObj = env->NewObject(AttributionSource_class,AttributionSource_init,uid,j_callingPkg,nullptr);

        auto IContentProvider_call_method = env->GetMethodID(IContentProvider_class,"call","(Landroid/content/AttributionSource;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Landroid/os/Bundle;)Landroid/os/Bundle;");
        ret_bundle = env->CallObjectMethod(provider_IContentProviderObj, IContentProvider_call_method, attributionSourceObj, j_AUTHORITY, j_method, j_uid, mExtras_BundleObj);
        env->CallObjectMethod(IActivityManager_Obj,IActivityManager_removeContentProviderExternalAsUser_method,j_AUTHORITY,token_ibinderObj,0);

    } else{

        auto IContentProvider_call_method = env->GetMethodID(IContentProvider_class,"call","(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Landroid/os/Bundle;)Landroid/os/Bundle;");
        ret_bundle = env->CallObjectMethod(provider_IContentProviderObj, IContentProvider_call_method, j_callingPkg,
                                           nullptr, j_AUTHORITY, j_method, j_uid, mExtras_BundleObj);
    }

//    jstring config = static_cast<jstring>(env->CallObjectMethod(ret_bundle, Bundle_getString_method,j_key));
//    const char *  enableUidList_str = env->GetStringUTFChars(config, nullptr);
//    LOGE("get RxConfigPrvider is %s",enableUidList_str);
    DEBUG()

    env->DeleteLocalRef(IActivityManager_Obj);
    DEBUG()

    env->DeleteLocalRef(j_AUTHORITY);

    DEBUG()
    env->DeleteLocalRef(tag_jstring);

    DEBUG()
    env->DeleteLocalRef(mExtras_BundleObj);

    DEBUG()
    env->DeleteLocalRef(token_ibinderObj);

    DEBUG()
    env->DeleteLocalRef(holder_ContentProviderHolderObj);

    DEBUG()
    env->DeleteLocalRef(provider_IContentProviderObj);

    DEBUG()
    env->DeleteLocalRef(j_callingPkg);

    DEBUG()
    env->DeleteLocalRef(j_method);

    DEBUG()
    env->DeleteLocalRef(j_uid);
    DEBUG()

    return ret_bundle;
}









