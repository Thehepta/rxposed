//
// Created by chic on 2024/12/4.
//

#include <sys/prctl.h>
#include <cstdio>
#include <sys/stat.h>
#include "RootImp.h"


// 常量定义
#define KERNEL_SU_OPTION 0xdeadbeefu
#define CMD_GET_VERSION 2

// 假定的最小和最大版本号，请根据实际需要设置
#define MIN_KSU_VERSION 10
#define MAX_KSU_VERSION 20
#define CMD_UID_GRANTED_ROOT 12


int RootImp::getProcessFlags(uid_t uid){
    int flag = 0;
    if(uid_is_manager(uid)){
        flag|=PROCESS_IS_MANAGER;
    } else{
        if(uid_granted_root(uid)) {
            flag|=PROCESS_GRANTED_ROOT;
        }
    }
    if(get_kernel_su() == VERSION_SUPPORTED){
        flag|=PROCESS_ROOT_IS_KSU;
    }
    if(get_magisk() == VERSION_SUPPORTED){
        flag|=PROCESS_ROOT_IS_MAGISK;
    }
    return flag;
}


int RootImp::get_kernel_su() {
    int version = 0;
    if(prctl(KERNEL_SU_OPTION,CMD_GET_VERSION,&version,0,0)<0){
        return VERSION_NONE;

    }
    // 判断版本号
    const int MAX_OLD_VERSION = MIN_KSU_VERSION - 1;
    if (version == 0) {
        return VERSION_NONE;
    } else if (version >= MIN_KSU_VERSION && version <= MAX_KSU_VERSION) {
        return VERSION_SUPPORTED;
    } else if (version >= 1 && version <= MAX_OLD_VERSION) {
        return VERSION_TOO_OLD;
    } else {
        return VERSION_ABNORMAL;
    }
}

int RootImp::get_magisk() {
    int version=0;

    if (is_command_available("magisk")) {
        FILE *fp = popen("magisk -V", "r");
        if (fp == NULL) {
            return 0;  // 如果打开管道失败，返回 0
        }
        char result[256];
        if (fgets(result, sizeof(result), fp) != NULL) {
            // 如果读取到结果，表示命令存在
            fclose(fp);
            version = atoi(result);
        } else {
            // 如果没有读取到结果，表示命令不存在
            fclose(fp);
            return VERSION_NONE;
        }
    } else {
        printf("Magisk command is not available.\n");
        return VERSION_NONE;
    }
    if(version <= 0){
        return VERSION_NONE;
    } else {
        return VERSION_SUPPORTED;

    }

    return 0;
}

int RootImp::is_command_available(const char *command) {
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "which %s", command);  // 使用 which 命令检查命令是否存在
    FILE *fp = popen(cmd, "r");
    if (fp == NULL) {
        return 0;  // 如果打开管道失败，返回 0
    }

    char result[256];
    if (fgets(result, sizeof(result), fp) != NULL) {
        // 如果读取到结果，表示命令存在
        fclose(fp);
        return 1;
    } else {
        // 如果没有读取到结果，表示命令不存在
        fclose(fp);
        return 0;
    }
}

bool RootImp::uid_is_manager(uid_t uid) {

    if(get_kernel_su() == VERSION_SUPPORTED){
        return is_kernel_su_manager(uid);
    }
    if(get_magisk() == VERSION_SUPPORTED){
        return is_magisk_manager(uid);
    }
    return false;
}

bool RootImp::is_kernel_su_manager(uid_t uid) {
    const char *path = "/data/user_de/0/me.weishu.kernelsu";
    struct stat file_stat;
    if (stat(path, &file_stat) == 0) {
        // 检查文件的用户 ID 是否与指定的 uid 匹配
        return file_stat.st_uid == uid;
    } else {
        perror("stat failed"); // 打印错误信息
        return false;              // 文件不存在或无法访问
    }

    return false;
}

bool RootImp::is_magisk_manager(uid_t uid) {
    return false;
}

bool RootImp::uid_granted_root(uid_t uid) {
    unsigned int result = 0;
    bool granted = false;

    // 调用 prctl
    int ret = prctl(
            KERNEL_SU_OPTION,
            CMD_UID_GRANTED_ROOT,
            uid,
            (long)&granted, // 强制转换为 long 类型以适配 prctl 参数
            (long)&result   // 强制转换为 long 类型以适配 prctl 参数
    );

    if (ret < 0) {
        perror("prctl failed");
        return false;
    }

    if (result != KERNEL_SU_OPTION) {
        fprintf(stderr, "uid_granted_root failed\n");
        return false;
    }

    return granted != 0;
}
