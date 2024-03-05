//
// Created by chic on 2023/5/23.
//

#include "artmethod_native_hook.h"
#include <map>
#include <dlfcn.h>

inline static bool IsIndexId(jmethodID mid) { return ((reinterpret_cast<uintptr_t>(mid) % 2) != 0); }

static jfieldID field_art_method = nullptr;

static std::map<uintptr_t , uintptr_t> hookFunMaps;


int access_flags_art_method_offset = -1;
static int jni_offset;
static int api;

//传入的是method索引
uintptr_t GetArtMethod(JNIEnv *env, jclass clazz, jmethodID methodId) {

    if (api >= __ANDROID_API_R__) {
        if (IsIndexId(methodId)) {
            jobject method = env->ToReflectedMethod(clazz, methodId, true);
            return env->GetLongField(method, field_art_method);
        }
    }
    return reinterpret_cast<uintptr_t>(methodId);
}
//传入的不是method索引，而是method对象的索引
uintptr_t GetArtMethod(JNIEnv *env, jclass clazz, jobject methodId) {

    if (api >= __ANDROID_API_R__) {

        return env->GetLongField(methodId, field_art_method);

    }
    return reinterpret_cast<uintptr_t>(methodId);
}

uintptr_t HookJniNativeFunction(uintptr_t *art_method, uintptr_t hook_fun_addr){
    if (__predict_false(art_method == nullptr)) {
        return NULL;
    }
    uintptr_t old_fun_addr = art_method[jni_offset];
    hookFunMaps.emplace(hook_fun_addr,old_fun_addr);
    art_method[jni_offset] = hook_fun_addr;
    return old_fun_addr;
}

void unHookJniNativeFunction(uintptr_t *art_method){
    if (__predict_false(art_method == nullptr)) {
        return ;
    }
    uintptr_t hook_fun_addr = art_method[jni_offset];
    uintptr_t old_fun_addr = hookFunMaps.at(hook_fun_addr);
    art_method[jni_offset] = old_fun_addr;
    hookFunMaps.erase(hook_fun_addr);
}


uintptr_t GetOriginalNativeFunction(const uintptr_t *art_method) {
    if (__predict_false(art_method == nullptr)) {
        return NULL;
    }
    return art_method[jni_offset];
}

//Executable 的artMethod 可能是隐藏的，需要注意是否能获取
jfieldID getArtMethod_filed(JNIEnv *env){

    jclass clazz = env->FindClass("java/lang/reflect/Executable");
    auto field = env->GetFieldID(static_cast<jclass>(clazz), "artMethod", "J");
    if (env->ExceptionCheck()) {
        env->ExceptionDescribe();
        env->ExceptionClear();

    }
    return field;
}



bool INIT_HOOK_PlatformABI(JNIEnv *env, jclass clazz, jmethodID methodId, uintptr_t *native, uint32_t flags) {

    api = android_get_device_api_level();
    if (api >= __ANDROID_API_R__) {
        field_art_method = getArtMethod_filed(env);
        CHECK(field_art_method);
    }
    uintptr_t *artMethod = reinterpret_cast<uintptr_t *>(GetArtMethod(env, clazz, methodId));

    bool success = false;
    for (int i = 0; i < 30; ++i) {

        if (reinterpret_cast<void *>(artMethod[i]) == native) {
            jni_offset = i;
            success = true;
            break;
        }
    }

    CHECK(success);
    if (api >= __ANDROID_API_Q__) {
        // 非内部隐藏类
        flags |= 0x10000000;
    }
    char *start = reinterpret_cast<char *>(artMethod);
    for (int i = 1; i < 18; ++i) {
        uint32_t value = *(uint32_t *)(start + i * 4);
        if (value == flags) {
            access_flags_art_method_offset = i * 4;
            success &= true;
            break;
        }
    }
    if (access_flags_art_method_offset < 0) {
        if (api >= __ANDROID_API_N__) {
            access_flags_art_method_offset = 4;
        } else if (api == __ANDROID_API_M__) {
            access_flags_art_method_offset = 12;
        } else if (api == __ANDROID_API_L_MR1__) {
            access_flags_art_method_offset = 20;
        } else if (api == __ANDROID_API_L__) {
            access_flags_art_method_offset = 56;
        }
    }
    return success;
}
// android version code adaptation
//bool art_method_hook_init(JNIEnv* env){
//
//
//    jclass  Process_cls = env->FindClass("android/os/Process");
//    jmethodID javamethod = env->GetStaticMethodID(Process_cls,"getUidForName", "(Ljava/lang/String;)I");
//    void * libandroid_runtime =  dlopen("libandroid_runtime.so",RTLD_NOW);
//    uintptr_t getUidForName = reinterpret_cast<uintptr_t>(dlsym(libandroid_runtime,"_Z32android_os_Process_getUidForNameP7_JNIEnvP8_jobjectP8_jstring"));
//
//
//    INIT_HOOK_PlatformABI(env, nullptr,javamethod,(uintptr_t*)getUidForName,0x109);
//
//    uintptr_t  art_javamethod_method = GetArtMethod(env, Process_cls,javamethod);
//    uintptr_t native_art_art_javamethod_method = GetOriginalNativeFunction((uintptr_t *)art_javamethod_method);
//
//    if(native_art_art_javamethod_method == getUidForName){
//        return true;
//    } else{
//        return false;
//    }
//
//}

uintptr_t getJmethod_JniFunction(JNIEnv* env,jclass jclass1,jmethodID jmethodId){


    uintptr_t  jmethodArtmethod = GetArtMethod(env, jclass1, jmethodId);
    uintptr_t native_art_javamethod_method = GetOriginalNativeFunction((uintptr_t *)jmethodArtmethod);
    return native_art_javamethod_method;
}

uintptr_t HookJmethod_JniFunction(JNIEnv* env,jclass jclass1,jmethodID jmethodId,uintptr_t fun_addr) {

    uintptr_t jmethodArtmethod = GetArtMethod(env, jclass1,jmethodId);
    if(jmethodArtmethod == NULL){
        return NULL;
    }
    return HookJniNativeFunction( (uintptr_t*)jmethodArtmethod,fun_addr);
}
void unHookJmethod_JniFunction(JNIEnv* env,jclass jclass1,jmethodID jmethodId) {

    uintptr_t  jmethodArtmethod = GetArtMethod(env, jclass1,jmethodId);
    unHookJniNativeFunction((uintptr_t *)jmethodArtmethod);
}
