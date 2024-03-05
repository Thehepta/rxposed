//
// Created by chic on 2023/11/25.
//

#pragma once


#include <string>
//#include "linker.h"
#include <elf.h>
#include <link.h>
#include "linker_mapped_file_fragment.h"


struct address_space_params {
    void* start_addr = nullptr;
    size_t reserved_size = 0;
    bool must_use_address = false;
};




class ElfReader {
public:
    ElfReader();

    bool Read(const char* name, int fd, off64_t file_offset, off64_t file_size);
    bool Load(address_space_params* address_space);

    const char* name() const { return name_.c_str(); }
    size_t phdr_count() const { return phdr_num_; }
    ElfW(Addr) load_start() const { return reinterpret_cast<ElfW(Addr)>(load_start_); }
    size_t load_size() const { return load_size_; }
    ElfW(Addr) load_bias() const { return load_bias_; }
    const ElfW(Phdr)* loaded_phdr() const { return loaded_phdr_; }
    const ElfW(Dyn)* dynamic() const { return dynamic_; }
    const char* get_string(ElfW(Word) index) const;
    bool is_mapped_by_caller() const { return mapped_by_caller_; }
    ElfW(Addr) entry_point() const { return header_.e_entry + load_bias_; }

private:
    bool ReadElfHeader();
    bool VerifyElfHeader();
    bool ReadProgramHeaders();
    bool ReadSectionHeaders();
    bool ReadDynamicSection();
    bool ReserveAddressSpace(address_space_params* address_space);
    bool LoadSegments();
    bool FindPhdr();
    bool CheckPhdr(ElfW(Addr));
    bool CheckFileRange(ElfW(Addr) offset, size_t size, size_t alignment);

    bool did_read_;
    bool did_load_;
    std::string name_;
    int fd_;
    off64_t file_offset_;   //elf EHdr所在的文件，映射到内存以后，相较于  file_offset_ =  elf EHdr地址 - 映射首地址，一般为0,大部分文件elf ehdr 都位于首地址
    off64_t file_size_;

    ElfW(Ehdr) header_;
    size_t phdr_num_;

    MappedFileFragment phdr_fragment_;
    const ElfW(Phdr)* phdr_table_;

    MappedFileFragment shdr_fragment_;
    const ElfW(Shdr)* shdr_table_;
    size_t shdr_num_;

    MappedFileFragment dynamic_fragment_;
    const ElfW(Dyn)* dynamic_;

    MappedFileFragment strtab_fragment_;
    const char* strtab_;
    size_t strtab_size_;

    // First page of reserved address space.
    void* load_start_;
    // Size in bytes of reserved address space.
    size_t load_size_;
    // Load bias.
    ElfW(Addr) load_bias_;

    // Loaded phdr.
    const ElfW(Phdr)* loaded_phdr_;

    // Is map owned by the caller
    bool mapped_by_caller_;
};

size_t phdr_table_get_load_size(const ElfW(Phdr)* phdr_table, size_t phdr_count,
        ElfW(Addr)* min_vaddr = nullptr, ElfW(Addr)* max_vaddr = nullptr);

int phdr_table_protect_segments(const ElfW(Phdr)* phdr_table,
size_t phdr_count, ElfW(Addr) load_bias);

int phdr_table_unprotect_segments(const ElfW(Phdr)* phdr_table, size_t phdr_count,
        ElfW(Addr) load_bias);

int phdr_table_protect_gnu_relro(const ElfW(Phdr)* phdr_table, size_t phdr_count,
        ElfW(Addr) load_bias);

int phdr_table_serialize_gnu_relro(const ElfW(Phdr)* phdr_table, size_t phdr_count,
        ElfW(Addr) load_bias, int fd, size_t* file_offset);

int phdr_table_map_gnu_relro(const ElfW(Phdr)* phdr_table, size_t phdr_count,
        ElfW(Addr) load_bias, int fd, size_t* file_offset);

#if defined(__arm__)
int phdr_table_get_arm_exidx(const ElfW(Phdr)* phdr_table, size_t phdr_count, ElfW(Addr) load_bias,
                             ElfW(Addr)** arm_exidx, size_t* arm_exidix_count);
#endif

void phdr_table_get_dynamic_section(const ElfW(Phdr)* phdr_table, size_t phdr_count,
        ElfW(Addr) load_bias, ElfW(Dyn)** dynamic,
ElfW(Word)* dynamic_flags);

const char* phdr_table_get_interpreter_name(const ElfW(Phdr) * phdr_table, size_t phdr_count,
        ElfW(Addr) load_bias);


