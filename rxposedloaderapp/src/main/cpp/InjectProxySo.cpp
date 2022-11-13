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
#include "rxposedLoader/include/rprocess.h"
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


//    rprocess * rprocess = rprocess::GetInstance();
//    rprocess->setRxposedContext(rprocess->getApplicationContext(pEnv));
//    rprocess->setProcessName("hepta.rxposed.rxposedloaderapp");
//    rprocess->setAUTHORITY("hepta.rxposed.manager:hepta.rxposed.manager.provider");
//    rprocess->GetConfigByProvider(pEnv);

    //返回java版本
    return JNI_VERSION_1_6;
}

__unused __attribute__((constructor(101)))
void constructor_101() { // __attribute__((constructor))修饰 最先执行

#if RXDEBUG
    // 调用 debug 版本方法
    LOGD("RXDEBUG TEXT");
#else
    LOGD("release TEXT");

#endif


}

//__unused __attribute__((constructor(102)))
//void constructor_102() { // __attribute__((constructor))修饰 最先执行
//    LOGD("constructor_102 TEXT");
//
//}

