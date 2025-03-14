// system lib
#include <asm/ptrace.h>
#include <cstdio>
#include <cstdlib>
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
#include <asm/unistd.h>
#include <sys/stat.h>
#include <sys/vfs.h>
#include <sys/system_properties.h>
#include "common/logging.h"
//// 系统lib路径
//struct process_libs{
//    const char *libc_path;
//    const char *linker_path;
//    const char *libdl_path;
//} process_libs = {"","",""};


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
};



///**
// * @brief 处理各架构预定义的库文件
// */
//
//__unused __attribute__((constructor(101)))
//void handle_libs(){ // __attribute__((constructor))修饰 最先执行
//    char sdk_ver[32];
//    __system_property_get("ro.build.version.sdk", sdk_ver);
//// 系统lib路径
//#if defined(__aarch64__) || defined(__x86_64__)
//    // 在安卓10(包含安卓10)以上 lib路径有所变动
//    if ( atoi(sdk_ver) >=  __ANDROID_API_Q__){
//        process_libs.libc_path = "/apex/com.android.runtime/lib64/bionic/libc.so";
//        process_libs.linker_path = "/apex/com.android.runtime/bin/linker64";
//        process_libs.libdl_path = "/apex/com.android.runtime/lib64/bionic/libdl.so";
//    } else {
//        process_libs.libc_path = "/system/lib64/libc.so";
//        process_libs.linker_path = "/system/bin/linker64";
//        process_libs.libdl_path = "/system/lib64/libdl.so";
//    }
//#else
//    // 在安卓10(包含安卓10)以上 lib路径有所变动
//    if (atoi(sdk_ver) >=  __ANDROID_API_Q__){
//        process_libs.libc_path = "/apex/com.android.runtime/lib/bionic/libc.so";
//        process_libs.linker_path = "/apex/com.android.runtime/bin/linker";
//        process_libs.libdl_path = "/apex/com.android.runtime/lib/bionic/libdl.so";
//    } else {
//        process_libs.libc_path = "/system/lib/libc.so";
//        process_libs.linker_path = "/system/bin/linker";
//        process_libs.libdl_path = "/system/lib/libdl.so";
//    }
//#endif
//    printf("[+] libc_path is %s\n", process_libs.libc_path);
//    printf("[+] linker_path is %s\n", process_libs.linker_path);
//    printf("[+] libdl_path is %s\n", process_libs.libdl_path);
//    printf("[+] system libs is OK\n");
//}



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

    LOGD("[+][get_remote_func_addr] lmod=0x%lX, rmod=0x%lX, lfunc=0x%lX, rfunc=0x%lX\n", LocalModuleAddr, RemoteModuleAddr, LocalFuncAddr, RemoteFuncAddr);
    return RemoteFuncAddr;
}

ssize_t read_proc(int pid, uintptr_t remote_addr, uintptr_t buf, size_t len) {
    struct iovec local{
            .iov_base = (void *) buf,
            .iov_len = len
    };
    struct iovec remote{
            .iov_base = (void *) remote_addr,
            .iov_len = len
    };
    auto l = process_vm_readv(pid, &local, 1, &remote, 1, 0);
    if (l == -1) {
        PLOGE("process_vm_readv");
    } else if (static_cast<size_t>(l) != len) {
        LOGW("not fully read: %zu, excepted %zu", l, len);
    }
    return l;
}

ssize_t write_proc(int pid, uintptr_t remote_addr, uintptr_t buf, size_t len) {
    LOGV("write to remote addr %" PRIxPTR " size %zu", remote_addr, len);
    struct iovec local{
            .iov_base = (void *) buf,
            .iov_len = len
    };
    struct iovec remote{
            .iov_base = (void *) remote_addr,
            .iov_len = len
    };
    auto l = process_vm_writev(pid, &local, 1, &remote, 1, 0);
    if (l == -1) {
        PLOGE("process_vm_writev");
    } else if (static_cast<size_t>(l) != len) {
        LOGW("not fully written: %zu, excepted %zu", l, len);
    }
    return l;
}


void wait_for_trace(int pid, int* status, int flags) {
    while (true) {
        auto result = waitpid(pid, status, flags);
        if (result == -1) {
            if (errno == EINTR) {
                continue;
            } else {
                PLOGE("wait %d failed", pid);
                exit(1);
            }
        }
        if (!WIFSTOPPED(*status)) {
//            LOGE("process %d not stopped for trace: %s, exit", pid, parse_status(*status).c_str());
            exit(1);
        }
        return;
    }
}

bool ends_with(std::string_view str, std::string_view suffix) {
    return str.size() >= suffix.size() &&
           str.substr(str.size() - suffix.size()) == suffix;
}

uintptr_t ge_remote_module_base(const std::string& pid,const std::string& libName){

    constexpr static auto kPermLength = 5;
    constexpr static auto kMapEntry = 7;
    std::vector<MapInfo> info;
    std::string file_name = std::string("/proc/") + pid + "/maps";
    auto maps = std::unique_ptr<FILE, decltype(&fclose)>{fopen(file_name.c_str(), "r"), &fclose};
    uintptr_t ret_addr=0;
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

            if(ends_with(libName,line + path_off)){
                ret_addr = start;
                break;
            }
        }
        free(line);
        return ret_addr;
    }
}


std::vector<MapInfo> MapScan(const std::string& pid) {
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


void *find_module_return_addr(std::vector<MapInfo> &info, std::string_view suffix) {
    for (auto &map: info) {
        if ((map.perms & PROT_EXEC) == 0 && ends_with(map.path,suffix)) {
            return (void *) map.start;
        }
    }
    return nullptr;
}

void *find_module_base(std::vector<MapInfo> &info, std::string_view suffix) {
    for (auto &map: info) {
        if (map.offset == 0 && ends_with(map.path,suffix)) {
            return (void *) map.start;
        }
    }
    return nullptr;
}

void *find_func_addr(
        std::vector<MapInfo> &local_info,
        std::vector<MapInfo> &remote_info,
        std::string_view module,
        std::string_view func) {
    auto lib = dlopen(module.data(), RTLD_NOW);
    if (lib == nullptr) {
        LOGE("failed to open lib %s: %s", module.data(), dlerror());
        return nullptr;
    }
    auto sym = reinterpret_cast<uint8_t *>(dlsym(lib, func.data()));
    if (sym == nullptr) {
        LOGE("failed to find sym %s in %s: %s", func.data(), module.data(), dlerror());
        dlclose(lib);
        return nullptr;
    }
    LOGD("sym %s: %p", func.data(), sym);
    dlclose(lib);
    auto local_base = reinterpret_cast<uint8_t *>(find_module_base(local_info, module));
    if (local_base == nullptr) {
        LOGE("failed to find local base for module %s", module.data());
        return nullptr;
    }
    auto remote_base = reinterpret_cast<uint8_t *>(find_module_base(remote_info, module));
    if (remote_base == nullptr) {
        LOGE("failed to find remote base for module %s", module.data());
        return nullptr;
    }
    LOGD("found local base %p remote base %p", local_base, remote_base);
    auto addr = (sym - local_base) + remote_base;
    LOGD("addr %p", addr);
    return addr;
}
















