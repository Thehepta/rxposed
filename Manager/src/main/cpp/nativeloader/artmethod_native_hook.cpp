//
// Created by chic on 2023/5/23.
//

#include "include/artmethod_native_hook.h"
inline static bool IsIndexId(jmethodID mid) { return ((reinterpret_cast<uintptr_t>(mid) % 2) != 0); }

static jfieldID field_art_method = nullptr;

int access_flags_art_method_offset = -1;
static int jni_offset;
static int api;

//传入的是method索引
void *GetArtMethod(JNIEnv *env, jclass clazz, jmethodID methodId) {

    if (api >= __ANDROID_API_R__) {
        if (IsIndexId(methodId)) {
            jobject method = env->ToReflectedMethod(clazz, methodId, true);
            return reinterpret_cast<void *>(env->GetLongField(method, field_art_method));
        }
    }
    return methodId;
}
//传入的不是method索引，而是method对象的索引
void *GetArtMethod(JNIEnv *env, jclass clazz, jobject methodId) {

    if (api >= __ANDROID_API_R__) {

        return reinterpret_cast<void *>(env->GetLongField(methodId, field_art_method));

    }
    return methodId;
}


void *GetOriginalNativeFunction(const uintptr_t *art_method) {
    if (__predict_false(art_method == nullptr)) {
        return nullptr;
    }
    return (void *)art_method[jni_offset];
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



bool INIT_HOOK_PlatformABI(JNIEnv *env, jclass clazz, jmethodID methodId, void *native, uint32_t flags) {

    api = android_get_device_api_level();
    if (api >= __ANDROID_API_R__) {
        field_art_method = getArtMethod_filed(env);
        CHECK(field_art_method);
    }

    uintptr_t *artMethod = static_cast<uintptr_t *>(GetArtMethod(env, clazz, methodId));

    bool success = false;
    for (int i = 0; i < 30; ++i) {

        if (reinterpret_cast<void *>(artMethod[i]) == native) {
            jni_offset = i;
            success = true;
            LOGE("found art method entrypoint jni offset: %d", i);
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
            LOGE("found art method match access flags offset: %d", i * 4);
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
