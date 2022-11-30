#include <dlfcn.h>
#include <asm-generic/mman-common.h>
#include <fcntl.h>
#include <linux/mman.h>
#include <sys/mman.h>
#include "include/rprocess.h"
#include "include/dobby.h"
int (*orig_loader_dlopen)(int a1);
void (*putty)(int , int, int);
char * tmpe;

//#ifdef __arm__
//int fake_artmethod_invoker(int a1)
//{
//    orig_loader_dlopen(a1);
//}
//#elif __aarch64__
//
//
//#endif
//
//#define

void rxposde_init();

extern void start_entry();
extern void start_java();


unsigned int (*fork_addr)();
unsigned int (*fork_addr_org)();

unsigned int fork_call(){

    unsigned int ret = fork_addr_org();
    if(ret == 0){
        //如果是子进程，销毁fork 的hook点
        DobbyDestroy((void *)fork_addr);
        rxposde_init();
//        LOGE("fork_call child process,ID is %d\n",getpid());
        return ret;
    }else{
//        LOGE("fork_call parent process,ID is %d\n",getpid());
        return ret;
    };
}



unsigned int (*vfork_addr)();
unsigned int (*vfork_addr_org)();

//vfork 不能hook
//1.据说vfork创建的是线程，不是进程
//2.有bug，如果有人调用了vfrok 会崩溃，目前没搞明白原因，frida也有这个bug
//unsigned int vfork_call(){
//    unsigned int pid =  vfork_addr_org();
//    if(pid<0)
//        printf("error in fork!\n");
//    else if(pid == 0)
//    {
////        DobbyDestroy((void *)vfork_addr);
//        LOGE("vfork_call child process,ID is %d\n",getpid());
//        return pid;
//    }
//    else
//    {
//        LOGE("vfork_call parent process,ID is %d\n",getpid());
//        return pid;
//    }
//}

int (*system_property_get_addr)( char*, char *);
int (*system_property_get_org)( char*, char *);
int system_property_get_call(char* name , char *value){
    int re;
    LOGD("native hook %s ret: %d",name,re);
    if(!strncmp(name,"rxposed_activity", strlen("rxposed_activity"))){
        re = strlen("true");
        memcpy(value, "true", strlen("true"));
//        DobbyDestroy((void *)system_property_get_addr);
    } else{
        re = system_property_get_org(name,value);
    }
    return re;
}





void (*android_os_Process_setArg_addr)(JNIEnv* env, jobject clazz, jstring name);
void (*android_os_Process_setArg_org)(JNIEnv* env, jobject clazz, jstring name);
void android_os_Process_setArg_call(JNIEnv* env, jobject clazz, jstring name){
    android_os_Process_setArg_org(env,clazz,name);
    if(rprocess::GetInstance()->is_isIsolatedProcess()){
        LOGD("isIsolatedProcess is recent not support hook uid = %d processName = %s",getuid(),env->GetStringUTFChars(name, nullptr));
        DobbyDestroy((void *)android_os_Process_setArg_addr);
        return;
    }

    bool Destroy = false;
//    LOGE("android_os_Process_setArg_call name %s ",env->GetStringUTFChars(name, nullptr));
    if(rprocess::GetInstance()->is_providerHostProcess(env, name)){
        //service process
        system_property_get_addr = (int (*)( char*, char *))DobbySymbolResolver (nullptr, "__system_property_get");
        DobbyHook((void *)system_property_get_addr, (void *)system_property_get_call, (void **)&system_property_get_org);
        Destroy = true;
    } else{
        Destroy = rprocess::GetInstance()->Init(env, name);
    }

    if(Destroy){
        DobbyDestroy((void *)android_os_Process_setArg_addr);
    }
}


int (*selinux_android_setcontext_addr)(uid_t,bool,const char*,const char *);
int (*selinux_android_setcontext_org)(uid_t,bool,const char*,const char *);
int selinux_android_setcontext_call(uid_t uid , bool isSystemServer , char* seinfo , char *pkgName){
    int re = selinux_android_setcontext_org(uid,isSystemServer,seinfo,pkgName);
    DobbyDestroy((void *)selinux_android_setcontext_addr);
    return re;
}

void rxposde_init() {
    //hook selinux_android_setcontext，获取进程安全上下文
    selinux_android_setcontext_addr = (int (*)(uid_t,bool,const char*,const char *))DobbySymbolResolver ("libandroid_runtime.so", "selinux_android_setcontext");
    DobbyHook((void *)selinux_android_setcontext_addr, (void *)selinux_android_setcontext_call, (void **)&selinux_android_setcontext_org);

    android_os_Process_setArg_addr = (void (*)(JNIEnv *env, jobject clazz, jstring name)) DobbySymbolResolver("libandroid_runtime.so","_Z27android_os_Process_setArgV0P7_JNIEnvP8_jobjectP8_jstring");
    DobbyHook((void *) android_os_Process_setArg_addr, (void *) android_os_Process_setArg_call, (void **) &android_os_Process_setArg_org);
}

void zygote_server_init() {
    fork_addr = (unsigned int (*)())DobbySymbolResolver("libc.so", "fork");
    DobbyHook((void *)fork_addr, (void *)fork_call, (void **)&fork_addr_org);
//    vfork_addr = (unsigned int (*)())DobbySymbolResolver("libc.so", "vfork");
//    DobbyHook((void *)vfork_addr, (void *)vfork_call, (void **)&vfork_addr_org);
}




void hook_setArgv0(){
//    android_os_Process_setArg_addr = (unsigned int (*)(unsigned int *,unsigned int *,unsigned int *))DobbySymbolResolver ("librxpopsed.so", "Java_com_example_rxpopsed_MainActivity_stringFromJNI");
//    DobbyHook((void *)android_os_Process_setArg_addr, (void *)android_os_Process_setArg_call, (void **)&android_os_Process_setArg_org);
}
void dobby(unsigned int serviceUid){
//    rprocess::GetInstance()->setServiceUid(serviceUid);
    zygote_server_init();
}
void dobby_str(const char* AUTHORITY){
    rprocess::GetInstance()->setAUTHORITY(strdup(AUTHORITY));
    zygote_server_init();

}