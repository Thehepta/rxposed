//
// Created by chic on 2024/11/19.
//

#ifndef RXPOSED_INJECTPROC_H
#define RXPOSED_INJECTPROC_H
#include <sys/types.h>
#include <string>
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

    bool is_zygote_process(pid_t pid){
       return is_zygote64_process(pid);
    }
    bool is_zygote64_process(pid_t pid){
        if(this->zygote64_pid != -1){
            if(this->zygote64_pid == pid){
                return true;
            }
        }
        return false;
    }
    void set_zygote64_Inject_So(std::string soPath){
        this->zygote64_Inject_So = soPath;
    }

    void setRequestoSocket(std::string rs){
        this->requestoSocket = rs;
    }
    std::string getRequestoSocket(){
        return this->requestoSocket;
    }

    void set_zygote32_Inject_So(std::string soPath){
        this->zygote32_Inject_So = soPath;
    }
    bool inject_zygote64_process(std::string str);
    bool inject_zygote32_process(std::string str);
    bool is_zygote32_process(pid_t pid){
        if(this->zygote32_pid != -1){
            if(this->zygote32_pid == pid){
                return true;
            }
        }
        return false;
    }

    bool filter_zygote_proc(pid_t pid);

    // 获取单例实例的静态方法
    static InjectProc& getInstance() {
        static InjectProc instance; // 使用static保证只创建一次
        return instance;
    }

private:
    InjectProc(){
        this->zygote32_pid = -1;
        this->zygote64_pid = -1;
    }
    std::string requestoSocket;
    pid_t traced_pid;
    pid_t zygote64_pid;
    std::string zygote64_Inject_So;
    std::string zygote32_Inject_So;
    pid_t zygote32_pid;
    std::set<pid_t> Child_Process;

};


#endif //RXPOSED_INJECTPROC_H
