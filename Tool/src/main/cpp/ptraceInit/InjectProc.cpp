//
// Created by chic on 2024/11/19.
//
// system lib
#include <asm/ptrace.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <cerrno>
#include <cstring>
#include <sys/mman.h>
#include <dlfcn.h>
#include <dirent.h>
#include <elf.h>
#include <sys/uio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/sysmacros.h>
#include <cinttypes>
#include "InjectProc.h"
#include <string>
#include <vector>
#include <array>
#include "link.h"
#include "common/logging.h"
#include "PtraceUtils.h"
using namespace std;

struct MapInfo {
    /// \brief The start address of the memory region.
    uintptr_t start;
    /// \brief The end address of the memory region.
    uintptr_t end;
    /// \brief The permissions of the memory region. This is a bit mask of the following values:
    /// - PROT_READ
    /// - PROT_WRITE
    /// - PROT_EXEC
    uint8_t perms;
    /// \brief Whether the memory region is private.
    bool is_private;
    /// \brief The offset of the memory region.
    uintptr_t offset;
    /// \brief The device number of the memory region.
    /// Major can be obtained by #major()
    /// Minor can be obtained by #minor()
    dev_t dev;
    /// \brief The inode number of the memory region.
    ino_t inode;
    /// \brief The path of the memory region.
    std::string path;

    /// \brief Scans /proc/self/maps and returns a list of \ref MapInfo entries.
    /// This is useful to find out the inode of the library to hook.
    /// \return A list of \ref MapInfo entries.
};

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

std::vector<MapInfo> Scan(const std::string& pid) {
    constexpr static auto kPermLength = 5;
    constexpr static auto kMapEntry = 7;
    std::vector<MapInfo> info;
    std::string file_name = std::string("/proc/") + pid + "/maps";
    auto maps = std::unique_ptr<FILE, decltype(&fclose)>{fopen(file_name.c_str(), "r"), &fclose};
    if (maps) {
        char *line = nullptr;
        size_t len = 0;
        ssize_t read;
        while ((read = getline(&line, &len, maps.get())) > 0) {
            line[read - 1] = '\0';
            uintptr_t start = 0;
            uintptr_t end = 0;
            uintptr_t off = 0;
            ino_t inode = 0;
            unsigned int dev_major = 0;
            unsigned int dev_minor = 0;
            std::array<char, kPermLength> perm{'\0'};
            int path_off;
            if (sscanf(line, "%" PRIxPTR "-%" PRIxPTR " %4s %" PRIxPTR " %x:%x %lu %n%*s", &start,
                       &end, perm.data(), &off, &dev_major, &dev_minor, &inode,
                       &path_off) != kMapEntry) {
                continue;
            }
            while (path_off < read && isspace(line[path_off])) path_off++;
            auto ref = MapInfo{start, end, 0, perm[3] == 'p', off,
                               static_cast<dev_t>(makedev(dev_major, dev_minor)),
                               inode, line + path_off};
            if (perm[0] == 'r') ref.perms |= PROT_READ;
            if (perm[1] == 'w') ref.perms |= PROT_WRITE;
            if (perm[2] == 'x') ref.perms |= PROT_EXEC;
            info.emplace_back(ref);

        }
        free(line);
    }
    return info;
}

bool stop_int_app_process_entry(pid_t pid){
    struct pt_regs CurrentRegs, OriginalRegs;
    if (ptrace_getregs(pid, &CurrentRegs) != 0){
        return false;
    }
    auto map = Scan(std::to_string(pid));
    auto arg = static_cast<uintptr_t>(CurrentRegs.sp);
    int argc;
    auto argv = reinterpret_cast<char **>(reinterpret_cast<uintptr_t *>(arg) + 1);
    read_proc(pid, arg, &argc, sizeof(argc));
    LOGV("argc %d", argc);
    auto envp = argv + argc + 1;
    LOGV("envp %p", envp);
    auto p = envp;
    while (true) {
        uintptr_t *buf;
        read_proc(pid, (uintptr_t) p, &buf, sizeof(buf));
        if (buf != nullptr) ++p;
        else break;
    }
    ++p;
    auto auxv = reinterpret_cast<ElfW(auxv_t) *>(p);
    auto v = auxv;
    uintptr_t entry_addr = 0;
    uintptr_t addr_of_entry_addr = 0;
    while (true) {
        ElfW(auxv_t) buf;
        read_proc(pid, (uintptr_t) v, &buf, sizeof(buf));
        if (buf.a_type == AT_ENTRY) {
            entry_addr = (uintptr_t) buf.a_un.a_val;
            addr_of_entry_addr = (uintptr_t) v + offsetof(ElfW(auxv_t), a_un);
            break;
        }
        if (buf.a_type == AT_NULL) break;
        v++;
    }
    if (entry_addr == 0) {
        LOGE("failed to get entry");
        return false;
    }

    uintptr_t break_addr = (-0x05ec1cff & ~1) | ((uintptr_t) entry_addr & 1);
    if (!write_proc(pid, (uintptr_t) addr_of_entry_addr, &break_addr, sizeof(break_addr))) return false;
    ptrace(PTRACE_CONT, pid, 0, 0);
    int status;
    wait_for_trace(pid, &status, __WALL);
    if (WIFSTOPPED(status) && WSTOPSIG(status) == SIGSEGV) {
        if (ptrace_getregs(pid, &CurrentRegs) != 0) {
            return false;
        }
        if (static_cast<uintptr_t>(CurrentRegs.pc & ~1) != (break_addr & ~1)) {
            LOGE("stopped at unknown addr %p", (void *) CurrentRegs.pc);
            return false;
        }
        // The linker has been initialized now, we can do dlopen
        LOGD("stopped at entry");

        // restore entry address
        if (!write_proc(pid, (uintptr_t) addr_of_entry_addr, &entry_addr,
                        sizeof(entry_addr)))
            return false;
        // reset pc to entry
        CurrentRegs.pc = (long) entry_addr;
        LOGD("invoke entry");
        // restore registers
        if (!ptrace_setregs(pid, &CurrentRegs)) return false;

        return true;
    }
    return false;

}

bool wait_nativePreFork(pid_t pid,char *LibPath,char *FunctionName){

    return stop_int_app_process_entry(pid);

//    struct pt_regs CurrentRegs, OriginalRegs;
//
//    memcpy(&OriginalRegs, &CurrentRegs, sizeof(CurrentRegs));
//
//    void *mmap_addr = get_mmap_address(pid);
//
//    long parameters[6];
//
//    // mmap映射 <-- 设置mmap的参数
//    // void *mmap(void *start, size_t length, int prot, int flags, int fd, off_t offsize);
//    parameters[0] = NULL; // 设置为NULL表示让系统自动选择分配内存的地址
//    parameters[1] = 0x3000; // 映射内存的大小
//    parameters[2] = PROT_READ | PROT_WRITE; // 表示映射内存区域 可读|可写|可执行
//    parameters[3] = MAP_ANONYMOUS | MAP_PRIVATE; // 建立匿名映射
//    parameters[4] = -1; //  若需要映射文件到内存中，则为文件的fd
//    parameters[5] = 0; //文件映射偏移量
//
//    // 调用远程进程的mmap函数 建立远程进程的内存映射 在目标进程中为libxxx.so分配内存
//    if (ptrace_call(pid, (uintptr_t)mmap_addr, parameters, 6, &CurrentRegs) == -1){
//        LOGD("[-][function:%s] Call Remote mmap Func Failed, err:%s\n",__func__ , strerror(errno));
//        return false;
//    }
//    // 打印一下
//    LOGD("[+][function:%s] ptrace_call mmap success, return value=%lX, pc=%lX\n",__func__ , ptrace_getret(&CurrentRegs), ptrace_getpc(&CurrentRegs));
//
//    // 获取mmap函数执行后的返回值，也就是内存映射的起始地址
//    // 从寄存器中获取mmap函数的返回值 即申请的内存首地址
//    void *RemoteMapMemoryAddr = (void *)ptrace_getret(&CurrentRegs);
//    LOGD("[+][function:%s] Remote Process Map Memory Addr:0x%lx\n",__func__ , (uintptr_t)RemoteMapMemoryAddr);
//
//
//
//    // 分别获取dlopen、dlsym、dlclose等函数的地址
//    void *dlopen_addr, *dlsym_addr, *dlclose_addr, *dlerror_addr;
//    dlopen_addr = get_dlopen_address(pid);
//    dlsym_addr = get_dlsym_address(pid);
//    dlclose_addr = get_dlclose_address(pid);
//    dlerror_addr = get_dlerror_address(pid);
//
//    // 打印一下
//    printf("[+][function:%s] Get imports: dlopen: %lx, dlsym: %lx, dlclose: %lx, dlerror: %lx\n",__func__ , dlopen_addr, dlsym_addr, dlclose_addr, dlerror_addr);
//
//    // 打印注入so的路径
//    printf("[+][function:%s] LibPath = %s\n",__func__ , LibPath);
//
//    // 将要加载的so库路径写入到远程进程内存空间中
//    /**
//     * pid  开始写入数据的地址   写入内容    写入数据大小
//     */
//    if (ptrace_writedata(pid, (uint8_t *) RemoteMapMemoryAddr, (uint8_t *) LibPath,strlen(LibPath) + 1) == -1) {
//        printf("[-][function:%s] Write LibPath:%s to RemoteProcess error\n",__func__ , LibPath);
//        return false;
//    }
//
//    // 设置dlopen的参数,返回值为模块加载的地址
//    // void *dlopen(const char *filename, int flag);
//    parameters[0] = (uintptr_t) RemoteMapMemoryAddr; // 写入的libPath
//    parameters[1] = RTLD_NOW ; // dlopen的标识                            不能使用RTLD_GLOBAL ,会导致无法dlclose 无法关闭so库
//
//    // 执行dlopen 载入so
//    if (ptrace_call(pid, (uintptr_t) dlopen_addr, parameters, 2, &CurrentRegs) == -1) {
//        printf("[+][function:%s] Call Remote dlopen Func Failed\n",__func__ );
//        return false;
//    }
//
//    // RemoteModuleAddr为远程进程加载注入模块的地址
//    void *RemoteModuleAddr = (void *) ptrace_getret(&CurrentRegs);
//    printf("[+][function:%s] ptrace_call dlopen success, Remote Process load module Addr:0x%lx\n",__func__ ,(long) RemoteModuleAddr);
//
//    // dlopen 错误
//    if ((long) RemoteModuleAddr == 0x0){
//        printf("[-][function:%s] dlopen error\n",__func__ );
//        if (ptrace_call(pid, (uintptr_t) dlerror_addr, parameters, 0, &CurrentRegs) == -1) {
//            printf("[-][function:%s] Call Remote dlerror Func Failed\n",__func__ );
//            return false;
//        }
//        char *Error = (char *) ptrace_getret(&CurrentRegs);
//        char LocalErrorInfo[1024] = {0};
//        ptrace_readdata(pid, (uint8_t *) Error, (uint8_t *) LocalErrorInfo, 1024);
//        printf("[-][function:%s] dlopen error:%s\n",__func__, LocalErrorInfo );
//        return false;
//    }
//
//    printf("[+][function:%s] Have func symbols is %s\n",__func__, FunctionName);
//    // 传入了函数的symbols
//    // 将so库中需要调用的函数名称写入到远程进程内存空间中
//    if (ptrace_writedata(pid, (uint8_t *) RemoteMapMemoryAddr + strlen(LibPath) + 2,(uint8_t *) FunctionName, strlen(FunctionName) + 1) == -1) {
//        printf("[-][function:%s] Write FunctionName:%s to RemoteProcess error\n",__func__, FunctionName);
//        return false;
//    }
//
//    // 设置dlsym的参数，返回值为远程进程内函数的地址 调用XXX功能
//    // void *dlsym(void *handle, const char *symbol);
//    parameters[0] = (uintptr_t) RemoteModuleAddr;
//    parameters[1] = (uintptr_t) ((uint8_t *) RemoteMapMemoryAddr + strlen(LibPath) + 2);
//    //调用dlsym
//    if (ptrace_call(pid, (uintptr_t) dlsym_addr, parameters, 2, &CurrentRegs) == -1) {
//        printf("[-][function:%s] Call Remote dlsym Func Failed\n",__func__);
//        return false;
//    }
//
//    // RemoteModuleFuncAddr为远程进程空间内获取的函数地址
//    void *RemoteModuleFuncAddr = (void *) ptrace_getret(&CurrentRegs);
//    if(RemoteModuleFuncAddr == 0){
//        printf("[-][function:%s] ptrace_call dlsym failed, Remote Process ModuleFunc Addr:0x%lx\n",__func__,(uintptr_t) RemoteModuleFuncAddr);
//    } else{
//        printf("[+][function:%s] ptrace_call dlsym success, Remote Process ModuleFunc Addr:0x%lx\n",__func__,(uintptr_t) RemoteModuleFuncAddr);
//    }


//        uintptr_t break_addr = (-0x05ec1cff & ~1) | ((uintptr_t) entry_addr & 1);
//        if (!write_proc(pid, (uintptr_t) addr_of_entry_addr, &break_addr, sizeof(break_addr))) return false;
//        ptrace(PTRACE_CONT, pid, 0, 0);
//        int status;
//        wait_for_trace(pid, &status, __WALL);






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

bool inject_process(pid_t pid,const char *LibPath,const char *FunctionName,const char*FunctionArgs){


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

        if (ptrace_writedata(pid, (uint8_t *) RemoteMapMemoryAddr,(uint8_t *) FunctionArgs, strlen(FunctionArgs) + 1) == -1) {
            printf("[-][function:%s] Write FunctionArgs:%s to RemoteProcess error\n",__func__, FunctionName);
            break;
        }
        parameters[1] = (uintptr_t) ((uint8_t *) RemoteMapMemoryAddr);

        printf("[+][function:%s] Call Function %s ArgAddr:0x%lx\n",__func__,FunctionName,(uintptr_t)parameters[1]);
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

}
bool InjectProc::inject_zygote64_process() {
    wait_nativePreFork(this->zygote64_pid,"libart.so","nativePreFork");
//    inject_process(this->zygote64_pid,zygote64_Inject_So.c_str(), "entry",this->requestoSocket.c_str());
    return true;
}



bool InjectProc::inject_zygote32_process() {
    inject_process(this->zygote32_pid,zygote32_Inject_So.c_str(), "entry",this->requestoSocket.c_str());
    return true;
}


