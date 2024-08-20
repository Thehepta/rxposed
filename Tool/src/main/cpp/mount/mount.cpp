
#include <sys/mount.h>
#include <cstdio>
#include <unistd.h>
#include <sys/syscall.h>
#include <linux/sched.h>
#include <sched.h>
#include <asm-generic/fcntl.h>
#include <fcntl.h>
#include <string.h>

int switch_mnt_ns(int pid) {
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
}

int main(int argc, char **argv) {
    const char * option = argv[1];
    const char *source = argv[2];

    if (switch_mnt_ns(1) == -1){
        perror("switch_mnt_ns failed");
        return -1;
    }
    if(strncmp(option,"umount", 6) == 0){
        printf("umount %s .\n",source);
        umount(source);
        return 0;
    }

    const char *filesystem_type = argv[3];
    const char *target = argv[4];
    unsigned long mountflags = 0;
    const void *data = nullptr;




        int result = mount(source, target, filesystem_type, mountflags, data);
    if (result == 0) {
        printf("Mount %s successful.\n",target);
    } else {
        printf("Mount %s failed.\n",target);
        perror("Mount failed");
    }

    return 0;
}