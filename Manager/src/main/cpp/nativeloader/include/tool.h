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
#include "elf_symbol_resolver.h"
using namespace std;



//#if 1

#define RxposedTag "rxposed"
// 调用 debug 版本方法
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,RxposedTag,__VA_ARGS__)
#define DEBUG(...) __android_log_print(ANDROID_LOG_ERROR,RxposedTag,"[file %s],[line %d],[function:%s]",__FILE__, __LINE__,__func__);
#define LOGD(...)  __android_log_print(ANDROID_LOG_ERROR,RxposedTag,__VA_ARGS__)
#define LOGF(...) __android_log_print(ANDROID_LOG_FATAL, RxposedTag, __VA_ARGS__)



//#else
//#define LOGE(...)
//#define LOGD(...)
//#define DEBUG(...)
//#endif




#define GET_META_DATA 0x00000080

#define RXPOSED_CLS_ENTRY "rxposed_clsentry"
#define RXPOSED_MTD_CLASS "rxposed_mtdentry"
#define RXPOSED_APK_PATH "apk_path"
#define RXPOSED_APK_NATIVE_LIB "apk_native_lib"

 class AppinfoNative{
public:
    string pkgName;
    string source;
    string NativelibPath;
    string Entry_class;
    string Entry_method;
    AppinfoNative(string pkgName,string source,string NativelibPath,string Entry_class,string Entry_method){
        this->source =source;
        this->pkgName = pkgName;
        this->Entry_class = Entry_class;
        this->Entry_method = Entry_method;
        this->NativelibPath = NativelibPath;

    }
};

extern vector<string> string_split(string str,string pattern);


bool NDK_ExceptionCheck(JNIEnv *env,const char* message);

jobject getSystemContext(JNIEnv *env);
jobject getContext(JNIEnv *env);
void load_apk_And_exe_Class_Method(JNIEnv *pEnv, jobject android_context,AppinfoNative *appinfoNativeVec) ;
void load_apk_And_exe_Class_Method_13(JNIEnv *pEnv, jobject android_context,AppinfoNative *appinfoNativeVec);
jobject CreateApplicationContext(JNIEnv *env, string pkgName);
jobject GetRxposedProvider(JNIEnv *env, jobject android_Context , string& AUTHORITY, const string& Provider_call_method, const string& Provider_call_arg);
AppinfoNative* GetPmAppInfoNative(JNIEnv *env, jobject android_Context, string pkgName);
AppinfoNative* GetRxAppInfoNative(JNIEnv *env, jobject android_Context,string AUTHORITY,string pkgName);
JNIEnv *Pre_GetEnv() ;
bool hook_init_and_text(JNIEnv* env);
void * getJmethod_JniFunction(JNIEnv* env,jclass jclass1,jmethodID jmethodId);
jobject getConfigByProvider(JNIEnv* env,string AUTHORITY , string callName,string method ,string uid);

#endif //RXPOSED_TOOL_H
