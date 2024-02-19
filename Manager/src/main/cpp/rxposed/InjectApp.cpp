//
// Created by chic on 2023/5/15.
//

#include <dlfcn.h>
#include "InjectApp.h"
#include "artmethod_native_hook.h"
#include "FunHook.h"

using namespace std;

InjectApp * InjectApp::instance_ =nullptr;


void InjectApp::LoadExternApk(char *arg) {
    JNIEnv* env = Pre_GetEnv();
    vector<string> arg_vec = string_split(arg, ":");
    string AUTHORITY = arg_vec[0];
    LOGE("AUTHORITY = %s",AUTHORITY.c_str());
    jobject android_context = getContext(env);
    for(int i=1;i<arg_vec.size();i++){
        auto appName = arg_vec[i];
        LOGE("appName = %s",appName.c_str());
//        AppinfoNative* appinfoNative = GetRxAppInfoNative(env, android_context,AUTHORITY, appName);     //android11的包可见性问题导致了contentprovider 请求失败
//        load_apk_And_Call_Class_Entry_Method(env, android_context, appinfoNative);
    }
}


bool hookinit(JNIEnv *env){

    jclass  Process_cls = env->FindClass("android/os/Process");
    DEBUG();
    jmethodID javamethod = env->GetStaticMethodID(Process_cls,"getUidForName", "(Ljava/lang/String;)I");
    DEBUG();

//    LOGE("setArgV0Native %p",setArgV0Native2);

    void * libandroid_runtime =  dlopen("libandroid_runtime.so",RTLD_NOW);
    void * getUidForName = reinterpret_cast<void *>(dlsym(libandroid_runtime,"_Z32android_os_Process_getUidForNameP7_JNIEnvP8_jobjectP8_jstring"));
    LOGE("setArgV0P7 %p",getUidForName);
    dlclose(libandroid_runtime);
    INIT_HOOK_PlatformABI(env, nullptr, javamethod, (uintptr_t *)getUidForName, 109);

    DEBUG();
}