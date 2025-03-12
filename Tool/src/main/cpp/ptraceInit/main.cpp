#include <sys/ptrace.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <iostream>
#include <event2/event.h>
#include <csignal>
#include <sys/user.h>
#include <cstring>
#include <set>
#include <fstream>
#include <sys/signalfd.h>
#include <sys/syscall.h>
#include <bits/glibc-syscalls.h>
#include <elf.h>
#include <thread>
#include "json.hpp"
#include "InjectProc.h"
#include "common/logging.h"
#include "zygiskd.h"

using namespace std;

#define WPTEVENT(x) (x >> 16)


inline const char* sigabbrev_np(int sig) {
    if (sig > 0 && sig < NSIG) return sys_signame[sig];
    return "(unknown)";
}




[[noreturn]]
void PtraceTask(){
    InjectProc & injectProc = InjectProc::getInstance();
    pid_t tracd_pid = injectProc.getTracePid();
    ptrace(PTRACE_SEIZE, tracd_pid, 0, PTRACE_O_TRACEFORK);
    int status;
    while(true){
        int pid = waitpid(-1, &status, __WALL);
        if (tracd_pid == -1) {
            continue;
        } else if(tracd_pid == 0){
            continue;
        }
        if(tracd_pid == pid){

            if (WIFEXITED(status) || WIFSIGNALED(status)) {
                LOGE("ptrace process exited\n");
//                kill(getpid(),SIGINT);
                continue;
            }
            if (STOPPED_WITH(status,SIGTRAP, PTRACE_EVENT_FORK)) {
                long child_pid;
                ptrace(PTRACE_GETEVENTMSG, pid, 0, &child_pid);
                LOGD("int fork monitor : %ld\n",child_pid);

            } else if (STOPPED_WITH(status,SIGTRAP, PTRACE_EVENT_STOP) ) {
                if (ptrace(PTRACE_DETACH, pid, 0, 0) == -1)
                    LOGE("failed to detach init\n");
                LOGE("stop tracing init\n");
                continue;
            }

            if (WIFSTOPPED(status)) {

                if (WPTEVENT(status) == 0) {
                    if (WSTOPSIG(status) != SIGSTOP && WSTOPSIG(status) != SIGTSTP && WSTOPSIG(status) != SIGTTIN && WSTOPSIG(status) != SIGTTOU) {
                        LOGD("recv signal : %s %d\n",sigabbrev_np(WSTOPSIG(status)),WSTOPSIG(status));
                        ptrace(PTRACE_CONT, pid, 0, WSTOPSIG(status));
                        continue;
                    } else {
                        LOGD("suppress stopping signal sent to init: %s %d\n",sigabbrev_np(WSTOPSIG(status)), WSTOPSIG(status));
                    }
                }
                ptrace(PTRACE_CONT, pid, 0, 0);
            }

        } else{

            std::set<pid_t> &process = injectProc.get_Tracee_Process();
            auto state = process.find(pid);
            if (state == process.end()) {  //运行到这里说明都是子进程信号
                //子进程如果不符合条件会被PTRACE_DETACH,所以要么是新创建的子进程,要么是符合条件的子进程
                cout<<"new process attached:"<<pid<<endl;
                process.emplace(pid);
                //前面ptrace的时候,使用的是PTRACE_O_TRACEFORK,所以子进程会在调用fork以后停止,并被追踪到
                ptrace(PTRACE_SETOPTIONS, pid, 0, PTRACE_O_TRACEEXEC); //这段代码 进程会停止在exec加载完,但是还没没有执行的时候
                ptrace(PTRACE_CONT, pid, 0, 0);
                continue;
            }else{

                cout<<"old process handle: "<<pid<<endl;
                if (STOPPED_WITH(status,SIGTRAP, PTRACE_EVENT_EXEC)){
                    kill(pid, SIGSTOP);             // 信号会在进程运行起来以后接受到
                    ptrace(PTRACE_CONT, pid, 0, 0); //由于进程当前已经停止,所以先运行起来
                    waitpid(pid, &status, __WALL);
                    if (STOPPED_WITH(status,SIGSTOP, 0)) {   //这个就是接受到的信号,前面 kill(pid, SIGSTOP);  发送的
                        if(injectProc.filter_zygote_proc(pid)){
                            injectProc.inject_zygote64_process();
//                            ptrace(PTRACE_SETOPTIONS, pid, 0, PTRACE_O_TRACESYSGOOD);
//                            ptrace(PTRACE_SYSCALL, pid, 0, 0);
                            ptrace(PTRACE_DETACH, pid, 0, 0);
                        } else{
                            ptrace(PTRACE_DETACH, pid, 0, 0);
                        }
                    }
                } else {
                    cout<<"old process handle: STOPPED_WITH is not"<<endl;
                }

                process.erase(state);
                if (WIFSTOPPED(status)) {
                    cout<<"detach process "<< pid<<endl;
                    ptrace(PTRACE_DETACH, pid, 0, 0);
                }
            }
        }
    }
}


void clean_trace(int arg) {
    cout<<"clean_trace "<<endl;
    InjectProc & injectProc = InjectProc::getInstance();
    std::set<pid_t> &process = injectProc.get_Tracee_Process();
    for (auto pid:process){
        cout<<"clean_trace detach pid "<< pid <<endl;
        ptrace(PTRACE_DETACH, pid, nullptr, nullptr);
    }
    ptrace(PTRACE_DETACH, injectProc.getTracePid(), nullptr, nullptr);
    exit(0);
}
void ZygiskTask() {
    zygiskd_main(InjectProc::getInstance().getRequestoSocket().c_str());
}


int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <config_file>" << std::endl;
        return -1;
    }
    signal(SIGINT, clean_trace);

    std::ifstream f(argv[1]);
    nlohmann::json jsonData = nlohmann::json::parse(f);
    InjectProc & injectProc = InjectProc::getInstance();

    pid_t traced_pid = 1;

    injectProc.set_zygote32_Inject_So(jsonData["zygote32_Inject_So"]);
    injectProc.set_zygote64_Inject_So(jsonData["zygote64_Inject_So"]);
    injectProc.setRequestoSocket(jsonData["requestSocketPath"]);

    std::ofstream log_file("initlog");
//    std::cout.rdbuf(log_file.rdbuf());
    cout<<"buile time: "<<__TIMESTAMP__<<endl;
    injectProc.setTracePid(traced_pid);
    std::thread ptraceThread(PtraceTask);
    std::thread ZygiskThread(ZygiskTask);
    ptraceThread.join();
    ZygiskThread.join();





    return 0;
}