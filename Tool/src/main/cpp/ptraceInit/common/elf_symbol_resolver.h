//
// Created by chic on 2023/6/2.
//

#pragma once



class SymbolName {
public:
    explicit SymbolName(const char* name)
            : name_(name), has_elf_hash_(false), has_gnu_hash_(false),
              elf_hash_(0), gnu_hash_(0) { }

    const char* get_name() {
        return name_;
    }


    uint32_t elf_hash();
    uint32_t gnu_hash();


private:
    const char* name_;
    bool has_elf_hash_;
    bool has_gnu_hash_;
    uint32_t elf_hash_;
    uint32_t gnu_hash_;

};

void *get_remote_load_Sym_Addr(void *so_addr, pid_t pid, const char *symbol_name) ;

void *get_self_load_Sym_Addr(const char *library_name, const char *symbol_name) ;

uintptr_t get_libFile_Symbol_off(char *lib_path,char *fun_name);