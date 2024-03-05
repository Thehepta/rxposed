//
// Created by thehepta on 2024/2/25.
//

#pragma once


void zygote_nativeSpecializeAppProcess_hook13() ;
jobject getConfigByProvider13(JNIEnv* env, string providerHost_providerName , string callName, string method , string uid_str);
bool art_method_hook_init13();