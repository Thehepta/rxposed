//
// Created by Intel on 2022/4/5.
//

#ifndef RXPOPSED_RPROCESS_H
#define RXPOPSED_RPROCESS_H


#include "tool.h"

#ifndef __aarch64__
#define APK_NATIVE_LIB "lib/arm64"

#else
#define APK_NATIVE_LIB "lib/arm"

#endif


using namespace std;

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
