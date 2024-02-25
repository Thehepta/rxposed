//
// Created by chic on 2023/8/27.
//

#pragma once


#include <fcntl.h>
#define SHM_HUGETLB    04000

#include <stdio.h>
#include <linux/ashmem.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <stddef.h>

/* Get common definition of System V style IPC.  */
#include <linux/ipc.h>

/* Get system dependent definition of `struct shmid_ds' and more.  */
#include <linux/shm.h>

typedef unsigned long long U64;
__BEGIN_DECLS


extern int create_shared_memory(const char* name, U64 size, int node, char*& addr, U64& shm_id);

extern int open_shared_memory(const char* name, int node, char*& addr, U64& shm_id);

extern int close_shared_memory(U64& shm_id, char*& addr);


__END_DECLS

