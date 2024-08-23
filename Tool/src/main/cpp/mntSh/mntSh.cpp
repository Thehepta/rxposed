
#include <sys/mount.h>
#include <cstdio>
#include <unistd.h>
#include <sys/syscall.h>
#include <linux/sched.h>
#include <sched.h>
#include <asm-generic/fcntl.h>
#include <fcntl.h>
#include <string.h>
#include <dirent.h>
#include <stdlib.h>

#ifdef __aarch64__
#define zygote_name "zygote64"
#define lib "lib64"
#else
#define zygote_name "zygote"
#define lib "lib"
#endif
bool get_pid_by_name(pid_t *pid, char *task_name){
    DIR *dir;
    struct dirent *ptr;
    FILE *fp;
    char filepath[150];
    char cur_task_name[1024];
    char buf[1024];

    dir = opendir("/proc");
    if (NULL != dir){
        while ((ptr = readdir(dir)) != NULL){ //循环读取/proc下的每一个文件/文件夹
            //如果读取到的是"."或者".."则跳过，读取到的不是文件夹名字也跳过
            if ((strcmp(ptr->d_name, ".") == 0) || (strcmp(ptr->d_name, "..") == 0))
                continue;
            if (DT_DIR != ptr->d_type)
                continue;

            sprintf(filepath, "/proc/%s/cmdline", ptr->d_name); //生成要读取的文件的路径
            fp = fopen(filepath, "r");
            if (NULL != fp){
                if (fgets(buf, 1024 - 1, fp) == NULL){
                    fclose(fp);
                    continue;
                }
                sscanf(buf, "%s", cur_task_name);
                //如果文件内容满足要求则打印路径的名字（即进程的PID）
                if (strstr(task_name, cur_task_name)){
                    *pid = atoi(ptr->d_name);
                    return true;
                }
                fclose(fp);
            }
        }
        closedir(dir);
    }
    return false;
}


int switch_mnt_ns() {
    pid_t pid ;
    if(get_pid_by_name(&pid,zygote_name)){
        int fd = syscall(SYS_pidfd_open, pid, 0), ret = -1;
        char mnt[32];
        snprintf(mnt, sizeof(mnt), "/proc/%d/ns/mnt", pid);
        if (fd >= 0 && (ret = setns(fd, CLONE_NEWNS)) == 0){
            printf("setns fd successful\n");
            goto return_result;
        }
        close(fd);
        // fall back
        if ((fd = open(mnt, O_RDONLY)) < 0)
            return 1;
        // Switch to its namespace
        ret = setns(fd, CLONE_NEWNS);
        return_result:
        close(fd);
        return ret;
    } else{
        printf(" get_pid_by_name failed \n");
        return -1;
    }
}

int main(int argc, char **argv) {
    char * option = argv[1];
    char *source = argv[2];

    if (switch_mnt_ns() == -1){
        perror("switch_mnt_ns failed");
        return -1;
    }
    if(strncmp(option,"umount", 6) == 0){
        printf("umount %s .\n",source);
        umount(source);
        return 0;
    }

    const char *shell_script_path = option;

    // 准备传递给shell的参数数组
    char *args[] = {
            "/system/bin/sh",    // shell的路径
            argv[1],      // 第一个参数作为shell命令
            argv[2],      // 第二个参数作为shell命令
            argv[3],      // 第三个参数作为shell命令
            argv[4],      // 第三个参数作为shell命令
            NULL          // 终止数组
    };
    // 使用 execvp 执行 shell 并传递参数
    execvp(args[0], args);
    // 如果execlp返回，表示执行失败
    perror("execlp");

    return 0;
}