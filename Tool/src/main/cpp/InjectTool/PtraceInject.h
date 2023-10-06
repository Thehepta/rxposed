// system lib
#include <asm/ptrace.h>
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

// user lib
#include "PtraceUtils.h"


struct Module {
    char name[256];
    uintptr_t start_address;
    uintptr_t end_address;
    uint8_t perms;
};



int map_hide(pid_t pid,char *path) {
    char maps_file_path[255];
    sprintf(maps_file_path, "/proc/%d/maps", pid);
    FILE* maps_file = fopen(maps_file_path, "r");
    if (!maps_file) {
        printf("[-][function:%s] Failed to open maps file: %s\n",__func__ ,maps_file_path);
        return -1;
    }
    char line[256];
    void *mprotect_addr = get_remote_func_addr(pid, process_libs.libc_path, (void *) mprotect);
    void *memcpy_addr = get_remote_func_addr(pid, process_libs.libc_path, (void *) memcpy);
    void *mremap_addr = get_remote_func_addr(pid, process_libs.libc_path, (void *) mremap);
    void *mmap_addr = get_mmap_address(pid);

    while (fgets(line, sizeof(line), maps_file)) {
        char address_range[256];
        char permissions[256];
        char offset[256];
        char device[256];
        char inode[256];
        char pathname[256];

        if (sscanf(line, "%s %s %s %s %s %s", address_range, permissions, offset, device, inode, pathname) == 6) {
            char start_address_str[256];
            char end_address_str[256];

            sscanf(address_range, "%[^-]-%s", start_address_str, end_address_str);

            unsigned long long start_address = strtoull(start_address_str, NULL, 16);
            unsigned long long end_address = strtoull(end_address_str, NULL, 16);

            struct Module module;
            memset(&module,0,sizeof(struct Module));
            strncpy(module.name, pathname, strlen(pathname));
            module.start_address = start_address;
            module.end_address = end_address;
            if(strncmp(module.name,path, strlen(module.name))==0){
                module.perms = 0;
                if (permissions[0] == 'r')
                    module.perms |= PROT_READ;
                if (permissions[1] == 'w')
                    module.perms |= PROT_WRITE;
                if (permissions[2] == 'x')
                    module.perms |= PROT_EXEC;
                printf("[+][function:%s] %s,perms:%d Address: %llx-%llx\n",__func__ , module.name,module.perms, module.start_address, module.end_address);
                size_t size = module.end_address - module.start_address;
                void *addr = reinterpret_cast<void *>(module.start_address);

                do {

                    struct pt_regs CurrentRegs, OriginalRegs;
                    if (ptrace_getregs(pid, &CurrentRegs) != 0){
                        break;
                    }
                    // 保存原始寄存器
                    memcpy(&OriginalRegs, &CurrentRegs, sizeof(CurrentRegs));


                    //  1.  void *copy = mmap(nullptr, size, PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
                    long mmap_parameters[6];
                    mmap_parameters[0] = NULL; // 设置为NULL表示让系统自动选择分配内存的地址
                    mmap_parameters[1] = size; // 映射内存的大小
                    mmap_parameters[2] = PROT_WRITE ; // 表示映射内存区域 可读|可写|可执行
                    mmap_parameters[3] = MAP_ANONYMOUS | MAP_PRIVATE; // 建立匿名映射
                    mmap_parameters[4] = -1; //  若需要映射文件到内存中，则为文件的fd
                    mmap_parameters[5] = 0; //文件映射偏移量

                    // 调用远程进程的mmap函数 建立远程进程的内存映射 在目标进程中为libxxx.so分配内存
                    if (ptrace_call(pid, (uintptr_t) mmap_addr, mmap_parameters, 6, &CurrentRegs) == -1) {
                        printf("[-][function:%s] Call Remote mmap Func Failed, err:%s\n", __func__ ,strerror(errno));
                        return -1;
                    }
                    void *copy = (void *)ptrace_getret(&CurrentRegs);

                    //  2 .if ((module.perms & PROT_READ) == 0) {
                    //        mprotect(addr, size, PROT_READ);
                    //    }

                    if ((module.perms & PROT_READ) == 0) {

                        long mprotect_parameters[3];
                        mprotect_parameters[0] = reinterpret_cast<long>(addr);
                        mprotect_parameters[1] = size;
                        mprotect_parameters[2] = PROT_READ;
                        if (ptrace_call(pid, (uintptr_t) mprotect_addr, mprotect_parameters, 3, &CurrentRegs) == -1) {
                            printf("[-][function:%s] Call Remote mprotect Func Failed, err:%s\n",__func__ ,strerror(errno));
                            return -1;
                        }
                    }

//     3.memcpy(copy, addr, size);

                    long memcpy_parameters[3];
                    memcpy_parameters[0] = reinterpret_cast<long>(copy);
                    memcpy_parameters[1] = reinterpret_cast<long>(addr);
                    memcpy_parameters[2] = size;
                    if (ptrace_call(pid, (uintptr_t) memcpy_addr, memcpy_parameters, 3, &CurrentRegs) == -1) {
                        printf("[-][function:%s] Call Remote memcpy Func Failed, err:%s\n", __func__ ,strerror(errno));
                        return -1;
                    }

//    4.mremap(copy, size, size, MREMAP_MAYMOVE | MREMAP_FIXED, addr);

                    long mremap_parameters[5];
                    mremap_parameters[0] = reinterpret_cast<long>(copy);
                    mremap_parameters[1] = size;
                    mremap_parameters[2] = size;
                    mremap_parameters[3] = MREMAP_MAYMOVE | MREMAP_FIXED;
                    mremap_parameters[4] = reinterpret_cast<long>(addr);
                    if (ptrace_call(pid, (uintptr_t) mremap_addr, mremap_parameters, 5, &CurrentRegs) == -1) {
                        printf("[-][function:%s] Call Remote mmap Func Failed, err:%s\n",__func__ ,strerror(errno));
                        return -1;
                    }


//      5. mprotect(addr, size, module.perms);
                    long mprotect_parameters[3];
                    mprotect_parameters[0] = reinterpret_cast<long>(addr);
                    mprotect_parameters[1] = size;
                    mprotect_parameters[2] = module.perms;
                    if (ptrace_call(pid, (uintptr_t) mprotect_addr, mprotect_parameters, 3, &CurrentRegs) == -1) {
                        printf("[-][function:%s] Call Remote mprotect Func Failed, err:%s\n",__func__ ,strerror(errno));
                        return -1;
                    }


                    if (ptrace_setregs(pid, &OriginalRegs) == -1) {
                        printf("[-][function:%s] Recover reges failed\n",__func__ );
                        return -1;
                    }

                    ptrace_getregs(pid, &CurrentRegs);
                    if (memcmp(&OriginalRegs, &CurrentRegs, sizeof(CurrentRegs)) != 0) {
                        printf("[-][function:%s] Set Regs Error\n",__func__ );
                        return -1;
                    }

                } while (false);

            }
        }
    }
    fclose(maps_file);
    return 0;
}


/**
 * @brief 通过远程直接调用dlopen/dlsym的方法ptrace注入so模块到远程进程中
 *
 * @param pid pid表示远程进程的ID
 * @param LibPath LibPath为被远程注入的so模块路径
 * @param FunctionName FunctionName为远程注入的模块后调用的函数
 * @param parameter FuncParameter指向被远程调用函数的参数（若传递字符串，需要先将字符串写入到远程进程空间中）
 * @param NumParameter NumParameter为参数的个数
 * @return int 返回0表示注入成功，返回-1表示失败
 */
int inject_remote_process(pid_t pid, char *LibPath, char *FunctionName, char *FunctionArgs){
    long parameters[6];
    // attach到目标进程
    if (ptrace_attach(pid) != 0){
        printf("[+][function:%s] ptrace_attach failed\n",__func__ );
        return -1;
    }

    /**
     * @brief 开始主要步骤
     */
    do{
        // CurrentRegs 当前寄存器
        // OriginalRegs 保存注入前寄存器
        struct pt_regs CurrentRegs, OriginalRegs;
        if (ptrace_getregs(pid, &CurrentRegs) != 0){
            break;
        }
        // 保存原始寄存器
        memcpy(&OriginalRegs, &CurrentRegs, sizeof(CurrentRegs));

        // 获取mmap函数在远程进程中的地址 以便为libxxx.so分配内存
        // 由于mmap函数在libc.so库中 为了将libxxx.so加载到目标进程中 就需要使用目标进程的mmap函数 所以需要查找到libc.so库在目标进程的起始地址
        void *mmap_addr = get_mmap_address(pid);
        printf("[+][function:%s] mmap RemoteFuncAddr:0x%lx\n",__func__ ,(uintptr_t)mmap_addr);

        // mmap映射 <-- 设置mmap的参数
        // void *mmap(void *start, size_t length, int prot, int flags, int fd, off_t offsize);
        parameters[0] = NULL; // 设置为NULL表示让系统自动选择分配内存的地址
        parameters[1] = 0x3000; // 映射内存的大小
        parameters[2] = PROT_READ | PROT_WRITE; // 表示映射内存区域 可读|可写|可执行
        parameters[3] = MAP_ANONYMOUS | MAP_PRIVATE; // 建立匿名映射
        parameters[4] = -1; //  若需要映射文件到内存中，则为文件的fd
        parameters[5] = 0; //文件映射偏移量

        // 调用远程进程的mmap函数 建立远程进程的内存映射 在目标进程中为libxxx.so分配内存
        if (ptrace_call(pid, (uintptr_t)mmap_addr, parameters, 6, &CurrentRegs) == -1){
            printf("[-][function:%s] Call Remote mmap Func Failed, err:%s\n",__func__ , strerror(errno));
            break;
        }

        // 打印一下
        printf("[+][function:%s] ptrace_call mmap success, return value=%lX, pc=%lX\n",__func__ , ptrace_getret(&CurrentRegs), ptrace_getpc(&CurrentRegs));

        // 获取mmap函数执行后的返回值，也就是内存映射的起始地址
        // 从寄存器中获取mmap函数的返回值 即申请的内存首地址
        void *RemoteMapMemoryAddr = (void *)ptrace_getret(&CurrentRegs);
        printf("[+][function:%s] Remote Process Map Memory Addr:0x%lx\n",__func__ , (uintptr_t)RemoteMapMemoryAddr);

        // 分别获取dlopen、dlsym、dlclose等函数的地址
        void *dlopen_addr, *dlsym_addr, *dlclose_addr, *dlerror_addr;
        dlopen_addr = get_dlopen_address(pid);
        dlsym_addr = get_dlsym_address(pid);
        dlclose_addr = get_dlclose_address(pid);
        dlerror_addr = get_dlerror_address(pid);

        // 打印一下
        printf("[+][function:%s] Get imports: dlopen: %x, dlsym: %x, dlclose: %x, dlerror: %x\n",__func__ , dlopen_addr, dlsym_addr, dlclose_addr, dlerror_addr);

        // 打印注入so的路径
        printf("[+][function:%s] LibPath = %s\n",__func__ , LibPath);

        // 将要加载的so库路径写入到远程进程内存空间中
        /**
         * pid  开始写入数据的地址   写入内容    写入数据大小
         */
        if (ptrace_writedata(pid, (uint8_t *) RemoteMapMemoryAddr, (uint8_t *) LibPath,strlen(LibPath) + 1) == -1) {
            printf("[-][function:%s] Write LibPath:%s to RemoteProcess error\n",__func__ , LibPath);
            break;
        }

        // 设置dlopen的参数,返回值为模块加载的地址
        // void *dlopen(const char *filename, int flag);
        parameters[0] = (uintptr_t) RemoteMapMemoryAddr; // 写入的libPath
        parameters[1] = RTLD_NOW | RTLD_GLOBAL; // dlopen的标识

        // 执行dlopen 载入so
        if (ptrace_call(pid, (uintptr_t) dlopen_addr, parameters, 2, &CurrentRegs) == -1) {
            printf("[+][function:%s] Call Remote dlopen Func Failed\n",__func__ );
            break;
        }

        // RemoteModuleAddr为远程进程加载注入模块的地址
        void *RemoteModuleAddr = (void *) ptrace_getret(&CurrentRegs);
        printf("[+][function:%s] ptrace_call dlopen success, Remote Process load module Addr:0x%lx\n",__func__ ,(long) RemoteModuleAddr);

        // dlopen 错误
        if ((long) RemoteModuleAddr == 0x0){
            printf("[-][function:%s] dlopen error\n",__func__ );
            if (ptrace_call(pid, (uintptr_t) dlerror_addr, parameters, 0, &CurrentRegs) == -1) {
                printf("[-][function:%s] Call Remote dlerror Func Failed\n",__func__ );
                break;
            }
            char *Error = (char *) ptrace_getret(&CurrentRegs);
            char LocalErrorInfo[1024] = {0};
            ptrace_readdata(pid, (uint8_t *) Error, (uint8_t *) LocalErrorInfo, 1024);
            printf("[-][function:%s] dlopen error:%s\n",__func__, LocalErrorInfo );
            break;
        }

        if(map_hide(pid,LibPath)==-1){
            printf("[-][function:%s] map_hide failed\n",__func__ );
            break;
        }


        // 判断是否传入symbols
        if (strcmp(FunctionName,"symbols") != 0){
            printf("[+][function:%s] Have func symbols is %s\n",__func__, FunctionName);
            // 传入了函数的symbols
            // 将so库中需要调用的函数名称写入到远程进程内存空间中
            if (ptrace_writedata(pid, (uint8_t *) RemoteMapMemoryAddr + strlen(LibPath) + 2,(uint8_t *) FunctionName, strlen(FunctionName) + 1) == -1) {
                printf("[-][function:%s] Write FunctionName:%s to RemoteProcess error\n",__func__, FunctionName);
                break;
            }

            // 设置dlsym的参数，返回值为远程进程内函数的地址 调用XXX功能
            // void *dlsym(void *handle, const char *symbol);
            parameters[0] = (uintptr_t) RemoteModuleAddr;
            parameters[1] = (uintptr_t) ((uint8_t *) RemoteMapMemoryAddr + strlen(LibPath) + 2);
            //调用dlsym
            if (ptrace_call(pid, (uintptr_t) dlsym_addr, parameters, 2, &CurrentRegs) == -1) {
                printf("[-][function:%s] Call Remote dlsym Func Failed\n",__func__);
                break;
            }

            // RemoteModuleFuncAddr为远程进程空间内获取的函数地址
            void *RemoteModuleFuncAddr = (void *) ptrace_getret(&CurrentRegs);
            if(RemoteModuleFuncAddr == 0){
                printf("[-][function:%s] ptrace_call dlsym failed, Remote Process ModuleFunc Addr:0x%lx\n",__func__,(uintptr_t) RemoteModuleFuncAddr);
            } else{
                printf("[+][function:%s] ptrace_call dlsym success, Remote Process ModuleFunc Addr:0x%lx\n",__func__,(uintptr_t) RemoteModuleFuncAddr);
            }
            int num_arg = 1;
            if (ptrace_writedata(pid, (uint8_t *) RemoteMapMemoryAddr,(uint8_t *) FunctionArgs, strlen(FunctionArgs) + 1) == -1) {
                printf("[-][function:%s] Write FunctionArgs:%s to RemoteProcess error\n",__func__, FunctionName);
                break;
            }
            parameters[0] = (uintptr_t) ((uint8_t *) RemoteMapMemoryAddr);
            printf("[+][function:%s] Call Function %s Arg:%d\n",__func__,FunctionName,parameters[0]);
            if (ptrace_call(pid, (uintptr_t) RemoteModuleFuncAddr, parameters,num_arg ,&CurrentRegs) == -1) {
                printf("[-][function:%s] Call Remote injected Func Failed\n",__func__);
                break;
            }
        } else {
            // 没有传入函数的symbols
            printf("[+][function:%s] No func !!\n",__func__);
        }


        if (ptrace_setregs(pid, &OriginalRegs) == -1) {
            printf("[-][function:%s] Recover reges failed\n",__func__);
            break;
        }

        printf("[+][function:%s] Recover Regs Success\n",__func__);

        ptrace_getregs(pid, &CurrentRegs);
        if (memcmp(&OriginalRegs, &CurrentRegs, sizeof(CurrentRegs)) != 0) {
            printf("[-][function:%s] Set Regs Error\n",__func__);
        }








        return 0;
    } while (false);

    // 解除attach
    ptrace_detach(pid);

    return -1;
}



// so注入所需要的一些核心数据 组成一个数据结构
struct process_inject{
    pid_t pid;
    char lib_path[1024];
    char func_symbols[1024];
    char func_args[1024];
} process_inject = {0, "", "symbols",""};

/**
 * @brief 参数处理
 * -p 目标进程pid
 * -n 目标App包名
 * -f 是否开启App
 * -so 注入的so路径
 * -func 指定启用so中的某功能
 *
 * @param argc
 * @param argv
 */
void handle_parameter(int argc, char *argv[]){
    pid_t pid = 0;
    int index = 0;
    char *pkg_name = NULL;
    char *lib_path = NULL;
    char *func_symbols = NULL;
    char *func_args=NULL;
    bool start_app_flag = false;

    while (index < argc){ // 循环判断参数

        if (strcmp("-f", argv[index]) == 0){ // 是否强制开启App
            start_app_flag = true; // 强制开启App
        }

        if (strcmp("-p", argv[index]) == 0){ // 判断是否传入pid参数
            if (index + 1 >= argc){
                printf("[-] Missing parameter -p\n");
                exit(-1);
            }
            index++;
            pid = atoi(argv[index]); // pid
        }

        if (strcmp("-n", argv[index]) == 0){ // 判断是否传入App包名
            if (index + 1 >= argc){
                printf("[-] Missing parameter -n\n");
                exit(-1);
            }
            index++;
            pkg_name = argv[index]; // 包名

            if (start_app_flag){ // 如果强制开启App参数开启
                start_app(pkg_name); // 启动App
                sleep(1);
            }
        }

        if (strcmp("-so", argv[index]) == 0){ // 判断是否传入so路径
            if (index + 1 >= argc){
                printf("[-] Missing parameter -so\n");
                exit(-1);
            }
            index++;
            lib_path = argv[index]; // so路径
        }

        if (strcmp("-symbols", argv[index]) == 0){ // 判断是否传入so路径
            if (index + 1 >= argc){
                printf("[-] Missing parameter -func\n");
                exit(-1);
            }
            index++;
            func_symbols = argv[index]; // so中的调用的函数
            if (index + 1 >= argc){
                printf("[-] function: %s not params\n",func_symbols);
            }else{
                index++;
                func_args = argv[index]; // so中的调用的函数
            }
        }

        index++;
    }

    // 开始参数处理

    // 如果有包名 则通过包名获取pid
    if (pkg_name != NULL){
        printf("[+] pkg_name is %s\n", pkg_name);
        if (get_pid_by_name(&pid, pkg_name)){
            printf("[+] get_pid_by_name pid is %d\n", pid);
        }
    }

    // 处理pid
    if (pid == 0){
        printf("[-] not found target & get_pid_by_name pid faild !\n");
        exit(0);
    } else {
        process_inject.pid = pid; // pid传给inject数据结构
    }

    // 处理so路径
    if (lib_path != NULL){ // 如果有so路径
        printf("[+] lib_path is %s\n", lib_path);
        strcpy(process_inject.lib_path, strdup(lib_path)); // 传递so路径到inject数据结构
    }

    // 处理功能名称
    if (func_symbols != NULL){ // 如果有功能名称
        printf("[+] symbols is %s\n", func_symbols);
        strcpy(process_inject.func_symbols,strdup(func_symbols)); // 传递功能名称到inject数据结构
    }
    // 处理功能名称
    if (func_args != NULL){ // 如果有功能名称
        printf("[+] func_args is %s\n", func_args);
        strcpy(process_inject.func_args,strdup(func_args)); // 传递功能名称到inject数据结构
    }
}

/**
 * @brief 初始化Inject
 *
 * @param argc
 * @param argv
 * @return int
 */
int init_inject(int argc, char *argv[]){

    // 参数处理
    handle_parameter(argc, argv);

    int selinux_status = get_selinux_status();
    printf("[+][SELinux] selinux statu：%d\n",selinux_status);

    // SELinux处理
    if (selinux_status == 1){ // 为严格模式
        printf("[+][SELinux] SELinux is Enforcing\n");
        if (set_selinux_state(0)){
            printf("[+][SELinux] Selinux has been changed to Permissive\n");
        }
    } else { // 已经为宽容模式 或者 关闭状态
        printf("[+][SELinux] SELinux is Permissive or Disabled\n");
    }
//    int re;
    int re = inject_remote_process(process_inject.pid, process_inject.lib_path, process_inject.func_symbols, process_inject.func_args);
    sleep(1);
    // 如果原SELinux状态为严格 则恢复状态

    printf("[+][SELinux] Restore selinux statu\n");
    set_selinux_state(selinux_status);
    return re;
}