#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/user.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <linux/elf.h>
#include <linux/uio.h>

void child_process() {
    ptrace(PTRACE_TRACEME, 0, NULL, NULL);
    raise(SIGSTOP); // 暂停自己，等待父进程追踪

    syscall(SYS_write, 1, "Hello, world!\n", 14);
    fork();
//    syscall(SYS_exit, 0);
}

int main() {
    pid_t child = fork();
    if (child == 0) {
        child_process();
    } else {
        int status;
#if defined(__aarch64__)
        struct user_pt_regs regs;
#else
        struct user_regs regs;
#endif
        struct iovec ioVec;
        ioVec.iov_base = &regs;
        ioVec.iov_len = sizeof(regs);
        // 等待子进程暂停
        waitpid(child, &status, 0);
        ptrace(PTRACE_SETOPTIONS, child, 0, PTRACE_O_TRACESYSGOOD);
        ptrace(PTRACE_SYSCALL, child, NULL, NULL);

        while (1) {
            waitpid(child, &status, 0);
            if (WIFEXITED(status)) break;

            // 获取系统调用号

#if defined(__aarch64__)
            ptrace(PTRACE_GETREGSET, child, NT_PRSTATUS, &ioVec);
            printf("Syscall entry: %llu\n", regs.regs[8]);
#else
            ptrace(PTRACE_GETREGS, child, NULL, &regs);
                printf("Syscall entry: %lu\n", regs.uregs[7]);
#endif

            ptrace(PTRACE_SYSCALL, child, NULL, NULL);
            waitpid(child, &status, 0);
            if (WIFEXITED(status)) break;

            // 获取返回值
//            ptrace(PTRACE_GETREGSET, child, NT_PRSTATUS, &regs);
//            printf("Syscall exit: %llu (return value: %llu)\n", regs.regs[8], regs.regs[0]);
            //每次获取完了以后需要在执行这个代码,才能继续追踪系统调用
            ptrace(PTRACE_SYSCALL, child, NULL, NULL);
        }
    }
    return 0;
}
