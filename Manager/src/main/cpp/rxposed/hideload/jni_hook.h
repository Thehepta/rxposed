//
// Created by thehepta on 2024/1/16.
//

/*
 * Copyright (C) 2006 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * JNI specification, as defined by Sun:
 * http://java.sun.com/javase/6/docs/technotes/guides/jni/spec/jniTOC.html
 *
 * Everything here is expected to be VM-neutral.
 */

#pragma once

#include <stdarg.h>
#include <stdint.h>
#include "jni.h"


JavaVM* jni_hook_init(JavaVM* vm,jobject classLoader);


struct Linker_JNIInvokeInterface {
    void*       reserved0;
    void*       reserved1;
    void*       reserved2;

    jint        (*DestroyJavaVM)(JavaVM*);
    jint        (*AttachCurrentThread)(JavaVM*, JNIEnv**, void*);
    jint        (*DetachCurrentThread)(JavaVM*);
    jint        (*GetEnv)(JavaVM*, void**, jint);
    jint        (*AttachCurrentThreadAsDaemon)(JavaVM*, JNIEnv**, void*);

    Linker_JNIInvokeInterface(){};
};


struct Linker_JavaVM {

    struct Linker_JNIInvokeInterface * functions;
    JavaVM * vm;
    jobject classLoader;
    Linker_JavaVM(JavaVM *vm,jobject classLoader) ;

};



/*
 * Table of interface function pointers.
 */
struct Linker_JNINativeInterface {
    void*       reserved0;
    void*       reserved1;
    void*       reserved2;
    void*       reserved3;

    jint        (*GetVersion)(JNIEnv *);

    jclass      (*DefineClass)(JNIEnv*, const char*, jobject, const jbyte*,
                               jsize);
    jclass      (*FindClass)(JNIEnv*, const char*);

    jmethodID   (*FromReflectedMethod)(JNIEnv*, jobject);
    jfieldID    (*FromReflectedField)(JNIEnv*, jobject);
    /* spec doesn't show jboolean parameter */
    jobject     (*ToReflectedMethod)(JNIEnv*, jclass, jmethodID, jboolean);

    jclass      (*GetSuperclass)(JNIEnv*, jclass);
    jboolean    (*IsAssignableFrom)(JNIEnv*, jclass, jclass);

    /* spec doesn't show jboolean parameter */
    jobject     (*ToReflectedField)(JNIEnv*, jclass, jfieldID, jboolean);

    jint        (*Throw)(JNIEnv*, jthrowable);
    jint        (*ThrowNew)(JNIEnv *, jclass, const char *);
    jthrowable  (*ExceptionOccurred)(JNIEnv*);
    void        (*ExceptionDescribe)(JNIEnv*);
    void        (*ExceptionClear)(JNIEnv*);
    void        (*FatalError)(JNIEnv*, const char*);

    jint        (*PushLocalFrame)(JNIEnv*, jint);
    jobject     (*PopLocalFrame)(JNIEnv*, jobject);

    jobject     (*NewGlobalRef)(JNIEnv*, jobject);
    void        (*DeleteGlobalRef)(JNIEnv*, jobject);
    void        (*DeleteLocalRef)(JNIEnv*, jobject);
    jboolean    (*IsSameObject)(JNIEnv*, jobject, jobject);

    jobject     (*NewLocalRef)(JNIEnv*, jobject);
    jint        (*EnsureLocalCapacity)(JNIEnv*, jint);

    jobject     (*AllocObject)(JNIEnv*, jclass);
    jobject     (*NewObject)(JNIEnv*, jclass, jmethodID, ...);
    jobject     (*NewObjectV)(JNIEnv*, jclass, jmethodID, va_list);
    jobject     (*NewObjectA)(JNIEnv*, jclass, jmethodID, const jvalue*);

    jclass      (*GetObjectClass)(JNIEnv*, jobject);
    jboolean    (*IsInstanceOf)(JNIEnv*, jobject, jclass);
    jmethodID   (*GetMethodID)(JNIEnv*, jclass, const char*, const char*);

    jobject     (*CallObjectMethod)(JNIEnv*, jobject, jmethodID, ...);
    jobject     (*CallObjectMethodV)(JNIEnv*, jobject, jmethodID, va_list);
    jobject     (*CallObjectMethodA)(JNIEnv*, jobject, jmethodID, const jvalue*);
    jboolean    (*CallBooleanMethod)(JNIEnv*, jobject, jmethodID, ...);
    jboolean    (*CallBooleanMethodV)(JNIEnv*, jobject, jmethodID, va_list);
    jboolean    (*CallBooleanMethodA)(JNIEnv*, jobject, jmethodID, const jvalue*);
    jbyte       (*CallByteMethod)(JNIEnv*, jobject, jmethodID, ...);
    jbyte       (*CallByteMethodV)(JNIEnv*, jobject, jmethodID, va_list);
    jbyte       (*CallByteMethodA)(JNIEnv*, jobject, jmethodID, const jvalue*);
    jchar       (*CallCharMethod)(JNIEnv*, jobject, jmethodID, ...);
    jchar       (*CallCharMethodV)(JNIEnv*, jobject, jmethodID, va_list);
    jchar       (*CallCharMethodA)(JNIEnv*, jobject, jmethodID, const jvalue*);
    jshort      (*CallShortMethod)(JNIEnv*, jobject, jmethodID, ...);
    jshort      (*CallShortMethodV)(JNIEnv*, jobject, jmethodID, va_list);
    jshort      (*CallShortMethodA)(JNIEnv*, jobject, jmethodID, const jvalue*);
    jint        (*CallIntMethod)(JNIEnv*, jobject, jmethodID, ...);
    jint        (*CallIntMethodV)(JNIEnv*, jobject, jmethodID, va_list);
    jint        (*CallIntMethodA)(JNIEnv*, jobject, jmethodID, const jvalue*);
    jlong       (*CallLongMethod)(JNIEnv*, jobject, jmethodID, ...);
    jlong       (*CallLongMethodV)(JNIEnv*, jobject, jmethodID, va_list);
    jlong       (*CallLongMethodA)(JNIEnv*, jobject, jmethodID, const jvalue*);
    jfloat      (*CallFloatMethod)(JNIEnv*, jobject, jmethodID, ...);
    jfloat      (*CallFloatMethodV)(JNIEnv*, jobject, jmethodID, va_list);
    jfloat      (*CallFloatMethodA)(JNIEnv*, jobject, jmethodID, const jvalue*);
    jdouble     (*CallDoubleMethod)(JNIEnv*, jobject, jmethodID, ...);
    jdouble     (*CallDoubleMethodV)(JNIEnv*, jobject, jmethodID, va_list);
    jdouble     (*CallDoubleMethodA)(JNIEnv*, jobject, jmethodID, const jvalue*);
    void        (*CallVoidMethod)(JNIEnv*, jobject, jmethodID, ...);
    void        (*CallVoidMethodV)(JNIEnv*, jobject, jmethodID, va_list);
    void        (*CallVoidMethodA)(JNIEnv*, jobject, jmethodID, const jvalue*);

    jobject     (*CallNonvirtualObjectMethod)(JNIEnv*, jobject, jclass,
                                              jmethodID, ...);
    jobject     (*CallNonvirtualObjectMethodV)(JNIEnv*, jobject, jclass,
                                               jmethodID, va_list);
    jobject     (*CallNonvirtualObjectMethodA)(JNIEnv*, jobject, jclass,
                                               jmethodID, const jvalue*);
    jboolean    (*CallNonvirtualBooleanMethod)(JNIEnv*, jobject, jclass,
                                               jmethodID, ...);
    jboolean    (*CallNonvirtualBooleanMethodV)(JNIEnv*, jobject, jclass,
                                                jmethodID, va_list);
    jboolean    (*CallNonvirtualBooleanMethodA)(JNIEnv*, jobject, jclass,
                                                jmethodID, const jvalue*);
    jbyte       (*CallNonvirtualByteMethod)(JNIEnv*, jobject, jclass,
                                            jmethodID, ...);
    jbyte       (*CallNonvirtualByteMethodV)(JNIEnv*, jobject, jclass,
                                             jmethodID, va_list);
    jbyte       (*CallNonvirtualByteMethodA)(JNIEnv*, jobject, jclass,
                                             jmethodID, const jvalue*);
    jchar       (*CallNonvirtualCharMethod)(JNIEnv*, jobject, jclass,
                                            jmethodID, ...);
    jchar       (*CallNonvirtualCharMethodV)(JNIEnv*, jobject, jclass,
                                             jmethodID, va_list);
    jchar       (*CallNonvirtualCharMethodA)(JNIEnv*, jobject, jclass,
                                             jmethodID, const jvalue*);
    jshort      (*CallNonvirtualShortMethod)(JNIEnv*, jobject, jclass,
                                             jmethodID, ...);
    jshort      (*CallNonvirtualShortMethodV)(JNIEnv*, jobject, jclass,
                                              jmethodID, va_list);
    jshort      (*CallNonvirtualShortMethodA)(JNIEnv*, jobject, jclass,
                                              jmethodID, const jvalue*);
    jint        (*CallNonvirtualIntMethod)(JNIEnv*, jobject, jclass,
                                           jmethodID, ...);
    jint        (*CallNonvirtualIntMethodV)(JNIEnv*, jobject, jclass,
                                            jmethodID, va_list);
    jint        (*CallNonvirtualIntMethodA)(JNIEnv*, jobject, jclass,
                                            jmethodID, const jvalue*);
    jlong       (*CallNonvirtualLongMethod)(JNIEnv*, jobject, jclass,
                                            jmethodID, ...);
    jlong       (*CallNonvirtualLongMethodV)(JNIEnv*, jobject, jclass,
                                             jmethodID, va_list);
    jlong       (*CallNonvirtualLongMethodA)(JNIEnv*, jobject, jclass,
                                             jmethodID, const jvalue*);
    jfloat      (*CallNonvirtualFloatMethod)(JNIEnv*, jobject, jclass,
                                             jmethodID, ...);
    jfloat      (*CallNonvirtualFloatMethodV)(JNIEnv*, jobject, jclass,
                                              jmethodID, va_list);
    jfloat      (*CallNonvirtualFloatMethodA)(JNIEnv*, jobject, jclass,
                                              jmethodID, const jvalue*);
    jdouble     (*CallNonvirtualDoubleMethod)(JNIEnv*, jobject, jclass,
                                              jmethodID, ...);
    jdouble     (*CallNonvirtualDoubleMethodV)(JNIEnv*, jobject, jclass,
                                               jmethodID, va_list);
    jdouble     (*CallNonvirtualDoubleMethodA)(JNIEnv*, jobject, jclass,
                                               jmethodID, const jvalue*);
    void        (*CallNonvirtualVoidMethod)(JNIEnv*, jobject, jclass,
                                            jmethodID, ...);
    void        (*CallNonvirtualVoidMethodV)(JNIEnv*, jobject, jclass,
                                             jmethodID, va_list);
    void        (*CallNonvirtualVoidMethodA)(JNIEnv*, jobject, jclass,
                                             jmethodID, const jvalue*);

    jfieldID    (*GetFieldID)(JNIEnv*, jclass, const char*, const char*);

    jobject     (*GetObjectField)(JNIEnv*, jobject, jfieldID);
    jboolean    (*GetBooleanField)(JNIEnv*, jobject, jfieldID);
    jbyte       (*GetByteField)(JNIEnv*, jobject, jfieldID);
    jchar       (*GetCharField)(JNIEnv*, jobject, jfieldID);
    jshort      (*GetShortField)(JNIEnv*, jobject, jfieldID);
    jint        (*GetIntField)(JNIEnv*, jobject, jfieldID);
    jlong       (*GetLongField)(JNIEnv*, jobject, jfieldID);
    jfloat      (*GetFloatField)(JNIEnv*, jobject, jfieldID);
    jdouble     (*GetDoubleField)(JNIEnv*, jobject, jfieldID);

    void        (*SetObjectField)(JNIEnv*, jobject, jfieldID, jobject);
    void        (*SetBooleanField)(JNIEnv*, jobject, jfieldID, jboolean);
    void        (*SetByteField)(JNIEnv*, jobject, jfieldID, jbyte);
    void        (*SetCharField)(JNIEnv*, jobject, jfieldID, jchar);
    void        (*SetShortField)(JNIEnv*, jobject, jfieldID, jshort);
    void        (*SetIntField)(JNIEnv*, jobject, jfieldID, jint);
    void        (*SetLongField)(JNIEnv*, jobject, jfieldID, jlong);
    void        (*SetFloatField)(JNIEnv*, jobject, jfieldID, jfloat);
    void        (*SetDoubleField)(JNIEnv*, jobject, jfieldID, jdouble);

    jmethodID   (*GetStaticMethodID)(JNIEnv*, jclass, const char*, const char*);

    jobject     (*CallStaticObjectMethod)(JNIEnv*, jclass, jmethodID, ...);
    jobject     (*CallStaticObjectMethodV)(JNIEnv*, jclass, jmethodID, va_list);
    jobject     (*CallStaticObjectMethodA)(JNIEnv*, jclass, jmethodID, const jvalue*);
    jboolean    (*CallStaticBooleanMethod)(JNIEnv*, jclass, jmethodID, ...);
    jboolean    (*CallStaticBooleanMethodV)(JNIEnv*, jclass, jmethodID,
                                            va_list);
    jboolean    (*CallStaticBooleanMethodA)(JNIEnv*, jclass, jmethodID, const jvalue*);
    jbyte       (*CallStaticByteMethod)(JNIEnv*, jclass, jmethodID, ...);
    jbyte       (*CallStaticByteMethodV)(JNIEnv*, jclass, jmethodID, va_list);
    jbyte       (*CallStaticByteMethodA)(JNIEnv*, jclass, jmethodID, const jvalue*);
    jchar       (*CallStaticCharMethod)(JNIEnv*, jclass, jmethodID, ...);
    jchar       (*CallStaticCharMethodV)(JNIEnv*, jclass, jmethodID, va_list);
    jchar       (*CallStaticCharMethodA)(JNIEnv*, jclass, jmethodID, const jvalue*);
    jshort      (*CallStaticShortMethod)(JNIEnv*, jclass, jmethodID, ...);
    jshort      (*CallStaticShortMethodV)(JNIEnv*, jclass, jmethodID, va_list);
    jshort      (*CallStaticShortMethodA)(JNIEnv*, jclass, jmethodID, const jvalue*);
    jint        (*CallStaticIntMethod)(JNIEnv*, jclass, jmethodID, ...);
    jint        (*CallStaticIntMethodV)(JNIEnv*, jclass, jmethodID, va_list);
    jint        (*CallStaticIntMethodA)(JNIEnv*, jclass, jmethodID, const jvalue*);
    jlong       (*CallStaticLongMethod)(JNIEnv*, jclass, jmethodID, ...);
    jlong       (*CallStaticLongMethodV)(JNIEnv*, jclass, jmethodID, va_list);
    jlong       (*CallStaticLongMethodA)(JNIEnv*, jclass, jmethodID, const jvalue*);
    jfloat      (*CallStaticFloatMethod)(JNIEnv*, jclass, jmethodID, ...);
    jfloat      (*CallStaticFloatMethodV)(JNIEnv*, jclass, jmethodID, va_list);
    jfloat      (*CallStaticFloatMethodA)(JNIEnv*, jclass, jmethodID, const jvalue*);
    jdouble     (*CallStaticDoubleMethod)(JNIEnv*, jclass, jmethodID, ...);
    jdouble     (*CallStaticDoubleMethodV)(JNIEnv*, jclass, jmethodID, va_list);
    jdouble     (*CallStaticDoubleMethodA)(JNIEnv*, jclass, jmethodID, const jvalue*);
    void        (*CallStaticVoidMethod)(JNIEnv*, jclass, jmethodID, ...);
    void        (*CallStaticVoidMethodV)(JNIEnv*, jclass, jmethodID, va_list);
    void        (*CallStaticVoidMethodA)(JNIEnv*, jclass, jmethodID, const jvalue*);

    jfieldID    (*GetStaticFieldID)(JNIEnv*, jclass, const char*,
                                    const char*);

    jobject     (*GetStaticObjectField)(JNIEnv*, jclass, jfieldID);
    jboolean    (*GetStaticBooleanField)(JNIEnv*, jclass, jfieldID);
    jbyte       (*GetStaticByteField)(JNIEnv*, jclass, jfieldID);
    jchar       (*GetStaticCharField)(JNIEnv*, jclass, jfieldID);
    jshort      (*GetStaticShortField)(JNIEnv*, jclass, jfieldID);
    jint        (*GetStaticIntField)(JNIEnv*, jclass, jfieldID);
    jlong       (*GetStaticLongField)(JNIEnv*, jclass, jfieldID);
    jfloat      (*GetStaticFloatField)(JNIEnv*, jclass, jfieldID);
    jdouble     (*GetStaticDoubleField)(JNIEnv*, jclass, jfieldID);

    void        (*SetStaticObjectField)(JNIEnv*, jclass, jfieldID, jobject);
    void        (*SetStaticBooleanField)(JNIEnv*, jclass, jfieldID, jboolean);
    void        (*SetStaticByteField)(JNIEnv*, jclass, jfieldID, jbyte);
    void        (*SetStaticCharField)(JNIEnv*, jclass, jfieldID, jchar);
    void        (*SetStaticShortField)(JNIEnv*, jclass, jfieldID, jshort);
    void        (*SetStaticIntField)(JNIEnv*, jclass, jfieldID, jint);
    void        (*SetStaticLongField)(JNIEnv*, jclass, jfieldID, jlong);
    void        (*SetStaticFloatField)(JNIEnv*, jclass, jfieldID, jfloat);
    void        (*SetStaticDoubleField)(JNIEnv*, jclass, jfieldID, jdouble);

    jstring     (*NewString)(JNIEnv*, const jchar*, jsize);
    jsize       (*GetStringLength)(JNIEnv*, jstring);
    const jchar* (*GetStringChars)(JNIEnv*, jstring, jboolean*);
    void        (*ReleaseStringChars)(JNIEnv*, jstring, const jchar*);
    jstring     (*NewStringUTF)(JNIEnv*, const char*);
    jsize       (*GetStringUTFLength)(JNIEnv*, jstring);
    /* JNI spec says this returns const jbyte*, but that's inconsistent */
    const char* (*GetStringUTFChars)(JNIEnv*, jstring, jboolean*);
    void        (*ReleaseStringUTFChars)(JNIEnv*, jstring, const char*);
    jsize       (*GetArrayLength)(JNIEnv*, jarray);
    jobjectArray (*NewObjectArray)(JNIEnv*, jsize, jclass, jobject);
    jobject     (*GetObjectArrayElement)(JNIEnv*, jobjectArray, jsize);
    void        (*SetObjectArrayElement)(JNIEnv*, jobjectArray, jsize, jobject);

    jbooleanArray (*NewBooleanArray)(JNIEnv*, jsize);
    jbyteArray    (*NewByteArray)(JNIEnv*, jsize);
    jcharArray    (*NewCharArray)(JNIEnv*, jsize);
    jshortArray   (*NewShortArray)(JNIEnv*, jsize);
    jintArray     (*NewIntArray)(JNIEnv*, jsize);
    jlongArray    (*NewLongArray)(JNIEnv*, jsize);
    jfloatArray   (*NewFloatArray)(JNIEnv*, jsize);
    jdoubleArray  (*NewDoubleArray)(JNIEnv*, jsize);

    jboolean*   (*GetBooleanArrayElements)(JNIEnv*, jbooleanArray, jboolean*);
    jbyte*      (*GetByteArrayElements)(JNIEnv*, jbyteArray, jboolean*);
    jchar*      (*GetCharArrayElements)(JNIEnv*, jcharArray, jboolean*);
    jshort*     (*GetShortArrayElements)(JNIEnv*, jshortArray, jboolean*);
    jint*       (*GetIntArrayElements)(JNIEnv*, jintArray, jboolean*);
    jlong*      (*GetLongArrayElements)(JNIEnv*, jlongArray, jboolean*);
    jfloat*     (*GetFloatArrayElements)(JNIEnv*, jfloatArray, jboolean*);
    jdouble*    (*GetDoubleArrayElements)(JNIEnv*, jdoubleArray, jboolean*);

    void        (*ReleaseBooleanArrayElements)(JNIEnv*, jbooleanArray,
                                               jboolean*, jint);
    void        (*ReleaseByteArrayElements)(JNIEnv*, jbyteArray,
                                            jbyte*, jint);
    void        (*ReleaseCharArrayElements)(JNIEnv*, jcharArray,
                                            jchar*, jint);
    void        (*ReleaseShortArrayElements)(JNIEnv*, jshortArray,
                                             jshort*, jint);
    void        (*ReleaseIntArrayElements)(JNIEnv*, jintArray,
                                           jint*, jint);
    void        (*ReleaseLongArrayElements)(JNIEnv*, jlongArray,
                                            jlong*, jint);
    void        (*ReleaseFloatArrayElements)(JNIEnv*, jfloatArray,
                                             jfloat*, jint);
    void        (*ReleaseDoubleArrayElements)(JNIEnv*, jdoubleArray,
                                              jdouble*, jint);

    void        (*GetBooleanArrayRegion)(JNIEnv*, jbooleanArray,
                                         jsize, jsize, jboolean*);
    void        (*GetByteArrayRegion)(JNIEnv*, jbyteArray,
                                      jsize, jsize, jbyte*);
    void        (*GetCharArrayRegion)(JNIEnv*, jcharArray,
                                      jsize, jsize, jchar*);
    void        (*GetShortArrayRegion)(JNIEnv*, jshortArray,
                                       jsize, jsize, jshort*);
    void        (*GetIntArrayRegion)(JNIEnv*, jintArray,
                                     jsize, jsize, jint*);
    void        (*GetLongArrayRegion)(JNIEnv*, jlongArray,
                                      jsize, jsize, jlong*);
    void        (*GetFloatArrayRegion)(JNIEnv*, jfloatArray,
                                       jsize, jsize, jfloat*);
    void        (*GetDoubleArrayRegion)(JNIEnv*, jdoubleArray,
                                        jsize, jsize, jdouble*);

    /* spec shows these without const; some jni.h do, some don't */
    void        (*SetBooleanArrayRegion)(JNIEnv*, jbooleanArray,
                                         jsize, jsize, const jboolean*);
    void        (*SetByteArrayRegion)(JNIEnv*, jbyteArray,
                                      jsize, jsize, const jbyte*);
    void        (*SetCharArrayRegion)(JNIEnv*, jcharArray,
                                      jsize, jsize, const jchar*);
    void        (*SetShortArrayRegion)(JNIEnv*, jshortArray,
                                       jsize, jsize, const jshort*);
    void        (*SetIntArrayRegion)(JNIEnv*, jintArray,
                                     jsize, jsize, const jint*);
    void        (*SetLongArrayRegion)(JNIEnv*, jlongArray,
                                      jsize, jsize, const jlong*);
    void        (*SetFloatArrayRegion)(JNIEnv*, jfloatArray,
                                       jsize, jsize, const jfloat*);
    void        (*SetDoubleArrayRegion)(JNIEnv*, jdoubleArray,
                                        jsize, jsize, const jdouble*);

    jint        (*RegisterNatives)(JNIEnv*, jclass, const JNINativeMethod*,
                                   jint);
    jint        (*UnregisterNatives)(JNIEnv*, jclass);
    jint        (*MonitorEnter)(JNIEnv*, jobject);
    jint        (*MonitorExit)(JNIEnv*, jobject);
    jint        (*GetJavaVM)(JNIEnv*, JavaVM**);

    void        (*GetStringRegion)(JNIEnv*, jstring, jsize, jsize, jchar*);
    void        (*GetStringUTFRegion)(JNIEnv*, jstring, jsize, jsize, char*);

    void*       (*GetPrimitiveArrayCritical)(JNIEnv*, jarray, jboolean*);
    void        (*ReleasePrimitiveArrayCritical)(JNIEnv*, jarray, void*, jint);

    const jchar* (*GetStringCritical)(JNIEnv*, jstring, jboolean*);
    void        (*ReleaseStringCritical)(JNIEnv*, jstring, const jchar*);

    jweak       (*NewWeakGlobalRef)(JNIEnv*, jobject);
    void        (*DeleteWeakGlobalRef)(JNIEnv*, jweak);

    jboolean    (*ExceptionCheck)(JNIEnv*);

    jobject     (*NewDirectByteBuffer)(JNIEnv*, void*, jlong);
    void*       (*GetDirectBufferAddress)(JNIEnv*, jobject);
    jlong       (*GetDirectBufferCapacity)(JNIEnv*, jobject);

    /* added in JNI 1.6 */
    jobjectRefType (*GetObjectRefType)(JNIEnv*, jobject);
};



struct Linker_JNIEnv {
    /* do not rename this; it does not seem to be entirely opaque */
    struct Linker_JNINativeInterface* functions;
    JNIEnv * env;
    jobject classLoader;
    Linker_JNIEnv(JNIEnv *env, jobject pJobject);
};








