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
    InjectProc(){
        this->zygote32_pid = -1;
        this->zygote64_pid = -1;
    }
    void setTracePid(pid_t pid){
        traced_pid = pid;
    }

    std::set<pid_t>& get_Tracee_Process(){
        return Child_Process;
    }
    pid_t getTracePid(){
        return traced_pid;
    }

    bool is_zygote64_process(pid_t pid){
        if(this->zygote64_pid != -1){
            if(this->zygote64_pid == pid){
                return true;
            }
        }
        return false;
    }
    bool is_zygote32_process(pid_t pid){
        if(this->zygote32_pid != -1){
            if(this->zygote32_pid == pid){
                return true;
            }
        }
        return false;
    }
        void monitor_proc(pid_t pid);
    bool filter_zygote_proc(pid_t pid);
private:
    pid_t traced_pid;
    pid_t zygote64_pid;
    pid_t zygote32_pid;
    std::set<pid_t> Child_Process;

};


#endif //RXPOSED_INJECTPROC_H
