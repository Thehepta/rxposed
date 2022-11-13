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
#define LOG_TAG "Native"
using namespace std;


using namespace std;


#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)


//注册函数映射
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved) {
    LOGD("JNI_OnLoad TEXT");

    JNIEnv *pEnv = NULL;
    //获取环境
    jint ret = vm->GetEnv((void**) &pEnv, JNI_VERSION_1_6);
    if (ret != JNI_OK) {
        LOGE("jni_replace JVM ERROR:GetEnv");
        return -1;
    }
    //返回java版本
    return JNI_VERSION_1_6;
}

__unused __attribute__((constructor(101)))
void constructor_101() { // __attribute__((constructor))修饰 最先执行
    LOGD("constructor_101 TEXT");

}

__unused __attribute__((constructor(102)))
void constructor_102() { // __attribute__((constructor))修饰 最先执行
    LOGD("constructor_102 TEXT");

}

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
    return re;

}