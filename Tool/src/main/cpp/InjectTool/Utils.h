// system lib
#include <asm/ptrace.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>
#include <sys/mman.h>
#include <dlfcn.h>
#include <dirent.h>
#include <elf.h>
#include <sys/uio.h>
#include <unistd.h>
#include <fcntl.h>
#include <asm/unistd.h>
#include <sys/stat.h>
#include <sys/vfs.h>
#include <sys/system_properties.h>

// 系统lib路径
struct process_libs{
    const char *libc_path;
    const char *linker_path;
    const char *libdl_path;
} process_libs = {"","",""};

// selinux状态
struct process_selinux{
    const char *selinux_mnt;
    int enforce;
} process_selinux = {nullptr, -1};

/**
 * @brief 处理各架构预定义的库文件
 */



__unused __attribute__((constructor(101)))
void handle_libs(){ // __attribute__((constructor))修饰 最先执行
    char sdk_ver[32];
    __system_property_get("ro.build.version.sdk", sdk_ver);
// 系统lib路径
#if defined(__aarch64__) || defined(__x86_64__)
    // 在安卓10(包含安卓10)以上 lib路径有所变动
    if ( atoi(sdk_ver) >=  __ANDROID_API_Q__){
        process_libs.libc_path = "/apex/com.android.runtime/lib64/bionic/libc.so";
        process_libs.linker_path = "/apex/com.android.runtime/bin/linker64";
        process_libs.libdl_path = "/apex/com.android.runtime/lib64/bionic/libdl.so";
    } else {
        process_libs.libc_path = "/system/lib64/libc.so";
        process_libs.linker_path = "/system/bin/linker64";
        process_libs.libdl_path = "/system/lib64/libdl.so";
    }
#else
    // 在安卓10(包含安卓10)以上 lib路径有所变动
    if (atoi(sdk_ver) >=  __ANDROID_API_Q__){
        process_libs.libc_path = "/apex/com.android.runtime/lib/bionic/libc.so";
        process_libs.linker_path = "/apex/com.android.runtime/bin/linker";
        process_libs.libdl_path = "/apex/com.android.runtime/lib/bionic/libdl.so";
    } else {
        process_libs.libc_path = "/system/lib/libc.so";
        process_libs.linker_path = "/system/bin/linker";
        process_libs.libdl_path = "/system/lib/libdl.so";
    }
#endif
    printf("[+] libc_path is %s\n", process_libs.libc_path);
    printf("[+] linker_path is %s\n", process_libs.linker_path);
    printf("[+] libdl_path is %s\n", process_libs.libdl_path);
    printf("[+] system libs is OK\n");
}

/**
 * @brief 处理各SELinux判断的初始化
 */
__unused __attribute__((constructor(102)))
void handle_selinux_init(){ // 执行优先级 102 切记执行优先级越低 越先执行
    // code from AOSP
    char buf[BUFSIZ], *p;
    FILE *fp = nullptr;
    struct statfs sfbuf;
    int rc;
    char *bufp;
    int exists = 0;

    if (process_selinux.selinux_mnt){ // 如果selinux_state有值了 就终止下面的行为
        return;
    }

    /* We check to see if the preferred mount point for selinux file
	 * system has a selinuxfs. */
    do {
        rc = statfs("/sys/fs/selinux", &sfbuf);
    } while (rc < 0 && errno == EINTR);
    if (rc == 0) {
        if ((uint32_t)sfbuf.f_type == (uint32_t)SELINUX_MAGIC) {
            process_selinux.selinux_mnt = strdup("/sys/fs/selinux"); // 为 selinux_mnt 赋值
            return;
        }
    }

    /* Drop back to detecting it the long way. */
    fp = fopen("/proc/filesystems", "r");
    if (!fp){
        return;
    }

    while ((bufp = fgets(buf, sizeof buf - 1, fp)) != nullptr) {
        if (strstr(buf, "selinuxfs")) {
            exists = 1;
            break;
        }
    }

    if (!exists){
        goto out;
    }

    fclose(fp);

    /* At this point, the usual spot doesn't have an selinuxfs so
	 * we look around for it */
    fp = fopen("/proc/mounts", "r");
    if (!fp){
        goto out;
    }

    while ((bufp = fgets(buf, sizeof buf - 1, fp)) != nullptr) {
        char *tmp;
        p = strchr(buf, ' ');
        if (!p){
            goto out;
        }
        p++;
        tmp = strchr(p, ' ');
        if (!tmp){
            goto out;
        }
        if (!strncmp(tmp + 1, "selinuxfs ", 10)) {
            *tmp = '\0';
            break;
        }
    }

    /* If we found something, dup it */
    if (bufp){
        process_selinux.selinux_mnt = strdup(p);
    }

    out:
    if (fp){
        fclose(fp);
    }

    return;
}

/**
 * @brief 检测SELinux是否为宽容模式 如果不是 则设置为宽容模式
 */
__unused __attribute__((constructor(103)))
void handle_selinux_detect() {
    // code from AOSP
    int fd, ret;
    char path[PATH_MAX];
    char buf[20];

    if (!process_selinux.selinux_mnt) { // selinux_mnt不为空
        errno = ENOENT;
        printf("[-] selinux_mnt is nullptr\n");
        return ;
    }

    snprintf(path, sizeof path, "%s/enforce", process_selinux.selinux_mnt);
    fd = open(path, O_RDONLY);
    if (fd < 0){ // 如果打开文件失败 那么终止
        printf("[-] Failed to open enforce\n");
        return ;
    }

    memset(buf, 0, sizeof buf);
    ret = read(fd, buf, sizeof buf - 1);
    close(fd);
    if (ret < 0){ // 如果值小于0 那么返回
        printf("[-] SELinux ret error\n");
        return ;
    }

    // 将buf写入到enforce
    if (sscanf(buf, "%d", (int*)&process_selinux.enforce) != 1){ // 如果失败 则终止
        printf("[-] sscanf error\n");
        return ;
    }
    printf("[+] handle_selinux_init is OK\n");
    return ;
}



/**
 *
 * @param value SELinux的状态值 0为宽容模式Permissive 1为严格模式Enforcing
 * @return
 */
bool set_selinux_state(int value) {
    bool succ = true;
//    int fd;
//    char path[PATH_MAX];
//    char buf[20];
//
//    if (!process_selinux.selinux_mnt) {
//        errno = ENOENT;
//        return -1;
//    }
//
//    snprintf(path, sizeof path, "%s/enforce", process_selinux.selinux_mnt);
//    fd = open(path, O_RDWR);
//    if (fd < 0)
//        return -1;
//
//    snprintf(buf, sizeof buf, "%d", (int)value);
//    int ret = write(fd, buf, strlen(buf));
//    close(fd);
//    if (ret < 0)
//        succ = false;

    char cmd[20];
    snprintf(cmd, 20, "setenforce %d", value);
    system(cmd);
    printf("[+] setlinux cmd :%s\n",cmd);
    return succ;
}

/**
 * @brief Get the pid by pkg_name
 * 成功返回 true
 * 失败返回 false
 *
 * @param pid
 * @param task_name
 * @return true
 * @return false
 */
bool get_pid_by_name(pid_t *pid, char *task_name){
    DIR *dir;
    struct dirent *ptr;
    FILE *fp;
    char filepath[50];
    char cur_task_name[50];
    char buf[1024];

    dir = opendir("/proc");
    if (NULL != dir){
        while ((ptr = readdir(dir)) != NULL){ //循环读取/proc下的每一个文件/文件夹
            //如果读取到的是"."或者".."则跳过，读取到的不是文件夹名字也跳过
            if ((strcmp(ptr->d_name, ".") == 0) || (strcmp(ptr->d_name, "..") == 0))
                continue;
            if (DT_DIR != ptr->d_type)
                continue;

            sprintf(filepath, "/proc/%s/cmdline", ptr->d_name); //生成要读取的文件的路径
            fp = fopen(filepath, "r");
            if (NULL != fp){
                if (fgets(buf, 1024 - 1, fp) == NULL){
                    fclose(fp);
                    continue;
                }
                sscanf(buf, "%s", cur_task_name);
                //如果文件内容满足要求则打印路径的名字（即进程的PID）
                if (strstr(task_name, cur_task_name)){
                    *pid = atoi(ptr->d_name);
                    return true;
                }
                fclose(fp);
            }
        }
        closedir(dir);
    }

    return false;
}

/**
 * @brief Get the app start activity object
 * 获取应用的android.intent.action.MAIN
 *
 * @param pkg_name // 获取的应用包名
 * @param start_activity_name // 存放启动的 main activity
 */
void get_app_start_activity(char *pkg_name, char *start_activity_name){
    char cmdstring[1024] = "dumpsys package ";
    char cmd_string[1024] = {0};
    char temp_file[] = "tmp_XXXXXX";

    strcat(cmdstring, pkg_name);
    int fd;

    if ((fd = mkstemp(temp_file)) == -1){
        printf("[-] create tmp file failed.\n");
    }

    sprintf(cmd_string, "%s > %s", cmdstring, temp_file);
    system(cmd_string);

    FILE *fp = fdopen(fd, "r");
    if (fp == NULL){
        printf("[-] can not load file!");
        return;
    }
    char line[1024];
    while (!feof(fp)){
        fgets(line, 1024, fp);
        if (strstr(line, "android.intent.action.MAIN")){
            fgets(line, 1024, fp);
            char *p;
            //选取第二个
            int index = 1;
            p = strtok(line, " ");
            while (p){
                if (index == 2){
                    strcpy(start_activity_name, p);
                }
                index++;
                p = strtok(NULL, " ");
            }
            break;
        }
    }
    fclose(fp);
    unlink(temp_file);
    return;
}

/**
 * @brief 输入App包名 利用 am 命令 启动App
 *
 * @param pkg_name
 */
void start_app(char *pkg_name){
    char start_activity_name[1024] = {0};
    get_app_start_activity(pkg_name, start_activity_name);
    printf("[+] app_start_activity is %s\n", start_activity_name);
    char start_cmd[1024] = "am start ";
    strcat(start_cmd, start_activity_name);
    printf("[+] %s\n", start_cmd);
    system(start_cmd);
}

/**
 * @brief 在指定进程中搜索对应模块的基址
 *
 * @param pid pid表示远程进程的ID 若为-1表示自身进程
 * @param ModuleName ModuleName表示要搜索的模块的名称
 * @return void* 返回0表示获取模块基址失败，返回非0为要搜索的模块基址
 */
void *get_module_base_addr(pid_t pid, const char *ModuleName){
    FILE *fp = NULL;
    long ModuleBaseAddr = 0;
    char szFileName[50] = {0};
    char szMapFileLine[1024] = {0};

    // 读取"/proc/pid/maps"可以获得该进程加载的模块
    if (pid < 0){
        //  枚举自身进程模块
        snprintf(szFileName, sizeof(szFileName), "/proc/self/maps");
    } else {
        snprintf(szFileName, sizeof(szFileName), "/proc/%d/maps", pid);
    }

    fp = fopen(szFileName, "r");

    if (fp != NULL){
        while (fgets(szMapFileLine, sizeof(szMapFileLine), fp)){
            if (strstr(szMapFileLine, ModuleName)){
                char *Addr = strtok(szMapFileLine, "-");
                ModuleBaseAddr = strtoul(Addr, NULL, 16);

                if (ModuleBaseAddr == 0x8000)
                    ModuleBaseAddr = 0;

                break;
            }
        }
        fclose(fp);
    }

    return (void *)ModuleBaseAddr;
}

/**
 * @brief 获取远程进程与本进程都加载的模块中函数的地址
 *
 * @param pid pid表示远程进程的ID
 * @param ModuleName ModuleName表示模块名称
 * @param LocalFuncAddr LocalFuncAddr表示本地进程中该函数的地址
 * @return void* 返回远程进程中对应函数的地址
 */
void *get_remote_func_addr(pid_t pid, const char *ModuleName, void *LocalFuncAddr){
    void *LocalModuleAddr, *RemoteModuleAddr, *RemoteFuncAddr;
    //获取本地某个模块的起始地址
    LocalModuleAddr = get_module_base_addr(-1, ModuleName);
    //获取远程pid的某个模块的起始地址
    RemoteModuleAddr = get_module_base_addr(pid, ModuleName);
    // local_addr - local_handle的值为指定函数(如mmap)在该模块中的偏移量，然后再加上remote_handle，结果就为指定函数在目标进程的虚拟地址
    RemoteFuncAddr = (void *)((uintptr_t)LocalFuncAddr - (uintptr_t)LocalModuleAddr + (uintptr_t)RemoteModuleAddr);

    printf("[+] [get_remote_func_addr] lmod=0x%lX, rmod=0x%lX, lfunc=0x%lX, rfunc=0x%lX\n", LocalModuleAddr, RemoteModuleAddr, LocalFuncAddr, RemoteFuncAddr);
    return RemoteFuncAddr;
}

