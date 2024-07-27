# 说明

### InjectTool

```
void *get_remote_func_addr(pid_t pid, const char *ModuleName, void *LocalFuncAddr){
void *LocalModuleAddr, *RemoteModuleAddr, *RemoteFuncAddr;
//获取本地某个模块的起始地址
LocalModuleAddr = get_module_base_addr(-1, ModuleName);
//获取远程pid的某个模块的起始地址
RemoteModuleAddr = get_module_base_addr(pid, ModuleName);
// local_addr - local_handle的值为指定函数(如mmap)在该模块中的偏移量，然后再加上remote_handle，结果就为指定函数在目标进程的虚拟地址
RemoteFuncAddr = (void *)((uintptr_t)LocalFuncAddr - (uintptr_t)LocalModuleAddr + (uintptr_t)RemoteModuleAddr);

    printf("[+][get_remote_func_addr] lmod=0x%lX, rmod=0x%lX, lfunc=0x%lX, rfunc=0x%lX\n", LocalModuleAddr, RemoteModuleAddr, LocalFuncAddr, RemoteFuncAddr);
    return RemoteFuncAddr;
}

```

通过将需要在目标进程中通过ptrace调用的函数，在本进程中计算出基于加载模块的偏移位置,然后获取目标对应加载模块的基址，通过目标模块基址加上函数偏移位置计算出目标进程中函数的地址直接进行调用
抄的大佬的