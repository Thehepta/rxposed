//
// Created by chic on 2023/5/15.
//

#ifndef RXPOSED_TOOL_H
#define RXPOSED_TOOL_H


#include <string>
#include <vector>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <jni.h>
#include <android/log.h>



#define LOG_TAG "Rxposed_Inject"
#if 1
// 调用 debug 版本方法
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#define DEBUG(...) __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,"[file %s],[line %d],[function:%s]",__FILE__, __LINE__,__func__);
#define LOGD(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#define LOGF(...) __android_log_print(ANDROID_LOG_FATAL, LOG_TAG, __VA_ARGS__)



#else
#define LOGE(...)
#define LOGD(...)
#define DEBUG(...)
#endif


static const char *getLibPath() {
#ifndef __aarch64__
    return "/system/lib/";
#else
    return "/system/lib64/";
#endif
}



#define GET_META_DATA 0x00000080

#define RXPOSED_CLS_ENTRY "rxposed_clsentry"
#define RXPOSED_MTD_CLASS "rxposed_mtdentry"
#define RXPOSED_APK_PATH "apk_path"
#define RXPOSED_APK_NATIVE_LIB "apk_native_lib"

 class AppinfoNative{
public:
    std::string pkgName;
     std::string source;
     std::string NativelibPath;
     std::string Entry_class;
     std::string Entry_method;
    AppinfoNative(std::string pkgName,std::string source,std::string NativelibPath,std::string Entry_class,std::string Entry_method){
        this->source =source;
        this->pkgName = pkgName;
        this->Entry_class = Entry_class;
        this->Entry_method = Entry_method;
        this->NativelibPath = NativelibPath;

    }
};

extern std::vector<std::string> string_split(std::string str,std::string pattern);


bool NDK_ExceptionCheck(JNIEnv *env,const char* message);

jobject getSystemContext(JNIEnv *env);
jobject getContext(JNIEnv *env);
void load_apk_And_exe_Class_Method(JNIEnv *pEnv, jobject android_context,AppinfoNative *appinfoNativeVec) ;
void load_apk_And_exe_Class_Method_13(JNIEnv *pEnv, jobject android_context,AppinfoNative *appinfoNativeVec);
jobject CreateApplicationContext(JNIEnv *env, std::string pkgName,uid_t currentUid);
jobject GetRxposedProvider(JNIEnv *env, jobject android_Context , std::string& AUTHORITY, const std::string& Provider_call_method, const std::string& Provider_call_arg);
AppinfoNative* GetPmAppInfoNative(JNIEnv *env, jobject android_Context, std::string pkgName);
AppinfoNative* GetRxAppInfoNative(JNIEnv *env, jobject android_Context,std::string AUTHORITY,std::string pkgName);
std::string getCurrentAppRxposedConfig(JNIEnv* env,std::string AUTHORITY , std::string callName,std::string method ,uid_t currentUid);
JNIEnv *Pre_GetEnv() ;
bool hook_init_and_text(JNIEnv* env);
void* dlsym_android_dlopen_ext(char* name);
void * getJmethod_JniFunction(JNIEnv* env,jclass jclass1,jmethodID jmethodId);
jobject getConfigByProvider(JNIEnv* env,std::string AUTHORITY , std::string callName,std::string method ,std::string uid_str);
#endif //RXPOSED_TOOL_H
