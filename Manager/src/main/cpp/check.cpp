#include <cinttypes>
#include <list>
#include <sys/mman.h>
#include <set>
#include <string_view>

#include <android/log.h>
#include <string>
#include <jni.h>
#include <dlfcn.h>
#include <fstream>
#include <unistd.h>
#include <vector>
#include "nativeloader/include/artmethod_native_hook.h"

#define LOG_TAG "Native"
using namespace std;


using namespace std;


#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)




bool hookinit(JNIEnv *env){

    jclass  Process_cls = env->FindClass("android/os/Process");
//    jmethodID javamethod = env->GetStaticMethodID(Process_cls,"getUidForName", "(Ljava/lang/String;)I");



    void * runtime = dlopen("/system/lib64/libandroid_runtime.so",RTLD_GLOBAL);
    if(runtime!= nullptr){
        void *getUidForName = dlsym(runtime,"_Z32android_os_Process_getUidForNameP7_JNIEnvP8_jobjectP8_jstring");
        LOGE("setArgV0P7 %p",getUidForName);
    }
//    LOGE("setArgV0Native %p",setArgV0Native2);
//    void *getUidForName =  DobbySymbolResolver("","_Z32android_os_Process_getUidForNameP7_JNIEnvP8_jobjectP8_jstring");
//    LOGE("setArgV0P7 %p",getUidForName);
//
//    INIT_HOOK_PlatformABI(env, nullptr,javamethod,getUidForName,109);

}


extern "C"
JNIEXPORT jboolean JNICALL
Java_hepta_rxposed_manager_fragment_check_checkFragment_chekc(JNIEnv *env, jobject thiz) {
    // TODO: implement chekc()

//    jclass  cls = env->Getc(thiz);
//    jmethodID javamethod  =  env->GetMethodID(cls,"chekc", "()Z");
//    void *native_addr = (void *)Java_hepta_rxposed_manager_fragment_check_checkFragment_chekc;
//    INIT_HOOK_PlatformABI(env, nullptr,javamethod,native_addr,109);
//
//    void* native_get_addr = GetArtMethod(env,cls,javamethod);
//
//
//    if(native_get_addr == native_addr){
//        return true;
//    } else{
//        return false;
//    }
    void * runtime = dlopen("/system/lib64/libandroid_runtime.so",RTLD_GLOBAL);
    if(runtime!= nullptr){
        void *getUidForName = dlsym(runtime,"_Z32android_os_Process_getUidForNameP7_JNIEnvP8_jobjectP8_jstring");
        LOGE("setArgV0P7 %p",getUidForName);
    }

    return false;

}