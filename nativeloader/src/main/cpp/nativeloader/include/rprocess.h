//
// Created by Intel on 2022/4/5.
//

#ifndef RXPOPSED_RPROCESS_H
#define RXPOPSED_RPROCESS_H

#include <string>
#include <vector>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include "jni.h"
#include "android/log.h"
using namespace std;

#define LOG_TAG "RxposedInject"
#if RXDEBUG
// 调用 debug 版本方法
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#define DEBUG(...) __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,"[file %s],[line %d],[function:%s]",__FILE__, __LINE__,__func__);
#define LOGD(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#else
#define LOGE(...)
#define LOGD(...)
#define DEBUG(...)
#endif


class rprocess {
    //全局访问接口
public:

    class AppinfoNative{
    public:
        string pkgName;
        string source;
        string NativelibPath;
        string Entry_class;
        string Entry_method;
        AppinfoNative(string pkgName,string source,string NativelibPath,string Entry_class,string Entry_method){
            this->source =source;
            this->pkgName = pkgName;
            this->Entry_class = Entry_class;
            this->Entry_method = Entry_method;
            this->NativelibPath = NativelibPath;

        }
    };



    static rprocess *GetInstance()
    {
        if( instance_ == nullptr )
        {
            instance_ = new rprocess();
        }
        return instance_;
    }

    ~rprocess()
    {
//        cout << "~Singleton"<< endl;
    }

    bool Init(JNIEnv *env, jstring name);
    AppinfoNative *GetAppInfoNative(JNIEnv *env, string pkgName, string Entry_class, string Entry_method);
    bool LoadFramework(JNIEnv *env);
    jobject getRxposedContext(JNIEnv *env);
    jobject getApplicationContext(JNIEnv *env,string pkgName);
    bool GetConfigByProvider(JNIEnv *env);
    bool is_providerHostProcess(JNIEnv *env, jstring pJstring);
    bool is_isIsolatedProcess();
    void load_apk_And_exe_Class_Method(JNIEnv *pEnv, AppinfoNative *appinfo);


    void setAUTHORITY(char* tmp);
    void setProcessName(char* tmp);
    void setRxposedContext(jobject RxposedContext);
    vector<AppinfoNative*> AppinfoNative_vec ;

protected:
    static rprocess  *instance_; //引用性声明

private:
    rprocess(const rprocess& other);
    rprocess & operator=(const rprocess & other);
    rprocess();
    string processName;
    jobject RxposedContext;
    uid_t currentUid;
    string AUTHORITY;
    string providerHost_pkgName;


    jobject getSystemContext(JNIEnv *pEnv);

    bool NDK_ExceptionCheck(JNIEnv *pEnv, const char *string);
};


#endif //RXPOPSED_RPROCESS_H
