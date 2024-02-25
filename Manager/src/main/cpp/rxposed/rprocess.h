//
// Created by Intel on 2022/4/5.
//

#ifndef RXPOPSED_RPROCESS_H
#define RXPOPSED_RPROCESS_H


#include "android_util_api.h"

#ifdef __aarch64__
#define APK_NATIVE_LIB "lib/arm64"

#else
#define APK_NATIVE_LIB "lib/arm"

#endif
using namespace std;



class AppinfoNative{
public:
    std::string pkgName;
    std::string source;
    std::string NativelibPath;
    std::string Entry_class;
    std::string Entry_method;
    std::string hide;
    AppinfoNative(std::string pkgName,std::string source,std::string NativelibPath,std::string Entry_class,std::string Entry_method,std::string hide){
        this->source =source;
        this->pkgName = pkgName;
        this->Entry_class = Entry_class;
        this->Entry_method = Entry_method;
        this->NativelibPath = NativelibPath;
        this->hide = hide;

    }
};

class rprocess {
    //全局访问接口
public:


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
    }

    void setProcessInfo(char* nice_name, uid_t uid, gid_t gid);
    bool LoadModule(JNIEnv* env);
    bool InitModuleInfo(JNIEnv *env);
    bool is_Start(JNIEnv* env, char * name);
    bool is_isIsolatedProcess();
    bool is_HostProcess();
    const char* getStatusAuthority();
    uint getHostUid();
    void setAuthorityInfo(const char* arg_tmp);
    vector<AppinfoNative*> AppinfoNative_vec ;
    bool InitEnable(JNIEnv *pEnv);
    string getCurrentAppRxposedConfig(JNIEnv* env, string providerHost_providerName , string callName, string method , uid_t currentUid);


    void (*zygote_nativeSpecializeAppProcess_hook)();
    jobject (*getConfigByProvider)(JNIEnv* env, string providerHost_providerName , string callName, string method , string uid_str);


protected:
    static rprocess  *instance_; //引用性声明

private:
    rprocess(const rprocess& other);
    rprocess & operator=(const rprocess & other);
    rprocess():processName(""),currentUid(1),gid(1){};
    string processName;
    jobject RxposedContext;
    uid_t hostUid;
    gid_t gid;
    uid_t currentUid;
    string AUTHORITY;
    string providerHost_pkgName;
    string providerHost_providerName;
    string UUID;
};


#endif //RXPOPSED_RPROCESS_H
