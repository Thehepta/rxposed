//
// Created by thehepta on 2024/1/16.
//

#include "jni_hook.h"

#include <android/log.h>
#define LOG_TAG "JNI_HOOK"
#define HOOKLOGE(...) __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)


JavaVM* jni_hook_init(JavaVM* vm,jobject classLoader){
    Linker_JavaVM *linkerJavaVm = new Linker_JavaVM(vm,classLoader);
    return (JavaVM*)linkerJavaVm;
}




jint  GetEnv(JavaVM * vm, void** env, jint version){

    HOOKLOGE("GetEnv");
    Linker_JavaVM* linkerJavaVm = (Linker_JavaVM *) vm;
    int re = linkerJavaVm->vm->GetEnv(env, version);
    Linker_JNIEnv * linkerJniEnv = new Linker_JNIEnv((JNIEnv *) *env, linkerJavaVm->classLoader);
    *env = linkerJniEnv;
    return re;
}
jint DestroyJavaVM(JavaVM*vm ){

    HOOKLOGE("DestroyJavaVM");
    Linker_JavaVM* linkerJavaVm = (Linker_JavaVM *) vm;
    return linkerJavaVm->vm->DestroyJavaVM();
}

jint AttachCurrentThread(JavaVM*vm,JNIEnv** p_env, void* thr_args){
    Linker_JavaVM* linkerJavaVm = (Linker_JavaVM *) vm;
    return linkerJavaVm->vm->AttachCurrentThread( p_env, thr_args);
}
jint DetachCurrentThread(JavaVM*vm){
    Linker_JavaVM* linkerJavaVm = (Linker_JavaVM *) vm;
    return linkerJavaVm->vm->DetachCurrentThread();
}

jint AttachCurrentThreadAsDaemon(JavaVM*vm,JNIEnv** p_env, void* thr_args){
    Linker_JavaVM* linkerJavaVm = (Linker_JavaVM *) vm;
    return linkerJavaVm->vm->AttachCurrentThreadAsDaemon( p_env, thr_args);
}


Linker_JavaVM::Linker_JavaVM(JavaVM *vm,jobject classLoader) {
    this->vm = vm;
    this->classLoader = classLoader;
    functions = new Linker_JNIInvokeInterface();
    functions->GetEnv = GetEnv;
    functions->DestroyJavaVM = DestroyJavaVM;
    functions->AttachCurrentThread = AttachCurrentThread;
    functions->DetachCurrentThread = DetachCurrentThread;
    functions->AttachCurrentThreadAsDaemon = AttachCurrentThreadAsDaemon;

}



const char* GetStringUTFChars(JNIEnv * env,jstring string, jboolean* isCopy){
    HOOKLOGE("GetStringUTFChars");
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    return functions->GetStringUTFChars( string, isCopy);

}
jint GetVersion(JNIEnv * env)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    return functions->GetVersion();
}

jclass DefineClass(JNIEnv * env,const char *name, jobject loader, const jbyte* buf,jsize bufLen){
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    return functions->DefineClass( name, loader, buf, bufLen);
}


jclass FindClass(JNIEnv * env,const char* name){
    HOOKLOGE("FindClass:%s",name);
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    jstring FindClass_name = functions->NewStringUTF(name);
    auto classloader = functions->FindClass("java/lang/ClassLoader");
    jmethodID method_loadClass = functions->GetMethodID(classloader,"loadClass","(Ljava/lang/String;)Ljava/lang/Class;");
    jobject FindClass_name_cls_obj = functions->CallObjectMethod(linkerJniEnv->classLoader,method_loadClass,FindClass_name);
    return static_cast<jclass>(FindClass_name_cls_obj);
}
jmethodID FromReflectedMethod(JNIEnv * env,jobject method)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    return functions->FromReflectedMethod( method);
}
jfieldID FromReflectedField(JNIEnv * env,jobject field)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    return functions->FromReflectedField( field);
}
jobject ToReflectedMethod(JNIEnv * env,jclass cls, jmethodID methodID, jboolean isStatic)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    return functions->ToReflectedMethod( cls, methodID, isStatic);
}
jclass GetSuperclass(JNIEnv * env,jclass clazz)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    return functions->GetSuperclass( clazz);
}
jboolean IsAssignableFrom(JNIEnv * env,jclass clazz1, jclass clazz2)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    return functions->IsAssignableFrom(clazz1, clazz2);
}
jobject ToReflectedField(JNIEnv * env,jclass cls, jfieldID fieldID, jboolean isStatic)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    return functions->ToReflectedField( cls, fieldID, isStatic);
}

jint Throw(JNIEnv * env,jthrowable obj)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    return functions->Throw( obj);
}

jint ThrowNew(JNIEnv * env,jclass clazz, const char* message)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    return functions->ThrowNew( clazz, message);
}

jthrowable ExceptionOccurred(JNIEnv * env)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    return functions->ExceptionOccurred();
}

void ExceptionDescribe(JNIEnv * env)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    functions->ExceptionDescribe();
}

void ExceptionClear(JNIEnv * env)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    functions->ExceptionClear();
}

void FatalError(JNIEnv * env,const char* msg)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    functions->FatalError(msg);
}
jint PushLocalFrame(JNIEnv * env,jint capacity)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    return functions->PushLocalFrame(capacity);
}

jobject PopLocalFrame(JNIEnv * env,jobject result)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    return functions->PopLocalFrame(result);
}

jobject NewGlobalRef(JNIEnv * env,jobject obj)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    return functions->NewGlobalRef(obj);
}

void DeleteGlobalRef(JNIEnv * env,jobject globalRef)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    functions->DeleteGlobalRef( globalRef);
}
void DeleteLocalRef(JNIEnv * env,jobject localRef)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    functions->DeleteLocalRef( localRef);
}

jboolean IsSameObject(JNIEnv * env,jobject ref1, jobject ref2)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    return functions->IsSameObject( ref1, ref2);
}

jobject NewLocalRef(JNIEnv * env,jobject ref)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    return functions->NewLocalRef( ref);
}

jint EnsureLocalCapacity(JNIEnv * env,jint capacity)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    return functions->EnsureLocalCapacity( capacity);
}

jobject AllocObject(JNIEnv * env,jclass clazz)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    return functions->AllocObject( clazz);
}

jobject NewObject(JNIEnv * env,jclass clazz, jmethodID methodID, ...)
{
    va_list args;
    va_start(args, methodID);
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    jobject result = functions->NewObjectV( clazz, methodID, args);
    va_end(args);
    return result;
}


jobject NewObjectV(JNIEnv * env,jclass clazz, jmethodID methodID, va_list args)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    return functions->NewObjectV(clazz, methodID, args);
}

jobject NewObjectA(JNIEnv * env,jclass clazz, jmethodID methodID, const jvalue* args)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    return functions->NewObjectA(clazz, methodID, args);
}

jclass GetObjectClass(JNIEnv * env,jobject obj)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    return functions->GetObjectClass(obj);
}

jboolean IsInstanceOf(JNIEnv * env,jobject obj, jclass clazz)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    return functions->IsInstanceOf(obj, clazz);
}

jmethodID GetMethodID(JNIEnv * env,jclass clazz, const char* name, const char* sig)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    return functions->GetMethodID(clazz, name, sig);
}


jobject CallObjectMethod(JNIEnv*env, jobject obj, jmethodID methodID, ...){

    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    va_list args;
    va_start(args, methodID);
    jobject re = functions->CallObjectMethod(obj, methodID, args);
    va_end(args);
    return  re;
}

jobject CallObjectMethodV(JNIEnv*env, jobject obj, jmethodID methodID, va_list args){

    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    return functions->CallObjectMethodV(obj, methodID, args);
}
jobject CallObjectMethodA(JNIEnv* env, jobject obj, jmethodID methodID, const jvalue*args){
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    return functions->CallObjectMethodA(obj, methodID, args);
}


jboolean CallBooleanMethod(JNIEnv*env, jobject obj, jmethodID methodID, ...){

    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    va_list args;
    va_start(args, methodID);
    jboolean re = functions->CallBooleanMethod(obj, methodID, args);
    va_end(args);
    return  re;
}

jboolean CallBooleanMethodV(JNIEnv*env, jobject obj, jmethodID methodID, va_list args){

    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    return functions->CallBooleanMethodV(obj, methodID, args);
}
jboolean CallBooleanMethodA(JNIEnv* env, jobject obj, jmethodID methodID, const jvalue*args){
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    return functions->CallBooleanMethodA(obj, methodID, args);
}


jbyte CallByteMethod(JNIEnv*env, jobject obj, jmethodID methodID, ...){

    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    va_list args;
    va_start(args, methodID);
    jbyte re = functions->CallByteMethod(obj, methodID, args);
    va_end(args);
    return  re;
}

jbyte CallByteMethodV(JNIEnv*env, jobject obj, jmethodID methodID, va_list args){

    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    return functions->CallByteMethodV(obj, methodID, args);
}
jbyte CallByteMethodA(JNIEnv* env, jobject obj, jmethodID methodID, const jvalue*args){
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    return functions->CallByteMethodA(obj, methodID, args);
}



jchar CallCharMethod(JNIEnv*env, jobject obj, jmethodID methodID, ...){

    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    va_list args;
    va_start(args, methodID);
    jchar re = functions->CallCharMethod(obj, methodID, args);
    va_end(args);
    return  re;
}

jchar CallCharMethodV(JNIEnv*env, jobject obj, jmethodID methodID, va_list args){

    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    return functions->CallCharMethodV(obj, methodID, args);
}
jchar CallCharMethodA(JNIEnv* env, jobject obj, jmethodID methodID, const jvalue*args){
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    return functions->CallCharMethodA(obj, methodID, args);
}


jshort CallShortMethod(JNIEnv*env, jobject obj, jmethodID methodID, ...){

    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    va_list args;
    va_start(args, methodID);
    jshort re = functions->CallShortMethod(obj, methodID, args);
    va_end(args);
    return  re;
}

jshort CallShortMethodV(JNIEnv*env, jobject obj, jmethodID methodID, va_list args){

    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    return functions->CallShortMethodV(obj, methodID, args);
}
jshort CallShortMethodA(JNIEnv* env, jobject obj, jmethodID methodID, const jvalue*args){
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    return functions->CallShortMethodA(obj, methodID, args);
}


jint CallIntMethod(JNIEnv*env, jobject obj, jmethodID methodID, ...){

    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    va_list args;
    va_start(args, methodID);
    jint re = functions->CallIntMethod(obj, methodID, args);
    va_end(args);
    return  re;
}

jint CallIntMethodV(JNIEnv*env, jobject obj, jmethodID methodID, va_list args){

    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    return functions->CallIntMethodV(obj, methodID, args);
}
jint CallIntMethodA(JNIEnv* env, jobject obj, jmethodID methodID, const jvalue*args){
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    return functions->CallIntMethodA(obj, methodID, args);
}


jlong CallLongMethod(JNIEnv*env, jobject obj, jmethodID methodID, ...){

    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    va_list args;
    va_start(args, methodID);
    jlong re = functions->CallLongMethod(obj, methodID, args);
    va_end(args);
    return  re;
}

jlong CallLongMethodV(JNIEnv*env, jobject obj, jmethodID methodID, va_list args){

    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    return functions->CallLongMethodV(obj, methodID, args);
}
jlong CallLongMethodA(JNIEnv* env, jobject obj, jmethodID methodID, const jvalue*args){
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    return functions->CallLongMethodA(obj, methodID, args);
}



jfloat CallFloatMethod(JNIEnv*env, jobject obj, jmethodID methodID, ...){

    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    va_list args;
    va_start(args, methodID);
    jfloat re = functions->CallFloatMethod(obj, methodID, args);
    va_end(args);
    return  re;
}

jfloat CallFloatMethodV(JNIEnv*env, jobject obj, jmethodID methodID, va_list args){

    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    return functions->CallFloatMethodV(obj, methodID, args);
}
jfloat CallFloatMethodA(JNIEnv* env, jobject obj, jmethodID methodID, const jvalue*args){
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    return functions->CallFloatMethodA(obj, methodID, args);
}


jdouble CallDoubleMethod(JNIEnv*env, jobject obj, jmethodID methodID, ...){

    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    va_list args;
    va_start(args, methodID);
    jdouble re = functions->CallDoubleMethod(obj, methodID, args);
    va_end(args);
    return  re;
}

jdouble CallDoubleMethodV(JNIEnv*env, jobject obj, jmethodID methodID, va_list args){

    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    return functions->CallDoubleMethodV(obj, methodID, args);
}
jdouble CallDoubleMethodA(JNIEnv* env, jobject obj, jmethodID methodID, const jvalue*args){
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    return functions->CallDoubleMethodA(obj, methodID, args);
}


void CallVoidMethod(JNIEnv*env, jobject obj, jmethodID methodID, ...){
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    va_list args;
    va_start(args, methodID);
    functions->CallVoidMethod(obj, methodID, args);
    va_end(args);
}
void CallVoidMethodV(JNIEnv*env, jobject obj, jmethodID methodID, va_list args){
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    functions->CallVoidMethodV(obj, methodID, args);
}
void CallVoidMethodA(JNIEnv* env, jobject obj, jmethodID methodID, const jvalue*args){
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    functions->CallVoidMethodA(obj, methodID, args);
}

jobject CallNonvirtualObjectMethod(JNIEnv*env, jobject obj,jclass clazz, jmethodID methodID, ...){
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    va_list args;
    va_start(args, methodID);
    jobject re = functions->CallNonvirtualObjectMethod(obj,clazz, methodID, args);
    va_end(args);
    return re;
}
jobject CallNonvirtualObjectMethodV(JNIEnv*env, jobject obj,jclass clazz, jmethodID methodID, va_list args){
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    return functions->CallNonvirtualObjectMethodV(obj, clazz,methodID, args);
}
jobject CallNonvirtualObjectMethodA(JNIEnv* env, jobject obj,jclass clazz, jmethodID methodID, const jvalue*args){
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    return functions->CallNonvirtualObjectMethodA(obj,clazz, methodID, args);
}

jboolean CallNonvirtualBooleanMethod(JNIEnv*env, jobject obj,jclass clazz, jmethodID methodID, ...){
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    va_list args;
    va_start(args, methodID);
    jboolean re = functions->CallNonvirtualBooleanMethod(obj,clazz, methodID, args);
    va_end(args);
    return re;
}
jboolean CallNonvirtualBooleanMethodV(JNIEnv*env, jobject obj,jclass clazz, jmethodID methodID, va_list args){
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    return functions->CallNonvirtualBooleanMethodV(obj, clazz,methodID, args);
}
jboolean CallNonvirtualBooleanMethodA(JNIEnv* env, jobject obj,jclass clazz, jmethodID methodID, const jvalue*args){
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    return functions->CallNonvirtualBooleanMethodA(obj,clazz, methodID, args);
}

jbyte CallNonvirtualByteMethod(JNIEnv*env, jobject obj,jclass clazz, jmethodID methodID, ...){
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    va_list args;
    va_start(args, methodID);
    jbyte re = functions->CallNonvirtualByteMethod(obj,clazz, methodID, args);
    va_end(args);
    return re;
}
jbyte CallNonvirtualByteMethodV(JNIEnv*env, jobject obj,jclass clazz, jmethodID methodID, va_list args){
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    return functions->CallNonvirtualByteMethodV(obj, clazz,methodID, args);
}
jbyte CallNonvirtualByteMethodA(JNIEnv* env, jobject obj,jclass clazz, jmethodID methodID, const jvalue*args){
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    return functions->CallNonvirtualByteMethodA(obj,clazz, methodID, args);
}

jchar CallNonvirtualCharMethod(JNIEnv*env, jobject obj,jclass clazz, jmethodID methodID, ...){
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    va_list args;
    va_start(args, methodID);
    jchar re = functions->CallNonvirtualCharMethod(obj,clazz, methodID, args);
    va_end(args);
    return re;
}
jchar CallNonvirtualCharMethodV(JNIEnv*env, jobject obj,jclass clazz, jmethodID methodID, va_list args){
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    return functions->CallNonvirtualCharMethodV(obj, clazz,methodID, args);
}
jchar CallNonvirtualCharMethodA(JNIEnv* env, jobject obj,jclass clazz, jmethodID methodID, const jvalue*args){
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    return functions->CallNonvirtualCharMethodA(obj,clazz, methodID, args);
}


jshort CallNonvirtualShortMethod(JNIEnv*env, jobject obj,jclass clazz, jmethodID methodID, ...){
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    va_list args;
    va_start(args, methodID);
    jshort re = functions->CallNonvirtualShortMethod(obj,clazz, methodID, args);
    va_end(args);
    return re;
}
jshort CallNonvirtualShortMethodV(JNIEnv*env, jobject obj,jclass clazz, jmethodID methodID, va_list args){
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    return functions->CallNonvirtualShortMethodV(obj, clazz,methodID, args);
}
jshort CallNonvirtualShortMethodA(JNIEnv* env, jobject obj,jclass clazz, jmethodID methodID, const jvalue*args){
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    return functions->CallNonvirtualShortMethodA(obj,clazz, methodID, args);
}


jint CallNonvirtualIntMethod(JNIEnv*env, jobject obj,jclass clazz, jmethodID methodID, ...){
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    va_list args;
    va_start(args, methodID);
    jint re = functions->CallNonvirtualIntMethod(obj,clazz, methodID, args);
    va_end(args);
    return re;
}
jint CallNonvirtualIntMethodV(JNIEnv*env, jobject obj,jclass clazz, jmethodID methodID, va_list args){
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    return functions->CallNonvirtualIntMethodV(obj, clazz,methodID, args);
}
jint CallNonvirtualIntMethodA(JNIEnv* env, jobject obj,jclass clazz, jmethodID methodID, const jvalue*args){
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    return functions->CallNonvirtualIntMethodA(obj,clazz, methodID, args);
}


jlong CallNonvirtualLongMethod(JNIEnv*env, jobject obj,jclass clazz, jmethodID methodID, ...){
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    va_list args;
    va_start(args, methodID);
    jlong re = functions->CallNonvirtualLongMethod(obj,clazz, methodID, args);
    va_end(args);
    return re;
}
jlong CallNonvirtualLongMethodV(JNIEnv*env, jobject obj,jclass clazz, jmethodID methodID, va_list args){
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    return functions->CallNonvirtualLongMethodV(obj, clazz,methodID, args);
}
jlong CallNonvirtualLongMethodA(JNIEnv* env, jobject obj,jclass clazz, jmethodID methodID, const jvalue*args){
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    return functions->CallNonvirtualLongMethodA(obj,clazz, methodID, args);
}


jfloat CallNonvirtualFloatMethod(JNIEnv*env, jobject obj,jclass clazz, jmethodID methodID, ...){
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    va_list args;
    va_start(args, methodID);
    jfloat re = functions->CallNonvirtualFloatMethod(obj,clazz, methodID, args);
    va_end(args);
    return re;
}
jfloat CallNonvirtualFloatMethodV(JNIEnv*env, jobject obj,jclass clazz, jmethodID methodID, va_list args){
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    return functions->CallNonvirtualFloatMethodV(obj, clazz,methodID, args);
}
jfloat CallNonvirtualFloatMethodA(JNIEnv* env, jobject obj,jclass clazz, jmethodID methodID, const jvalue*args){
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    return functions->CallNonvirtualFloatMethodA(obj,clazz, methodID, args);
}


jdouble CallNonvirtualDoubleMethod(JNIEnv*env, jobject obj,jclass clazz, jmethodID methodID, ...){
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    va_list args;
    va_start(args, methodID);
    jdouble re = functions->CallNonvirtualDoubleMethod(obj,clazz, methodID, args);
    va_end(args);
    return re;
}
jdouble CallNonvirtualDoubleMethodV(JNIEnv*env, jobject obj,jclass clazz, jmethodID methodID, va_list args){
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    return functions->CallNonvirtualDoubleMethodV(obj, clazz,methodID, args);
}
jdouble CallNonvirtualDoubleMethodA(JNIEnv* env, jobject obj,jclass clazz, jmethodID methodID, const jvalue*args){
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    return functions->CallNonvirtualDoubleMethodA(obj,clazz, methodID, args);
}


void CallNonvirtualVoidMethod(JNIEnv*env, jobject obj,jclass clazz, jmethodID methodID, ...){
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    va_list args;
    va_start(args, methodID);
    functions->CallNonvirtualDoubleMethod(obj,clazz, methodID, args);
    va_end(args);
}
void CallNonvirtualVoidMethodV(JNIEnv*env, jobject obj,jclass clazz, jmethodID methodID, va_list args){
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    functions->CallNonvirtualDoubleMethodV(obj, clazz,methodID, args);
}
void CallNonvirtualVoidMethodA(JNIEnv* env, jobject obj,jclass clazz, jmethodID methodID, const jvalue*args){
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    functions->CallNonvirtualDoubleMethodA(obj,clazz, methodID, args);
}

jfieldID GetFieldID(JNIEnv* env, jclass clazz, const char* name, const char* sig)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    return functions->GetFieldID( clazz, name, sig);
}

jobject GetObjectField(JNIEnv* env, jobject obj, jfieldID fieldID)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    return functions->GetObjectField( obj, fieldID);
}
jboolean GetBooleanField(JNIEnv* env, jobject obj, jfieldID fieldID)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    return functions->GetBooleanField( obj, fieldID);
}
jbyte GetByteField(JNIEnv* env, jobject obj, jfieldID fieldID)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    return functions->GetByteField( obj, fieldID);
}
jchar GetCharField(JNIEnv* env, jobject obj, jfieldID fieldID)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    return functions->GetCharField( obj, fieldID);
}
jshort GetShortField(JNIEnv* env, jobject obj, jfieldID fieldID)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    return functions->GetShortField( obj, fieldID);
}
jint GetIntField(JNIEnv* env, jobject obj, jfieldID fieldID)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    return functions->GetIntField(obj, fieldID);
}
jlong GetLongField(JNIEnv* env, jobject obj, jfieldID fieldID)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    return functions->GetLongField(obj, fieldID);
}
jfloat GetFloatField(JNIEnv* env, jobject obj, jfieldID fieldID)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    return functions->GetFloatField(obj, fieldID);
}
jdouble GetDoubleField(JNIEnv* env, jobject obj, jfieldID fieldID)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    return functions->GetDoubleField(obj, fieldID);
}

void SetObjectField(JNIEnv* env, jobject obj, jfieldID fieldID, jobject value)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    functions->SetObjectField(obj, fieldID, value);
}
void SetBooleanField(JNIEnv* env, jobject obj, jfieldID fieldID, jboolean value)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    functions->SetBooleanField( obj, fieldID, value);
}
void SetByteField(JNIEnv* env, jobject obj, jfieldID fieldID, jbyte value)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    functions->SetByteField(obj, fieldID, value);
}
void SetCharField(JNIEnv* env, jobject obj, jfieldID fieldID, jchar value)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    functions->SetCharField( obj, fieldID, value);
}
void SetShortField(JNIEnv* env, jobject obj, jfieldID fieldID, jshort value)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    functions->SetShortField( obj, fieldID, value);
}
void SetIntField(JNIEnv* env, jobject obj, jfieldID fieldID, jint value)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    functions->SetIntField( obj, fieldID, value);
}
void SetLongField(JNIEnv* env, jobject obj, jfieldID fieldID, jlong value)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    functions->SetLongField(obj, fieldID, value);
}
void SetFloatField(JNIEnv* env, jobject obj, jfieldID fieldID, jfloat value)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    functions->SetFloatField(obj, fieldID, value);
}
void SetDoubleField(JNIEnv* env, jobject obj, jfieldID fieldID, jdouble value)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    functions->SetDoubleField(obj, fieldID, value);
}
jmethodID GetStaticMethodID(JNIEnv* env,jclass clazz, const char* name, const char* sig)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    return functions->GetStaticMethodID( clazz, name, sig);
}

#define LINKER_CALL_STATIC_TYPE_METHOD(_jtype, _jname)                             \
    _jtype CallStatic##_jname##Method(JNIEnv* env,jclass clazz, jmethodID methodID, ...)  \
    {                                                                       \
        Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;                      \
        JNIEnv * functions = linkerJniEnv->env;                                                  \
        _jtype result;                                                      \
        va_list args;                                                       \
        va_start(args, methodID);                                           \
        result = functions->CallStatic##_jname##MethodV( clazz, methodID, args);   \
        va_end(args);                                                       \
        return result;                                                      \
    }
#define LINKER_CALL_STATIC_TYPE_METHODV(_jtype, _jname)                            \
    _jtype CallStatic##_jname##MethodV(JNIEnv* env,jclass clazz, jmethodID methodID,    \
        va_list args)                                                       \
    {                                                                              \
        Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;                      \
        JNIEnv * functions = linkerJniEnv->env;                           \
        return functions->CallStatic##_jname##MethodV( clazz, methodID,  args);      \
    }
#define LINKER_CALL_STATIC_TYPE_METHODA(_jtype, _jname)                            \
    _jtype CallStatic##_jname##MethodA(JNIEnv* env,jclass clazz, jmethodID methodID,    \
                                       const jvalue* args)                  \
    {                                                                              \
        Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;                      \
        JNIEnv * functions = linkerJniEnv->env; \
        return functions->CallStatic##_jname##MethodA( clazz, methodID,  args);      \
    }

#define LINKER_CALL_STATIC_TYPE(_jtype, _jname)                                    \
    LINKER_CALL_STATIC_TYPE_METHOD(_jtype, _jname)                                 \
    LINKER_CALL_STATIC_TYPE_METHODV(_jtype, _jname)                                \
    LINKER_CALL_STATIC_TYPE_METHODA(_jtype, _jname)

LINKER_CALL_STATIC_TYPE(jobject, Object)
LINKER_CALL_STATIC_TYPE(jboolean, Boolean)
LINKER_CALL_STATIC_TYPE(jbyte, Byte)
LINKER_CALL_STATIC_TYPE(jchar, Char)
LINKER_CALL_STATIC_TYPE(jshort, Short)
LINKER_CALL_STATIC_TYPE(jint, Int)
LINKER_CALL_STATIC_TYPE(jlong, Long)
LINKER_CALL_STATIC_TYPE(jfloat, Float)
LINKER_CALL_STATIC_TYPE(jdouble, Double)

void CallStaticVoidMethod(JNIEnv* env,jclass clazz, jmethodID methodID, ...)
{
    va_list args;
    va_start(args, methodID);
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    functions->CallStaticVoidMethod( clazz, methodID, args);
    va_end(args);
}
void CallStaticVoidMethodV(JNIEnv* env,jclass clazz, jmethodID methodID, va_list args)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    functions->CallStaticVoidMethodV( clazz, methodID, args);
}
void CallStaticVoidMethodA(JNIEnv* env,jclass clazz, jmethodID methodID, const jvalue* args)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    functions->CallStaticVoidMethodA( clazz, methodID, args);
}

jfieldID GetStaticFieldID(JNIEnv* env,jclass clazz, const char* name, const char* sig)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    return functions->GetStaticFieldID( clazz, name, sig);
}

jobject GetStaticObjectField(JNIEnv* env,jclass clazz, jfieldID fieldID)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    return functions->GetStaticObjectField( clazz, fieldID);
}
jboolean GetStaticBooleanField(JNIEnv* env,jclass clazz, jfieldID fieldID)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    return functions->GetStaticBooleanField( clazz, fieldID);
}
jbyte GetStaticByteField(JNIEnv* env,jclass clazz, jfieldID fieldID)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    return functions->GetStaticByteField( clazz, fieldID);
}
jchar GetStaticCharField(JNIEnv* env,jclass clazz, jfieldID fieldID)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    return functions->GetStaticCharField( clazz, fieldID);
}
jshort GetStaticShortField(JNIEnv* env,jclass clazz, jfieldID fieldID)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    return functions->GetStaticShortField( clazz, fieldID);
}
jint GetStaticIntField(JNIEnv* env,jclass clazz, jfieldID fieldID)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    return functions->GetStaticIntField( clazz, fieldID);
}
jlong GetStaticLongField(JNIEnv* env,jclass clazz, jfieldID fieldID)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    return functions->GetStaticLongField( clazz, fieldID);
}
jfloat GetStaticFloatField(JNIEnv* env,jclass clazz, jfieldID fieldID)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    return functions->GetStaticFloatField( clazz, fieldID);
}
jdouble GetStaticDoubleField(JNIEnv* env,jclass clazz, jfieldID fieldID)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    return functions->GetStaticDoubleField( clazz, fieldID);
}

void SetStaticObjectField(JNIEnv* env,jclass clazz, jfieldID fieldID, jobject value)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    functions->SetStaticObjectField( clazz, fieldID, value);
}
void SetStaticBooleanField(JNIEnv* env,jclass clazz, jfieldID fieldID, jboolean value)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    functions->SetStaticBooleanField( clazz, fieldID, value);
}
void SetStaticByteField(JNIEnv* env,jclass clazz, jfieldID fieldID, jbyte value)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    functions->SetStaticByteField( clazz, fieldID, value);
}
void SetStaticCharField(JNIEnv* env,jclass clazz, jfieldID fieldID, jchar value)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    functions->SetStaticCharField( clazz, fieldID, value);
}
void SetStaticShortField(JNIEnv* env,jclass clazz, jfieldID fieldID, jshort value)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    functions->SetStaticShortField( clazz, fieldID, value);
}
void SetStaticIntField(JNIEnv* env,jclass clazz, jfieldID fieldID, jint value)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    functions->SetStaticIntField( clazz, fieldID, value);
}
void SetStaticLongField(JNIEnv* env,jclass clazz, jfieldID fieldID, jlong value)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    functions->SetStaticLongField( clazz, fieldID, value);
}
void SetStaticFloatField(JNIEnv* env,jclass clazz, jfieldID fieldID, jfloat value)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    functions->SetStaticFloatField( clazz, fieldID, value);
}
void SetStaticDoubleField(JNIEnv* env,jclass clazz, jfieldID fieldID, jdouble value)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    functions->SetStaticDoubleField( clazz, fieldID, value);
}

jstring NewString(JNIEnv* env,const jchar* unicodeChars, jsize len)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    return functions->NewString( unicodeChars, len);
}

jsize GetStringLength(JNIEnv* env,jstring string)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    return functions->GetStringLength( string);
}

const jchar* GetStringChars(JNIEnv* env,jstring string, jboolean* isCopy)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    return functions->GetStringChars( string, isCopy);
}

void ReleaseStringChars(JNIEnv* env,jstring string, const jchar* chars)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    functions->ReleaseStringChars( string, chars);
}

jstring NewStringUTF(JNIEnv* env,const char* bytes)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    return functions->NewStringUTF( bytes);
}

jsize GetStringUTFLength(JNIEnv* env,jstring string)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    return functions->GetStringUTFLength( string);
}

void ReleaseStringUTFChars(JNIEnv* env,jstring string, const char* utf)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    functions->ReleaseStringUTFChars( string, utf);
}

jsize GetArrayLength(JNIEnv* env,jarray array)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    return functions->GetArrayLength( array);
}

jobjectArray NewObjectArray(JNIEnv* env,jsize length, jclass elementClass,
                            jobject initialElement)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    return functions->NewObjectArray( length, elementClass,
                                      initialElement);
}

jobject GetObjectArrayElement(JNIEnv* env,jobjectArray array, jsize index)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    return functions->GetObjectArrayElement( array, index);
}

void SetObjectArrayElement(JNIEnv* env,jobjectArray array, jsize index, jobject value)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    functions->SetObjectArrayElement( array, index, value);
}


jbooleanArray NewBooleanArray(JNIEnv* env,jsize length)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    return functions->NewBooleanArray( length);
}
jbyteArray NewByteArray(JNIEnv* env,jsize length)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    return functions->NewByteArray( length);
}
jcharArray NewCharArray(JNIEnv* env,jsize length)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    return functions->NewCharArray( length);
}
jshortArray NewShortArray(JNIEnv* env,jsize length)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    return functions->NewShortArray( length);
}
jintArray NewIntArray(JNIEnv* env,jsize length)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    return functions->NewIntArray( length);
}
jlongArray NewLongArray(JNIEnv* env,jsize length)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    return functions->NewLongArray( length);
}
jfloatArray NewFloatArray(JNIEnv* env,jsize length)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    return functions->NewFloatArray( length);
}
jdoubleArray NewDoubleArray(JNIEnv* env,jsize length)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    return functions->NewDoubleArray( length);
}
jboolean* GetBooleanArrayElements(JNIEnv* env,jbooleanArray array, jboolean* isCopy)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    return functions->GetBooleanArrayElements(array, isCopy);
}
jbyte* GetByteArrayElements(JNIEnv* env,jbyteArray array, jboolean* isCopy)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    return functions->GetByteArrayElements( array, isCopy);
}
jchar* GetCharArrayElements(JNIEnv* env,jcharArray array, jboolean* isCopy)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    return functions->GetCharArrayElements( array, isCopy);
}
jshort* GetShortArrayElements(JNIEnv* env,jshortArray array, jboolean* isCopy)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    return functions->GetShortArrayElements( array, isCopy);
}
jint* GetIntArrayElements(JNIEnv* env,jintArray array, jboolean* isCopy)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    return functions->GetIntArrayElements( array, isCopy);
}
jlong* GetLongArrayElements(JNIEnv* env,jlongArray array, jboolean* isCopy)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    return functions->GetLongArrayElements( array, isCopy);
}
jfloat* GetFloatArrayElements(JNIEnv* env,jfloatArray array, jboolean* isCopy)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    return functions->GetFloatArrayElements( array, isCopy);
}
jdouble* GetDoubleArrayElements(JNIEnv* env,jdoubleArray array, jboolean* isCopy)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    return functions->GetDoubleArrayElements( array, isCopy);
}

void ReleaseBooleanArrayElements(JNIEnv* env,jbooleanArray array, jboolean* elems,
                                 jint mode)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    functions->ReleaseBooleanArrayElements( array, elems, mode);
}
void ReleaseByteArrayElements(JNIEnv* env,jbyteArray array, jbyte* elems,
                              jint mode)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    functions->ReleaseByteArrayElements( array, elems, mode);
}
void ReleaseCharArrayElements(JNIEnv* env,jcharArray array, jchar* elems,
                              jint mode)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    functions->ReleaseCharArrayElements( array, elems, mode);
}
void ReleaseShortArrayElements(JNIEnv* env,jshortArray array, jshort* elems,
                               jint mode)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    functions->ReleaseShortArrayElements( array, elems, mode);
}
void ReleaseIntArrayElements(JNIEnv* env,jintArray array, jint* elems,
                             jint mode)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    functions->ReleaseIntArrayElements( array, elems, mode);
}
void ReleaseLongArrayElements(JNIEnv* env,jlongArray array, jlong* elems,
                              jint mode)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    functions->ReleaseLongArrayElements( array, elems, mode);
}
void ReleaseFloatArrayElements(JNIEnv* env,jfloatArray array, jfloat* elems,
                               jint mode)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    functions->ReleaseFloatArrayElements( array, elems, mode);
}
void ReleaseDoubleArrayElements(JNIEnv* env,jdoubleArray array, jdouble* elems,
                                jint mode)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    functions->ReleaseDoubleArrayElements( array, elems, mode);
}


void GetBooleanArrayRegion(JNIEnv* env,jbooleanArray array, jsize start, jsize len,
                           jboolean* buf)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    functions->GetBooleanArrayRegion( array, start, len, buf);
}
void GetByteArrayRegion(JNIEnv* env,jbyteArray array, jsize start, jsize len,
                        jbyte* buf)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    functions->GetByteArrayRegion( array, start, len, buf);
}
void GetCharArrayRegion(JNIEnv* env,jcharArray array, jsize start, jsize len,
                        jchar* buf)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    functions->GetCharArrayRegion( array, start, len, buf);
}
void GetShortArrayRegion(JNIEnv* env,jshortArray array, jsize start, jsize len,
                         jshort* buf)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    functions->GetShortArrayRegion( array, start, len, buf);
}
void GetIntArrayRegion(JNIEnv* env,jintArray array, jsize start, jsize len,
                       jint* buf)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    functions->GetIntArrayRegion( array, start, len, buf);
}
void GetLongArrayRegion(JNIEnv* env,jlongArray array, jsize start, jsize len,
                        jlong* buf)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    functions->GetLongArrayRegion( array, start, len, buf);
}
void GetFloatArrayRegion(JNIEnv* env,jfloatArray array, jsize start, jsize len,
                         jfloat* buf)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    functions->GetFloatArrayRegion( array, start, len, buf);
}
void GetDoubleArrayRegion(JNIEnv* env,jdoubleArray array, jsize start, jsize len,
                          jdouble* buf)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    functions->GetDoubleArrayRegion( array, start, len, buf);
}

void SetBooleanArrayRegion(JNIEnv* env,jbooleanArray array, jsize start, jsize len,
                           const jboolean* buf)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    functions->SetBooleanArrayRegion( array, start, len, buf);
}
void SetByteArrayRegion(JNIEnv* env,jbyteArray array, jsize start, jsize len,
                        const jbyte* buf)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    functions->SetByteArrayRegion( array, start, len, buf);
}
void SetCharArrayRegion(JNIEnv* env,jcharArray array, jsize start, jsize len,
                        const jchar* buf)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    functions->SetCharArrayRegion( array, start, len, buf);
}
void SetShortArrayRegion(JNIEnv* env,jshortArray array, jsize start, jsize len,
                         const jshort* buf)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    functions->SetShortArrayRegion( array, start, len, buf);
}
void SetIntArrayRegion(JNIEnv* env,jintArray array, jsize start, jsize len,
                       const jint* buf)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    functions->SetIntArrayRegion( array, start, len, buf);
}
void SetLongArrayRegion(JNIEnv* env,jlongArray array, jsize start, jsize len,
                        const jlong* buf)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    functions->SetLongArrayRegion( array, start, len, buf);
}
void SetFloatArrayRegion(JNIEnv* env,jfloatArray array, jsize start, jsize len,
                         const jfloat* buf)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    functions->SetFloatArrayRegion( array, start, len, buf);
}
void SetDoubleArrayRegion(JNIEnv* env,jdoubleArray array, jsize start, jsize len,
                          const jdouble* buf)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    functions->SetDoubleArrayRegion( array, start, len, buf);
}

jint RegisterNatives(JNIEnv* env,jclass clazz, const JNINativeMethod* methods,
                     jint nMethods)
{
    HOOKLOGE("RegisterNatives");
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    return functions->RegisterNatives( clazz, methods, nMethods); }

jint UnregisterNatives(JNIEnv* env,jclass clazz)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    return functions->UnregisterNatives( clazz); }

jint MonitorEnter(JNIEnv* env,jobject obj)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    return functions->MonitorEnter( obj); }

jint MonitorExit(JNIEnv* env,jobject obj)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    return functions->MonitorExit( obj); }

jint GetJavaVM(JNIEnv* env,JavaVM** vm)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    return functions->GetJavaVM( vm); }

void GetStringRegion(JNIEnv* env,jstring str, jsize start, jsize len, jchar* buf)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    functions->GetStringRegion( str, start, len, buf); }

void GetStringUTFRegion(JNIEnv* env,jstring str, jsize start, jsize len, char* buf)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    return functions->GetStringUTFRegion( str, start, len, buf); }

void* GetPrimitiveArrayCritical(JNIEnv* env,jarray array, jboolean* isCopy)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    return functions->GetPrimitiveArrayCritical( array, isCopy); }

void ReleasePrimitiveArrayCritical(JNIEnv* env,jarray array, void* carray, jint mode)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    functions->ReleasePrimitiveArrayCritical( array, carray, mode); }

const jchar* GetStringCritical(JNIEnv* env,jstring string, jboolean* isCopy)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    return functions->GetStringCritical( string, isCopy); }

void ReleaseStringCritical(JNIEnv* env,jstring string, const jchar* carray)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    functions->ReleaseStringCritical( string, carray); }

jweak NewWeakGlobalRef(JNIEnv* env,jobject obj)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    return functions->NewWeakGlobalRef( obj); }

void DeleteWeakGlobalRef(JNIEnv* env,jweak obj)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    functions->DeleteWeakGlobalRef( obj); }

jboolean ExceptionCheck(JNIEnv* env)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    return functions->ExceptionCheck(); }

jobject NewDirectByteBuffer(JNIEnv* env,void* address, jlong capacity)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    return functions->NewDirectByteBuffer( address, capacity); }

void* GetDirectBufferAddress(JNIEnv* env,jobject buf)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    return functions->GetDirectBufferAddress( buf); }

jlong GetDirectBufferCapacity(JNIEnv* env,jobject buf)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    return functions->GetDirectBufferCapacity( buf); }

/* added in JNI 1.6 */
jobjectRefType GetObjectRefType(JNIEnv* env,jobject obj)
{
    Linker_JNIEnv * linkerJniEnv = (Linker_JNIEnv *) env;
    JNIEnv * functions = linkerJniEnv->env;
    return functions->GetObjectRefType( obj); }

Linker_JNIEnv::Linker_JNIEnv(JNIEnv *env, jobject classLoader) {
    this->env = env;
    this->classLoader = classLoader;
    functions = new Linker_JNINativeInterface();
    functions->GetVersion =  GetVersion;
    functions->DefineClass =  DefineClass;
    functions->FromReflectedMethod =  FromReflectedMethod;
    functions->FromReflectedField =  FromReflectedField;
    functions->ToReflectedMethod =  ToReflectedMethod;
    functions->GetSuperclass =  GetSuperclass;
    functions->IsAssignableFrom =  IsAssignableFrom;
    functions->ToReflectedField =  ToReflectedField;
    functions->Throw =  Throw;
    functions->ThrowNew =  ThrowNew;
    functions->ExceptionOccurred =  ExceptionOccurred;
    functions->ExceptionDescribe =  ExceptionDescribe;
    functions->ExceptionClear =  ExceptionClear;
    functions->GetStringUTFChars =  GetStringUTFChars;
    functions->FindClass = FindClass;
    functions->FatalError = FatalError;
    functions->PushLocalFrame = PushLocalFrame;
    functions->PopLocalFrame = PopLocalFrame;
    functions->NewGlobalRef = NewGlobalRef;
    functions->DeleteGlobalRef = DeleteGlobalRef;
    functions->DeleteLocalRef = DeleteLocalRef;
    functions->IsSameObject = IsSameObject;
    functions->NewLocalRef = NewLocalRef;
    functions->EnsureLocalCapacity = EnsureLocalCapacity;
    functions->AllocObject = AllocObject;
    functions->NewObject = NewObject;
    functions->NewObjectV = NewObjectV;
    functions->NewObjectA = NewObjectA;
    functions->GetObjectClass = GetObjectClass;
    functions->IsInstanceOf = IsInstanceOf;
    functions->GetMethodID = GetMethodID;
    functions->CallObjectMethod = CallObjectMethod;
    functions->CallObjectMethodV = CallObjectMethodV;
    functions->CallObjectMethodA = CallObjectMethodA;
    functions->CallBooleanMethod = CallBooleanMethod;
    functions->CallBooleanMethodV = CallBooleanMethodV;
    functions->CallBooleanMethodA = CallBooleanMethodA;
    functions->CallByteMethod = CallByteMethod;
    functions->CallByteMethodV = CallByteMethodV;
    functions->CallByteMethodA = CallByteMethodA;
    functions->CallCharMethod = CallCharMethod;
    functions->CallCharMethodV = CallCharMethodV;
    functions->CallCharMethodA = CallCharMethodA;
    functions->CallShortMethod = CallShortMethod;
    functions->CallShortMethodV = CallShortMethodV;
    functions->CallShortMethodA = CallShortMethodA;
    functions->CallIntMethod = CallIntMethod;
    functions->CallIntMethodV = CallIntMethodV;
    functions->CallIntMethodA = CallIntMethodA;
    functions->CallLongMethod = CallLongMethod;
    functions->CallLongMethodV = CallLongMethodV;
    functions->CallLongMethodA = CallLongMethodA;
    functions->CallFloatMethod = CallFloatMethod;
    functions->CallFloatMethodV = CallFloatMethodV;
    functions->CallFloatMethodA = CallFloatMethodA;
    functions->CallDoubleMethod = CallDoubleMethod;
    functions->CallDoubleMethodV = CallDoubleMethodV;
    functions->CallDoubleMethodA = CallDoubleMethodA;
    functions->CallVoidMethod = CallVoidMethod;
    functions->CallVoidMethodV = CallVoidMethodV;
    functions->CallVoidMethodA = CallVoidMethodA;
    functions->CallNonvirtualObjectMethod = CallNonvirtualObjectMethod;
    functions->CallNonvirtualObjectMethodV = CallNonvirtualObjectMethodV;
    functions->CallNonvirtualObjectMethodA = CallNonvirtualObjectMethodA;
    functions->CallNonvirtualBooleanMethod = CallNonvirtualBooleanMethod;
    functions->CallNonvirtualBooleanMethodV = CallNonvirtualBooleanMethodV;
    functions->CallNonvirtualBooleanMethodA = CallNonvirtualBooleanMethodA;
    functions->CallNonvirtualByteMethod = CallNonvirtualByteMethod;
    functions->CallNonvirtualByteMethodV = CallNonvirtualByteMethodV;
    functions->CallNonvirtualByteMethodA = CallNonvirtualByteMethodA;
    functions->CallNonvirtualCharMethod = CallNonvirtualCharMethod;
    functions->CallNonvirtualCharMethodV = CallNonvirtualCharMethodV;
    functions->CallNonvirtualCharMethodA = CallNonvirtualCharMethodA;
    functions->CallNonvirtualShortMethod = CallNonvirtualShortMethod;
    functions->CallNonvirtualShortMethodV = CallNonvirtualShortMethodV;
    functions->CallNonvirtualShortMethodA = CallNonvirtualShortMethodA;
    functions->CallNonvirtualIntMethod = CallNonvirtualIntMethod;
    functions->CallNonvirtualIntMethodV = CallNonvirtualIntMethodV;
    functions->CallNonvirtualIntMethodA = CallNonvirtualIntMethodA;
    functions->CallNonvirtualLongMethod = CallNonvirtualLongMethod;
    functions->CallNonvirtualLongMethodV = CallNonvirtualLongMethodV;
    functions->CallNonvirtualLongMethodA = CallNonvirtualLongMethodA;
    functions->CallNonvirtualFloatMethod = CallNonvirtualFloatMethod;
    functions->CallNonvirtualFloatMethodV = CallNonvirtualFloatMethodV;
    functions->CallNonvirtualFloatMethodA = CallNonvirtualFloatMethodA;
    functions->CallNonvirtualDoubleMethod = CallNonvirtualDoubleMethod;
    functions->CallNonvirtualDoubleMethodV = CallNonvirtualDoubleMethodV;
    functions->CallNonvirtualDoubleMethodA = CallNonvirtualDoubleMethodA;
    functions->CallNonvirtualVoidMethod = CallNonvirtualVoidMethod;
    functions->CallNonvirtualVoidMethodV = CallNonvirtualVoidMethodV;
    functions->CallNonvirtualVoidMethodA = CallNonvirtualVoidMethodA;
    functions->GetFieldID = GetFieldID;
    functions->GetObjectField = GetObjectField;
    functions->GetBooleanField = GetBooleanField;
    functions->GetByteField = GetByteField;
    functions->GetCharField = GetCharField;
    functions->GetShortField = GetShortField;
    functions->GetIntField = GetIntField;
    functions->GetLongField = GetLongField;
    functions->GetFloatField = GetFloatField;
    functions->GetDoubleField = GetDoubleField;

    functions->SetObjectField = SetObjectField;
    functions->SetBooleanField = SetBooleanField;
    functions->SetByteField = SetByteField;
    functions->SetCharField = SetCharField;
    functions->SetShortField = SetShortField;
    functions->SetIntField = SetIntField;
    functions->SetLongField = SetLongField;
    functions->SetFloatField = SetFloatField;
    functions->SetDoubleField = SetDoubleField;

    functions->GetStaticMethodID = GetStaticMethodID;

    functions->CallStaticObjectMethod = CallStaticObjectMethod;
    functions->CallStaticObjectMethodV = CallStaticObjectMethodV;
    functions->CallStaticObjectMethodA = CallStaticObjectMethodA;
    functions->CallStaticBooleanMethod = CallStaticBooleanMethod;
    functions->CallStaticBooleanMethodV = CallStaticBooleanMethodV;
    functions->CallStaticBooleanMethodA = CallStaticBooleanMethodA;
    functions->CallStaticByteMethod = CallStaticByteMethod;
    functions->CallStaticByteMethodV = CallStaticByteMethodV;
    functions->CallStaticByteMethodA = CallStaticByteMethodA;
    functions->CallStaticCharMethod = CallStaticCharMethod;
    functions->CallStaticCharMethodV = CallStaticCharMethodV;
    functions->CallStaticCharMethodA = CallStaticCharMethodA;
    functions->CallStaticShortMethod = CallStaticShortMethod;
    functions->CallStaticShortMethodV = CallStaticShortMethodV;
    functions->CallStaticShortMethodA = CallStaticShortMethodA;
    functions->CallStaticIntMethod = CallStaticIntMethod;
    functions->CallStaticIntMethodV = CallStaticIntMethodV;
    functions->CallStaticIntMethodA = CallStaticIntMethodA;
    functions->CallStaticLongMethod = CallStaticLongMethod;
    functions->CallStaticLongMethodV = CallStaticLongMethodV;
    functions->CallStaticLongMethodA = CallStaticLongMethodA;
    functions->CallStaticFloatMethod = CallStaticFloatMethod;
    functions->CallStaticFloatMethodV = CallStaticFloatMethodV;
    functions->CallStaticFloatMethodA = CallStaticFloatMethodA;
    functions->CallStaticDoubleMethod = CallStaticDoubleMethod;
    functions->CallStaticDoubleMethodV = CallStaticDoubleMethodV;
    functions->CallStaticDoubleMethodA = CallStaticDoubleMethodA;

    functions->CallStaticVoidMethod = CallStaticVoidMethod;
    functions->CallStaticVoidMethodV = CallStaticVoidMethodV;
    functions->CallStaticVoidMethodA = CallStaticVoidMethodA;

    functions->GetStaticFieldID = GetStaticFieldID;
    functions->GetStaticObjectField = GetStaticObjectField;
    functions->GetStaticBooleanField = GetStaticBooleanField;
    functions->GetStaticCharField = GetStaticCharField;
    functions->GetStaticShortField = GetStaticShortField;
    functions->GetStaticIntField = GetStaticIntField;
    functions->GetStaticLongField = GetStaticLongField;
    functions->GetStaticFloatField = GetStaticFloatField;
    functions->GetStaticDoubleField = GetStaticDoubleField;
    functions->GetStaticByteField = GetStaticByteField;

    functions->SetStaticObjectField = SetStaticObjectField;
    functions->SetStaticBooleanField = SetStaticBooleanField;
    functions->SetStaticByteField = SetStaticByteField;
    functions->SetStaticCharField = SetStaticCharField;
    functions->SetStaticShortField = SetStaticShortField;
    functions->SetStaticIntField = SetStaticIntField;
    functions->SetStaticLongField = SetStaticLongField;
    functions->SetStaticFloatField = SetStaticFloatField;
    functions->SetStaticDoubleField = SetStaticDoubleField;

    functions->NewString = NewString;
    functions->GetStringLength = GetStringLength;
    functions->GetStringChars = GetStringChars;
    functions->ReleaseStringChars = ReleaseStringChars;
    functions->NewStringUTF = NewStringUTF;
    functions->GetStringUTFLength = GetStringUTFLength;
    functions->ReleaseStringUTFChars = ReleaseStringUTFChars;
    functions->GetArrayLength = GetArrayLength;

    functions->NewObjectArray = NewObjectArray;

    functions->GetObjectArrayElement = GetObjectArrayElement;
    functions->SetObjectArrayElement = SetObjectArrayElement;

    functions->NewBooleanArray = NewBooleanArray;
    functions->NewByteArray = NewByteArray;
    functions->NewCharArray = NewCharArray;
    functions->NewShortArray = NewShortArray;
    functions->NewIntArray = NewIntArray;
    functions->NewLongArray = NewLongArray;
    functions->NewFloatArray = NewFloatArray;
    functions->NewDoubleArray = NewDoubleArray;

    functions->GetBooleanArrayElements = GetBooleanArrayElements;
    functions->GetByteArrayElements = GetByteArrayElements;
    functions->GetCharArrayElements = GetCharArrayElements;
    functions->GetShortArrayElements = GetShortArrayElements;
    functions->GetIntArrayElements = GetIntArrayElements;
    functions->GetLongArrayElements = GetLongArrayElements;
    functions->GetFloatArrayElements = GetFloatArrayElements;
    functions->GetDoubleArrayElements = GetDoubleArrayElements;

    functions->ReleaseBooleanArrayElements = ReleaseBooleanArrayElements;
    functions->ReleaseByteArrayElements = ReleaseByteArrayElements;
    functions->ReleaseCharArrayElements = ReleaseCharArrayElements;
    functions->ReleaseShortArrayElements = ReleaseShortArrayElements;
    functions->ReleaseIntArrayElements = ReleaseIntArrayElements;
    functions->ReleaseLongArrayElements = ReleaseLongArrayElements;
    functions->ReleaseFloatArrayElements = ReleaseFloatArrayElements;
    functions->ReleaseDoubleArrayElements = ReleaseDoubleArrayElements;

    functions->GetBooleanArrayRegion = GetBooleanArrayRegion;
    functions->GetByteArrayRegion = GetByteArrayRegion;
    functions->GetCharArrayRegion = GetCharArrayRegion;
    functions->GetShortArrayRegion = GetShortArrayRegion;
    functions->GetIntArrayRegion = GetIntArrayRegion;
    functions->GetLongArrayRegion = GetLongArrayRegion;
    functions->GetFloatArrayRegion = GetFloatArrayRegion;
    functions->GetDoubleArrayRegion = GetDoubleArrayRegion;

    functions->SetBooleanArrayRegion = SetBooleanArrayRegion;
    functions->SetByteArrayRegion = SetByteArrayRegion;
    functions->SetCharArrayRegion = SetCharArrayRegion;
    functions->SetShortArrayRegion = SetShortArrayRegion;
    functions->SetIntArrayRegion = SetIntArrayRegion;
    functions->SetLongArrayRegion = SetLongArrayRegion;
    functions->SetFloatArrayRegion = SetFloatArrayRegion;
    functions->SetDoubleArrayRegion = SetDoubleArrayRegion;

    functions->RegisterNatives = RegisterNatives;
    functions->UnregisterNatives = UnregisterNatives;
    functions->MonitorEnter = MonitorEnter;
    functions->MonitorExit = MonitorExit;
    functions->GetJavaVM = GetJavaVM;
    functions->GetStringRegion = GetStringRegion;
    functions->GetStringUTFRegion = GetStringUTFRegion;
    functions->GetPrimitiveArrayCritical = GetPrimitiveArrayCritical;
    functions->ReleasePrimitiveArrayCritical = ReleasePrimitiveArrayCritical;
    functions->GetStringCritical = GetStringCritical;
    functions->ReleaseStringCritical = ReleaseStringCritical;
    functions->NewWeakGlobalRef = NewWeakGlobalRef;
    functions->DeleteWeakGlobalRef = DeleteWeakGlobalRef;
    functions->ExceptionCheck = ExceptionCheck;
    functions->NewDirectByteBuffer = NewDirectByteBuffer;
    functions->GetDirectBufferAddress = GetDirectBufferAddress;
    functions->GetDirectBufferCapacity = GetDirectBufferCapacity;
    functions->GetObjectRefType = GetObjectRefType;

}






