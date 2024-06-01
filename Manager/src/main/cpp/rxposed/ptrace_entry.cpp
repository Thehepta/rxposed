//
// Created by thehepta on 2024/2/23.
//


#include <dlfcn.h>
#include <asm-generic/mman-common.h>
#include <fcntl.h>
#include <linux/mman.h>
#include <sys/prctl.h>
#include <sys/mman.h>
#include "rprocess.h"
#include "android_util_api.h"
#include "android14_hook.h"
#include "android12_hook.h"
#include "android11_hook.h"

//改这个函数名字记得一定要同步修改注入调用入口函数的符号，改 参数，const都会导致符号变更
void Ptrace_Zygotes(const char* Authority_arg){
    rprocess::GetInstance()->setAuthorityInfo(Authority_arg);
    rprocess::GetInstance()->zygote_nativeSpecializeAppProcess_hook();
}


void Inject_Porcess(const char* AUTHORITY_pkgName){

//    vector<string> arg_vec = string_split(strdup(AUTHORITY_pkgName), ":");
//    InjectApp::GetInstance()->LoadExternApk(strdup(AUTHORITY_pkgName));

}


void rxposed_init() __attribute__((constructor)) {
    if(android_get_device_api_level() >= 34){
        android14::art_method_hook_init();
        rprocess::GetInstance()->zygote_nativeSpecializeAppProcess_hook = android14::zygote_hook;
        rprocess::GetInstance()->getConfigByProvider = android14::getConfigByProvider;
    }else if(android_get_device_api_level() >= 31){
        android12::art_method_hook_init();
        rprocess::GetInstance()->zygote_nativeSpecializeAppProcess_hook = android12::zygote_hook;
        rprocess::GetInstance()->getConfigByProvider = android12::getConfigByProvider;
    } else if (android_get_device_api_level() == 30){
        android11::art_method_hook_init();
        rprocess::GetInstance()->zygote_nativeSpecializeAppProcess_hook = android11::zygote_hook;
        rprocess::GetInstance()->getConfigByProvider = android11::getConfigByProvider;
    }
}




