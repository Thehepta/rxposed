// user lib
#include "PtraceInject.h"

// 由于Inject是命令行工具 因此 替换全部的LOG为printf 以方便观察注入流程
int main(int argc, char *argv[]) {

    printf("[+] Start Inject\n");

    // 开始注入

    /** 以下是Inject命令行工具的参数
     ** 部分参数选填
     * -p 目标进程pid <-- 不传pid就传包名
     * -n 目标App包名 <-- 不传包名就传pid
     * -f 是否开启App <-- 看你要不要强制唤醒App
     * ---- // 由于 -f 参数需要创建中间文件 因此 请务必在Inject工具目录执行该工具
     * ---- // 即 /data/local/tmp/Inject -f -n XXX <-- 错误
     * ---- // 即 cd /data/local/tmp && ./Inject -f -n XXX <-- 正确
     * -so 注入的so路径 <-- 必填 本来就是so注入工具 只能是绝对路径!!
     * -symbols 指定启用so中的某功能 <-- 选填 可以指定so中某功能的symbols 也可以通过__attribute__((constructor))让so注入后自行初始化
     */
    pid_t pid = 0;
    char *pkg_name = NULL;

    if (init_inject(argc, argv) == 0){
        printf("[+] Finish Inject\n");
    } else {
        printf("[-] Inject Erro\n");
    }
//#if defined(__aarch64__) // 真机64位
//    if (get_pid_by_name(&pid, "zygote64")){
//        printf("[+] get_pid_by_name zygote64 pid is %d\n", pid);
//    }
//#elif defined(__arm__)
//    if (get_pid_by_name(&pid, "zygote")){
//        printf("[+] get_pid_by_name zygote pid is %d\n", pid);
//    }
//    /**
//     * eg:
//     * cd /data/local/tmp && ./Inject -f -n bin.mt.plus -so /data/local/tmp/libHook.so -symbols hello
//     */
//#endif

    return 0;
}
