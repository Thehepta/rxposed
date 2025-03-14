//
// Created by chic on 2023/6/2.
//

#pragma once


void *ResolveElfInternalSymbol(const char *library_name, const char *symbol_name) ;

long get_libFile_Symbol_off(char *lib_path,char *fun_name);