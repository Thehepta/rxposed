//
// Created by chic on 2023/8/27.
//

#include "android_shm.h"

#define ASHMEM_DEVICE  "/dev/ashmem"

//ret= 0 创建成功；ret=-1，失败；
//注：ret =1，共享内存已经存在，但是目前这个没用，暂时放这
int create_shared_memory(const char* name, U64 size, int node, char*& addr, U64& shm_id){
    U64 fd = open(ASHMEM_DEVICE, O_RDWR);
    if(fd < 0){
        return -1;
    }
    U64 len = ioctl(fd, ASHMEM_GET_SIZE, NULL);
    if(len > 0){
        addr = (char*)mmap(NULL, size , PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        shm_id = fd;
        return 1;
    }else{
        int ret = ioctl(fd, ASHMEM_SET_NAME,name);
        if(ret < 0){
            close(fd);
            return -1;
        }
        ret = ioctl(fd,ASHMEM_SET_SIZE, size);
        if(ret < 0){
            close(fd);
            return -1;
        }
        addr = (char*)mmap(NULL, size , PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        shm_id = fd;
    }
    return 0;
}

int open_shared_memory(const char* name, int node, char*& addr, U64& shm_id){
    U64 size = ioctl(shm_id, ASHMEM_GET_SIZE,NULL);
    if(size > 0){
        addr = (char*)mmap(NULL, size , PROT_READ | PROT_WRITE, MAP_SHARED, shm_id, 0);
    }else{
        return -1;
    }
    return 0;
}

int close_shared_memory(U64& shm_id, char*& addr){
    U64 size = ioctl(shm_id, ASHMEM_GET_SIZE, NULL);
    if(size <0){
        return -1;
    }
    int ret = munmap((void*)addr, size);
    if(ret == -1){
        return -1;
    }
    ret = close(shm_id);
    if(ret == -1){
        return -1;
    }
    return 0;
}
