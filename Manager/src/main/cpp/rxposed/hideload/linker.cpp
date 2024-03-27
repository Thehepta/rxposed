//
// Created by chic on 2023/11/26.
//

#include <cstdio>
#include <locale>
#include <linux/ashmem.h>
#include "linker.h"

#include <sys/mman.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <vector>
#include "elf_symbol_resolver.h"
#include "linker_debug.h"
#include <sys/syscall.h>
#include <sys/utsname.h>
#include "jni_hook.h"


soinfo* (*solist_get_head)() = (soinfo* (*)()) linkerResolveElfInternalSymbol(
        get_android_linker_path(), "__dl__Z15solist_get_headv");

soinfo* (*solist_get_somain)() = (soinfo* (*)()) linkerResolveElfInternalSymbol(
        get_android_linker_path(), "__dl__Z17solist_get_somainv");

char* (*soinfo_get_soname)(soinfo*) = (char* (*)(soinfo*)) linkerResolveElfInternalSymbol(
        get_android_linker_path(), "__dl__ZNK6soinfo10get_sonameEv");




static inline uintptr_t untag_address(uintptr_t p) {
#if defined(__aarch64__)
    return p & ((1ULL << 56) - 1);
#else
    return p;
#endif
}

template <typename T>
static inline T* untag_address(T* p) {
    return reinterpret_cast<T*>(untag_address(reinterpret_cast<uintptr_t>(p)));
}

//通过动态计算结构内部成员的位置，循环找到soinfo 内部的next的位置，然后通过next进行遍历
soinfo* find_all_library_byname(const char* soname){
    std::vector<void *> linker_solist;

    static uintptr_t *solist_head = NULL;
    if (!solist_head)
        solist_head = (uintptr_t *)solist_get_head();


    static uintptr_t somain = 0;

    if (!somain)
        somain = (uintptr_t)solist_get_somain();

    // Generate the name for an offset.
#define PARAM_OFFSET(type_, member_) __##type_##__##member_##__offset_
#define STRUCT_OFFSET PARAM_OFFSET
    int STRUCT_OFFSET(solist, next) = 0;
    for (size_t i = 0; i < 1024 / sizeof(void *); i++) {
        if (*(uintptr_t *)((uintptr_t)solist_head + i * sizeof(void *)) == somain) {
            STRUCT_OFFSET(solist, next) = i * sizeof(void *);
            break;
        }
    }

    linker_solist.push_back(solist_head);

    uintptr_t sonext = 0;
    sonext = *(uintptr_t *)((uintptr_t)solist_head + STRUCT_OFFSET(solist, next));
    while (sonext) {
        linker_solist.push_back((void *)sonext);
        sonext = *(uintptr_t *)((uintptr_t)sonext + STRUCT_OFFSET(solist, next));
        if(sonext == 0){
            continue;
        }
        char* ret_name = soinfo_get_soname(reinterpret_cast<soinfo *>(sonext));
        LOGE("get_soname : %s",ret_name);
        LOGE("get_soname == null so->realpath: %s",((soinfo*)sonext)->get_realpath());

    }

    return nullptr;
}

soinfo* find_system_library_byname(const char* soname) {

    for (soinfo* si = solist_get_head(); si != nullptr; si = si->next) {
        char* ret_name = soinfo_get_soname(si);
        if(ret_name!= nullptr){
            if(0 == strncmp(ret_name,soname, strlen(soname))) {
                return si;
            }
        }

    }
    return nullptr;
}

soinfo* find_containing_library(const void* p) {

//    static soinfo* (*solist_get_head)() = NULL;
//    if (!solist_get_head)
//        solist_get_head = (soinfo* (*)())linkerResolveElfInternalSymbol(get_android_linker_path(), "__dl__Z15solist_get_headv");

//    static soinfo* (*solist_get_somain)() = NULL;
//    if (!solist_get_somain)
//        solist_get_somain = (soinfo* (*)())linkerResolveElfInternalSymbol(get_android_linker_path(), "__dl__Z17solist_get_somainv");

    ElfW(Addr) address = reinterpret_cast<ElfW(Addr)>(untag_address(p));
    for (soinfo* si = solist_get_head(); si != nullptr; si = si->next) {
        if (address < si->base || address - si->base >= si->size) {
            continue;
        }
        ElfW(Addr) vaddr = address - si->load_bias;
        for (size_t i = 0; i != si->phnum; ++i) {
            const ElfW(Phdr)* phdr = &si->phdr[i];
            if (phdr->p_type != PT_LOAD) {
                continue;
            }
            if (vaddr >= phdr->p_vaddr && vaddr < phdr->p_vaddr + phdr->p_memsz) {
                return si;
            }
        }
    }
    return nullptr;
}


int memfd_create(const char* name, unsigned int flags) {
    // Check kernel version supports memfd_create(). Some older kernels segfault executing
    // memfd_create() rather than returning ENOSYS (b/116769556).
    static constexpr int kRequiredMajor = 3;
    static constexpr int kRequiredMinor = 17;
    struct utsname uts;
    int major, minor;
    if (uname(&uts) != 0 ||
        strcmp(uts.sysname, "Linux") != 0 ||
        sscanf(uts.release, "%d.%d", &major, &minor) != 2 ||
        (major < kRequiredMajor || (major == kRequiredMajor && minor < kRequiredMinor))) {
        errno = ENOSYS;
        return -1;
    }
    return syscall(__NR_memfd_create, name, flags);
}




uint8_t * Creatememfd(int *fd, int size){

    *fd = memfd_create("test", MFD_CLOEXEC);
    if (*fd < 0) {
        perror("Creatememfd Filede: open /dev/ashmem failed");
        exit(EXIT_FAILURE);
    }

    ftruncate(*fd, size);

    uint8_t *ptr = static_cast<uint8_t *>(mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, *fd,0));
    if (ptr == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }
    return ptr;
}



//只支持单so，删除互相依赖处理
soinfo* find_library(std::vector<LoadTask*> &load_tasks,const char *soname) {

//    LoadTask* find_soinfo = nullptr;
//    for (auto&& task : load_tasks) {
//        if(0 == strncmp(task->get_soinfo()->get_soname(),soname, strlen(soname))){
//            find_soinfo = task;
//        }
//    }
//    if(find_soinfo != nullptr) {
//        if(find_soinfo->get_soinfo()->is_linked()){
//            find_soinfo->soload(load_tasks, nullptr);
//        }
//        return find_soinfo->get_soinfo();
//
//    }else{
    return find_system_library_byname(soname);
//    }

}

const char* fix_dt_needed(const char* dt_needed, const char* sopath __unused) {
#if !defined(__LP64__)
    int app_target_api_level = android_get_application_target_sdk_version();
    if (app_target_api_level < 23) {
        const char* bname = basename(dt_needed);
        if (bname != dt_needed) {
            DL_WARN_documented_change(23,
                                      "invalid-dt_needed-entries-enforced-for-api-level-23",
                                      "library \"%s\" has invalid DT_NEEDED entry \"%s\"",
                                      sopath, dt_needed, app_target_api_level);
//      add_dlwarning(sopath, "invalid DT_NEEDED entry",  dt_needed);
        }

        return bname;
    }
#endif
    return dt_needed;
}


void LoadTask::soload(std::vector<LoadTask *> &load_tasks, JNIEnv *pEnv) {

    SymbolLookupList lookup_list;

    for (const ElfW(Dyn)* d = get_elf_reader().dynamic(); d->d_tag != DT_NULL; ++d) {
        if (d->d_tag == DT_NEEDED) {
            const char* name = fix_dt_needed(get_elf_reader().get_string(d->d_un.d_val), get_elf_reader().name());
            LOGE("NEED name: %s",name);
            soinfo* system_si = find_library(load_tasks, name);
            soinfo *custom_si  = new soinfo();
            custom_si->set_soname(name);
            custom_si->transform(system_si);
            SymbolLookupLib SyLib = custom_si->get_lookup_lib(true);
            lookup_list.addSymbolLib(SyLib);
        }
    }


    address_space_params  default_params;
    load(&default_params);
    get_soinfo()->prelink_image();
    get_soinfo()->set_dt_flags_1(get_soinfo()->get_dt_flags_1() | DF_1_GLOBAL);
    lookup_list.addSymbolLib(get_soinfo()->get_lookup_lib(false));
    get_soinfo()->link_image(lookup_list);
    get_soinfo()->set_linked();
    get_soinfo()->call_constructors();

    auto it =     lookup_list.getVectorSymLib().begin();
    auto end = lookup_list.getVectorSymLib().end();
    std::vector<SymbolLookupLib>::iterator  lib ;
    while (true) {
        if (it == end) return;
        lib = it++;
//        if(lib->system_sonifo){
            delete lib->si_ ;
//        }
    }
}

void LoadTask::init_call(JNIEnv *pEnv, jobject g_currentDexLoad) {


    SymbolName symbol_JNI_OnLoad("JNI_OnLoad");
    const ElfW(Sym)* sym = get_soinfo()->find_symbol_by_name(symbol_JNI_OnLoad, nullptr);
    if(sym== nullptr){
        return;
    }
    int(*JNI_OnLoadFn)(JavaVM*, void*) = ( int(*)(JavaVM*, void*))(sym->st_value+get_soinfo()->load_bias);

    JavaVM *vm;
    pEnv->GetJavaVM(reinterpret_cast<JavaVM **>(&vm));
    void * linkerJniInvokeInterface = jni_hook_init(vm,g_currentDexLoad);
    JNI_OnLoadFn(static_cast<JavaVM *>(linkerJniInvokeInterface), nullptr);
}

void LoadTask::hack() {
    ElfReader & elfReader = get_elf_reader();
    elfReader.loaded_phdr();
}



//void LoadTask::hideso() {
//
//    solist_remove_soinfo(this->get_soinfo());
//
//}






jobject hideLoadApkModule(JNIEnv *env, mz_zip_archive& zip_archive){

    jobject currentDexLoad = nullptr;
    auto classloader = env->FindClass("java/lang/ClassLoader");
    auto getsyscl_mid = env->GetStaticMethodID(classloader, "getSystemClassLoader", "()Ljava/lang/ClassLoader;");
    auto sys_classloader = env->CallStaticObjectMethod(classloader, getsyscl_mid);

    if (!sys_classloader){
        LOGE("getSystemClassLoader failed!!!");
        return nullptr;
    }
    auto in_memory_classloader = env->FindClass("dalvik/system/InMemoryDexClassLoader");
    auto initMid = env->GetMethodID(in_memory_classloader, "<init>", "([Ljava/nio/ByteBuffer;Ljava/lang/ClassLoader;)V");
    auto byte_buffer_class = env->FindClass("java/nio/ByteBuffer");


    std::unordered_map<const soinfo*, ElfReader> readers_map;
    std::vector<LoadTask*> load_tasks;
    std::vector<jobject> load_dexs;

    std::vector<ApkNativeInfo> vec_apkNativeInfo;
    int file_count = (int)mz_zip_reader_get_num_files(&zip_archive);
    for (int i = 0; i < file_count; i++) {
        mz_zip_archive_file_stat file_stat;
        if (!mz_zip_reader_file_stat(&zip_archive, i, &file_stat)) {
            printf("Could not retrieve file info.\n");
            mz_zip_reader_end(&zip_archive);
            return nullptr;
        }
        if(strstr(file_stat.m_filename,APK_NATIVE_LIB)!= NULL) {
            int fd;
            uint8_t * somem_addr = Creatememfd(&fd, file_stat.m_uncomp_size);

            if (!somem_addr) {
                printf("Failed to allocate memory.\n");
                mz_zip_reader_end(&zip_archive);
                return nullptr;
            }
            if (!mz_zip_reader_extract_to_mem(&zip_archive, i, somem_addr, file_stat.m_uncomp_size, 0)) {
                printf("Failed to extract file.\n");
                mz_zip_reader_end(&zip_archive);
                return nullptr;
            }

            LoadTask* task =  new LoadTask(file_stat.m_filename, nullptr, nullptr, &readers_map);
            task->set_fd(fd, false);
            task->set_file_offset(0);
            task->set_file_size(file_stat.m_uncomp_size);
            load_tasks.push_back(task);
//            printf("Filename: \"%s\", Comment: \"%s\", Uncompressed size: %llu\n",file_stat.m_filename, file_stat.m_comment ? file_stat.m_comment : "",(mz_uint64) file_stat.m_uncomp_size);
        }
        if(strstr(file_stat.m_filename, ".dex") != NULL){
            uint8_t *file_data = (unsigned char *)malloc(file_stat.m_uncomp_size);
            if (!file_data) {
                printf("Memory allocation failed\n");
                mz_zip_reader_end(&zip_archive);
                return nullptr;
            }
            if (!mz_zip_reader_extract_to_mem(&zip_archive, i, file_data, file_stat.m_uncomp_size, 0)) {
                printf("Failed to extract file\n");
                free(file_data);
                mz_zip_reader_end(&zip_archive);
                return nullptr;
            }
            auto dex_buffer = env->NewDirectByteBuffer(file_data, file_stat.m_uncomp_size);
            load_dexs.push_back(dex_buffer);
        }
    }
    mz_zip_reader_end(&zip_archive);

    jobjectArray byteBuffers = env->NewObjectArray(load_dexs.size(), byte_buffer_class, NULL);

    for (size_t i = 0; i<load_dexs.size(); ++i) {
        jobject load_dex = load_dexs[i];
        env->SetObjectArrayElement(byteBuffers, i, load_dex);
    }
    currentDexLoad = env->NewObject(in_memory_classloader, initMid, byteBuffers, sys_classloader);
    if (currentDexLoad != nullptr ) {

        jobject g_currentDexLoad =  env->NewGlobalRef(currentDexLoad);
//        linker_protect();
        for (size_t i = 0; i<load_tasks.size(); ++i) {

            LoadTask* task = load_tasks[i];
//            soinfo* si = soinf_alloc_fun(g_default_namespace, ""/*real path*/, nullptr, 0, RTLD_GLOBAL);
            soinfo* si = new soinfo(nullptr, ""/*real path*/, nullptr, 0, RTLD_GLOBAL);
            if (si == nullptr) {
                return nullptr;
            }
            task->set_soinfo(si);

            if (!task->read()) {
//            soinfo_free(si);
                task->set_soinfo(nullptr);
                return nullptr;
            }
            const ElfReader& elf_reader = task->get_elf_reader();
            for (const ElfW(Dyn)* d = elf_reader.dynamic(); d->d_tag != DT_NULL; ++d) {
                if (d->d_tag == DT_RUNPATH) {
                    si->set_dt_runpath(elf_reader.get_string(d->d_un.d_val));
                }
                if (d->d_tag == DT_SONAME) {
                    si->set_soname(elf_reader.get_string(d->d_un.d_val));
                }
            }
        }


        for (auto&& task : load_tasks) {
            task->soload(load_tasks, env);
            task->init_call(env, g_currentDexLoad);
            task->hack();
            delete task;
        }

//        linker_unprotect();
        env->DeleteGlobalRef(g_currentDexLoad);
        return currentDexLoad;
    } else{
        return nullptr;
    }
}




jobject FilehideLoadApkModule(JNIEnv *env, char * apkSource){


    mz_zip_archive zip_archive;
    memset(&zip_archive, 0, sizeof(zip_archive));

    mz_bool status = mz_zip_reader_init_file(&zip_archive, apkSource, 0);
    if (!status) {
        printf("Could not initialize zip reader.\n");
        return nullptr;
    }
    return hideLoadApkModule(env,zip_archive);
}


jobject memhideLoadApkModule(JNIEnv *env, unsigned char *zip_data, size_t zip_size) {


    // 使用 miniz 库解压缩内存中的 zip 文件
    mz_zip_archive zip_archive;
    memset(&zip_archive, 0, sizeof(zip_archive));
    mz_bool status = mz_zip_reader_init_mem(&zip_archive, zip_data, zip_size, 0);
    if (!status) {
        printf("Could not initialize zip reader.\n");
        return nullptr;
    }
    jobject resutl_classloader = hideLoadApkModule(env,zip_archive);
    free(zip_data); // 释放内存
    return resutl_classloader;

}

uintptr_t ToPathLoadSoGetSymbolAddr(char * sopath,char *topath,char*call_arg){

}

uintptr_t ToMemLoadSoGetSymbolAddr(char * sopath,char*call_arg){

}













































