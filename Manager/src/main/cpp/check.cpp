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
#include "rxposed/artmethod_native_hook.h"
#include "rxposed/tool.h"
#include "hideload/elf_symbol_resolver.h"

using namespace std;



extern "C"
JNIEXPORT jboolean JNICALL
Java_hepta_rxposed_manager_util_CheckTool_chekcPreGetenv(JNIEnv *env, jobject thiz) {
    void *Pre_GetEnv  =  linkerResolveElfInternalSymbol("libandroid_runtime.so", "_ZN7android14AndroidRuntime9getJNIEnvEv");

    JNIEnv *pEnv =((JNIEnv*(*)())Pre_GetEnv)();
    if(pEnv == env){
        return true;
    }
    return false;
}




jboolean hook_fun_addr(JNIEnv *env, jclass clazz) {

    LOGE("hook_fun_addr");
    return true;
}

extern "C"
JNIEXPORT jboolean JNICALL
Java_hepta_rxposed_manager_util_CheckTool_jni_1hook_1test(JNIEnv *env, jobject thiz) {
    LOGE("hook_test");
    return false;
}


extern "C"
JNIEXPORT void JNICALL
Java_hepta_rxposed_manager_util_CheckTool_jni_1hook(JNIEnv *env, jobject thiz) {


    // TODO: implement jni_hook()
    jclass  cls = env->GetObjectClass(thiz);
    jmethodID javamethod  =  env->GetMethodID(cls,"jni_hook_test", "()Z");
    HookJmethod_JniFunction(env,cls,javamethod,(uintptr_t)hook_fun_addr);

    LOGE("Java_hepta_rxposed_manager_fragment_check_checkFragment_jni_1hook");
}

extern "C"
JNIEXPORT void JNICALL
Java_hepta_rxposed_manager_util_CheckTool_jni_1unhook(JNIEnv *env, jobject thiz) {
    // TODO: implement jni_unhook()
    jclass  cls = env->GetObjectClass(thiz);
    jmethodID javamethod  =  env->GetMethodID(cls,"jni_hook_test", "()Z");
    unHookJmethod_JniFunction(env,cls,javamethod);
}


extern "C"
JNIEXPORT jboolean JNICALL
Java_hepta_rxposed_manager_util_CheckTool_chekc_1GetArtmethodNative_1init(JNIEnv *env,
                                                                          jobject thiz) {
    jclass  cls = env->GetObjectClass(thiz);
    jmethodID javamethod  =  env->GetMethodID(cls,"chekc_GetArtmethodNative_init", "()Z");
    uintptr_t native_fun_addr = reinterpret_cast<uintptr_t>(Java_hepta_rxposed_manager_util_CheckTool_chekc_1GetArtmethodNative_1init);
    INIT_HOOK_PlatformABI(env, cls, javamethod, (uintptr_t*)native_fun_addr, 109);
    uintptr_t jni_native_fun_addr = getJmethod_JniFunction(env,cls,javamethod);

    if(jni_native_fun_addr == native_fun_addr){
        return true;
    } else{
        return false;
    }}
extern "C"
JNIEXPORT jboolean JNICALL
Java_hepta_rxposed_manager_util_CheckTool_chekc_1android_1os_1Process_1getUidForName(JNIEnv *env,
                                                                                     jobject thiz) {
    //如果rxposed已经激活了，那么这个检测应该是失败的
    jclass  Process_cls = env->FindClass("android/os/Process");
    jmethodID javamethod = env->GetStaticMethodID(Process_cls,"getUidForName", "(Ljava/lang/String;)I");

    uintptr_t  getUidForName = reinterpret_cast<uintptr_t>(linkerResolveElfInternalSymbol("libandroid_runtime.so","_Z32android_os_Process_getUidForNameP7_JNIEnvP8_jobjectP8_jstring"));
    LOGE("getUidForName = %lx",getUidForName);

    uintptr_t native_get_addr = getJmethod_JniFunction(env,Process_cls,javamethod);

    if(getUidForName == native_get_addr){
        return true;
    }
    return false;
}
extern "C"
JNIEXPORT jboolean JNICALL
Java_hepta_rxposed_manager_util_CheckTool_ELFresolveSymbol(JNIEnv *env, jobject thiz) {
    // TODO: implement ELFresolveSymbol()
    uintptr_t __system_property_get_addr = reinterpret_cast<uintptr_t>(__system_property_get);
    uintptr_t __system_property_get_resolve_addr  = reinterpret_cast<uintptr_t>(linkerResolveElfInternalSymbol(
            "libc.so", "__system_property_get"));
    if(__system_property_get_addr == __system_property_get_resolve_addr){
        return true;
    }
    return false;
}