#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sys/uio.h>
#include <unistd.h>
#include <errno.h>
#include <stdint.h>
#include <fcntl.h>

void read_memory(pid_t pid, void *remote_addr, size_t size, const char *output_file) {
    struct iovec local_iov;
    struct iovec remote_iov;
    uint8_t *buffer = static_cast<uint8_t *>(malloc(size));
    if (!buffer) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    local_iov.iov_base = buffer;
    local_iov.iov_len = size;
    remote_iov.iov_base = remote_addr;
    remote_iov.iov_len = size;

    ssize_t bytes_read = process_vm_readv(pid, &local_iov, 1, &remote_iov, 1, 0);
    if (bytes_read == -1) {
        perror("process_vm_readv");
        free(buffer);
        exit(EXIT_FAILURE);
    }
    printf("Read %zd bytes from process %d at address %p\n", bytes_read, pid, remote_addr);

    int fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        perror("open");
        free(buffer);
        exit(EXIT_FAILURE);
    }
    write(fd, buffer, bytes_read);
    close(fd);
    free(buffer);
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <pid> <start_hex_address> <end_hex_address>\n", argv[0]);
        return EXIT_FAILURE;
    }

    pid_t pid = atoi(argv[1]);
    void *start_addr = (void *)strtoul(argv[2], NULL, 16);
    void *end_addr = (void *)strtoul(argv[3], NULL, 16);
    if (start_addr >= end_addr) {
        fprintf(stderr, "Invalid address range\n");
        return EXIT_FAILURE;
    }
    size_t size = (size_t)((uintptr_t)end_addr - (uintptr_t)start_addr);

    read_memory(pid, start_addr, size, "dump.bin");
    printf("Data saved to dump.bin\n");

    return EXIT_SUCCESS;
}
