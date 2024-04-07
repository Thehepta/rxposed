#include <dlfcn.h>
#include <asm-generic/mman-common.h>
#include <fcntl.h>
#include <linux/mman.h>
#include <sys/prctl.h>
#include <sys/mman.h>
#include "rprocess.h"
#include "android_util_api.h"
#include "artmethod_native_hook.h"

#define nativeForkAndSpecialize_sign   "(II[II[[IILjava/lang/String;Ljava/lang/String;[I[IZLjava/lang/String;Ljava/lang/String;Z[Ljava/lang/String;[Ljava/lang/String;ZZ)I"

#define nativeSpecializeAppProcess_sign "(II[II[[IILjava/lang/String;Ljava/lang/String;ZLjava/lang/String;Ljava/lang/String;Z[Ljava/lang/String;[Ljava/lang/String;ZZ)V"


namespace android12 {
    string setArgV0_method_name = "";


    void zygote_unhook(JNIEnv* env, jclass clazz){

        jmethodID nativeForkAndSpecialize_method = env->GetStaticMethodID(clazz,"nativeForkAndSpecialize",  nativeForkAndSpecialize_sign);
        unHookJmethod_JniFunction(env,clazz,nativeForkAndSpecialize_method);

        jmethodID nativeSpecializeAppProcess_method = env->GetStaticMethodID(clazz,"nativeSpecializeAppProcess",  nativeSpecializeAppProcess_sign);
        unHookJmethod_JniFunction(env,clazz,nativeSpecializeAppProcess_method);
    }



    void (*android_os_Process_setArg_org)(JNIEnv *env, jclass clazz, jstring name);
    void android_os_Process_setArg_hook(JNIEnv *env, jclass clazz, jstring name) {
        DEBUG()
        char *pkgName = const_cast<char *>(env->GetStringUTFChars(name, nullptr));
        if (rprocess::GetInstance()->is_Start(env, pkgName)) {
            rprocess::GetInstance()->LoadModule(env);
            android_os_Process_setArg_org(env, clazz, name);
            jmethodID javamethod = env->GetStaticMethodID(clazz, setArgV0_method_name.c_str(),
                                                          "(Ljava/lang/String;)V");
            unHookJmethod_JniFunction(env, clazz, javamethod);
        } else {
            android_os_Process_setArg_org(env, clazz, name);
        }
        DEBUG()
    }


    void HOOK_Process_setArgv0(JNIEnv *env) {
        if(!rprocess::GetInstance()->is_Enable()){
            return ;
        }
        DEBUG()
        jclass Process_cls = env->FindClass("android/os/Process");
        jmethodID javamethod = env->GetStaticMethodID(Process_cls, setArgV0_method_name.c_str(),"(Ljava/lang/String;)V");
        android_os_Process_setArg_org = reinterpret_cast<void (*)(JNIEnv *, jclass,jstring)>(HookJmethod_JniFunction(
                env, Process_cls, javamethod, (uintptr_t) android_os_Process_setArg_hook));
        DEBUG()
    }




    // hook getUidForName 函数是为了进行rxposed进程激活状态
    jint (*android_os_Process_getUidForName_org)(JNIEnv *env, jclass clazz, jstring name);
    jint android_os_Process_getUidForName_hook(JNIEnv *env, jclass clazz, jstring name) {
        DEBUG()
        const char *Authority_arg = const_cast<char *>(env->GetStringUTFChars(name, nullptr));
        const char *Authority = rprocess::GetInstance()->getStatusAuthority();
        int ret = 0;
        if (strncmp(Authority_arg, Authority, strlen(Authority_arg)) == 0) {
            ret = rprocess::GetInstance()->getHostUid();
        } else {
            ret = android_os_Process_getUidForName_org(env, clazz, name);
        }
        DEBUG()
        return ret;
    }

    void HOOK_Process_getUidForName(JNIEnv *env) {
        jclass Process_cls = env->FindClass("android/os/Process");
        jmethodID getUidForName_Jmethod = env->GetStaticMethodID(Process_cls, "getUidForName",
                                                                 "(Ljava/lang/String;)I");

        android_os_Process_getUidForName_org = reinterpret_cast<jint (*)(JNIEnv *, jclass,
                                                                         jstring)>(HookJmethod_JniFunction(
                env, Process_cls, getUidForName_Jmethod,
                (uintptr_t) android_os_Process_getUidForName_hook));
    }




    jint (*nativeForkAndSpecialize_org)(JNIEnv* env, jclass, jint uid, jint gid, jintArray gids,
                                        jint runtime_flags, jobjectArray rlimits,
                                        jint mount_external, jstring se_info, jstring nice_name,
                                        jintArray managed_fds_to_close, jintArray managed_fds_to_ignore, jboolean is_child_zygote,
                                        jstring instruction_set, jstring app_data_dir, jboolean is_top_app,
                                        jobjectArray pkg_data_info_list, jobjectArray whitelisted_data_info_list,
                                        jboolean mount_data_dirs, jboolean mount_storage_dirs);

    jint nativeForkAndSpecialize_hook(JNIEnv* env, jclass clazz, jint uid, jint gid, jintArray gids,
                                      jint runtime_flags, jobjectArray rlimits,
                                      jint mount_external, jstring se_info, jstring nice_name,
                                      jintArray managed_fds_to_close, jintArray managed_fds_to_ignore, jboolean is_child_zygote,
                                      jstring instruction_set, jstring app_data_dir, jboolean is_top_app,
                                      jobjectArray pkg_data_info_list, jobjectArray whitelisted_data_info_list,
                                      jboolean mount_data_dirs, jboolean mount_storage_dirs){
        DEBUG()
        LOGE("nativeForkAndSpecialize_before start uid = %d currentuid = %d pid = %d",uid,getuid(),getpid());
        char * pkgName = const_cast<char *>(env->GetStringUTFChars(nice_name, nullptr));

        rprocess::GetInstance()->setProcessInfo(pkgName, uid, gid);
        bool is_HostProcess = rprocess::GetInstance()->is_HostProcess();
        bool is_Init = false;
        if(!is_HostProcess){
            is_Init = rprocess::GetInstance()->InitEnable(env);
        }

//    nativeForkAndSpecialize_org 函数必须后调用，发起contextprovider请求才能成功
        int pid = nativeForkAndSpecialize_org(  env,  clazz,  uid,  gid,  gids,runtime_flags,  rlimits,
                                                mount_external,  se_info,  nice_name,managed_fds_to_close,
                                                managed_fds_to_ignore,  is_child_zygote,instruction_set,  app_data_dir,
                                                is_top_app,pkg_data_info_list,  whitelisted_data_info_list,
                                                mount_data_dirs,  mount_storage_dirs);
//        zygote_unhook(env, clazz);
        if(pid == 0){
            LOGE(" child nativeForkAndSpecialize_afore start uid = %d currentuid = %d pid = %d",uid,getuid(),getpid());
            if(is_HostProcess){
                HOOK_Process_getUidForName(env);
            } else if (is_Init) {
                HOOK_Process_setArgv0(env);
            }
            zygote_unhook(env, clazz);
        } else {
            LOGE("parent nativeForkAndSpecialize_afore start uid = %d currentuid = %d pid = %d",uid,getuid(),getpid());
            rprocess::GetInstance()->clearAppinfoNative();
        }
        DEBUG()
        return pid;
    };
    void zygote_nativeForkAndSpecialize_hook(JNIEnv* env){
        DEBUG()
        if(env != nullptr){
            jclass Zygote_cls =  env->FindClass("com/android/internal/os/Zygote");                        //                      "(II[II[[IILjava/lang/String;Ljava/lang/String;[I[IZLjava/lang/String;Ljava/lang/String;Z[Ljava/lang/String;[Ljava/lang/String;ZZ)I"
            jmethodID nativeSpecializeAppProcess_method = env->GetStaticMethodID(Zygote_cls,"nativeForkAndSpecialize", nativeForkAndSpecialize_sign);
            nativeForkAndSpecialize_org = reinterpret_cast<jint (*)(JNIEnv *, jclass, jint, jint,
                                                                    jintArray, jint, jobjectArray,
                                                                    jint, jstring, jstring,
                                                                    jintArray, jintArray, jboolean,
                                                                    jstring, jstring, jboolean,
                                                                    jobjectArray, jobjectArray,
                                                                    jboolean,
                                                                    jboolean)>(HookJmethod_JniFunction(
                    env, Zygote_cls, nativeSpecializeAppProcess_method,
                    (uintptr_t) nativeForkAndSpecialize_hook));
        } else {
            LOGE("ptrace zygote  Pre_GetEnv failed");
        }
        DEBUG()
    }


    void (*nativeSpecializeAppProcess_org)(JNIEnv *env, jclass clazz, jint uid, jint gid, jintArray gids,
                                           jint runtime_flags,
                                           jobjectArray rlimits, jint mount_external, jstring se_info,
                                           jstring nice_name,
                                           jboolean is_child_zygote, jstring instruction_set,
                                           jstring app_data_dir,
                                           jboolean is_top_app, jobjectArray pkg_data_info_list,
                                           jobjectArray allowlisted_data_info_list,
                                           jboolean mount_data_dirs,
                                           jboolean mount_storage_dirs);

    void nativeSpecializeAppProcess_hook(JNIEnv *env, jclass clazz, jint uid, jint gid, jintArray gids,
                                         jint runtime_flags,
                                         jobjectArray rlimits, jint mount_external, jstring se_info,
                                         jstring nice_name,
                                         jboolean is_child_zygote, jstring instruction_set,
                                         jstring app_data_dir,
                                         jboolean is_top_app, jobjectArray pkg_data_info_list,
                                         jobjectArray allowlisted_data_info_list,
                                         jboolean mount_data_dirs,
                                         jboolean mount_storage_dirs) {
//    nativeSpecializeAppProcess  函数在android 13 小米上已经不能从libandroid_runtime.so 中找到native符号了
        DEBUG()
        LOGE("nativeSpecializeAppProcess_hook start uid = %d currentuid = %d pid = %d", uid, getuid(),getpid());
        char *pkgName = const_cast<char *>(env->GetStringUTFChars(nice_name, nullptr));

        rprocess::GetInstance()->setProcessInfo(pkgName, uid, gid);

        if (rprocess::GetInstance()->is_HostProcess()) {
            HOOK_Process_getUidForName(env);
        } else if (rprocess::GetInstance()->InitEnable(env)) {
            HOOK_Process_setArgv0(env);
        }
//    nativeSpecializeAppProcess_org 函数必须后调用，需要先发起contextprovider请求

        nativeSpecializeAppProcess_org(env, clazz, uid, gid, gids, runtime_flags, rlimits,
                                       mount_external, se_info, nice_name, is_child_zygote,
                                       instruction_set, app_data_dir, is_top_app,
                                       pkg_data_info_list, allowlisted_data_info_list,
                                       mount_data_dirs, mount_storage_dirs);
        zygote_unhook(env,clazz);
        DEBUG()
        return;
    };

    void zygote_nativeSpecializeAppProcess_hook(JNIEnv *env) {
        DEBUG()
        if (env != nullptr) {
            jclass Zygote_cls = env->FindClass("com/android/internal/os/Zygote");                        //                    "(II[II[[IILjava/lang/String;Ljava/lang/String;[I[IZLjava/lang/String;Ljava/lang/String;Z[Ljava/lang/String;[Ljava/lang/String;ZZ)I"
            jmethodID nativeSpecializeAppProcess_method = env->GetStaticMethodID(Zygote_cls,"nativeSpecializeAppProcess", nativeSpecializeAppProcess_sign);
            nativeSpecializeAppProcess_org = reinterpret_cast<void (*)(JNIEnv *, jclass, jint, jint,
                                                                       jintArray, jint,
                                                                       jobjectArray, jint,
                                                                       jstring, jstring,
                                                                       jboolean, jstring, jstring,
                                                                       jboolean,
                                                                       jobjectArray, jobjectArray,
                                                                       jboolean,
                                                                       jboolean)>(HookJmethod_JniFunction(
                    env, Zygote_cls, nativeSpecializeAppProcess_method,(uintptr_t) nativeSpecializeAppProcess_hook));
        } else {
            LOGE("ptrace zygote  Pre_GetEnv failed");
        }
        DEBUG()
    }


    // android version code adaptation
    bool art_method_hook_init() {

        JNIEnv *env = Pre_GetEnv();
        jclass Process_cls = env->FindClass("android/os/Process");
        jmethodID javamethod = env->GetStaticMethodID(Process_cls, "getUidForName","(Ljava/lang/String;)I");
        void *libandroid_runtime = dlopen("libandroid_runtime.so", RTLD_NOW);
        uintptr_t getUidForName = reinterpret_cast<uintptr_t>(dlsym(libandroid_runtime,"_Z32android_os_Process_getUidForNameP7_JNIEnvP8_jobjectP8_jstring"));
        INIT_HOOK_PlatformABI(env, nullptr, javamethod, (uintptr_t *) getUidForName, 0x109);

        uintptr_t art_javamethod_method = GetArtMethod(env, Process_cls, javamethod);
        uintptr_t native_art_art_javamethod_method = GetOriginalNativeFunction((uintptr_t *) art_javamethod_method);

        if (native_art_art_javamethod_method == getUidForName) {
            return true;
        } else {
            return false;
        }
    }

    void zygote_hook(){
        JNIEnv *env = Pre_GetEnv();
        setArgV0_method_name = get_Process_setArgV0(env);
        zygote_nativeSpecializeAppProcess_hook(env);
        zygote_nativeForkAndSpecialize_hook(env);
    }



    jobject getConfigByProvider(JNIEnv* env, string providerHost_providerName , string callName, string method , string uid_str){
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
        jstring j_providerHost_providerName = env->NewStringUTF(providerHost_providerName.c_str());
        jstring tag_jstring = env->NewStringUTF("*cmd*");
        jstring j_callingPkg = env->NewStringUTF(callName.c_str());
        jstring j_method = env->NewStringUTF(method.c_str());
        jstring j_uid = env->NewStringUTF(uid_str.c_str());

        auto token_ibinderObj = env->NewObject(Binder_class,Binder_init);
        auto mExtras_BundleObj = env->NewObject(Bundle_class,Bundle_init);

        LOGE("j_providerHost_providerName : %s", providerHost_providerName.c_str());
        jobject holder_ContentProviderHolderObj = env->CallObjectMethod(IActivityManager_Obj, IActivityManager_getContentProviderExternal_method, j_providerHost_providerName, 0, token_ibinderObj, tag_jstring);
        jobject  provider_IContentProviderObj = env->GetObjectField(holder_ContentProviderHolderObj,ContentProviderHolder_provider_filed);
        jobject ret_bundle;
        auto AttributionSource_class = env->FindClass("android/content/AttributionSource");
        jmethodID AttributionSource_init = env->GetMethodID(AttributionSource_class, "<init>","(ILjava/lang/String;Ljava/lang/String;)V");
        jint uid = env->CallStaticIntMethod(Binder_class,Binder_getCallingUid);
        auto  attributionSourceObj = env->NewObject(AttributionSource_class,AttributionSource_init,uid,j_callingPkg,nullptr);

        auto IContentProvider_call_method = env->GetMethodID(IContentProvider_class,"call","(Landroid/content/AttributionSource;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Landroid/os/Bundle;)Landroid/os/Bundle;");
        ret_bundle = env->CallObjectMethod(provider_IContentProviderObj, IContentProvider_call_method, attributionSourceObj, j_providerHost_providerName, j_method, j_uid, mExtras_BundleObj);
        env->CallObjectMethod(IActivityManager_Obj, IActivityManager_removeContentProviderExternalAsUser_method, j_providerHost_providerName, token_ibinderObj, 0);
//    jstring config = static_cast<jstring>(env->CallObjectMethod(ret_bundle, Bundle_getString_method,j_key));
//    const char *  enableUidList_str = env->GetStringUTFChars(config, nullptr);
//    LOGE("get RxConfigPrvider is %s",enableUidList_str);
        env->DeleteLocalRef(IActivityManager_Obj);
        env->DeleteLocalRef(j_providerHost_providerName);
        env->DeleteLocalRef(tag_jstring);
        env->DeleteLocalRef(mExtras_BundleObj);
        env->DeleteLocalRef(token_ibinderObj);
        env->DeleteLocalRef(holder_ContentProviderHolderObj);
        env->DeleteLocalRef(provider_IContentProviderObj);
        env->DeleteLocalRef(j_callingPkg);
        env->DeleteLocalRef(j_method);
        env->DeleteLocalRef(j_uid);
        DEBUG()
        return ret_bundle;
    }




}