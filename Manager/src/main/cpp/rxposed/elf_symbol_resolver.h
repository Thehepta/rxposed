//
// Created by chic on 2023/6/2.
//

#ifndef THEPTAVPN_ELF_SYMBOL_RESOLVER_H
#define THEPTAVPN_ELF_SYMBOL_RESOLVER_H

//和dobby 符号冲突了，所以改了一下

void *linker_resolve_elf_internal_symbol(const char *library_name, const char *symbol_name) ;


#endif //THEPTAVPN_ELF_SYMBOL_RESOLVER_H
