//
// Created by chic on 2023/6/2.
//

#pragma once


int get_android_system_version();
const char *get_android_linker_path();
void *linkerResolveElfInternalSymbol(const char *library_name, const char *symbol_name) ;

void *linkerResolveElfInternalSymbolBase64(const char *library_name, const char *symbol_name);
