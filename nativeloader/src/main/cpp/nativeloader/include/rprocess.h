//
// Created by Intel on 2022/4/5.
//

#ifndef RXPOPSED_RPROCESS_H
#define RXPOPSED_RPROCESS_H


#include "dobby.h"
#include "tool.h"


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

    void SetProcessInfo(char* nice_name, uid_t uid, gid_t gid);
    bool LoadModule(JNIEnv *env);
    bool InitModuleInfo();
    bool is_Load(JNIEnv* env,char * name);
    bool is_isIsolatedProcess();
    void setAUTHORITY(char* arg_tmp);
    vector<AppinfoNative*> AppinfoNative_vec ;
    bool Enable();

protected:
    static rprocess  *instance_; //引用性声明

private:
    rprocess(const rprocess& other);
    rprocess & operator=(const rprocess & other);
    rprocess();
    string processName;
    jobject RxposedContext;
    uid_t hostUid;
    gid_t gid;
    uid_t currentUid;
    string AUTHORITY;
    string providerHost_pkgName;
};


#endif //RXPOPSED_RPROCESS_H
