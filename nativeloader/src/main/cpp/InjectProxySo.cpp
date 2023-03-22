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
#include "nativeloader/include/rprocess.h"
#define LOG_TAG "Native"
using namespace std;


using namespace std;


//测试注入库的代码

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
//        rprocess::GetInstance()->setRxposedContext(rprocess::GetInstance()->getApplicationContext(pEnv,"hepta.rxposed.nativeloader"));
//        rprocess::GetInstance()->setProcessName("hepta.rxposed.nativeloader");
//        rprocess::GetInstance()->setAUTHORITY("hepta.rxposed.manager:hepta.rxposed.manager.Provider");
//        rprocess::GetInstance()->GetConfigByProvider(pEnv);
//        rprocess::AppinfoNative * appinfoNative =  rprocess::GetInstance()->GetAppInfoNative(pEnv,"hepta.rxposed.loadxposed","hepta.rxposed.loadxposed.XposedEntry","text");
//        rprocess::GetInstance()->load_apk_And_exe_Class_Method(pEnv,appinfoNative);

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

extern "C"
JNIEXPORT jobject JNICALL
Java_hepta_rxposed_nativeloader_LoaderApplication_getApplicationContext(JNIEnv *env, jclass clazz,
                                                                        jstring app_name) {
//    // TODO: implement getApplicationContext()
    rprocess * rprocess = rprocess::GetInstance();
    string c_appName = env->GetStringUTFChars(app_name, nullptr);
    jobject context = rprocess->getApplicationContext(env,c_appName);
    return context;
    }