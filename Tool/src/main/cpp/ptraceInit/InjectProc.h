//
// Created by chic on 2024/11/19.
//

#ifndef RXPOSED_INJECTPROC_H
#define RXPOSED_INJECTPROC_H
#include <sys/types.h>
#include <set>

#define STOPPED_WITH(status,sig, event) WIFSTOPPED(status) && (status >> 8 == ((sig) | (event << 8)))

class InjectProc {

public:
    void setTracePid(pid_t pid){
        traced_pid = pid;
    }

    std::set<pid_t>& get_Tracee_Process(){
        return Child_Process;
    }
    pid_t getTracePid(){
        return traced_pid;
    }

    bool findPid(pid_t pid){
        auto state = Child_Process.find(pid);
        if (state == Child_Process.end()) {
            return true;
        }

        return false;
    }
    void monitor_proc(pid_t pid);
    bool filter_proc(pid_t pid);
private:
    pid_t traced_pid;
    std::set<pid_t> Child_Process;

};


#endif //RXPOSED_INJECTPROC_H
