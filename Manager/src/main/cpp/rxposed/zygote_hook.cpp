#include <dlfcn.h>
#include <asm-generic/mman-common.h>
#include <fcntl.h>
#include <linux/mman.h>
#include <sys/prctl.h>
#include <sys/mman.h>
#include "rprocess.h"
#include "InjectApp.h"
#include "tool.h"
#include "artmethod_native_hook.h"


void (*android_os_Process_setArg_org)(JNIEnv* env, jclass clazz, jstring name);
void android_os_Process_setArg_hook(JNIEnv* env, jclass clazz, jstring name){
    DEBUG()
    char * pkgName = const_cast<char *>(env->GetStringUTFChars(name, nullptr));
    if(rprocess::GetInstance()->is_Start(env, pkgName)){
        rprocess::GetInstance()->LoadModule(env);
        android_os_Process_setArg_org(env,clazz,name);
        jmethodID javamethod = env->GetStaticMethodID(clazz,"setArgV0Native", "(Ljava/lang/String;)V");
        unHookJmethod_JniFunction(env,clazz,javamethod);
    } else{
        android_os_Process_setArg_org(env,clazz,name);
    }
    DEBUG()
}


void HOOK_Process_setArgv0(JNIEnv* env) {
    DEBUG()
    jclass  Process_cls = env->FindClass("android/os/Process");
    jmethodID javamethod = env->GetStaticMethodID(Process_cls,"setArgV0Native", "(Ljava/lang/String;)V");
    android_os_Process_setArg_org = reinterpret_cast<void (*)(JNIEnv *, jclass ,jstring)>(HookJmethod_JniFunction(env,Process_cls,javamethod,(uintptr_t) android_os_Process_setArg_hook));

    DEBUG()

}

jint (*android_os_Process_getUidForName_org)(JNIEnv* env, jclass clazz, jstring name);

jint android_os_Process_getUidForName_hook(JNIEnv* env, jclass clazz, jstring name){
    DEBUG()
    const char * Authority_arg = const_cast<char *>(env->GetStringUTFChars(name, nullptr));
    const char * Authority = rprocess::GetInstance()->getStatusAuthority();
    int ret = 0;
    if(strncmp(Authority_arg,Authority, strlen(Authority_arg) )==0){
        ret = rprocess::GetInstance()->getHostUid();
    } else{
        ret = android_os_Process_getUidForName_org(env,clazz,name);
    }
    DEBUG()
    return ret;
}

void HOOK_Process_getUidForName(JNIEnv* env) {
    jclass  Process_cls = env->FindClass("android/os/Process");
    jmethodID getUidForName_Jmethod = env->GetStaticMethodID(Process_cls,"getUidForName", "(Ljava/lang/String;)I");

    android_os_Process_getUidForName_org = reinterpret_cast<jint (*)(JNIEnv *, jclass,
                                                                     jstring)>(HookJmethod_JniFunction(
            env, Process_cls, getUidForName_Jmethod,
            (uintptr_t) android_os_Process_getUidForName_hook));
}


void (*nativeSpecializeAppProcess_org)(JNIEnv* env, jclass clazz, jint uid, jint gid, jintArray gids, jint runtime_flags,
                                       jobjectArray rlimits, jint mount_external, jstring se_info, jstring nice_name,
                                       jboolean is_child_zygote, jstring instruction_set, jstring app_data_dir,
                                       jboolean is_top_app, jobjectArray pkg_data_info_list,
                                       jobjectArray allowlisted_data_info_list, jboolean mount_data_dirs,
                                       jboolean mount_storage_dirs);

void nativeSpecializeAppProcess_hook(JNIEnv* env, jclass clazz, jint uid, jint gid, jintArray gids, jint runtime_flags,
                                     jobjectArray rlimits, jint mount_external, jstring se_info, jstring nice_name,
                                     jboolean is_child_zygote, jstring instruction_set, jstring app_data_dir,
                                     jboolean is_top_app, jobjectArray pkg_data_info_list,
                                     jobjectArray allowlisted_data_info_list, jboolean mount_data_dirs,
                                     jboolean mount_storage_dirs){
//    nativeSpecializeAppProcess  函数在android 13 小米上已经不能从libandroid_runtime.so 中找到native符号了
    DEBUG()
    LOGE("nativeSpecializeAppProcess_hook start uid = %d currentuid = %d ",uid,getuid());
    char * pkgName = const_cast<char *>(env->GetStringUTFChars(nice_name, nullptr));

    rprocess::GetInstance()->setProcessInfo(pkgName, uid, gid);

    if( rprocess::GetInstance()->is_HostProcess()){
        HOOK_Process_getUidForName(env);
    } else if (rprocess::GetInstance()->InitEnable(env)) {
        HOOK_Process_setArgv0(env);
    }
//    nativeSpecializeAppProcess_org 函数必须后调用，发起contextprovider请求才能成功
    nativeSpecializeAppProcess_org( env,  clazz,  uid,  gid,  gids,  runtime_flags,rlimits,  mount_external,  se_info,  nice_name,is_child_zygote,
                                    instruction_set,  app_data_dir,is_top_app,  pkg_data_info_list,allowlisted_data_info_list,  mount_data_dirs,mount_storage_dirs);
    jmethodID nativeSpecializeAppProcess_method = env->GetStaticMethodID(clazz,"nativeSpecializeAppProcess",  "(II[II[[IILjava/lang/String;Ljava/lang/String;ZLjava/lang/String;Ljava/lang/String;Z[Ljava/lang/String;[Ljava/lang/String;ZZ)V");
    unHookJmethod_JniFunction(env,clazz,nativeSpecializeAppProcess_method);

    DEBUG()
    return;
};


void zygote_nativeSpecializeAppProcess_hook() {
    DEBUG()
    JNIEnv* env = Pre_GetEnv();
    if(env != nullptr){
        if(art_method_hook_init(env)){
            jclass Zygote_cls =  env->FindClass("com/android/internal/os/Zygote");                        //                    "(II[II[[IILjava/lang/String;Ljava/lang/String;[I[IZLjava/lang/String;Ljava/lang/String;Z[Ljava/lang/String;[Ljava/lang/String;ZZ)I"
            jmethodID nativeSpecializeAppProcess_method = env->GetStaticMethodID(Zygote_cls,"nativeSpecializeAppProcess",  "(II[II[[IILjava/lang/String;Ljava/lang/String;ZLjava/lang/String;Ljava/lang/String;Z[Ljava/lang/String;[Ljava/lang/String;ZZ)V");
            nativeSpecializeAppProcess_org = reinterpret_cast<void (*)(JNIEnv *, jclass, jint, jint,
                                                                       jintArray, jint, jobjectArray, jint,
                                                                       jstring, jstring,
                                                                       jboolean, jstring, jstring, jboolean,
                                                                       jobjectArray, jobjectArray, jboolean,
                                                                       jboolean)>(HookJmethod_JniFunction(
                    env, Zygote_cls, nativeSpecializeAppProcess_method,(uintptr_t) nativeSpecializeAppProcess_hook));

        } else{
            LOGE("art_method_hook_init failed");
        }
    } else {
        LOGE("ptrace zygote  Pre_GetEnv failed");

    }

    DEBUG()
}

void hide_self(){

}

//改这个函数名字记得一定要同步修改注入调用入口函数的符号，改 参数，const都会导致符号变更
void Ptrace_Zygotes(const char* Authority_arg){
    hide_self();
    rprocess::GetInstance()->setAuthorityInfo(Authority_arg);
    zygote_nativeSpecializeAppProcess_hook();
}


void Inject_Porcess(const char* AUTHORITY_pkgName){

    vector<string> arg_vec = string_split(strdup(AUTHORITY_pkgName), ":");
    InjectApp::GetInstance()->LoadExternApk(strdup(AUTHORITY_pkgName));

}




