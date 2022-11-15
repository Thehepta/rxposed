//
// Created by Intel on 2022/5/11.
//


#include <jni.h>
#include <iostream>
#include <android/log.h>
#include "include/NDKhelper.h"

//清除异常，打印java堆栈好用户输入，继续执行
bool NDK_ExceptionCheck(JNIEnv *env,char* message){

    if(env->ExceptionCheck()){
        LOGD("%s",message);
        env->ExceptionDescribe();
        env->ExceptionClear();
        return true;
    }
    return false;
}


jobject getSystemContext(JNIEnv *env)
{
    //获取Activity Thread的实例对象
    jclass activityThreadClass = env->FindClass("android/app/ActivityThread");
    if(NDK_ExceptionCheck(env,"find class android/app/ActivityThread failed")){
        return nullptr;
    }
    jmethodID currentActivityThread = env->GetStaticMethodID(activityThreadClass, "currentActivityThread", "()Landroid/app/ActivityThread;");
    jobject at = env->CallStaticObjectMethod(activityThreadClass, currentActivityThread);
    if(at == nullptr){
//        LOGE("Failed to call currentActivityThread,at is null");
        return nullptr;
    }
    //ContextImpl，RxposedContext
    jmethodID getApplication = env->GetMethodID(activityThreadClass, "getRxposedContext", "()Landroid/app/ContextImpl;");
    jobject context = env->CallObjectMethod(at, getApplication);
    if(context == nullptr){
        LOGE("Failed to call getRxposedContext,context is null");
        return nullptr;
    }
    env->DeleteLocalRef(at);
    return context;
}

jobject GetAppInfobyUID(JNIEnv *env, int uid){
    jobject  context = getSystemContext(env);
    if(context == nullptr){
        return nullptr;
    }
    jclass Context_cls = env->FindClass("android/content/Context");
    jmethodID getPackageManager_method = env->GetMethodID(Context_cls, "getPackageManager", "()Landroid/content/pm/PackageManager;");
    jclass PackageManager_cls = env->FindClass("android/content/pm/PackageManager");
    jmethodID pm_getNameForUid_method = env->GetMethodID(PackageManager_cls, "getNameForUid","(I)Ljava/lang/String;");
    jmethodID pm_getApplicationInfo_method = env->GetMethodID(PackageManager_cls, "getApplicationInfo", "(Ljava/lang/String;I)Landroid/content/pm/ApplicationInfo;");
    jobject PackageManager = env->CallObjectMethod(context,getPackageManager_method);
    jobject java_pkaName = env->CallObjectMethod(PackageManager,pm_getNameForUid_method,uid);
    jobject appinfo = env->CallObjectMethod(PackageManager,pm_getApplicationInfo_method,java_pkaName,0);
    return appinfo;
}

jstring GetApkSourcebyUid(JNIEnv *env,int uid){

    jclass ApplicationInfo_cls = env->FindClass("android/content/pm/ApplicationInfo");
    jfieldID sourceDir_field = env->GetFieldID(ApplicationInfo_cls,"sourceDir","Ljava/lang/String;");

    jobject appinfo = GetAppInfobyUID(env,uid);
    if(appinfo == nullptr){
        return nullptr;
    }
    if(env->ExceptionCheck()){
        env->ExceptionDescribe();
        env->ExceptionClear();//清除引发的异常，在Java层不会打印异常堆栈信息，如果不清除，后面的调用ThrowNew抛出的异常堆栈信息会
        //覆盖前面的异常信息
        jclass cls_exception = env->FindClass("java/lang/Exception");
        env->ThrowNew(cls_exception,"call java static method ndk error");
        return nullptr;
    } else{
        jstring sourceDir = static_cast<jstring>(env->GetObjectField(appinfo,sourceDir_field));
        return sourceDir;
    }
}



jstring GetConfigByProvider(JNIEnv *env,int uid,std::string AUTHORITY){
    jobject  context = getSystemContext(env);
    jclass Context_cls = env->FindClass("android/content/Context");
    jclass Uri_cls = env->FindClass("android/net/Uri");
    jclass ContentResolver_cls = env->FindClass("android/content/ContentResolver");
    jclass Bundle_cls = env->FindClass("android/os/Bundle");
    jmethodID context_getContentResolver_method = env->GetMethodID(Context_cls, "getContentResolver","()Landroid/content/ContentResolver;");
    jmethodID ContentResolver_call_method = env->GetMethodID(ContentResolver_cls, "call","(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Landroid/os/Bundle;)Landroid/os/Bundle;");
    jmethodID Uri_parse_method = env->GetStaticMethodID(Uri_cls, "parse","(Ljava/lang/String;)Landroid/net/Uri;");
    jmethodID Bundle_getString_method = env->GetMethodID(Bundle_cls, "getString","(Ljava/lang/String;)Ljava/lang/String;");

    jobject URI = env->CallStaticObjectMethod(Uri_cls,Uri_parse_method,env->NewStringUTF(AUTHORITY.c_str()));
    return nullptr;
}


std::string GetNativelibPathByUID(JNIEnv *env,int uid){

    jclass ApplicationInfo_cls = env->FindClass("android/content/pm/ApplicationInfo");
    jfieldID sourceDir_field = env->GetFieldID(ApplicationInfo_cls,"sourceDir","Ljava/lang/String;");

    jfieldID nativeLibraryDir_field = env->GetFieldID(ApplicationInfo_cls,"nativeLibraryDir","Ljava/lang/String;");

    jobject appinfo = GetAppInfobyUID(env,uid);
    jstring nativeLibraryDir = static_cast<jstring>(env->GetObjectField(appinfo,
                                                                        nativeLibraryDir_field));

    const char* jnamestr = env->GetStringUTFChars(nativeLibraryDir, NULL);
    std::string stdFileName(jnamestr);
    return stdFileName;
}
void load_apk(JNIEnv *pEnv, jstring apk, jstring dexOutput, jstring nativelib,std::string class_name,std::string method_name) {
    jstring  name = pEnv->NewStringUTF(class_name.c_str());
    jobjectArray JAAR = nullptr;

    jstring  call_method_name = pEnv->NewStringUTF(method_name.c_str());
    jclass DexClassLoader_cls = pEnv->FindClass("dalvik/system/DexClassLoader");
    jmethodID init = pEnv->GetMethodID(DexClassLoader_cls, "<init>", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/ClassLoader;)V");
    jclass ClassLoader_cls = pEnv->FindClass("java/lang/ClassLoader");
    jmethodID method_getSystemClassLoader = pEnv->GetStaticMethodID(ClassLoader_cls,"getSystemClassLoader","()Ljava/lang/ClassLoader;");
    jmethodID method_loadClass = pEnv->GetMethodID(ClassLoader_cls,"loadClass","(Ljava/lang/String;)Ljava/lang/Class;");
    jclass Class_cls = pEnv->FindClass("java/lang/Class");
    jclass Method_cls = pEnv->FindClass("java/lang/reflect/Method");
    jmethodID invoke_met =  pEnv->GetMethodID(Method_cls,"invoke","(Ljava/lang/Object;[Ljava/lang/Object;)Ljava/lang/Object;");
    jmethodID clsmethod_method = pEnv->GetMethodID(Class_cls,"getMethod","(Ljava/lang/String;[Ljava/lang/Class;)Ljava/lang/reflect/Method;");



    jobject ClassLoader = pEnv->CallStaticObjectMethod(ClassLoader_cls,method_getSystemClassLoader);
    jobject dexClassLoader = pEnv->NewObject(DexClassLoader_cls,init,apk,dexOutput,nativelib,ClassLoader);

//    Class clazz = dexClassLoader.loadClass(name);
    jobject XposedTest = pEnv->CallObjectMethod(dexClassLoader,method_loadClass,name);
    if(pEnv->ExceptionCheck()){
        pEnv->ExceptionDescribe();
        pEnv->ExceptionClear();//清除引发的异常，在Java层不会打印异常堆栈信息，如果不清除，后面的调用ThrowNew抛出的异常堆栈信息会
        pEnv->DeleteLocalRef(ClassLoader);
        pEnv->DeleteLocalRef(dexClassLoader);
        pEnv->DeleteLocalRef(XposedTest);
        return ;

    }
//    Method native_hook = clazz.getMethod("native_hook");
    jobject user_metohd = pEnv->CallObjectMethod(XposedTest, clsmethod_method,call_method_name,JAAR);
    if(pEnv->ExceptionCheck()){
        pEnv->ExceptionDescribe();
        pEnv->ExceptionClear();//清除引发的异常，在Java层不会打印异常堆栈信息，如果不清除，后面的调用ThrowNew抛出的异常堆栈信息会
        pEnv->DeleteLocalRef(user_metohd);
        pEnv->DeleteLocalRef(ClassLoader);
        pEnv->DeleteLocalRef(dexClassLoader);
        pEnv->DeleteLocalRef(XposedTest);
        return ;
    }
//    native_hook.invoke(clazz);
    pEnv->CallObjectMethod(user_metohd, invoke_met,XposedTest,JAAR);
    if(pEnv->ExceptionCheck()){
        pEnv->ExceptionDescribe();
        pEnv->ExceptionClear();//清除引发的异常，在Java层不会打印异常堆栈信息，如果不清除，后面的调用ThrowNew抛出的异常堆栈信息会
        return ;
    }
    pEnv->DeleteLocalRef(user_metohd);
    pEnv->DeleteLocalRef(ClassLoader);
    pEnv->DeleteLocalRef(dexClassLoader);
    pEnv->DeleteLocalRef(XposedTest);
}


char* jstringTostring(JNIEnv* env, jstring jstr)
{
    char* rtn = NULL;
    jclass clsstring = env->FindClass("java/lang/String");
    jstring strencode = env->NewStringUTF("utf-8");
    jmethodID mid = env->GetMethodID(clsstring, "getBytes", "(Ljava/lang/String;)[B");
    jbyteArray barr= (jbyteArray)env->CallObjectMethod(jstr, mid, strencode);
    jsize alen = env->GetArrayLength(barr);
    jbyte* ba = env->GetByteArrayElements(barr, JNI_FALSE);
    if (alen > 0)
    {
        rtn = (char*)malloc(alen + 1);
        memcpy(rtn, ba, alen);
        rtn[alen] = 0;
    }
    env->ReleaseByteArrayElements(barr, ba, 0);
    return rtn;
}
