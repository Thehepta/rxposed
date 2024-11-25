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

#include "InjectProc.h"
using namespace std;

#define WPTEVENT(x) (x >> 16)


void ptrace_event_cb(evutil_socket_t, short, void *arg) {
    InjectProc *injectProc  = (InjectProc*)arg;
    int status;
    int pid = waitpid(-1, &status, __WALL | WNOHANG);
    if (pid == -1) {
        return;
    } else if(pid == 0){
        return;
    }
    if (pid == injectProc->getTracePid()) {

        if (WIFEXITED(status) || WIFSIGNALED(status)) {
            cout << "parent process exited" << std::endl;
            return;
        }
        if (STOPPED_WITH(status,SIGTRAP, PTRACE_EVENT_FORK)) {
            long child_pid;
            ptrace(PTRACE_GETEVENTMSG, pid, 0, &child_pid);
            cout<<"int fork monitor : "<< child_pid<<endl;
        } else if (STOPPED_WITH(status,SIGTRAP, PTRACE_EVENT_STOP) ) {
            if (ptrace(PTRACE_DETACH, pid, 0, 0) == -1)
                cout<<"failed to detach init"<<endl;
            cout<<"stop tracing init"<<endl;
            return;
        }
        if (WIFSTOPPED(status)) {
            if (WPTEVENT(status) == 0) {
                if (WSTOPSIG(status) != SIGSTOP && WSTOPSIG(status) != SIGTSTP && WSTOPSIG(status) != SIGTTIN && WSTOPSIG(status) != SIGTTOU) {
                    cout<<"inject signal sent to init: "<<WSTOPSIG(status)<<endl;
                    ptrace(PTRACE_CONT, pid, 0, WSTOPSIG(status));
                    return;
                } else {
                    cout<<"suppress stopping signal sent to init: "<< WSTOPSIG(status)<<endl;
                }
            }
            ptrace(PTRACE_CONT, pid, 0, 0);
        }
        return;
    }
    std::set<pid_t> &process = injectProc->get_Tracee_Process();
    auto state = process.find(pid);

    if (state == process.end()) {
        cout<<"new process attached:"<<pid<<endl;
        process.emplace(pid);
        ptrace(PTRACE_SETOPTIONS, pid, 0, PTRACE_O_TRACEEXEC);
        ptrace(PTRACE_CONT, pid, 0, 0);
        return;
    }else{
        cout<<"old process handle: "<<pid<<endl;
        if (STOPPED_WITH(status,SIGTRAP, PTRACE_EVENT_EXEC)){
            kill(pid, SIGSTOP);
            ptrace(PTRACE_CONT, pid, 0, 0);
            waitpid(pid, &status, __WALL);
            if (STOPPED_WITH(status,SIGSTOP, 0)) {
                if(injectProc->filter_proc(pid)){
                    ptrace(PTRACE_DETACH, pid, 0, SIGSTOP);
                    injectProc->monitor_proc(pid);
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
void clean_trace(evutil_socket_t, short, void *arg) {
    InjectProc *injectProc = (InjectProc *) arg;
    cout<<"clean_trace "<<endl;
    std::set<pid_t> &process = injectProc->get_Tracee_Process();
    for (auto pid:process){
        cout<<"clean_trace detach pid "<< pid <<endl;
        ptrace(PTRACE_DETACH, pid, nullptr, nullptr);
    }
    ptrace(PTRACE_DETACH, injectProc->getTracePid(), nullptr, nullptr);
    exit(0);
}


int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <pid>" << std::endl;
        return -1;
    }

    std::ofstream log_file("initlog");
//    std::cout.rdbuf(log_file.rdbuf());
    pid_t traced_pid = atoi(argv[1]);
    cout<<"buile time: "<<__TIMESTAMP__<<endl;
    InjectProc *injectProc = new InjectProc();
    injectProc->setTracePid(traced_pid);
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGCHLD);
    if (sigprocmask(SIG_BLOCK, &mask, nullptr) == -1) {
        cout<<"set sigprocmask"<<endl;
        return -1;
    }
    int signal_fd_ = signalfd(-1, &mask, SFD_NONBLOCK | SFD_CLOEXEC);
    if (signal_fd_ == -1) {
        cout<<"create signalfd"<<endl;
        return -1;
    }


    if (ptrace(PTRACE_SEIZE, traced_pid, 0, PTRACE_O_TRACEFORK) == -1) {
        perror("ptrace attach failed");
        return -1;
    }

    // 进入事件循环
    struct event_base *base = event_base_new();

    // 监听 SIGCHLD 信号
    struct event *ev_sigchild = event_new(base, signal_fd_,EV_READ|EV_PERSIST, ptrace_event_cb, injectProc);
    struct event *ev_clean = evsignal_new(base, SIGINT, clean_trace, injectProc);
    event_add(ev_sigchild, nullptr);
    event_add(ev_clean, nullptr);

    // 启动事件循环
    event_base_dispatch(base);

    // 释放资源
    event_free(ev_clean);
    event_free(ev_sigchild);
    event_base_free(base);

    // 分离 ptrace
    ptrace(PTRACE_DETACH, traced_pid, nullptr, nullptr);
    return 0;
}