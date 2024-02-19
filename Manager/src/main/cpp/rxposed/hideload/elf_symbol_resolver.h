//
// Created by chic on 2023/6/2.
//

#pragma once

int get_android_system_version() {
    char os_version_str[100];
    __system_property_get("ro.build.version.sdk", os_version_str);
    int os_version_int = atoi(os_version_str);
    return os_version_int;
}


const char *get_android_linker_path() {
#if __LP64__
    if (get_android_system_version() >= 29) {
        return (const char *)"/apex/com.android.runtime/bin/linker64";
    } else {
        return (const char *)"/system/bin/linker64";
    }
#else
    if (get_android_system_version() >= 29) {
        return (const char *)"/apex/com.android.runtime/bin/linker";
    } else {
        return (const char *)"/system/bin/linker";
    }
#endif
}
void *linker_resolve_elf_internal_symbol(const char *library_name, const char *symbol_name) ;

void *linker_resolve_elf_internal_symbol_base64(const char *library_name, const char *symbol_name);
