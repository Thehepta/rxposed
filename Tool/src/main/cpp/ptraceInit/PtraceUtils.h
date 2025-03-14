/**
 * 让Ptrace注入兼容多平台的主要步骤在这里
 */

// system lib
#include <asm/ptrace.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/mman.h>
#include <dlfcn.h>
#include <dirent.h>
#include <elf.h>
#include <sys/uio.h>
#include <unistd.h>
#include <fcntl.h>

// user lib
#include "Utils.h"

// 各构架预定义
#if defined(__aarch64__) // 真机64位
#define pt_regs user_pt_regs
#define uregs regs
#define ARM_pc pc
#define ARM_sp sp
#define ARM_cpsr pstate
#define ARM_lr regs[30]
#define ARM_r0 regs[0]
#define BREAKPOINT_INSTR 0xD4200000 // BRK #0
// 这两个宏定义比较有意思 意思就是在 arm64下
// 强制 PTRACE_GETREGS 为 PTRACE_GETREGSET 这种
#define PTRACE_GETREGS PTRACE_GETREGSET
#define PTRACE_SETREGS PTRACE_SETREGSET
#elif defined(__x86_64__) // ？？未知架构
#define pt_regs user_regs_struct
#define eax rax
#define esp rsp
#define eip rip
#elif defined(__i386__) // 模拟器
#define pt_regs user_regs_struct
#endif

// 其余预定义
#define CPSR_T_MASK (1u << 5)

/** ============== 分界线 ==============
 */

/**
 * @brief 使用ptrace Attach附加到指定进程,发送SIGSTOP信号给指定进程让其停止下来并对其进行跟踪。
 * 但是被跟踪进程(tracee)不一定会停下来，因为同时attach和传递SIGSTOP可能会将SIGSTOP丢失。
 * 所以需要waitpid(2)等待被跟踪进程被停下
 *
 * @param pid pid表示远程进程的ID
 * @return int 返回0表示attach成功，返回-1表示失败
 */
int ptrace_attach(pid_t pid){
    int status = 0;
    if (ptrace(PTRACE_ATTACH, pid, NULL, NULL) < 0){
        LOGE("[-] ptrace attach process error, pid:%d, err:%s\n", pid, strerror(errno));
        return -1;
    }

    LOGD("[+] attach porcess success, pid:%d\n", pid);
    waitpid(pid, &status, WUNTRACED);

    return 0;
}

/**
 * @brief ptrace使远程进程继续运行
 *
 * @param pid pid表示远程进程的ID
 * @return int 返回0表示continue成功，返回-1表示失败
 */
int ptrace_continue(pid_t pid){
    if (ptrace(PTRACE_CONT, pid, NULL, NULL) < 0){
        LOGE("[-] ptrace continue process error, pid:%d, err:%ss\n", pid, strerror(errno));
        return -1;
    }

    LOGD("[+] ptrace continue process success, pid:%d\n", pid);
    return 0;
}

/**
 * @brief 使用ptrace detach指定进程,完成对指定进程的跟踪操作后，使用该参数即可解除附加
 *
 * @param pid pid表示远程进程的ID
 * @return int 返回0表示detach成功，返回-1表示失败
 */
int ptrace_detach(pid_t pid, int i) {
    if (ptrace(PTRACE_DETACH, pid, NULL, 0) < 0){
        LOGE("[-] detach process error, pid:%d, err:%s\n", pid, strerror(errno));
        return -1;
    }

    LOGD("[+] detach process success, pid:%d\n", pid);
    return 0;
}

/**
 * @brief 使用ptrace获取远程进程的寄存器值
 *
 * @param pid pid表示远程进程的ID
 * @param regs regs为pt_regs结构，存储了寄存器值
 * @return int 返回0表示获取寄存器成功，返回-1表示失败
 */
int ptrace_getregs(pid_t pid, struct pt_regs *regs){
#if defined(__aarch64__)
    int regset = NT_PRSTATUS;
    struct iovec ioVec;

    ioVec.iov_base = regs;
    ioVec.iov_len = sizeof(*regs);
    if (ptrace(PTRACE_GETREGSET, pid, (void *)regset, &ioVec) < 0){
        LOGE("[-] ptrace_getregs: Can not get register values, io %llx, %d\n", ioVec.iov_base,ioVec.iov_len);
        return -1;
    }

    return 0;
#else
    if (ptrace(PTRACE_GETREGS, pid, NULL, regs) < 0){
        printf("[-] Get Regs error, pid:%d, err:%s\n", pid, strerror(errno));
        return -1;
    }
#endif
    return 0;
}

/**
 * @brief 使用ptrace设置远程进程的寄存器值
 *
 * @param pid pid表示远程进程的ID
 * @param regs regs为pt_regs结构 存储需要修改的寄存器值
 * @return int 返回0表示设置寄存器成功 返回-1表示失败
 */
int ptrace_setregs(pid_t pid, struct pt_regs *regs){
#if defined(__aarch64__)
    int regset = NT_PRSTATUS;
    struct iovec ioVec;

    ioVec.iov_base = regs;
    ioVec.iov_len = sizeof(*regs);
    if (ptrace(PTRACE_SETREGSET, pid, (void *)regset, &ioVec) < 0){
        LOGE("[-] ptrace_setregs: Can not get register values");
        return -1;
    }

    return 0;
#else
    if (ptrace(PTRACE_SETREGS, pid, NULL, regs) < 0){
        printf("[-] Set Regs error, pid:%d, err:%s\n", pid, strerror(errno));
        return -1;
    }
#endif
    return 0;
}

/**
 * @brief 获取返回值，ARM处理器中返回值存放在ARM_r0寄存器中
 * @param regs regs存储远程进程当前的寄存器值
 * @return 在ARM处理器下返回r0寄存器值
 */
long ptrace_getret(struct pt_regs *regs) {
#if defined(__i386__) || defined(__x86_64__) // 模拟器&x86_64
    return regs->eax;
#elif defined(__arm__) || defined(__aarch64__) // 真机
    return regs->ARM_r0;
#else
    printf("Not supported Environment %s\n", __FUNCTION__);
#endif
}

/**
 * @brief 获取当前执行代码的地址 ARM处理器下存放在ARM_pc中
 * @param regs regs存储远程进程当前的寄存器值
 * @return 在ARM处理器下返回pc寄存器值
 */
long ptrace_getpc(struct pt_regs *regs) {
#if defined(__i386__) || defined(__x86_64__)
    return regs->eip;
#elif defined(__arm__) || defined(__aarch64__)
    return regs->ARM_pc;
#else
    printf("Not supported Environment %s\n", __FUNCTION__);
#endif
}



/**
 * @brief 使用ptrace从远程进程内存中读取数据
 * 这里的*_t类型是typedef定义一些基本类型的别名，用于跨平台。例如uint8_t表示无符号8位也就是无符号的char类型
 * @param pid pid表示远程进程的ID
 * @param pSrcBuf pSrcBuf表示从远程进程读取数据的内存地址
 * @param pDestBuf pDestBuf表示用于存储读取出数据的地址
 * @param size size表示读取数据的大小
 * @return 返回0表示读取数据成功
 */
int ptrace_readdata(pid_t pid, uint8_t *pSrcBuf, uint8_t *pDestBuf, size_t size) {
    long nReadCount = 0;
    long nRemainCount = 0;
    uint8_t *pCurSrcBuf = pSrcBuf;
    uint8_t *pCurDestBuf = pDestBuf;
    long lTmpBuf = 0;
    long i = 0;

    nReadCount = size / sizeof(long);
    nRemainCount = size % sizeof(long);

    for (i = 0; i < nReadCount; i++) {
        lTmpBuf = ptrace(PTRACE_PEEKTEXT, pid, pCurSrcBuf, 0);
        memcpy(pCurDestBuf, (char *) (&lTmpBuf), sizeof(long));
        pCurSrcBuf += sizeof(long);
        pCurDestBuf += sizeof(long);
    }

    if (nRemainCount > 0) {
        lTmpBuf = ptrace(PTRACE_PEEKTEXT, pid, pCurSrcBuf, 0);
        memcpy(pCurDestBuf, (char *) (&lTmpBuf), nRemainCount);
    }

    return 0;
}

/**
 * @brief 使用ptrace将数据写入到远程进程空间中
 *
 * @param pid pid表示远程进程的ID
 * @param pWriteAddr pWriteAddr表示写入数据到远程进程的内存地址
 * @param pWriteData pWriteData用于存储写入数据的地址
 * @param size size表示写入数据的大小
 * @return int 返回0表示写入数据成功，返回-1表示写入数据失败
 */
int ptrace_writedata(pid_t pid, uint8_t *pWriteAddr, uint8_t *pWriteData, size_t size){

    long nWriteCount = 0;
    long nRemainCount = 0;
    uint8_t *pCurSrcBuf = pWriteData;
    uint8_t *pCurDestBuf = pWriteAddr;
    long lTmpBuf = 0;
    long i = 0;

    nWriteCount = size / sizeof(long);
    nRemainCount = size % sizeof(long);

    // 先讲数据以sizeof(long)字节大小为单位写入到远程进程内存空间中
    for (i = 0; i < nWriteCount; i++){
        memcpy((void *)(&lTmpBuf), pCurSrcBuf, sizeof(long));
        if (ptrace(PTRACE_POKETEXT, pid, (void *)pCurDestBuf, (void *)lTmpBuf) < 0){ // PTRACE_POKETEXT表示从远程内存空间写入一个sizeof(long)大小的数据
            LOGE("[-] Write Remote Memory error, MemoryAddr:0x%lx, err:%s\n", (uintptr_t)pCurDestBuf, strerror(errno));
            return -1;
        }
        pCurSrcBuf += sizeof(long);
        pCurDestBuf += sizeof(long);
    }
    // 将剩下的数据写入到远程进程内存空间中
    if (nRemainCount > 0){
        lTmpBuf = ptrace(PTRACE_PEEKTEXT, pid, pCurDestBuf, NULL); //先取出原内存中的数据，然后将要写入的数据以单字节形式填充到低字节处
        memcpy((void *)(&lTmpBuf), pCurSrcBuf, nRemainCount);
        if (ptrace(PTRACE_POKETEXT, pid, pCurDestBuf, lTmpBuf) < 0){
            LOGE("[-] Write Remote Memory error, MemoryAddr:0x%lx, err:%s\n", (uintptr_t)pCurDestBuf, strerror(errno));
            return -1;
        }
    }
    return 0;
}

/**
 * @brief 使用ptrace远程call函数
 *
 * @param pid pid表示远程进程的ID
 * @param ExecuteAddr ExecuteAddr为远程进程函数的地址
 * @param parameters parameters为函数参数的地址
 * @param num_params regs为远程进程call函数前的寄存器环境
 * @param regs
 * @return 返回0表示call函数成功，返回-1表示失败
 */
int ptrace_call(pid_t pid, uintptr_t ExecuteAddr, long *parameters, long num_params,struct pt_regs *regs){
#if defined(__i386__) // 模拟器
    // 写入参数到堆栈
    regs->esp -= (num_params) * sizeof(long); // 分配栈空间，栈的方向是从高地址到低地址
    if (0 != ptrace_writedata(pid, (uint8_t *)regs->esp, (uint8_t *)parameters,(num_params) * sizeof(long))){
        return -1;
    }

    long tmp_addr = 0x0;
    regs->esp -= sizeof(long);
    if (0 != ptrace_writedata(pid, (uint8_t *)regs->esp, (uint8_t *)&tmp_addr, sizeof(tmp_addr))){
        return -1;
    }

    //设置eip寄存器为需要调用的函数地址
    regs->eip = ExecuteAddr;

    // 开始执行
    if (-1 == ptrace_setregs(pid, regs) || -1 == ptrace_continue(pid)){
        printf("[-] ptrace set regs or continue error, pid:%d\n", pid);
        return -1;
    }

    int stat = 0;
    // 对于使用ptrace_cont运行的子进程，它会在3种情况下进入暂停状态：①下一次系统调用；②子进程退出；③子进程的执行发生错误。
    // 参数WUNTRACED表示当进程进入暂停状态后，立即返回
    waitpid(pid, &stat, WUNTRACED);

    // 判断是否成功执行函数
    printf("[+] ptrace call ret status is %d\n", stat);
    while (stat != 0xb7f){
        if (ptrace_continue(pid) == -1){
            printf("[-] ptrace call error");
            return -1;
        }
        waitpid(pid, &stat, WUNTRACED);
    }

    // 获取远程进程的寄存器值，方便获取返回值
    if (ptrace_getregs(pid, regs) == -1){
        printf("[-] After call getregs error");
        return -1;
    }

#elif defined(__x86_64__) // ？？
    int num_param_registers = 6;
    // x64处理器，函数传递参数，将整数和指针参数前6个参数从左到右保存在寄存器rdi,rsi,rdx,rcx,r8和r9
    // 更多的参数则按照从右到左的顺序依次压入堆栈。
    if (num_params > 0)
        regs->rdi = parameters[0];
    if (num_params > 1)
        regs->rsi = parameters[1];
    if (num_params > 2)
        regs->rdx = parameters[2];
    if (num_params > 3)
        regs->rcx = parameters[3];
    if (num_params > 4)
        regs->r8 = parameters[4];
    if (num_params > 5)
        regs->r9 = parameters[5];

    if (num_param_registers < num_params){
        regs->esp -= (num_params - num_param_registers) * sizeof(long); // 分配栈空间，栈的方向是从高地址到低地址
        if (0 != ptrace_writedata(pid, (uint8_t *)regs->esp, (uint8_t *)&parameters[num_param_registers], (num_params - num_param_registers) * sizeof(long))){
            return -1;
        }
    }

    long tmp_addr = 0x0;
    regs->esp -= sizeof(long);
    if (0 != ptrace_writedata(pid, (uint8_t *)regs->esp, (uint8_t *)&tmp_addr, sizeof(tmp_addr))){
        return -1;
    }

    //设置eip寄存器为需要调用的函数地址
    regs->eip = ExecuteAddr;

    // 开始执行
    if (-1 == ptrace_setregs(pid, regs) || -1 == ptrace_continue(pid)){
        printf("[-] ptrace set regs or continue error, pid:%d", pid);
        return -1;
    }

    int stat = 0;
    // 对于使用ptrace_cont运行的子进程，它会在3种情况下进入暂停状态：①下一次系统调用；②子进程退出；③子进程的执行发生错误。
    // 参数WUNTRACED表示当进程进入暂停状态后，立即返回
    waitpid(pid, &stat, WUNTRACED);

    // 判断是否成功执行函数
    printf("ptrace call ret status is %lX\n", stat);
    while (stat != 0xb7f){
        if (ptrace_continue(pid) == -1){
            printf("[-] ptrace call error");
            return -1;
        }
        waitpid(pid, &stat, WUNTRACED);
    }

#elif defined(__arm__) || defined(__aarch64__) // 真机
    #if defined(__arm__) // 32位真机
    int num_param_registers = 4;
#elif defined(__aarch64__) // 64位真机
    int num_param_registers = 8;
#endif
    int i = 0;
    // ARM处理器，函数传递参数，将前四个参数放到r0-r3，剩下的参数压入栈中
    for (i = 0; i < num_params && i < num_param_registers; i++){
        regs->uregs[i] = parameters[i];
    }

    if (i < num_params){
        regs->ARM_sp -= (num_params - i) * sizeof(long); // 分配栈空间，栈的方向是从高地址到低地址
        if (ptrace_writedata(pid, (uint8_t *)(regs->ARM_sp), (uint8_t *)&parameters[i], (num_params - i) * sizeof(long)) == -1)
            return -1;
    }

    regs->ARM_pc = ExecuteAddr; //设置ARM_pc寄存器为需要调用的函数地址
    // 与BX跳转指令类似，判断跳转的地址位[0]是否为1，如果为1，则将CPST寄存器的标志T置位，解释为Thumb代码
    // 若为0，则将CPSR寄存器的标志T复位，解释为ARM代码
    if (regs->ARM_pc & 1){
        /* thumb */
        regs->ARM_pc &= (~1u);
        regs->ARM_cpsr |= CPSR_T_MASK;
    } else {
        /* arm */
        regs->ARM_cpsr &= ~CPSR_T_MASK;
    }

    regs->ARM_lr = 0;

    // Android 7.0以上修正lr为libc.so的起始地址 getprop获取ro.build.version.sdk
    uintptr_t lr_val = 0;
    char sdk_ver[32];
    memset(sdk_ver, 0, sizeof(sdk_ver));
    __system_property_get("ro.build.version.sdk", sdk_ver);
    //    printf("ro.build.version.sdk: %s", sdk_ver);
    if (atoi(sdk_ver) <= 23){
        lr_val = 0;
    } else { // Android 7.0
        static uintptr_t start_ptr = 0;
        if (start_ptr == 0){
            start_ptr = ge_remote_module_base(std::to_string(pid), "libc.so");
        }
        lr_val = start_ptr;
    }
    regs->ARM_lr = lr_val;

    if (ptrace_setregs(pid, regs) == -1 || ptrace_continue(pid) == -1){
        LOGD("[-] ptrace set regs or continue error, pid:%d\n", pid);
        return -1;
    }

    int stat = 0;
    // 对于使用ptrace_cont运行的子进程，它会在3种情况下进入暂停状态：①下一次系统调用；②子进程退出；③子进程的执行发生错误。
    // 参数WUNTRACED表示当进程进入暂停状态后，立即返回
    // 将ARM_lr（存放返回地址）设置为0，会导致子进程执行发生错误，则子进程进入暂停状态
    waitpid(pid, &stat, WUNTRACED);

    // 判断是否成功执行函数
    LOGD("[+] ptrace call ret status is %d\n", stat);
    while ((stat & 0xFF) != 0x7f){
        if (ptrace_continue(pid) == -1){
            LOGE("[-] ptrace call error\n");
            return -1;
        }
        waitpid(pid, &stat, WUNTRACED);
    }

    // 获取远程进程的寄存器值，方便获取返回值
    if (ptrace_getregs(pid, regs) == -1){
        LOGE("[-] After call getregs error\n");
        return -1;
    }

#else // 设备不符合注入器构架
    printf("[-] Not supported Environment %s\n", __FUNCTION__);
#endif
    return 0;
}


bool stop_int_app_process_entry(pid_t pid){
    struct pt_regs CurrentRegs;
    if (ptrace_getregs(pid, &CurrentRegs) != 0){
        return false;
    }
    auto arg = static_cast<uintptr_t>(CurrentRegs.sp);
    int argc;
    auto argv = reinterpret_cast<char **>(reinterpret_cast<uintptr_t *>(arg) + 1);
    read_proc(pid, arg,  (uintptr_t)&argc, sizeof(argc));
    LOGV("argc %d", argc);
    auto envp = argv + argc + 1;
    LOGV("envp %p", envp);
    auto p = envp;
    while (true) {
        uintptr_t *buf;
        read_proc(pid, (uintptr_t) p,  (uintptr_t)&buf, sizeof(buf));
        if (buf != nullptr) ++p;
        else break;
    }
    ++p;
    auto auxv = reinterpret_cast<ElfW(auxv_t) *>(p);
    auto v = auxv;
    uintptr_t entry_addr = 0;
    uintptr_t addr_of_entry_addr = 0;
    while (true) {
        ElfW(auxv_t) buf;
        read_proc(pid, (uintptr_t) v, (uintptr_t)&buf, sizeof(buf));
        if (buf.a_type == AT_ENTRY) {
            entry_addr = (uintptr_t) buf.a_un.a_val;
            addr_of_entry_addr = (uintptr_t) v + offsetof(ElfW(auxv_t), a_un);
            break;
        }
        if (buf.a_type == AT_NULL) break;
        v++;
    }
    if (entry_addr == 0) {
        LOGE("failed to get entry");
        return false;
    }

    uintptr_t break_addr = (-0x05ec1cff & ~1) | ((uintptr_t) entry_addr & 1);

    if (!write_proc(pid, (uintptr_t) addr_of_entry_addr,  (uintptr_t)&break_addr, sizeof(break_addr))) return false;
    ptrace(PTRACE_CONT, pid, 0, 0);
    int status;
    wait_for_trace(pid, &status, __WALL);
    if (WIFSTOPPED(status) && WSTOPSIG(status) == SIGSEGV) {
        if (ptrace_getregs(pid, &CurrentRegs) != 0) {
            return false;
        }
        if (static_cast<uintptr_t>(CurrentRegs.pc & ~1) != (break_addr & ~1)) {
            LOGE("stopped at unknown addr %p", (void *) CurrentRegs.pc);
            return false;
        }
        // The linker has been initialized now, we can do dlopen
        LOGD("stopped at entry");

        // restore entry address
        if (!write_proc(pid, (uintptr_t) addr_of_entry_addr, (uintptr_t)&entry_addr,
                        sizeof(entry_addr)))
            return false;
        // reset pc to entry
        CurrentRegs.pc = (long) entry_addr;

        LOGD("invoke entry");
        // restore registers
        ptrace_setregs(pid, &CurrentRegs);

        return true;
    }
    return false;

}



bool remote_ptrace_dlopen(pid_t pid,char*LibPath){
    struct pt_regs CurrentRegs, OriginalRegs;
    if (ptrace_getregs(pid, &CurrentRegs) != 0){
        return false;
    }
    auto map = MapScan(std::to_string(pid));
    auto local_map = MapScan(std::to_string(getpid()));

    memcpy(&OriginalRegs, &CurrentRegs, sizeof(CurrentRegs));

    do{
        void *mmap_addr = find_func_addr(local_map, map, "libc.so", "mmap");
        long parameters[6];
        // mmap映射 <-- 设置mmap的参数
        // void *mmap(void *start, size_t length, int prot, int flags, int fd, off_t offsize);
        parameters[0] = NULL; // 设置为NULL表示让系统自动选择分配内存的地址
        parameters[1] = 0x3000; // 映射内存的大小
        parameters[2] = PROT_READ | PROT_WRITE; // 表示映射内存区域 可读|可写|可执行
        parameters[3] = MAP_ANONYMOUS | MAP_PRIVATE; // 建立匿名映射
        parameters[4] = -1; //  若需要映射文件到内存中，则为文件的fd
        parameters[5] = 0; //文件映射偏移量

        // 调用远程进程的mmap函数 建立远程进程的内存映射 在目标进程中为libxxx.so分配内存
        if (ptrace_call(pid, (uintptr_t)mmap_addr, parameters, 6, &CurrentRegs) == -1){
            LOGE("[-][function:%s] Call Remote mmap Func Failed, err:%s\n",__func__ , strerror(errno));
            break;
        }
        // 打印一下
        LOGD("[+][function:%s] ptrace_call mmap success, return value=%lX, pc=%lX\n",__func__ , ptrace_getret(&CurrentRegs), ptrace_getpc(&CurrentRegs));

        // 获取mmap函数执行后的返回值，也就是内存映射的起始地址
        // 从寄存器中获取mmap函数的返回值 即申请的内存首地址
        auto RemoteMapMemoryAddr = (uintptr_t)ptrace_getret(&CurrentRegs);
        LOGD("[+][function:%s] Remote Process Map Memory Addr:0x%lx\n",__func__ , RemoteMapMemoryAddr);

//    // 分别获取dlopen、dlsym、dlclose等函数的地址
        auto dlopen_addr = find_func_addr(local_map, map, "libdl.so", "dlopen");
        auto dlsym_addr = find_func_addr(local_map, map, "libdl.so", "dlsym");
        auto dlclose_addr = find_func_addr(local_map, map, "libdl.so", "dlclose");
        auto dlerror_addr = find_func_addr(local_map, map, "libdl.so", "dlerror");

        //    // 打印一下
//    LOGD("[+][function:%s] Get imports: dlopen: %lx, dlsym: %lx, dlclose: %lx, dlerror: %lx\n",__func__ , dlopen_addr, dlsym_addr, dlclose_addr, dlerror_addr);

        // 打印注入so的路径
        LOGD("[+][function:%s] LibPath = %s\n",__func__ , LibPath);

        // 将要加载的so库路径写入到远程进程内存空间中
        /**
         * pid  开始写入数据的地址   写入内容    写入数据大小
         */
        if (write_proc(pid,  RemoteMapMemoryAddr, (uintptr_t) LibPath,strlen(LibPath) + 1) == -1) {
            LOGE("[-][function:%s] Write LibPath:%s to RemoteProcess error\n",__func__ , LibPath);
            break;
        }

        // 设置dlopen的参数,返回值为模块加载的地址
        // void *dlopen(const char *filename, int flag);
        parameters[0] = (uintptr_t) RemoteMapMemoryAddr; // 写入的libPath
        parameters[1] = RTLD_NOW ; // dlopen的标识                            不能使用RTLD_GLOBAL ,会导致无法dlclose 无法关闭so库

        // 执行dlopen 载入so
        if (ptrace_call(pid, (uintptr_t) dlopen_addr, parameters, 2, &CurrentRegs) == -1) {
            LOGE("[-][function:%s] Call Remote dlopen Func Failed\n",__func__ );
            break;
        }

        // RemoteModuleAddr为远程进程加载注入模块的地址
        void *RemoteModuleAddr = (void *) ptrace_getret(&CurrentRegs);
        LOGD("[+][function:%s] ptrace_call dlopen success, Remote Process load module Addr:0x%lx\n",__func__ ,(long) RemoteModuleAddr);

        // dlopen 错误
        if ((long) RemoteModuleAddr == 0x0){
            if (ptrace_call(pid, (uintptr_t) dlerror_addr, parameters, 0, &CurrentRegs) == -1) {
                LOGE("[-][function:%s] Call Remote dlerror Func Failed\n",__func__ );
                break;
            }
            uintptr_t Error =  ptrace_getret(&CurrentRegs);
            char LocalErrorInfo[1024] = {0};
            read_proc(pid,  Error, ( uintptr_t) LocalErrorInfo, 1024);
            LOGE("[-][function:%s] dlopen error:%s\n",__func__, LocalErrorInfo );
            break;
        }
        if (ptrace_setregs(pid, &OriginalRegs) == -1) {
            LOGE("[-][function:%s] Recover reges failed\n",__func__);
            break;
        }

        LOGD("[+][function:%s] Recover Regs Success\n",__func__);

        ptrace_getregs(pid, &CurrentRegs);
        if (memcmp(&OriginalRegs, &CurrentRegs, sizeof(CurrentRegs)) != 0) {
            LOGE("[-][function:%s] Set Regs Error\n",__func__);
            break;
        }
        return true;
    } while (false);

    return false;
}

