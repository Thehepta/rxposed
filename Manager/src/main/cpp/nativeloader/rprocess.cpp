//
// Created by Intel on 2022/4/5.
//

#include "jni.h"
#include "include/rprocess.h"
#include "android/log.h"



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

void rprocess::Init(char* nice_name,uid_t uid,gid_t gid) {
    currentUid = uid;
    processName = strdup(nice_name);
    this->gid = gid;
}




bool rprocess::InitModuleInfo() {
    JNIEnv *env = Pre_GetEnv();
    string Provider_call_method = "getConfig";
    string Provider_call_arg = "null";
    jstring key = env->NewStringUTF("ModuleList");
    string uid_str = std::to_string(currentUid);
    jobject obj_bundle = getConfigByProvider(env,  AUTHORITY,processName  ,Provider_call_method,uid_str);
    jclass Bundle_cls = env->FindClass("android/os/Bundle");
    jmethodID Bundle_getStringArrayList_method = env->GetMethodID(Bundle_cls, "getStringArrayList","(Ljava/lang/String;)Ljava/util/ArrayList;");
    jclass ArrayList_cls = env->FindClass("java/util/ArrayList");
    jmethodID ArrayList_size_method = env->GetMethodID(ArrayList_cls, "size","()I");
    jmethodID ArrayList_get_method = env->GetMethodID(ArrayList_cls, "get","(I)Ljava/lang/Object;");
    jobject config = env->CallObjectMethod(obj_bundle, Bundle_getStringArrayList_method,key);
    string bask = "base.apk";
    jint size = env->CallIntMethod(config,ArrayList_size_method);
    for(int i=0;i<size;i++){
        jstring element = static_cast<jstring>(env->CallObjectMethod(config, ArrayList_get_method,i));
        string appinfo = env->GetStringUTFChars(element,0);
        vector<string> vectorApp = string_split(appinfo,":");
        string source(vectorApp[0]);
        string pkgName = "";
        string Entry_class = vectorApp[1];
        string Entry_method = vectorApp[2];
        size_t startPost = source.find(bask);
#ifdef __aarch64__
        string NativelibPath = vectorApp[0].replace(startPost,bask.length(),"lib/arm64");
#else
        string NativelibPath = vectorApp[0].replace(startPost,bask.length(),"lib/arm");
#endif
        LOGE("source:%s",source.c_str());
        LOGE("NativelibPath:%s",NativelibPath.c_str());
        AppinfoNative* appinfoNative = new AppinfoNative(pkgName,source,NativelibPath,Entry_class,Entry_method);
        AppinfoNative_vec.push_back(appinfoNative);
    }
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
        load_apk_And_exe_Class_Method(env, RxposedContext, appinfoNativeVec);
    }

    return true;
}


bool rprocess::is_isIsolatedProcess() {
    DEBUG();

    return (currentUid >= 99000 && currentUid <= 99999)|| (currentUid >= 90000 && currentUid <= 98999);
}



bool rprocess::Enable() {
    DEBUG()
    LOGE("hostUid = %d currentUid = %d packaName = %s gid =%d",hostUid,currentUid,processName.c_str(),gid);
    if( hostUid != currentUid){
        if(rprocess::GetInstance()->is_isIsolatedProcess()) {
            return false;
        }
        rprocess::GetInstance()->InitModuleInfo();
        return true;
    } else{
//        void *system_property_get_addr = DobbySymbolResolver (nullptr, "__system_property_get");
//        DobbyHook((void *)system_property_get_addr, (void *)system_property_get_call, (void **)&system_property_get_org);
        return false;
    }


}

bool rprocess::is_Load(JNIEnv* env,char * name) {
    DEBUG();
    RxposedContext = CreateApplicationContext(env, processName);
    if(RxposedContext != nullptr){
        LoadModule(env);
        return true;
    }
    return false;

}
