//
// Created by thehepta on 2023/6/8.
//

#include <cstring>
#include "include/FunHook.h"
#include "include/elf_resolver.h"


#if defined(__aarch64__) // 真机64位


void * get_AndroidRuntimeGetEnv_addr(){

    void*getAndroidRuntimeEnv = rxposed::resolve_elf_internal_symbol("/system/lib64/libandroid_runtime.so", "_ZN7android14AndroidRuntime9getJNIEnvEv");
    return getAndroidRuntimeEnv;
}

void * get_getUidForName_addr(){
    void *getUidForName =  rxposed::resolve_elf_internal_symbol("/system/lib64/libandroid_runtime.so","_Z32android_os_Process_getUidForNameP7_JNIEnvP8_jobjectP8_jstring");
    return getUidForName;
}

void * get_android_os_Process_setArgV0_addr(){

    void *android_os_Process_setArg_addr = rxposed::resolve_elf_internal_symbol("/system/lib64/libandroid_runtime.so","_Z27android_os_Process_setArgV0P7_JNIEnvP8_jobjectP8_jstring");
    return android_os_Process_setArg_addr;
}

void * get_selinux_android_setcontext_addr(){
    void *selinux_android_setcontext_addr = rxposed::resolve_elf_internal_symbol("/system/lib64/libselinux.so","selinux_android_setcontext");

    return selinux_android_setcontext_addr;

}
#else

void * get_AndroidRuntimeGetEnv_addr(){

    void*getAndroidRuntimeEnv = rxposed::resolve_elf_internal_symbol("/system/lib/libandroid_runtime.so", "_ZN7android14AndroidRuntime9getJNIEnvEv");
    return getAndroidRuntimeEnv;
}

void * get_getUidForName_addr(){
    void *getUidForName =  rxposed::resolve_elf_internal_symbol("/system/lib/libandroid_runtime.so","_Z32android_os_Process_getUidForNameP7_JNIEnvP8_jobjectP8_jstring");
    return getUidForName;
}

void * get_android_os_Process_setArgV0_addr(){

    void *android_os_Process_setArg_addr = rxposed::resolve_elf_internal_symbol("/system/lib/libandroid_runtime.so","_Z27android_os_Process_setArgV0P7_JNIEnvP8_jobjectP8_jstring");
    return android_os_Process_setArg_addr;
}

void * get_selinux_android_setcontext_addr(){
    void *selinux_android_setcontext_addr = rxposed::resolve_elf_internal_symbol("/system/lib/libselinux.so","selinux_android_setcontext");
    return selinux_android_setcontext_addr;
}
#endif
