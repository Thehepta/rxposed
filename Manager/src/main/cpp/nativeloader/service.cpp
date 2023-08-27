

//未使用这个文件
#include<pthread.h>
#include "include/service.h"
#include <jni.h>
#include "include/tool.h"
#include "include/elf_symbol_resolver.h"
#define SERLOG_TAG "Rxposed_Zygote"

// 调用 debug 版本方法
#define SERLOGE(...) __android_log_print(ANDROID_LOG_ERROR,SERLOG_TAG,__VA_ARGS__)
#define SERDEBUG(...) __android_log_print(ANDROID_LOG_ERROR,SERLOG_TAG,"[file %s],[line %d],[function:%s]",__FILE__, __LINE__,__func__);
#define SERLOGD(...)  __android_log_print(ANDROID_LOG_ERROR,SERLOG_TAG,__VA_ARGS__)
#define SERLOGF(...) __android_log_print(ANDROID_LOG_FATAL, SERLOG_TAG, __VA_ARGS__)



static int read_int(int fd) {
    int val;
    int len = read(fd, &val, sizeof(int));
    if (len != sizeof(int)) {
        SERLOGE("unable to read_int");

    }
    return val;
}

static void write_int(int fd, int val) {
    int written = write(fd, &val, sizeof(int));
    if (written != sizeof(int)) {
        SERLOGE("unable to write int");
    }
}


static char* read_string(int fd) {
    int len = read_int(fd);
    if (len > PATH_MAX || len < 0) {
        SERLOGE("invalid string length");
    }
    char* val = static_cast<char *>(malloc(sizeof(char) * (len + 1)));
    if (val == NULL) {
        SERLOGE("unable to malloc string");
    }
    val[len] = '\0';
    int amount = read(fd, val, len);
    if (amount != len) {
        SERLOGE("unable to read string");
    }
    return val;
}

static void write_string(int fd, char* val) {
    int len = strlen(val);
    write_int(fd, len);
    int written = write(fd, val, len);
    if (written != len) {
        SERLOGE("unable to write string");
    }
}



int fork_zero_fucks() {
    int pid = fork();
    if (pid) {
        int status;
        waitpid(pid, &status, 0);
        return pid;
    }
    else {
        if (pid = fork())
            exit(0);
        return 0;
    }
}


void* daemon_accept_handlr(void* arg) {

    JavaVM *_vm;

    jint (*Android_JNI_GetCreatedJavaVMs)(JavaVM**, jsize, jsize*) = (jint (*)(JavaVM**, jsize, jsize*))rxposed::resolve_elf_internal_symbol("/apex/com.android.art/lib64/libart.so", "JNI_GetCreatedJavaVMs");

    jsize count=0;
    int result = Android_JNI_GetCreatedJavaVMs(&_vm,sizeof(JavaVM)*10,&count);
    SERLOGE("Android_JNI_GetCreatedJavaVMs addr:%lx ret=%d",Android_JNI_GetCreatedJavaVMs,result);


    int clientfd = *(int*)arg;
    SERLOGE("daemon_accept_handlr:%d",clientfd);
    uid_t currentUid =  read_int(clientfd);
    SERDEBUG()
    std::string AUTHORITY = read_string(clientfd);
    SERDEBUG()
    std::string processName = read_string(clientfd);
    SERDEBUG()
    JNIEnv *env;
    result = _vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6);
    if (result != JNI_OK) {
        // 处理获取JNIEnv对象失败的情况
        SERLOGE("vm GetEnv failed");
    }

    int res = _vm->AttachCurrentThread(&env, nullptr);
    LOGE("AttachCurrentThread ret = %d",res);
    SERDEBUG()
    std::string Provider_call_method = "getConfig";
    std::string Provider_call_arg = "null";
    std::string appinfoList = getCurrentAppRxposedConfig(env,  AUTHORITY,processName  ,Provider_call_method,currentUid);
    SERLOGE("getCurrentAppRxposedConfig ret :%s",appinfoList.c_str());

    write_string(clientfd,(char*)appinfoList.c_str());
    close(clientfd);
    _vm->DetachCurrentThread();
    pthread_exit(NULL);
}




int service_main(std::string REQUESTOR_SOCKET)
{


    //socket
    int listenfd;

    //listenfd=socket(PF_INET,SOCK_STREAM,0);
    if((listenfd=socket(AF_LOCAL,SOCK_STREAM,0))<0)
    {
        SERLOGE("socket");
    }

    if (fcntl(listenfd, F_SETFD, FD_CLOEXEC)) {
        SERLOGE("fcntl FD_CLOEXEC");
    }

    //填充地址结构
    struct sockaddr_un servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sun_family=AF_LOCAL;
    strcpy(servaddr.sun_path+1,REQUESTOR_SOCKET.c_str());
    servaddr.sun_path[0]='\0';

    //bind 绑定listenfd和本地地址结构
    if(bind(listenfd,(struct sockaddr*)&servaddr,sizeof(servaddr))<0)
    {
        SERLOGE("bind");
    }

    if(listen(listenfd,SOMAXCONN)<0)
    {
        SERLOGE("listen");
    }


    int client;   //已连接套接字(主动)
    SERLOGE("curren pid %d\n",getpid());
    while ((client = accept(listenfd, NULL,NULL)) > 0) {
        pthread_t tid;
        pthread_create(&tid, NULL, daemon_accept_handlr, (void*)&client);
    }
//    SERDEBUG()
    //关闭套接口
    close(client);
    close(listenfd);
    return 0;
}


std::string client_main(std::string REQUESTOR_SOCKET,uid_t currentUid,std::string AUTHORITY,std::string pkgName)
{

//    fork_for_samsung();  //可以不用，也可以写这个进程接受client进程退出状态
    //socket
    int socketfd = socket(AF_LOCAL, SOCK_STREAM, 0);
    if (socketfd < 0) {
        SERLOGE("socket");
        exit(-1);
    }
    if (fcntl(socketfd, F_SETFD, FD_CLOEXEC)) {
        SERLOGE("fcntl FD_CLOEXEC");
        exit(-1);
    }
    struct sockaddr_un cliaddr;

    memset(&cliaddr, 0, sizeof(cliaddr));
    cliaddr.sun_family=AF_UNIX;
    strcpy(cliaddr.sun_path+1,REQUESTOR_SOCKET.c_str());
    cliaddr.sun_path[0]='\0';


    //客户端不需要绑定和监听
    //connect 用本地套接字连接服务器的地址
    if(connect(socketfd,(struct sockaddr*)&cliaddr,sizeof(cliaddr))<0)
        SERLOGE("connect failed");

    write_int(socketfd,currentUid);
    write_string(socketfd,(char*)AUTHORITY.c_str());
    write_string(socketfd,(char*)pkgName.c_str());
    char *ret  = read_string(socketfd);

    SERLOGE("%s",ret);
    close(socketfd);
//    printf("client exited %d\n", code);

    return "";
}
