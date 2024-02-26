//
// Created by thehepta on 2023/12/17.
//

#pragma one


#include <elf.h>
#include <link.h>
#include <vector>
#include <string>
#include "linker_common_types.h"

struct soinfo;

struct SymbolLookupLib {
    uint32_t gnu_maskwords_ = 0;
    uint32_t gnu_shift2_ = 0;
    ElfW(Addr)* gnu_bloom_filter_ = nullptr;

    const char* strtab_;
    size_t strtab_size_;
    const ElfW(Sym)* symtab_;
    const ElfW(Versym)* versym_;

    const uint32_t* gnu_chain_;
    size_t gnu_nbucket_;
    uint32_t* gnu_bucket_;

    soinfo* si_ = nullptr;

    bool needs_sysv_lookup() const { return si_ != nullptr && gnu_bloom_filter_ == nullptr; }
};


// A list of libraries to search for a symbol.
class SymbolLookupList {
    std::vector<SymbolLookupLib> libs_;
    SymbolLookupLib sole_lib_;
    const SymbolLookupLib* begin_;
    const SymbolLookupLib* end_;
    size_t slow_path_count_ = 0;   // 可能表示某个代码段或函数存在一些特殊情况，无法通过常规的快速路径进行优化，需要使用更复杂或更慢的处理方式

public:
    SymbolLookupList(){};
//    SymbolLookupList(const soinfo_list_t& global_group, const soinfo_list_t& local_group);
//    void set_dt_symbolic_lib(soinfo* symbolic_lib);

    const SymbolLookupLib* begin() const { return begin_; }
    const SymbolLookupLib* end() const { return end_; }
    bool needs_slow_path() const { return slow_path_count_ > 0; }
    void addSymbolLib(SymbolLookupLib SyLib){
        libs_.push_back(SyLib);
        slow_path_count_+= SyLib.needs_sysv_lookup();     // 加入每个的so都会进行累计
    }
    std::vector<SymbolLookupLib> getVectorSymLib(){
        return this->libs_;
    }

};





