//
// Created by thehepta on 2024/2/19.
//

#include <android/log.h>
#define LOG_TAG "Rxposed_Test"
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

void Ptrace_Zygotes(const char* AUTHORITY){
    LOGE("Rxposed_Test inject successful");
}


void Inject_Porcess(const char* AUTHORITY_pkgName){


}
