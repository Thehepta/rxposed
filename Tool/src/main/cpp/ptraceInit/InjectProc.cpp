//
// Created by chic on 2024/11/19.
//
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
#include "InjectProc.h"
#include <string>
#include "logging.h"
#include "PtraceUtils.h"
using namespace std;


std::string get_program(int pid) {
    std::string path = "/proc/";
    path += std::to_string(pid);
    path += "/exe";
    constexpr const auto SIZE = 256;
    char buf[SIZE + 1];
    auto sz = readlink(path.c_str(), buf, SIZE);
    if (sz == -1) {
        PLOGE("readlink /proc/%d/exe", pid);
        return "";
    }
    buf[sz] = 0;
    return buf;
}



bool InjectProc::filter_zygote_proc(pid_t pid){
    auto program = get_program(pid);
    LOGD("filter_zygote_proc %s", program.c_str());
    if(program =="/system/bin/app_process64"){
        this->zygote64_pid = pid;
        return true;
    }
//    else if(program =="/system/bin/app_process32"){
//        this->zygote32_pid = pid;
//        return true;
//    }
    return false;
}

bool inject_process(pid_t pid,const char *LibPath,const char *FunctionName){


    do{
        long parameters[6];

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
        LOGD("[+][function:%s] mmap RemoteFuncAddr:0x%lx\n",__func__ ,(uintptr_t)mmap_addr);

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
            LOGD("[-][function:%s] Call Remote mmap Func Failed, err:%s\n",__func__ , strerror(errno));
            break;
        }
        // 打印一下
        LOGD("[+][function:%s] ptrace_call mmap success, return value=%lX, pc=%lX\n",__func__ , ptrace_getret(&CurrentRegs), ptrace_getpc(&CurrentRegs));

        // 获取mmap函数执行后的返回值，也就是内存映射的起始地址
        // 从寄存器中获取mmap函数的返回值 即申请的内存首地址
        void *RemoteMapMemoryAddr = (void *)ptrace_getret(&CurrentRegs);
        LOGD("[+][function:%s] Remote Process Map Memory Addr:0x%lx\n",__func__ , (uintptr_t)RemoteMapMemoryAddr);

        // 分别获取dlopen、dlsym、dlclose等函数的地址
        void *dlopen_addr, *dlsym_addr, *dlclose_addr, *dlerror_addr;
        dlopen_addr = get_dlopen_address(pid);
        dlsym_addr = get_dlsym_address(pid);
        dlclose_addr = get_dlclose_address(pid);
        dlerror_addr = get_dlerror_address(pid);

        // 打印一下
        printf("[+][function:%s] Get imports: dlopen: %lx, dlsym: %lx, dlclose: %lx, dlerror: %lx\n",__func__ , dlopen_addr, dlsym_addr, dlclose_addr, dlerror_addr);

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
        parameters[1] = RTLD_NOW ; // dlopen的标识                            不能使用RTLD_GLOBAL ,会导致无法dlclose 无法关闭so库

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

        int num_arg = 2;
        printf("[+][function:%s] Call Function %s ArgAddr:0x%lx\n",__func__,FunctionName,(uintptr_t)parameters[0]);
        if (ptrace_call(pid, (uintptr_t) RemoteModuleFuncAddr, parameters,num_arg ,&CurrentRegs) == -1) {
            printf("[-][function:%s] Call Remote injected Func Failed\n",__func__);
            break;
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

    }while(false);



    return true;








































//    struct user_regs_struct regs{}, backup{};

//    if (!get_regs(pid, regs)) return false;
//    memcpy(&backup, &regs, sizeof(regs));
//    auto map = MapInfo::Scan(std::to_string(pid));
//    auto local_map = MapInfo::Scan();
//    auto libc_return_addr = find_module_return_addr(map, "libc.so");
//    LOGD("libc return addr %p", libc_return_addr);
//
//    // call dlopen
//    auto dlopen_addr = find_func_addr(local_map, map, "libdl.so", "dlopen");
//    if (dlopen_addr == nullptr) return false;

//    std::vector<long> args;
//    auto str = push_string(pid, regs, LibPath);
//    args.clear();
//    args.push_back((long) str);
//    args.push_back((long) RTLD_NOW);
//    auto remote_handle = remote_call(pid, regs, (uintptr_t) dlopen_addr, (uintptr_t) libc_return_addr, args);
//    LOGD("remote handle %p", (void *) remote_handle);
//    if (remote_handle == 0) {
//        LOGE("handle is null");
//        // call dlerror
//        auto dlerror_addr = find_func_addr(local_map, map, "libdl.so", "dlerror");
//        if (dlerror_addr == nullptr) {
//            LOGE("find dlerror");
//            return false;
//        }
//        args.clear();
//        auto dlerror_str_addr = remote_call(pid, regs, (uintptr_t) dlerror_addr, (uintptr_t) libc_return_addr, args);
//        LOGD("dlerror str %p", (void*) dlerror_str_addr);
//        if (dlerror_str_addr == 0) return false;
//        auto strlen_addr = find_func_addr(local_map, map, "libc.so", "strlen");
//        if (strlen_addr == nullptr) {
//            LOGE("find strlen");
//            return false;
//        }
//        args.clear();
//        args.push_back(dlerror_str_addr);
//        auto dlerror_len = remote_call(pid, regs, (uintptr_t) strlen_addr, (uintptr_t) libc_return_addr, args);
//        if (dlerror_len <= 0) {
//            LOGE("dlerror len <= 0");
//            return false;
//        }
//        std::string err;
//        err.resize(dlerror_len + 1, 0);
////        read_p
//        read_proc(pid, (uintptr_t) dlerror_str_addr, (void*)err.data(), (size_t)dlerror_len);
//        LOGE("dlerror info %s", err.c_str());
//        return false;
//    }
//
//    // call dlsym(handle, "entry")
//    auto dlsym_addr = find_func_addr(local_map, map, "libdl.so", "dlsym");
//    if (dlsym_addr == nullptr) return false;
//    args.clear();
//    str = push_string(pid, regs, FunctionName);
//    args.push_back(remote_handle);
//    args.push_back((long) str);
//    auto injector_entry = remote_call(pid, regs, (uintptr_t) dlsym_addr, (uintptr_t) libc_return_addr, args);
//    LOGD("injector entry %p", (void*) injector_entry);
//    if (injector_entry == 0) {
//        LOGE("injector entry is null");
//        return false;
//    }
//
//    // call injector entry(handle, path)
//    args.clear();
//    args.push_back(remote_handle);
//    str = push_string(pid, regs, "");
//    args.push_back((long) str);
//    remote_call(pid, regs, injector_entry, (uintptr_t) libc_return_addr, args);







//    // reset pc to entry
//    LOGD("invoke entry");
//    // restore registers
//    if (!set_regs(pid, backup)) return false;




}
bool InjectProc::inject_zygote64_process() {
    inject_process(this->zygote64_pid,zygote64_Inject_So.c_str(), "entry");
    return true;
}



bool InjectProc::inject_zygote32_process() {
    inject_process(this->zygote64_pid,zygote64_Inject_So.c_str(), "entry");
    return true;
}


