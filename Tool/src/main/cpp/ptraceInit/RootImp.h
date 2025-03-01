//
// Created by chic on 2024/12/4.
//

#ifndef ZYGISKNEXT_ROOTIMP_H
#define ZYGISKNEXT_ROOTIMP_H




// 枚举类型定义
typedef enum {
    VERSION_SUPPORTED,
    VERSION_TOO_OLD,
    VERSION_ABNORMAL,
    VERSION_NONE
} Version;



enum : uint32_t {
    PROCESS_GRANTED_ROOT = (1u << 0),
    PROCESS_ON_DENYLIST =  (1u << 1),

    PROCESS_IS_MANAGER = (1u << 28),
    PROCESS_ROOT_IS_KSU = (1u << 29),
    PROCESS_ROOT_IS_MAGISK = (1u << 30),
    PROCESS_IS_SYS_UI = (1u << 31),

    PRIVATE_MASK = PROCESS_IS_SYS_UI
};



class RootImp {


public:
    int getProcessFlags(uid_t uid);
    bool uid_granted_root(uid_t uid) ;
    bool is_kernel_su_manager(uid_t uid);
    bool is_magisk_manager(uid_t uid);
    int get_kernel_su();
    int get_magisk();
    int is_command_available(const char *command);

    static RootImp& getInstance(){
        static RootImp instance;
        return instance;
    }

    RootImp(const RootImp&)= delete;
    RootImp& operator=(const RootImp)=delete;

    bool uid_is_manager(uid_t uid);

private:
    ~RootImp() {};
    RootImp(){};
};


#endif //ZYGISKNEXT_ROOTIMP_H
