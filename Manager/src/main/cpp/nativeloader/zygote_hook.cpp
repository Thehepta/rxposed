#include <dlfcn.h>
#include <asm-generic/mman-common.h>
#include <fcntl.h>
#include <linux/mman.h>
#include <sys/mman.h>
#include "include/rprocess.h"
#include "include/InjectApp.h"
#include "include/And64InlineHook.h"
#include "include/FunHook.h"

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
    rprocess::GetInstance()->Init(pkgName,uid,gid);
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

    LOGE("nativeSpecializeAppProcess_hook uid = %d currentuid = %d ",uid,getuid());
    char * pkgName = const_cast<char *>(env->GetStringUTFChars(nice_name, nullptr));
    rprocess::GetInstance()->Init(pkgName,uid,gid);
    if (rprocess::GetInstance()->Enable()) {
            application_hook_init();
    }
    nativeSpecializeAppProcess_org( env,  clazz,  uid,  gid,  gids,  runtime_flags,rlimits,  mount_external,  se_info,  nice_name,is_child_zygote,
                                    instruction_set,  app_data_dir,is_top_app,  pkg_data_info_list,allowlisted_data_info_list,  mount_data_dirs,mount_storage_dirs);

    DobbyDestroy((void *)nativeForkAndSpecialize_addr);

    return;
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
    
    if(!hook_init_and_text(env)){
        LOGE("hook_init_and_text failed");
    }
    jclass Zygote_cls =  env->FindClass("com/android/internal/os/Zygote");                        //                    "(II[II[[IILjava/lang/String;Ljava/lang/String;[I[IZLjava/lang/String;Ljava/lang/String;Z[Ljava/lang/String;[Ljava/lang/String;ZZ)I"
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
        rprocess::GetInstance()->LoadModule(env);
        DEBUG();
        android_os_Process_setArg_org(env,clazz,name);
        DobbyDestroy(android_os_Process_setArg_addr);
    } else{

        android_os_Process_setArg_org(env,clazz,name);

    }

}


int (*selinux_android_setcontext_addr)(uid_t,bool,const char*,const char *);
int (*selinux_android_setcontext_org)(uid_t,bool,const char*,const char *);
int selinux_android_setcontext_call(uid_t uid , bool isSystemServer , char* seinfo , char *pkgName){
    LOGE("selinux_android_setcontext_call : %s uid :%d",pkgName,uid);
    int re = selinux_android_setcontext_org(uid,isSystemServer,seinfo,pkgName);
    DobbyDestroy((void *)selinux_android_setcontext_addr);
    return re;
}

void application_hook_init() {

    android_os_Process_setArg_addr = get_android_os_Process_setArgV0_addr();
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




