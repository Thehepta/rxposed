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
#include "artmethod_native_hook.h"
#include "tool.h"
#include "FunHook.h"

using namespace std;



extern "C"
JNIEXPORT jboolean JNICALL
Java_hepta_rxposed_manager_fragment_check_checkFragment_chekc_1GetArtmethodNative_1init(JNIEnv *env, jobject thiz) {


    jclass  cls = env->GetObjectClass(thiz);
    jmethodID javamethod  =  env->GetMethodID(cls,"chekc_GetArtmethodNative_init", "()Z");
    uintptr_t native_fun_addr = reinterpret_cast<uintptr_t>(Java_hepta_rxposed_manager_fragment_check_checkFragment_chekc_1GetArtmethodNative_1init);
    INIT_HOOK_PlatformABI(env, cls, javamethod, (uintptr_t*)native_fun_addr, 109);
    uintptr_t jni_native_fun_addr = getJmethod_JniFunction(env,cls,javamethod);

    if(jni_native_fun_addr == native_fun_addr){
        return true;
    } else{
        return false;
    }

}







extern "C"
JNIEXPORT jboolean JNICALL
Java_hepta_rxposed_manager_fragment_check_checkFragment_chekc_1android_1os_1Process_1getUidForName(JNIEnv *env, jobject thiz) {

    jclass  Process_cls = env->FindClass("android/os/Process");
    jmethodID javamethod = env->GetStaticMethodID(Process_cls,"getUidForName", "(Ljava/lang/String;)I");

    void * libandroid_runtime =  dlopen("libandroid_runtime.so",RTLD_NOW);
    uintptr_t  getUidForName = reinterpret_cast<uintptr_t>(dlsym(libandroid_runtime,"_Z32android_os_Process_getUidForNameP7_JNIEnvP8_jobjectP8_jstring"));
    LOGE("getUidForName = %lx",getUidForName);

    uintptr_t native_get_addr = getJmethod_JniFunction(env,Process_cls,javamethod);

    if(getUidForName == native_get_addr){
        return true;
    }
    return false;
}

extern "C"
JNIEXPORT jboolean JNICALL
Java_hepta_rxposed_manager_fragment_check_checkFragment_chekc_1android_1os_1Process_1setArgV0(
        JNIEnv *env, jobject thiz) {

//    void *setArgV0 =  get_android_os_Process_setArgV0_addr();
//    if(setArgV0 != nullptr){
//        return true;
//    }
    return false;
}


extern "C"
JNIEXPORT jboolean JNICALL
Java_hepta_rxposed_manager_fragment_check_checkFragment_chekcPreGetenv(JNIEnv *env,
                                                                       jobject thiz) {
    JNIEnv *pEnv =Pre_GetEnv();
    if(pEnv == env){
        return true;
    }
    return false;
}






int (*text_system_property_get_org)( char*, char *);

int text_system_property_get_call(char* name , char *value){
    int re;
    if(!strncmp(name,"rxposed_activity", strlen("rxposed_activity"))){
        re = strlen("true");
        memcpy(value, "true", strlen("true"));
    } else{
        re = text_system_property_get_org(name,value);
    }
    return re;
}

extern "C"
JNIEXPORT jboolean JNICALL
Java_hepta_rxposed_manager_fragment_check_checkFragment_check_1inline_1hook(JNIEnv *env,
                                                                            jobject thiz) {

    jboolean re = false;
    char sdk_ver[32];
    __system_property_get("rxposed_activity", sdk_ver);
    if(!strncmp(sdk_ver,"true", strlen("true"))){
        re = true;
        return true;
    }



//    DobbyHook((void *)__system_property_get, (void *)text_system_property_get_call, (void **)&text_system_property_get_org);
//    LOGE("system_property_get_addr = %p",__system_property_get);
//
//    memset(sdk_ver,0,32);
//    __system_property_get("rxposed_activity", sdk_ver);
//    if(!strncmp(sdk_ver,"true", strlen("true"))){
//        re = true;
//    }
//    DobbyDestroy((void *)__system_property_get);
    return re;
}
extern "C"
JNIEXPORT jboolean JNICALL
Java_hepta_rxposed_manager_fragment_check_checkFragment_ELFresolveSymbol(JNIEnv *env,
                                                                         jobject thiz) {
    // TODO: implement ELFresolveSymbol()


    return true;

}





