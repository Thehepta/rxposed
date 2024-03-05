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
#include "android13_hook.h"

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


//    rprocess::GetInstance()->Init();
    if(android_get_device_api_level() == 33){
        art_method_hook_init13();
        rprocess::GetInstance()->zygote_nativeSpecializeAppProcess_hook = zygote_nativeSpecializeAppProcess_hook13;
        rprocess::GetInstance()->getConfigByProvider= getConfigByProvider13;
    } else if (android_get_device_api_level() == 32){

    }
//    PPLOGD("Enter pp_init......");
}




