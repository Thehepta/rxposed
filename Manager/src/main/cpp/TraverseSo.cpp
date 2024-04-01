//
// Created by chic on 2024/2/19.
//

#include "android/log.h"
#include "rxposed/hideload/elf_symbol_resolver.h"
#include "TraverseSo.h"
#define LOG_TAG "sotravers"

#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)



soinfo* (*solist_get_head)() = (soinfo* (*)()) linkerResolveElfInternalSymbol(
        get_android_linker_path(), "__dl__Z15solist_get_headv");

soinfo* (*solist_get_somain)() = (soinfo* (*)()) linkerResolveElfInternalSymbol(
        get_android_linker_path(), "__dl__Z17solist_get_somainv");

char* (*soinfo_get_soname)(soinfo*) = (char* (*)(soinfo*)) linkerResolveElfInternalSymbol(
        get_android_linker_path(), "__dl__ZNK6soinfo10get_sonameEv");

bool (*solist_remove_soinfo)(soinfo*) = (bool  (*)(soinfo*)) linkerResolveElfInternalSymbol(
        get_android_linker_path(), "__dl__Z20solist_remove_soinfoP6soinfo");

char* (*soinfo_get_realpath)(soinfo*) = (char* (*)(soinfo*)) linkerResolveElfInternalSymbol(
        get_android_linker_path(), "__dl__ZNK6soinfo12get_realpathEv");



void text(){
    char* (*soinfo_get_realpath)(soinfo*) = (char* (*)(soinfo*)) linkerResolveElfInternalSymbol(
            get_android_linker_path(), "__dl__ZNK6soinfo12get_realpathEv");

    char* (*soinfo_get_realpath_bsee64)(soinfo*) = (char* (*)(soinfo*)) linkerResolveElfInternalSymbolBase64(
            get_android_linker_path(), "X19kbF9fWk5LNnNvaW5mbzEyZ2V0X3JlYWxwYXRoRXY=");



    if(*soinfo_get_realpath == *soinfo_get_realpath_bsee64){
        LOGE("soinfo_get_realpath == soinfo_get_realpath_bsee64");

    } else{
        LOGE("soinfo_get_realpath != soinfo_get_realpath_bsee64");

    }

    return;
}

//通过动态计算结构内部成员的位置，循环找到soinfo 内部的next的位置，然后通过next进行遍历
soinfo* find_all_library_byname(const char* soname){

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


    uintptr_t sonext = 0;
    sonext = *(uintptr_t *)((uintptr_t)solist_head + STRUCT_OFFSET(solist, next));
    while (sonext) {
        sonext = *(uintptr_t *)((uintptr_t)sonext + STRUCT_OFFSET(solist, next));
        if(sonext == 0){
            continue;
        }
        char* so_name = soinfo_get_soname(reinterpret_cast<soinfo *>(sonext));
        char* so_realpath = soinfo_get_realpath(reinterpret_cast<soinfo *>(sonext));

        LOGE("so->addr  : 0x%lx", sonext);
        LOGE("so->soname  : %s", so_name);
        LOGE("so->realpath: %s",  so_realpath);
    }
    return nullptr;
}