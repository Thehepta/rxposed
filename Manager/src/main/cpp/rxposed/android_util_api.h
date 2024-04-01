//
// Created by chic on 2023/5/15.
//

#ifndef RXPOSED_ANDROID_UTIL_API_H
#define RXPOSED_ANDROID_UTIL_API_H


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
#define DEBUG(...) __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,"[function %s],[line %d]",__func__, __LINE__);
#define LOGD(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#define LOGF(...) __android_log_print(ANDROID_LOG_FATAL, LOG_TAG, __VA_ARGS__)

#else
#define LOGE(...)
#define LOGD(...)
#define DEBUG(...)
#endif


std::vector<std::string> string_split(std::string str,std::string pattern);
bool NDK_ExceptionCheck(JNIEnv *env,const char* message);
void load_apk_And_Call_Class_Entry_Method(JNIEnv *pEnv, jobject android_context,std::string source,std::string NativelibPath,std::string Entry_class,std::string Entry_method,std::string hide,std::string argument);
jobject CreateApplicationContext(JNIEnv *env, std::string pkgName,uid_t currentUid);
JNIEnv *Pre_GetEnv();
const char* get_Process_setArgV0(JNIEnv *env);
void print_java_stack(JNIEnv *env);
#endif //RXPOSED_ANDROID_UTIL_API_H
