#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/user.h>
#include <sys/mman.h>
#include <errno.h>



#if defined(__aarch64__)
#include <asm/ptrace.h>
typedef struct user_pt_regs regs_t;
#define IP_REG pc
#define BREAKPOINT_INSTR 0xD4200000 // BRK #0
#elif defined(__arm__)
#include <asm/ptrace.h>
    typedef struct pt_regs regs_t;
    #define IP_REG ARM_pc
    #define BREAKPOINT_INSTR 0xE1200070 // BKPT #0
#elif defined(__x86_64__)
    #include <sys/user.h>
    typedef struct user_regs_struct regs_t;
    #define IP_REG rip
    #define BREAKPOINT_INSTR 0xCC       // int3
#elif defined(__i386__)
    #include <sys/user.h>
    typedef struct user_regs_struct regs_t;
    #define IP_REG eip
    #define BREAKPOINT_INSTR 0xCC       // int3
#else
    #error "Unsupported architecture"
#endif



void wait_for_trace(int pid, int* status, int flags) {
    while (true) {
        auto result = waitpid(pid, status, flags);
        if (result == -1) {
            if (errno == EINTR) {
                continue;
            } else {
                printf("wait %d failed\n", pid);
//                exit(1);
            }
        }
        if (!WIFSTOPPED(*status)) {
//            LOGE("process %d not stopped for trace: %s, exit", pid, parse_status(*status).c_str());
            exit(1);
        }
        return;
    }
}




void target_function() {
    for(int i = 0;i<10;i++){
        printf("Target function is executing!\n");

    }
}

void child_process() {
//    ptrace(PTRACE_TRACEME, 0, NULL, NULL);
//    raise(SIGSTOP);  // 暂停等待父进程设置断点
    target_function();
}

int main() {
    getchar();
    child_process();
    void *breakpoint_addr = (void *) target_function;  // 目标函数地址

    pid_t pid = fork();
    if (pid == 0) {
        child_process();
//        execl("/bin/cat", "cat", "/proc/self/maps", nullptr);
    } else {
        int status;
        waitpid(pid, &status, 0);  // 等待子进程 SIGSTOP
        printf("1Child stopped, now tracing...\n");
        long orig_instr = ptrace(PTRACE_PEEKTEXT, pid, breakpoint_addr, NULL);
        regs_t regs;
        ptrace(PTRACE_POKETEXT, pid, breakpoint_addr, (orig_instr & ~0xFF) | BREAKPOINT_INSTR);


        ptrace(PTRACE_CONT, pid, nullptr, nullptr);  // 继续子进程
        // 继续执行子进程
        ptrace(PTRACE_CONT, pid, NULL, NULL);
        wait_for_trace(pid, &status, __WALL);
        // 捕获 SIGTRAP 并获取子进程寄存器
        if (WIFSTOPPED(status) && WSTOPSIG(status) == SIGTRAP) {
            ptrace(PTRACE_GETREGSET, pid, NULL, &regs);
#ifdef __x86_64__
            printf("Breakpoint hit at RIP = %llx\n", regs.rip);
#elif defined(__i386__)
            printf("Breakpoint hit at EIP = %lx\n", regs.eip);
#elif defined(__arm__)
            printf("Breakpoint hit at PC = %lx\n", regs.ARM_pc);
#elif defined(__aarch64__)
            printf("Breakpoint hit at PC = %llx\n", regs.pc);
#endif

            ptrace(PTRACE_POKETEXT, pid, breakpoint_addr, orig_instr);
            ptrace(PTRACE_SINGLESTEP, pid, NULL, NULL);
            waitpid(pid, NULL, 0);

            // 继续执行并分离
            ptrace(PTRACE_CONT, pid, NULL, NULL);
            ptrace(PTRACE_DETACH, pid, NULL, NULL);
        } else{
            printf("not SIGTRAP status = %d\n",status);
        }
        // 恢复原始指令并执行一步


    }


    return 0;
}
