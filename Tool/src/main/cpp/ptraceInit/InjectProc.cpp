//
// Created by chic on 2024/11/19.
//

#include <unistd.h>
#include "InjectProc.h"
#include "string"
#include "iostream"
#include <fstream>
#include <linux/ptrace.h>
#include <sys/ptrace.h>
#include <sys/wait.h>

using namespace std;
std::string get_program(int pid) {
    std::string path = "/proc/";
    path += std::to_string(pid);
    path += "/exe";
    constexpr const auto SIZE = 256;
    char buf[SIZE + 1];
    auto sz = readlink(path.c_str(), buf, SIZE);
    if (sz == -1) {
        printf("readlink /proc/%d/exe", pid);
        return "";
    }
    buf[sz] = 0;
    return buf;
}

bool InjectProc::filter_proc(pid_t pid){
    auto program = get_program(pid);
    if(program =="/system/bin/app_process64"){
        return true;
    } else if(program =="/system/bin/app_process32"){
        return true;
    }
    return false;
}


void wait_for_trace(int pid, int* status, int flags) {
    while (true) {
        auto result = waitpid(pid, status, flags);
        if (result == -1) {
            if (errno == EINTR) {
                continue;
            } else {
                cout<<"wait"<<pid<<"failed"<<endl;
                exit(1);
            }
        }
        if (!WIFSTOPPED(*status)) {
            cout<<"process"<<pid<<"not stopped for trace"<<endl;
            exit(1);
        }
        return;
    }
}


void handle_process(pid_t pid){
    int status;
    std::string file_name = std::to_string(pid)+".log";

    std::ofstream log_file(file_name);
    std::cout.rdbuf(log_file.rdbuf());
    if (ptrace(PTRACE_SEIZE, pid, 0, PTRACE_O_EXITKILL) == -1) {
        std::cout<<"PTRACE_SEIZE failed"<<std::endl;
    }
    wait_for_trace(pid, &status, __WALL);
    if (STOPPED_WITH(status,SIGSTOP, PTRACE_EVENT_STOP)) {
//        string lib_path =  "libzygisk.so";
//        if (!inject_on_main(pid, lib_path.c_str())) {
//            printf("failed to inject");
//            return ;
//        }
        std::cout<<"inject done, continue process"<<endl;
        if (kill(pid, SIGCONT)) {
            std::cout<<"kill"<<endl;
            return;
        }
        if (ptrace(PTRACE_CONT, pid, 0, 0) == -1) {
            std::cout<<"cont"<<endl;
            return ;
        }
        wait_for_trace(pid, &status, __WALL);
        if (STOPPED_WITH(status,SIGTRAP, PTRACE_EVENT_STOP)) {
            if (ptrace(PTRACE_CONT, pid, 0, 0) == -1) {
                std::cout<<"cont"<<endl;
                return ;
            }
            wait_for_trace(pid, &status, __WALL);
            if (STOPPED_WITH(status,SIGCONT, 0)) {
                std::cout<<"received SIGCONT"<<endl;

                ptrace(PTRACE_DETACH, pid, 0, SIGCONT);
            }
        } else {
            std::cout<<"nknown state,not SIGTRAP + EVENT_STOP"<<endl;

//            LOGE("unknown state %s, not SIGTRAP + EVENT_STOP", parse_status(status).c_str());
            ptrace(PTRACE_DETACH, pid, 0, 0);
            return ;
        }
    } else {
        std::cout<<"unknown state , not SIGSTOP + EVENT_STOP"<<endl;
        ptrace(PTRACE_DETACH, pid, 0, 0);
        return ;
    }

}



void InjectProc::monitor_proc(pid_t pid){
    pid_t trace_pid = fork();
    if(trace_pid == 0){
        handle_process(pid);

    }
}


