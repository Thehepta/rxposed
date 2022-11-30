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



#if RXDEBUG
// 调用 debug 版本方法
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#else
#define LOGE(...)
#define LOGD(...)
#endif




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
    if(rprocess::GetInstance()->is_isIsolatedProcess()){
        LOGE("JNI_OnLoad current process is isIsolatedProcess");
    } else{
        LOGE("JNI_OnLoad current process is ApplicationProcess");
        rprocess::GetInstance()->setRxposedContext(rprocess::GetInstance()->getApplicationContext(pEnv,"hepta.rxposed.rxposedloaderapp"));
        rprocess::GetInstance()->setProcessName("hepta.rxposed.rxposedloaderapp");
        rprocess::GetInstance()->setAUTHORITY("hepta.rxposed.manager:hepta.rxposed.manager.Provider");
        rprocess::GetInstance()->GetConfigByProvider(pEnv);
        rprocess::AppinfoNative * appinfoNative =  rprocess::GetInstance()->GetAppInfoNative(pEnv,"hepta.rxposed.loadxposed","hepta.rxposed.loadxposed.XposedEntry","Entry");
        rprocess::GetInstance()->load_apk_And_exe_Class_Method(pEnv,appinfoNative);

    }


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


extern "C"
JNIEXPORT jobject JNICALL
Java_hepta_rxposed_rxposedloaderapp_LoaderApplication_getApplicationContext(JNIEnv *env,
                                                                            jclass thiz,jstring AppName) {
    // TODO: implement getApplicationContext()
    rprocess * rprocess = rprocess::GetInstance();
    string c_appName = env->GetStringUTFChars(AppName, nullptr);
    jobject context = rprocess->getApplicationContext(env,c_appName);
    return context;
}
extern "C"
JNIEXPORT void JNICALL
Java_hepta_rxposed_rxposedloaderapp_LoaderApplication_NDK_1ExceptionCheckTest(JNIEnv *pEnv,
                                                                              jclass clazz) {

    pEnv->FindClass("eqweqeqwewqewq");
    if(NDK_ExceptionCheck(pEnv,"ExceptionText eqweqeqwewqewq not found")){
        LOGE("start");
    }
}