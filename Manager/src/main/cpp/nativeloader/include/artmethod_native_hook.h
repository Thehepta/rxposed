//
// Created by chic on 2023/5/23.
//



#ifndef NDKAPPLICATION_ARTMETHOD_NATIVE_HOOK_H
#define NDKAPPLICATION_ARTMETHOD_NATIVE_HOOK_H

#include <jni.h>
#include <cstdlib>
#include <android/log.h>

#define Rxposed_jnihook "rxposed_jni"


#define async_safe_fatal(...)                                                                                          \
  do {                                                                                                                 \
    __android_log_print(ANDROID_LOG_ERROR,Rxposed_jnihook,__VA_ARGS__);                                                                                            \
    abort();                                                                                                           \
  } while (0)


#define CHECK(predicate)                                                                                               \
  do {                                                                                                                 \
    if (!(predicate)) {                                                                                                \
      async_safe_fatal("%s:%d: %s CHECK '" #predicate "' failed", __FILE__, __LINE__, __FUNCTION__);                   \
    }                                                                                                                  \
  } while (0)


bool INIT_HOOK_PlatformABI(JNIEnv *env, jclass clazz, jmethodID methodId, void *native, uint32_t flags);

void *GetArtMethod(JNIEnv *env, jclass clazz, jmethodID methodId);

void *GetOriginalNativeFunction(const uintptr_t *art_method);


#endif //NDKAPPLICATION_ARTMETHOD_NATIVE_HOOK_H
