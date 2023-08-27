#include <dlfcn.h>
#include <asm-generic/mman-common.h>
#include <fcntl.h>
#include <linux/mman.h>
#include <sys/mman.h>
#include "include/rprocess.h"
#include "include/InjectApp.h"


void application_hook_init();

void * nativeForkAndSpecialize_addr;
int (*nativeForkAndSpecialize_org)(
        JNIEnv* env, jclass, jint uid, jint gid, jintArray gids, jint runtime_flags,
        jobjectArray rlimits, jint mount_external, jstring se_info, jstring nice_name,
        jintArray managed_fds_to_close, jintArray managed_fds_to_ignore, jboolean is_child_zygote,
        jstring instruction_set, jstring app_data_dir, jboolean is_top_app,
        jobjectArray pkg_data_info_list, jobjectArray allowlisted_data_info_list,
        jboolean mount_data_dirs, jboolean mount_storage_dirs);

int nativeForkAndSpecialize_hook(
        JNIEnv* env, jclass clazz, jint uid, jint gid, jintArray gids, jint runtime_flags,
        jobjectArray rlimits, jint mount_external, jstring se_info, jstring nice_name,
        jintArray managed_fds_to_close, jintArray managed_fds_to_ignore, jboolean is_child_zygote,
        jstring instruction_set, jstring app_data_dir, jboolean is_top_app,
        jobjectArray pkg_data_info_list, jobjectArray allowlisted_data_info_list,
        jboolean mount_data_dirs, jboolean mount_storage_dirs){
    DEBUG()
    char * pkgName = const_cast<char *>(env->GetStringUTFChars(nice_name,reinterpret_cast<jboolean *>(true)));
    rprocess::GetInstance()->setProcessInfo(pkgName, uid, gid);
    int ret = nativeForkAndSpecialize_org(env,clazz,uid,gid,gids,runtime_flags,rlimits,mount_external,se_info,nice_name,managed_fds_to_close,managed_fds_to_ignore,is_child_zygote,
                                          instruction_set, app_data_dir,is_top_app,pkg_data_info_list,allowlisted_data_info_list,mount_data_dirs,mount_storage_dirs);

    return ret;
};


void* nativeSpecializeAppProcess_addr;
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
    DEBUG()
    LOGE("nativeSpecializeAppProcess_hook uid = %d currentuid = %d ",uid,getuid());
    char * pkgName = const_cast<char *>(env->GetStringUTFChars(nice_name, nullptr));
    rprocess::GetInstance()->setProcessInfo(pkgName, uid, gid);
    if (rprocess::GetInstance()->Enable()) {
            application_hook_init();
    }
//        process_unhook();
    nativeSpecializeAppProcess_org( env,  clazz,  uid,  gid,  gids,  runtime_flags,rlimits,  mount_external,  se_info,  nice_name,is_child_zygote,
                                                  instruction_set,  app_data_dir,is_top_app,  pkg_data_info_list,allowlisted_data_info_list,  mount_data_dirs,mount_storage_dirs);

};


void *fork_addr;
unsigned int (*fork_org)();

void process_unhook(){
    DobbyDestroy((void *)fork_addr);
    DobbyDestroy((void *)nativeForkAndSpecialize_addr);
}

unsigned int fork_hook(){
    DEBUG()
    unsigned int ret = fork_org();
    if(ret == 0){
//        if (rprocess::GetInstance()->Enable()) {
//            application_hook_init();
//        }
//        process_unhook();
        return ret;
    }else{
        return ret;
    }
}



int (*setregid_org)(gid_t rgid, gid_t egid);
int setregid_hook(gid_t rgid, gid_t egid){
    LOGE("setregid_hook %d currentuid = %d",rgid,getgid());
    return setregid_org(rgid,egid);
}

int (*setreuid_org)(gid_t rgid, gid_t egid);
int setreuid_hook(gid_t ruid, gid_t euid){
    LOGE("setreuid_hook %d currentuid = %d",ruid,getuid());
    return setreuid_org(ruid,euid);
}


void zygote_server_init() {
    JNIEnv* env = Pre_GetEnv();
    DEBUG();
//    void *android_os_Process_setArg_addr =  DobbySymbolResolver("","_Z27android_os_Process_setArgV0P7_JNIEnvP8_jobjectP8_jstring");
//    LOGE("android_os_Process_setArg_addr %p",android_os_Process_setArg_addr);

//    void * setreuid = DobbySymbolResolver("","setreuid");
//    void * setresuid = DobbySymbolResolver("","setresuid");
//    void * setregid = DobbySymbolResolver("","setregid");
//    void * setresgid = DobbySymbolResolver("","setresgid");
//    fork_addr = DobbySymbolResolver("","fork");
//    DobbyHook(setregid,reinterpret_cast<dobby_dummy_func_t>(setregid_hook),reinterpret_cast<dobby_dummy_func_t *>(&setregid_org));
//    DobbyHook(setresgid,reinterpret_cast<dobby_dummy_func_t>(setregid_hook),reinterpret_cast<dobby_dummy_func_t *>(&setregid_org));
//    DobbyHook(setreuid,reinterpret_cast<dobby_dummy_func_t>(setreuid_hook),reinterpret_cast<dobby_dummy_func_t *>(&setreuid_org));
//    DobbyHook(setresuid,reinterpret_cast<dobby_dummy_func_t>(setreuid_hook),reinterpret_cast<dobby_dummy_func_t *>(&setreuid_org));
//    DobbyHook(fork_addr, reinterpret_cast<dobby_dummy_func_t>(fork_hook),reinterpret_cast<dobby_dummy_func_t *>(&fork_org));

    if(!hook_init_and_text(env)){
        LOGE("hook_init_and_text failed");
    }
    jclass Zygote_cls =  env->FindClass("com/android/internal/os/Zygote");                        //                    "(II[II[[IILjava/lang/String;Ljava/lang/String;[I[IZLjava/lang/String;Ljava/lang/String;Z[Ljava/lang/String;[Ljava/lang/String;ZZ)I"
    jmethodID nativeForkAndSpecialize_method = env->GetStaticMethodID(Zygote_cls,"nativeForkAndSpecialize", "(II[II[[IILjava/lang/String;Ljava/lang/String;[I[IZLjava/lang/String;Ljava/lang/String;Z[Ljava/lang/String;[Ljava/lang/String;ZZ)I");
    nativeForkAndSpecialize_addr = getJmethod_JniFunction(env,Zygote_cls,nativeForkAndSpecialize_method);
    LOGE("get nativeForkAndSpecialize_addr %p",nativeForkAndSpecialize_addr);
    DobbyHook(nativeForkAndSpecialize_addr,reinterpret_cast<dobby_dummy_func_t>(nativeForkAndSpecialize_hook),reinterpret_cast<dobby_dummy_func_t *>(&nativeForkAndSpecialize_org));


    jmethodID nativeSpecializeAppProcess_method = env->GetStaticMethodID(Zygote_cls,"nativeSpecializeAppProcess",  "(II[II[[IILjava/lang/String;Ljava/lang/String;ZLjava/lang/String;Ljava/lang/String;Z[Ljava/lang/String;[Ljava/lang/String;ZZ)V");
    nativeSpecializeAppProcess_addr = getJmethod_JniFunction(env,Zygote_cls,nativeSpecializeAppProcess_method);
    LOGE("get nativeSpecializeAppProcess_addr %p",nativeSpecializeAppProcess_addr);
    DobbyHook(nativeSpecializeAppProcess_addr,reinterpret_cast<dobby_dummy_func_t>(nativeSpecializeAppProcess_hook),reinterpret_cast<dobby_dummy_func_t *>(&nativeSpecializeAppProcess_org));

}







void *android_os_Process_setArg_addr;
void (*android_os_Process_setArg_org)(JNIEnv* env, jobject clazz, jstring name);
void android_os_Process_setArg_call(JNIEnv* env, jobject clazz, jstring name){
    char * pkgName = const_cast<char *>(env->GetStringUTFChars(name, nullptr));
    if(rprocess::GetInstance()->is_Load(env,pkgName)){
//        rprocess::GetInstance()->LoadModule(env);
        DEBUG();
        DobbyDestroy(android_os_Process_setArg_addr);
    }

    android_os_Process_setArg_org(env,clazz,name);


}


int (*selinux_android_setcontext_addr)(uid_t,bool,const char*,const char *);
int (*selinux_android_setcontext_org)(uid_t,bool,const char*,const char *);
int selinux_android_setcontext_call(uid_t uid , bool isSystemServer , char* seinfo , char *pkgName){
    LOGE("selinux_android_setcontext_call getuid :%d",getuid());
    LOGE("selinux_android_setcontext_call : %s uid :%d",pkgName,uid);
    int re = selinux_android_setcontext_org(uid,isSystemServer,seinfo,pkgName);
    DobbyDestroy((void *)selinux_android_setcontext_addr);
    return re;
}

void application_hook_init() {
    //hook selinux_android_setcontext，获取进程安全上下文
    selinux_android_setcontext_addr = (int (*)(uid_t,bool,const char*,const char *))DobbySymbolResolver ("libandroid_runtime.so", "selinux_android_setcontext");
    DobbyHook((void *)selinux_android_setcontext_addr, (void *)selinux_android_setcontext_call, (void **)&selinux_android_setcontext_org);

    android_os_Process_setArg_addr = DobbySymbolResolver("libandroid_runtime.so","_Z27android_os_Process_setArgV0P7_JNIEnvP8_jobjectP8_jstring");
    DobbyHook(android_os_Process_setArg_addr, (void *) android_os_Process_setArg_call, (void **) &android_os_Process_setArg_org);
}


void Ptrace_Zygotes(const char* AUTHORITY){
    rprocess::GetInstance()->setAUTHORITY(strdup(AUTHORITY));
    zygote_server_init();
}
void Inject_Porcess(const char* AUTHORITY_pkgName){

    vector<string> arg_vec = string_split(strdup(AUTHORITY_pkgName), ":");
    InjectApp::GetInstance()->LoadExternApk(strdup(AUTHORITY_pkgName));

}




