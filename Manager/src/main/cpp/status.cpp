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
#include "tool.h"
//#include "hideload/linker.h"
using namespace std;


extern "C"
JNIEXPORT jboolean JNICALL
 Java_hepta_rxposed_manager_fragment_HomeFragment_get_1rxposed_1activity(JNIEnv *env, jobject thiz) {
    // TODO: implement get_rxposed_activity()
    jboolean re = false;
    char sdk_ver[32];
    memset(sdk_ver,0,32);
    __system_property_get("rxposed_activity", sdk_ver);
    if(!strncmp(sdk_ver,"true", strlen("true"))){
        re = true;
    }
//    soinfo* si = find_all_library_byname("libnativeloader.so") ;

    return re;

}



jboolean hook_fun_addr(JNIEnv *env, jclass clazz) {

    LOGE("hook_fun_addr");
    return true;
}


extern "C"
JNIEXPORT jboolean JNICALL
Java_hepta_rxposed_manager_fragment_check_checkFragment_jni_1hook_1text(JNIEnv *env, jobject thiz) {

    LOGE("hook_test");
    return false;
}


extern "C"
JNIEXPORT jboolean JNICALL
Java_hepta_rxposed_manager_fragment_check_checkFragment_jni_1hook(JNIEnv *env, jobject thiz) {


    // TODO: implement jni_hook()
    jclass  cls = env->GetObjectClass(thiz);
    jmethodID javamethod  =  env->GetMethodID(cls,"jni_hook_text", "()Z");
//    void *native_addr = (void *)Java_hepta_rxposed_manager_fragment_check_checkFragment_jni_1hook_1text;

    HookJmethod_JniFunction(env,cls,javamethod,(uintptr_t)hook_fun_addr);
//    void* jni_native_fun_addr = getJmethod_JniFunction(env,cls,javamethod);

    LOGE("Java_hepta_rxposed_manager_fragment_check_checkFragment_jni_1hook");
    return false;
}
