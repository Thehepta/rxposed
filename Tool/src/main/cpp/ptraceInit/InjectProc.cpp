//
// Created by chic on 2024/11/19.
//

#include <unistd.h>
#include "InjectProc.h"
#include <string>
#include "iostream"
#include <fstream>
#include <linux/ptrace.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <sys/signalfd.h>
#include "utils.hpp"
#include <vector>
#include <dlfcn.h>

using namespace std;

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


    struct user_regs_struct regs{}, backup{};

    if (!get_regs(pid, regs)) return false;
    memcpy(&backup, &regs, sizeof(regs));
    auto map = MapInfo::Scan(std::to_string(pid));
    auto local_map = MapInfo::Scan();
    auto libc_return_addr = find_module_return_addr(map, "libc.so");
    LOGD("libc return addr %p", libc_return_addr);

    // call dlopen
    auto dlopen_addr = find_func_addr(local_map, map, "libdl.so", "dlopen");
    if (dlopen_addr == nullptr) return false;
    std::vector<long> args;
    auto str = push_string(pid, regs, LibPath);
    args.clear();
    args.push_back((long) str);
    args.push_back((long) RTLD_NOW);
    auto remote_handle = remote_call(pid, regs, (uintptr_t) dlopen_addr, (uintptr_t) libc_return_addr, args);
    LOGD("remote handle %p", (void *) remote_handle);
    if (remote_handle == 0) {
        LOGE("handle is null");
        // call dlerror
        auto dlerror_addr = find_func_addr(local_map, map, "libdl.so", "dlerror");
        if (dlerror_addr == nullptr) {
            LOGE("find dlerror");
            return false;
        }
        args.clear();
        auto dlerror_str_addr = remote_call(pid, regs, (uintptr_t) dlerror_addr, (uintptr_t) libc_return_addr, args);
        LOGD("dlerror str %p", (void*) dlerror_str_addr);
        if (dlerror_str_addr == 0) return false;
        auto strlen_addr = find_func_addr(local_map, map, "libc.so", "strlen");
        if (strlen_addr == nullptr) {
            LOGE("find strlen");
            return false;
        }
        args.clear();
        args.push_back(dlerror_str_addr);
        auto dlerror_len = remote_call(pid, regs, (uintptr_t) strlen_addr, (uintptr_t) libc_return_addr, args);
        if (dlerror_len <= 0) {
            LOGE("dlerror len <= 0");
            return false;
        }
        std::string err;
        err.resize(dlerror_len + 1, 0);
//        read_p
        read_proc(pid, (uintptr_t) dlerror_str_addr, (void*)err.data(), (size_t)dlerror_len);
        LOGE("dlerror info %s", err.c_str());
        return false;
    }

    // call dlsym(handle, "entry")
    auto dlsym_addr = find_func_addr(local_map, map, "libdl.so", "dlsym");
    if (dlsym_addr == nullptr) return false;
    args.clear();
    str = push_string(pid, regs, FunctionName);
    args.push_back(remote_handle);
    args.push_back((long) str);
    auto injector_entry = remote_call(pid, regs, (uintptr_t) dlsym_addr, (uintptr_t) libc_return_addr, args);
    LOGD("injector entry %p", (void*) injector_entry);
    if (injector_entry == 0) {
        LOGE("injector entry is null");
        return false;
    }

    // call injector entry(handle, path)
    args.clear();
    args.push_back(remote_handle);
    str = push_string(pid, regs, "");
    args.push_back((long) str);
    remote_call(pid, regs, injector_entry, (uintptr_t) libc_return_addr, args);

    // reset pc to entry
    LOGD("invoke entry");
    // restore registers
    if (!set_regs(pid, backup)) return false;




}
bool InjectProc::inject_zygote64_process() {
    inject_process(this->zygote64_pid,zygote64_Inject_So.c_str(), "entry");
}



bool InjectProc::inject_zygote32_process() {
    inject_process(this->zygote64_pid,zygote64_Inject_So.c_str(), "entry");
}


