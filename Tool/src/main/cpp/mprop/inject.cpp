//
// Created by thehepta on 2024/2/21.
//
#include <stdio.h>
#include <stdlib.h>
#include <sys/user.h>
#include <asm/ptrace.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <dlfcn.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <elf.h>
#include <android/log.h>
#include <sys/uio.h>

#if defined(__i386__)
#define pt_regs         user_regs_struct
#elif defined(__aarch64__)
#define pt_regs         user_pt_regs
#define uregs    regs
#define ARM_pc    pc
#define ARM_sp    sp
#define ARM_cpsr    pstate
#define ARM_lr        regs[30]
#define ARM_r0        regs[0]
#define PTRACE_GETREGS PTRACE_GETREGSET
#define PTRACE_SETREGS PTRACE_SETREGSET
#endif

#define ENABLE_DEBUG 0

#if ENABLE_DEBUG
#define  LOG_TAG "INJECT"
#define  LOGD(fmt, args...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG, fmt, ##args)
#define DEBUG_PRINT(format, args...) \
    LOGD(format, ##args)
#else
#define DEBUG_PRINT(format,args...)
#endif

#define CPSR_T_MASK     ( 1u << 5 )



//进程的每个module信息
struct mode_node {
    long begin;
    long end;
    char permiss[6];
};

//储存进程的module
struct root {
    struct mode_node start[5000];
    int length;
};

//libc.so的信息
struct mode_node libc_node;
int found = 0;

//switch on|off
int switch_statue;

//向进程写入数据
int ptrace_setData(pid_t pid, const void *addr, const void *data, int size) {
    int count = size / sizeof(long);
    int remain = size % sizeof(long);
    long buf;
    int i = 0;
    for (i = 0; i < count; i++) {
        memcpy(&buf, data, sizeof(long));
        if (ptrace(PTRACE_POKETEXT, pid, addr, buf) == -1)
            printf("write data error:%d\n", i);
        data = ((long *) data) + 1;
        addr = ((long *) addr) + 1;
    }
    if (remain > 0) {
        buf = ptrace(PTRACE_PEEKTEXT, pid, addr, NULL);
        memcpy(&buf, data, remain);
        if (ptrace(PTRACE_POKETEXT, pid, addr, buf) == -1) {
            perror("wirte remain data error");
            return -1;
        }
    }
    return -1;
}

//读取进程的内存数据
char *ptrace_getData(pid_t pid, unsigned long addr, unsigned long size) {
    int count = size / sizeof(long);
    int remain = size % sizeof(long);
    char *str = (char *) malloc(size + 1);
    memset(str, 0, size + 1);
    int LONG_SIZE = sizeof(long);
    char *point = str;
    union u {
        long val;
        char chars[sizeof(long)];
    } d;

    int i;
    for (i = 0; i < count; i++) {
        d.val = ptrace(PTRACE_PEEKTEXT, pid, addr, 0);
        memcpy(point, d.chars, LONG_SIZE);
        addr += LONG_SIZE;
        point += LONG_SIZE;
    }

    if (remain > 0) {
        d.val = ptrace(PTRACE_PEEKTEXT, pid, addr, 0);
        memcpy(point, d.chars, remain);
    }
    return str;
}

int ptrace_writedata(pid_t pid, uint8_t *dest, uint8_t *data, size_t size) {
    long i, j, remain;
    uint8_t *laddr;
    size_t bytes_width = sizeof(long);

    union u {
        long val;
//		char chars[bytes_width];
        char chars[8];
    } d;

    j = size / bytes_width;
    remain = size % bytes_width;

    laddr = data;

    for (i = 0; i < j; i++) {
        memcpy(d.chars, laddr, bytes_width);
        ptrace(PTRACE_POKETEXT, pid, dest, d.val);

        dest += bytes_width;
        laddr += bytes_width;
    }

    if (remain > 0) {
        d.val = ptrace(PTRACE_PEEKTEXT, pid, dest, 0);
        for (i = 0; i < remain; i++) {
            d.chars[i] = *laddr++;
        }

        ptrace(PTRACE_POKETEXT, pid, dest, d.val);
    }

    return 0;
}

int ptrace_getregs(pid_t pid, struct pt_regs *regs) {
#if defined (__aarch64__)
    int regset = NT_PRSTATUS;
    struct iovec ioVec;

    ioVec.iov_base = regs;
    ioVec.iov_len = sizeof(*regs);
    if (ptrace(PTRACE_GETREGSET, pid, (void *) regset, &ioVec) < 0) {
        perror("ptrace_getregs: Can not get register values");
        printf(" io %llx, %d", ioVec.iov_base, ioVec.iov_len);
        return -1;
    }

    return 0;
#else
    if (ptrace(PTRACE_GETREGS, pid, NULL, regs) < 0) {
        perror("ptrace_getregs: Can not get register values");
        return -1;
    }

    return 0;
#endif
}

int ptrace_setregs(pid_t pid, struct pt_regs *regs) {
#if defined (__aarch64__)
    int regset = NT_PRSTATUS;
    struct iovec ioVec;

    ioVec.iov_base = regs;
    ioVec.iov_len = sizeof(*regs);
    if (ptrace(PTRACE_SETREGSET, pid, (void *) regset, &ioVec) < 0) {
        perror("ptrace_setregs: Can not get register values");
        return -1;
    }

    return 0;
#else
    if (ptrace(PTRACE_SETREGS, pid, NULL, regs) < 0) {
        perror("ptrace_setregs: Can not set register values");
        return -1;
    }

    return 0;
#endif
}

int ptrace_continue(pid_t pid) {
    if (ptrace(PTRACE_CONT, pid, NULL, 0) < 0) {
        perror("ptrace_cont");
        return -1;
    }

    return 0;
}



int ptrace_call(pid_t pid, uintptr_t addr, long *params, int num_params, struct pt_regs *regs) {
    int i;
#if defined(__arm__)
    int num_param_registers = 4;
#elif defined(__aarch64__)
    int num_param_registers = 8;
#endif

    for (i = 0; i < num_params && i < num_param_registers; i++) {
        regs->uregs[i] = params[i];
    }

    //
    // push remained params onto stack
    //
    if (i < num_params) {
        regs->ARM_sp -= (num_params - i) * sizeof(long);
        ptrace_writedata(pid, (uint8_t *) regs->ARM_sp, (uint8_t *) params[i],
                         (num_params - i) * sizeof(long));
    }

    regs->ARM_pc = addr;
    if (regs->ARM_pc & 1) {
        /* thumb */
        regs->ARM_pc &= (~1u);
        regs->ARM_cpsr |= CPSR_T_MASK;
    } else {
        /* arm */
        regs->ARM_cpsr &= ~CPSR_T_MASK;
    }

    regs->ARM_lr = 0;

    if (ptrace_setregs(pid, regs) == -1
        || ptrace_continue(pid) == -1) {
        printf("error\n");
        return -1;
    }

    int stat = 0;
    waitpid(pid, &stat, WUNTRACED);
    while (stat != 0xb7f) {
        if (ptrace_continue(pid) == -1) {
            printf("error\n");
            return -1;
        }
        waitpid(pid, &stat, WUNTRACED);
    }

    return 0;
}




int ptrace_attach(pid_t pid) {
    if (ptrace(PTRACE_ATTACH, pid, NULL, 0) < 0) {
        perror("ptrace_attach");
        return -1;
    }

    int status = 0;
    waitpid(pid, &status, WUNTRACED);

    return 0;
}

int ptrace_detach(pid_t pid) {
    if (ptrace(PTRACE_DETACH, pid, NULL, 0) < 0) {
        perror("ptrace_detach");
        return -1;
    }

    return 0;
}


void *get_module_base(pid_t pid, const char *module_name) {
    FILE *fp;
    long addr = 0;
    char *pch;
    char filename[32];
    char line[1024];

    if (pid < 0) {
        /* self process */
        snprintf(filename, sizeof(filename), "/proc/self/maps", pid);
    } else {
        snprintf(filename, sizeof(filename), "/proc/%d/maps", pid);
    }

    fp = fopen(filename, "r");

    if (fp != NULL) {
        while (fgets(line, sizeof(line), fp)) {
            if (strstr(line, module_name)) {
                pch = strtok(line, "-");
                addr = strtoull(pch, NULL, 16);

                if (addr == 0x8000)
                    addr = 0;

                break;
            }
        }

        fclose(fp);
    }

    return (void *) addr;
}

//获取进程对应的所有可读的moudle
struct root *get_all_module_r(pid_t pid) {
    FILE *fp;
    int rc;
    long addr = 0;
    char *pch, *cursor;
    char filename[32];
    char line[1024];
    char copy[1024];
    long begin, end;
    char *params;
    struct root *link = 0;
    struct mode_node *tmp;

    if (pid < 0) {
        /* self process */
        snprintf(filename, sizeof(filename), "/proc/self/maps", pid);
    } else {
        snprintf(filename, sizeof(filename), "/proc/%d/maps", pid);
    }
    fp = fopen(filename, "r");
    if (fp != NULL) {
        link = static_cast<root *>(malloc(sizeof(struct root)));
        memset(link, 0, sizeof(struct root));
        memset(line, 0, sizeof(line));
        while (fgets(line, sizeof(line), fp)) {
//            if (strstr(line, "/init") == NULL && strstr(line, "libc.so") == NULL)
//                continue;
            strcpy(copy, line);
            pch = strtok(copy, "-");
            begin = strtoll(pch, NULL, 16);
            pch = strtok(NULL, " ");
            end = strtoll(pch, NULL, 16);
            params = strtok(NULL, " ");
            if (strstr(line, "libc.so") != NULL) {
                if (found)
                    continue;
                libc_node.begin = begin;
                libc_node.end = end;
                found = 1;
            } else {
                tmp = &link->start[link->length];
                tmp->begin = begin;
                tmp->end = end;
                strcpy(tmp->permiss, params);
                link->length++;
            }
        }
    }

    fclose(fp);
    return link;
}


int find_pid_of(const char *process_name) {
    int id;
    pid_t pid = -1;
    DIR *dir;
    FILE *fp;
    char filename[32];
    char cmdline[256];

    struct dirent *entry;

    if (process_name == NULL)
        return -1;

    dir = opendir("/proc");
    if (dir == NULL)
        return -1;

    while ((entry = readdir(dir)) != NULL) {
        id = atoi(entry->d_name);
        if (id != 0) {
            sprintf(filename, "/proc/%d/cmdline", id);
            fp = fopen(filename, "r");
            if (fp) {
                fgets(cmdline, sizeof(cmdline), fp);
                fclose(fp);

                if (strcmp(process_name, cmdline) == 0) {
                    /* process found */
                    pid = id;
                    break;
                }
            }
        }
    }

    closedir(dir);
    return pid;
}

uint64_t ptrace_retval(struct pt_regs *regs) {
#if defined(__arm__) || defined(__aarch64__)
    return regs->ARM_r0;
#elif defined(__i386__)
    return regs->eax;
#else
#error "Not supported"
#endif
}

uint64_t ptrace_ip(struct pt_regs *regs) {
#if defined(__arm__) || defined(__aarch64__)
    return regs->ARM_pc;
#elif defined(__i386__)
    return regs->eip;
#else
#error "Not supported"
#endif
}

int ptrace_call_wrapper(pid_t target_pid, const char *func_name, void *func_addr, long *parameters,
                        int param_num, struct pt_regs *regs) {
    DEBUG_PRINT("[+] Calling %s in target process.\n", func_name);
    if (ptrace_call(target_pid, (uintptr_t) func_addr, parameters, param_num, regs) == -1)
        return -1;
    DEBUG_PRINT("[+] Calling success , see return");
    if (ptrace_getregs(target_pid, regs) == -1)
        return -1;
    DEBUG_PRINT("[+] Target process returned from %s, return value=%llx, pc=%llx \n",
                func_name, ptrace_retval(regs), ptrace_ip(regs));
    return 0;
}


int main(int argc, char **argv) {
    pid_t target_pid;
    if (argc >= 3) {
        target_pid = atoi(argv[1]);
        if (strcmp(argv[2], "--on") == 0) {
            switch_statue = 1;
        } else if (strcmp(argv[2], "--off") == 0) {
            switch_statue = 0;
        } else {
            perror("only support params: --on | --off");
            return 0;
        }
    } else {
//        target_pid = find_pid_of("init");
        perror("such as:  mprop $pid --on|--off\npid:the pid of init\n");
        return 0;
    }
    if (target_pid == -1) {
        perror("counld find the pid of init, please add -t $pid");
        return 0;
    }
    printf("process: init  -- pid:%d\n", target_pid);

    char ro[4] = {0x72, 0x6f, 0x2e, 0x00};
    char sd[4] = {0x72, 0x6f, 0x2f, 0x00};

    struct root *link = get_all_module_r(target_pid);
    long tmp_base, tmp_end ,rec;
    char *str;
    long parm[4];
    char tmp[5];
    struct pt_regs regs, original_regs;
    if (link && libc_node.begin) {
//        printf("libc moudle : %lx, %lx\n", libc_node.begin, libc_node.end);
        void *local_libc_addr = get_module_base(-1, "libc.so");
//        printf("mylibc addr:%p  -- mproject:%p", local_libc_addr, (void *) mprotect);
        void *remote_mproject = (void*) ((uintptr_t) mprotect - (uintptr_t) local_libc_addr
                                                     + libc_node.begin);
        printf("found remote func:%p\n", remote_mproject);

        if (ptrace_attach(target_pid) == -1)
            goto exit;

        if (ptrace_getregs(target_pid, &regs) == -1)
            goto exit;

        memcpy(&original_regs, &regs, sizeof(regs));

        printf("you should may sure that the selinux is closed!!!!\n");
        printf("patch code ing....it may cost few minutes....\n");

        for (int i = 0; i < link->length; i++) {
            tmp_base = link->start[i].begin;
            tmp_end = link->start[i].end;

            //全设置为可读可写可执行，之后设置回去
            parm[0] = tmp_base;
            parm[1] = tmp_end - tmp_base;
            parm[2] = PROT_READ | PROT_WRITE | PROT_EXEC;
            if (ptrace_call_wrapper(target_pid, "mprotect", remote_mproject, parm, 3, &regs) == -1)
                goto exit;

            if (switch_statue) {
                for (; tmp_base < tmp_end; tmp_base++) {
                    str = ptrace_getData(target_pid, tmp_base, 4);
                    if (strcmp(str, ro) == 0) {
                        ptrace_setData(target_pid, (void *)tmp_base, sd, 4);
                    }
                }
            } else {
                for (; tmp_base < tmp_end; tmp_base++) {
                    str = ptrace_getData(target_pid, tmp_base, 4);
                    if (strcmp(str, sd) == 0) {
                        ptrace_setData(target_pid, (void *)tmp_base, ro, 4);
                    }
                }
            }


            parm[2] = 0;
            if (link->start[i].permiss[0] == 'r') {
                parm[2] |= PROT_READ;
            }
            if (link->start[i].permiss[1] == 'w') {
                parm[2] |= PROT_WRITE;
            }
            if (link->start[i].permiss[2] == 'x') {
                parm[2] |= PROT_EXEC;
            }
            if (ptrace_call_wrapper(target_pid, "mprotect", remote_mproject, parm, 3, &regs) == -1)
                goto exit;

        }
    } else if (!link) {
        perror("未定位到init所在的module\n");
        return 0;
    } else {
        perror("未找到init中libc.so所在\n");
        return 0;
    }
    printf("oprea is finished !!\n");

    exit:

    ptrace_setregs(target_pid, &original_regs);
    ptrace_detach(target_pid);
    return 0;
}