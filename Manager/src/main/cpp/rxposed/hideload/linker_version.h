//
// Created by thehepta on 2023/12/17.
//

#pragma once

#include "linker_soinfo.h"


class VersionTracker {

public:
    VersionTracker() = default;
    bool init(const soinfo *si_from, const SymbolLookupList &list);

    const version_info * get_version_info(ElfW(Versym) source_symver) const;
private:
    bool init_verneed(const soinfo *si_from,  SymbolLookupList list);
    bool init_verdef(const soinfo* si_from);
    void add_version_info(size_t source_index, ElfW(Word) elf_hash,
                          const char* ver_name, const soinfo* target_si);

    std::vector<version_info> version_infos;

    DISALLOW_COPY_AND_ASSIGN(VersionTracker);
};



ElfW(Versym) find_verdef_version_index(const soinfo* si, const version_info* vi);


constexpr ElfW(Versym) kVersymNotNeeded = 0;
constexpr ElfW(Versym) kVersymGlobal = 1;