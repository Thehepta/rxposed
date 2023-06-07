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
#include "nativeloader/include/tool.h"
#include "nativeloader/include/InjectApp.h"
#include "nativeloader/include/artmethod_native_hook.h"

#define LOG_TAG "Native"
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
Java_hepta_rxposed_nativeloader_LoaderApplication_ndk_1getApplicationContext(JNIEnv *env, jclass clazz,
                                                                             jstring app_name) {
//    // TODO: implement CreateApplicationContext()
//    LOGD("Java_hepta_rxposed_nativeloader_LoaderApplication_ndk_1getApplicationContext");
//
   void*nativeForkAndSpecialize  = DobbySymbolResolver(nullptr, "com_android_internal_os_Zygote_nativeForkAndSpecialize");
//   void*_nativeForkRepeatedly  = DobbySymbolResolver(nullptr, "com_android_internal_os_ZygoteCommandBuffer_nativeForkRepeatedly");
//    if(_nativeForkRepeatedly!= nullptr){
//        LOGD("nativeForkAndSpecialize %p",_nativeForkRepeatedly);
//
//    } else{
//        LOGD("nativeForkAndSpecialize is null");
//
//    }
    string c_appName = env->GetStringUTFChars(app_name, nullptr);
    jobject context = CreateApplicationContext(env, c_appName);
    return context;
}
extern "C"
JNIEXPORT jboolean JNICALL
Java_hepta_rxposed_nativeloader_LoaderApplication_ndk_1GetAppInfoNative(JNIEnv *env, jclass clazz,
                                                                        jstring app_name,jstring entry_class,jstring entry_method) {
    // TODO: implement ndk_GetAppInfoNative()
    string c_appName = env->GetStringUTFChars(app_name, nullptr);
    string c_entry_method = env->GetStringUTFChars(entry_method, nullptr);
    string c_entry_class = env->GetStringUTFChars(entry_class, nullptr);

    AppinfoNative *appinfoNative = GetPmAppInfoNative(env, getContext(env), c_appName);
    LOGE("%s",appinfoNative->pkgName.c_str());
    LOGE("%s",appinfoNative->Entry_class.c_str());
    LOGE("%s",appinfoNative->Entry_method.c_str());

    if((c_entry_method == appinfoNative->Entry_method) && (c_entry_class == appinfoNative->Entry_class)){
        return true;
    } else{
        return false;
    }

}
extern "C"
JNIEXPORT void JNICALL
Java_hepta_rxposed_nativeloader_MainActivity_natvieloadapk(JNIEnv *env, jobject thiz ,jstring JAUTHORITY_app_name) {
    string AUTHORITY_app_name = env->GetStringUTFChars(JAUTHORITY_app_name, nullptr);
    InjectApp::GetInstance()->LoadExternApk(const_cast<char *>(AUTHORITY_app_name.c_str()));

}
extern "C"
JNIEXPORT void JNICALL
Java_hepta_rxposed_nativeloader_MainActivity_getjavaJniMethod(JNIEnv *env, jobject thiz,
                                                              jstring appname) {

    DEBUG();

}
