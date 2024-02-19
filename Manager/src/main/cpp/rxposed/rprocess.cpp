//
// Created by Intel on 2022/4/5.
//

#include <sys/wait.h>
#include "jni.h"
#include <sys/ipc.h>
#include <sys/shm.h>
#include "rprocess.h"
#include "android/log.h"
#include "android_shm.h"

int (*system_property_get_org)( char*, char *);

int system_property_get_call(char* name , char *value){
    int re;
    if(!strncmp(name,"rxposed_activity", strlen("rxposed_activity"))){
        re = strlen("true");
        memcpy(value, "true", strlen("true"));
    } else{
        re = system_property_get_org(name,value);
    }
    return re;
}




rprocess::rprocess() {
    processName = "null";
    currentUid = -1;
    gid = -1;

}

rprocess * rprocess::instance_ =nullptr;

void rprocess::setProcessInfo(char* nice_name, uid_t uid, gid_t gid) {
    currentUid = uid;
    processName = strdup(nice_name);
    this->gid = gid;
}

bool rprocess::InitModuleInfo() {

    char* buf;
    U64 ufd = 0;
    int ret = create_shared_memory("rxposed",4096,-1,buf,ufd);

    pid_t pid = fork();

    if (pid == 0) {
        // 子进程读取数据
        JNIEnv *env = Pre_GetEnv();
        std::string Provider_call_method = "getConfig";
        std::string appinfoList = getCurrentAppRxposedConfig(env,  AUTHORITY,processName  ,Provider_call_method,currentUid);
        LOGE("appinfoList:%s len:%d",appinfoList.c_str(),appinfoList.length());
        memcpy(buf,appinfoList.c_str(),appinfoList.length());
        _exit(0);
    }else if (pid > 0) {
        // 等待子进程结束
        wait(NULL);
        LOGE("buf:%s len:%d",buf, strlen(buf));
        vector<string> vectorApp = string_split(buf,"|");
        string bask = "base.apk";

        for(int i=0;i<vectorApp.size();i++){
            string appinfo = vectorApp[i];
            if(appinfo.empty()){
                continue;
            }
            vector<string> info = string_split(appinfo,":");
            string source(info[0]);
            string pkgName = "";
            string Entry_class = info[1];
            string Entry_method = info[2];
            string hide = info[3];
            size_t startPost = source.find(bask);
#ifdef __aarch64__
            string NativelibPath = info[0].replace(startPost,bask.length(),"lib/arm64");
#else
            string NativelibPath = vectorApp[0].replace(startPost,bask.length(),"lib/arm");
#endif
            LOGE("source:%s",source.c_str());
            LOGE("NativelibPath:%s",NativelibPath.c_str());
            AppinfoNative* appinfoNative = new AppinfoNative(pkgName,source,NativelibPath,Entry_class,Entry_method,hide);
            AppinfoNative_vec.push_back(appinfoNative);
        }
    }
    close_shared_memory(ufd,buf);
    DEBUG()
    return true;
}

void rprocess::setAUTHORITY(char* arg_tmp){

    vector<string> arg = string_split(arg_tmp, ":");
    hostUid = atoi(arg[0].c_str());
    LOGE("UID: %d",hostUid);
    providerHost_pkgName=arg[1];
    LOGE("UID: %s",providerHost_pkgName.c_str());
    AUTHORITY =arg[2];
    LOGE("AUTHORITY: %s",AUTHORITY.c_str());

}

bool rprocess::LoadModule(JNIEnv *env){
    DEBUG();
    for (auto appinfoNativeVec : AppinfoNative_vec)
    {
        load_apk_And_Call_Class_Entry_Method(env, RxposedContext, appinfoNativeVec);
    }

    return true;
}


bool rprocess::is_isIsolatedProcess() {
    DEBUG();
    return (currentUid >= 99000 && currentUid <= 99999)|| (currentUid >= 90000 && currentUid <= 98999);
}



bool rprocess::InitEnable() {
    DEBUG()
    LOGE("hostUid = %d currentUid = %d packaName = %s gid =%d",hostUid,currentUid,processName.c_str(),gid);
    if( hostUid != currentUid){    //不是rxposed 管理进程(hostUid)
        if(rprocess::GetInstance()->is_isIsolatedProcess()) {   //也不能是is_isIsolatedProcess，目前不支持
            return false;
        }
        rprocess::GetInstance()->InitModuleInfo();
        return true;
    }
    return false;
    DEBUG()
}

bool rprocess::is_Load(JNIEnv* env,char * name) {
    DEBUG();
    RxposedContext = CreateApplicationContext(env, processName,currentUid);
    if(RxposedContext != nullptr){
        return true;
    }
    return false;

}

void rprocess::setUUID(char *nice_name) {
    UUID = nice_name;
}

