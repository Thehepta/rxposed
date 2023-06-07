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
#include "include/artmethod_native_hook.h"
#include "include/dobby.h"
#include "include/tool.h"

using namespace std;



extern "C"
JNIEXPORT jboolean JNICALL
Java_hepta_rxposed_manager_fragment_check_checkFragment_chekc_1GetArtmethodNative_1init(JNIEnv *env, jobject thiz) {


    jclass  cls = env->GetObjectClass(thiz);
    jmethodID javamethod  =  env->GetMethodID(cls,"chekc_GetArtmethodNative_init", "()Z");
    void *native_addr = (void *)Java_hepta_rxposed_manager_fragment_check_checkFragment_chekc_1GetArtmethodNative_1init;
    INIT_HOOK_PlatformABI(env, cls,javamethod,native_addr,109);
    void* native_get_addr = getJmethod_JniFunction(env,cls,javamethod);
    if(native_get_addr == native_addr){
        return true;
    } else{
        return false;
    }


}





void * custom_dlopen(){
    struct android_namespace_t *ns = NULL;


    struct android_namespace_t* (* dlsym_android_create_namespace)(const char* name,
                                                                   const char* ld_library_path,
                                                                   const char* default_library_path,
                                                                   uint64_t type,
                                                                   const char* permitted_when_isolated_path,
                                                                   struct android_namespace_t* parent);

    void* dl_so = dlopen("libdl.so",RTLD_NOW);
    dlsym_android_create_namespace = (android_namespace_t* (*)(const char* ,const char* ,const char* ,uint64_t ,const char* ,struct android_namespace_t*  )) dlsym(dl_so,"__loader_android_create_namespace");

    const char *lib_path = getLibPath();
    ns = dlsym_android_create_namespace(
            "trustme",
            lib_path,
            lib_path,
            ANDROID_NAMESPACE_TYPE_SHARED |
            ANDROID_NAMESPACE_TYPE_ISOLATED,
            "/system/lib64/:/vendor/",
            NULL);

    const android_dlextinfo dlextinfo = {
            .flags = ANDROID_DLEXT_USE_NAMESPACE,
            .library_namespace = ns,
    };}



extern "C"
JNIEXPORT jboolean JNICALL
Java_hepta_rxposed_manager_fragment_check_checkFragment_chekc_1android_1os_1Process_1getUidForName(JNIEnv *env, jobject thiz) {

    jclass  Process_cls = env->FindClass("android/os/Process");
    jmethodID javamethod = env->GetStaticMethodID(Process_cls,"getUidForName", "(Ljava/lang/String;)I");

    void *getUidForName =  DobbySymbolResolver("/system/lib64/libandroid_runtime.so","_Z32android_os_Process_getUidForNameP7_JNIEnvP8_jobjectP8_jstring");





    void * native_get_addr = getJmethod_JniFunction(env,Process_cls,javamethod);

    if(getUidForName == native_get_addr){
        return true;
    }
    return false;
}

extern "C"
JNIEXPORT jboolean JNICALL
Java_hepta_rxposed_manager_fragment_check_checkFragment_chekc_1android_1os_1Process_1setArgV0(
        JNIEnv *env, jobject thiz) {

    void *getUidForName =  DobbySymbolResolver("/system/lib64/libandroid_runtime.so","_Z27android_os_Process_setArgV0P7_JNIEnvP8_jobjectP8_jstring");
    if(getUidForName != nullptr){
        return true;
    }
    return false;}
extern "C"
JNIEXPORT jboolean JNICALL
Java_hepta_rxposed_manager_fragment_check_checkFragment_chekc_1PreGetenv(JNIEnv *env,
                                                                         jobject thiz) {
    JNIEnv *pEnv =Pre_GetEnv();
    if(pEnv == env){
        return true;
    }
    return false;
}