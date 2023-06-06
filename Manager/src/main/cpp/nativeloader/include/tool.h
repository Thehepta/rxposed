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
#include "debug.h"
using namespace std;



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
jobject CreateApplicationContext(JNIEnv *env, string pkgName);
jobject GetRxposedProvider(JNIEnv *env, jobject android_Context , string& AUTHORITY, const string& Provider_call_method, const string& Provider_call_arg);
AppinfoNative* GetPmAppInfoNative(JNIEnv *env, jobject android_Context, string pkgName);
AppinfoNative* GetRxAppInfoNative(JNIEnv *env, jobject android_Context,string AUTHORITY,string pkgName);
JNIEnv *Pre_GetEnv() ;
bool hook_init_and_text(JNIEnv* env);
void * getJmethod_JniFunction(JNIEnv* env,jclass jclass1,jmethodID jmethodId);
jobject getConfigByProvider(JNIEnv* env,string AUTHORITY , string callName,string method ,string processName);
#endif //RXPOSED_TOOL_H
