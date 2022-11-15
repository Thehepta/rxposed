#ifndef QMULKT_JNI_HELPER1_H
#define QMULKT_JNI_HELPER1_H

#include <android/log.h>
#include <jni.h>
#define LOG_TAG "RxposedInject"
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)


jobject getGlobalContext(JNIEnv *env);
jstring GetApkSourcebyUid(JNIEnv *env,int uid);
std::string GetNativelibPathByUID(JNIEnv *env,int uid);
void load_apk(JNIEnv *pEnv, jstring apk, jstring dexOutput, jstring nativelib,std::string class_name,std::string method_name);
//AppinfoNative *GetAppInfoNative(JNIEnv *env,int uid);
jstring GetConfigByProvider(JNIEnv *env,int uid,std::string AUTHORITY);
jobject getSystemContext(JNIEnv *env);
bool NDK_ExceptionCheck(JNIEnv *env,char * message);




#endif //QMULKT_JNI_HELPER1_H
