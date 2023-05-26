//
// Created by chic on 2023/5/15.
//

#include "include/InjectApp.h"



InjectApp * InjectApp::instance_ =nullptr;


void InjectApp::LoadExternApk(char *arg) {
    JNIEnv* env = Pre_GetEnv();
    vector<string> arg_vec = string_split(arg, "|");
    string AUTHORITY = arg_vec[0];
    vector<string> app_vec = string_split(arg_vec[1],":");

    jobject android_context = getContext(env);
    for (auto appName : app_vec){
        AppinfoNative* appinfoNative = GetRxAppInfoNative(env, android_context,AUTHORITY, appName);
        load_apk_And_exe_Class_Method(env, android_context, appinfoNative);
    }
}


JNIEnv *InjectApp::Pre_GetEnv() {
    void*getAndroidRuntimeEnv = DobbySymbolResolver("libandroid_runtime.so", "_ZN7android14AndroidRuntime9getJNIEnvEv");
    return ((JNIEnv*(*)())getAndroidRuntimeEnv)();
}
