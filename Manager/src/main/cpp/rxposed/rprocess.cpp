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




rprocess * rprocess::instance_ =nullptr;

void rprocess::setProcessInfo(char* nice_name, uid_t uid, gid_t arg_gid) {
    currentUid = uid;
    processName = strdup(nice_name);
    this->gid = arg_gid;
}

string rprocess::getCurrentAppRxposedConfig(JNIEnv* env, string providerHost_providerName , string callName, string method , uid_t currentUid) {

    DEBUG();
    jstring key = env->NewStringUTF("ModuleList");
    DEBUG();
    string uid_str = std::to_string(currentUid);
    DEBUG();
    jobject obj_bundle = getConfigByProvider(env, providerHost_providerName, callName  , method, uid_str);
    DEBUG();
    jclass Bundle_cls = env->FindClass("android/os/Bundle");
    jmethodID Bundle_getStringArrayList_method = env->GetMethodID(Bundle_cls, "getStringArrayList","(Ljava/lang/String;)Ljava/util/ArrayList;");
    jclass ArrayList_cls = env->FindClass("java/util/ArrayList");
    jmethodID ArrayList_size_method = env->GetMethodID(ArrayList_cls, "size","()I");
    jmethodID ArrayList_get_method = env->GetMethodID(ArrayList_cls, "get","(I)Ljava/lang/Object;");
    jobject config = env->CallObjectMethod(obj_bundle, Bundle_getStringArrayList_method,key);
    if(config == nullptr){
        return "";
    }
//    string bask = "base.apk";
    jint size = env->CallIntMethod(config,ArrayList_size_method);
    string appinfoList;
    for(int i=0;i<size;i++){
        jstring element = static_cast<jstring>(env->CallObjectMethod(config, ArrayList_get_method,i));
        string appinfo = env->GetStringUTFChars(element,0);
        appinfoList = appinfoList +"|"+appinfo;
    }
    return appinfoList;
}

bool rprocess::InitModuleInfo(JNIEnv *env) {

    char* buf;
    U64 ufd = 0;
    int ret = create_shared_memory("rxposed",4096,-1,buf,ufd);
    if(ret==-1){
        return false;
    }
    pid_t pid = fork();

    if (pid == 0) {
        // 子进程读取数据
        std::string Provider_call_method = "getConfig";
        std::string appinfoList = getCurrentAppRxposedConfig(env,  providerHost_providerName,processName  ,Provider_call_method,currentUid);
        memcpy(buf,appinfoList.c_str(),appinfoList.length());
        _exit(0);
    }else if (pid > 0) {
        // 等待子进程结束
        wait(NULL);
        LOGE("shared_memory appinfoList :%s len:%zu",buf, strlen(buf));
        vector<string> vectorApp = string_split(buf,"|");
        string base = "base.apk";

        for(int i=0;i<vectorApp.size();i++){
            string appinfo = vectorApp[i];
            if(appinfo.empty()){
                continue;
            }
            vector<string> info = string_split(appinfo,":");
            string pkgName = "";

            string source(info[0]);
            string Entry_class = info[1];
            string Entry_method = info[2];
            string hide = info[3];
            string argument = info[4];
            size_t startPost = source.find(base);
            string NativelibPath = info[0].replace(startPost,base.length(),APK_NATIVE_LIB);
            LOGE("source:%s",source.c_str());
            LOGE("NativelibPath:%s",NativelibPath.c_str());
            AppinfoNative* appinfoNative = new AppinfoNative(pkgName,source,NativelibPath,Entry_class,Entry_method,hide,argument);
            AppinfoNative_vec.push_back(appinfoNative);
        }
    }
    close_shared_memory(ufd,buf);
    DEBUG()
    return true;
}

void rprocess::setAuthorityInfo(const char* arg_tmp){

    vector<string> arg = string_split(arg_tmp, ":");
    AUTHORITY = arg_tmp;
    hostUid = atoi(arg[0].c_str());
    LOGE("UID: %d",hostUid);
    providerHost_pkgName=arg[1];
    LOGE("providerHost_pkgName: %s",providerHost_pkgName.c_str());
    providerHost_providerName =arg[2];
    LOGE("providerHost_providerName: %s",providerHost_providerName.c_str());
}

bool rprocess::LoadModule(JNIEnv* env){
    DEBUG();
    for (auto appinfoNativeVec : AppinfoNative_vec)
    {
        load_apk_And_Call_Class_Entry_Method(env, RxposedContext,
                                             appinfoNativeVec->source
                                             ,appinfoNativeVec->NativelibPath,
                                             appinfoNativeVec->Entry_class,
                                             appinfoNativeVec->Entry_method,
                                             appinfoNativeVec->hide
                                             appinfoNativeVec->argument
                                             );
    }

    return true;
}


bool rprocess::is_isIsolatedProcess() {
    return (currentUid >= 99000 && currentUid <= 99999)|| (currentUid >= 90000 && currentUid <= 98999);
}

bool rprocess::is_HostProcess() {
    if( hostUid == currentUid){    // 管理进程(hostUid)
        return true;
    }
    return false;

}


bool rprocess::InitEnable(JNIEnv *pEnv) {
    DEBUG()
    LOGE("hostUid = %d currentUid = %d packaName = %s gid =%d",hostUid,currentUid,processName.c_str(),gid);

    if(is_isIsolatedProcess()) {   //也不能是is_isIsolatedProcess，目前不支持
        return false;
    }
    return InitModuleInfo(pEnv);
    DEBUG()
}

const char* rprocess::getStatusAuthority() {
    return AUTHORITY.c_str();
}
bool rprocess::is_Start(JNIEnv* env, char * name) {
    DEBUG();
    RxposedContext = CreateApplicationContext(env, processName,currentUid);
    if(RxposedContext != nullptr){
        return true;
    }
    return false;

}

uint rprocess::getHostUid() {
    return hostUid;
}

