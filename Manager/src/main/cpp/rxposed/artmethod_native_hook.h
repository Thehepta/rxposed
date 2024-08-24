//
// Created by chic on 2023/5/23.
//



#pragma once
#include <jni.h>
#include <cstdlib>
#include <android/log.h>

#define LOG_TAG "RxpInSO"


#define async_safe_fatal(...)                                                                                          \
  do {                                                                                                                 \
    __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__);                                                                                            \
    abort();                                                                                                           \
  } while (0)


#define CHECK(predicate)                                                                                               \
  do {                                                                                                                 \
    if (!(predicate)) {                                                                                                \
      async_safe_fatal("%s:%d: %s CHECK '" #predicate "' failed", __FILE__, __LINE__, __FUNCTION__);                   \
    }                                                                                                                  \
  } while (0)


bool art_method_hook_init(JNIEnv* env);
uintptr_t getJmethod_JniFunction(JNIEnv* env,jclass jclass1,jmethodID jmethodId);
uintptr_t  HookJmethod_JniFunction(JNIEnv* env,jclass jclass1,jmethodID jmethodId,uintptr_t fun_addr) ;
void unHookJmethod_JniFunction(JNIEnv* env,jclass jclass1,jmethodID jmethodId);

bool INIT_HOOK_PlatformABI(JNIEnv *env, jclass clazz, jmethodID methodId, uintptr_t *native, uint32_t flags);

uintptr_t GetArtMethod(JNIEnv *env, jclass clazz, jmethodID methodId);

uintptr_t GetArtMethod(JNIEnv *env, jclass clazz, jobject methodId);

uintptr_t GetOriginalNativeFunction(const uintptr_t *art_method);

uintptr_t HookJniNativeFunction(uintptr_t *art_method, uintptr_t fun_addr);

void unHookJniNativeFunction(uintptr_t *art_method);

