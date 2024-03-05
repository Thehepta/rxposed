//
// Created by thehepta on 2023/12/17.
//

#pragma once

#include "linker_reloc_iterators.h"

static constexpr ElfW(Versym) kVersymHiddenBit = 0x8000;


class VersionTracker;
struct soinfo;
struct version_info;
struct SymbolLookupList;
enum RelocationKind {
    kRelocAbsolute = 0,
    kRelocRelative,
    kRelocSymbol,
    kRelocSymbolCached,
    kRelocMax
};


#define R_GENERIC_NONE 0 // R_*_NONE is always 0

#if defined (__aarch64__)

#define R_GENERIC_JUMP_SLOT     R_AARCH64_JUMP_SLOT
// R_AARCH64_ABS64 is classified as a static relocation but it is common in DSOs.
#define R_GENERIC_ABSOLUTE      R_AARCH64_ABS64
#define R_GENERIC_GLOB_DAT      R_AARCH64_GLOB_DAT
#define R_GENERIC_RELATIVE      R_AARCH64_RELATIVE
#define R_GENERIC_IRELATIVE     R_AARCH64_IRELATIVE
#define R_GENERIC_COPY          R_AARCH64_COPY
#define R_GENERIC_TLS_DTPMOD    R_AARCH64_TLS_DTPMOD
#define R_GENERIC_TLS_DTPREL    R_AARCH64_TLS_DTPREL
#define R_GENERIC_TLS_TPREL     R_AARCH64_TLS_TPREL
#define R_GENERIC_TLSDESC       R_AARCH64_TLSDESC

#elif defined (__arm__)

#define R_GENERIC_JUMP_SLOT     R_ARM_JUMP_SLOT
// R_ARM_ABS32 is classified as a static relocation but it is common in DSOs.
#define R_GENERIC_ABSOLUTE      R_ARM_ABS32
#define R_GENERIC_GLOB_DAT      R_ARM_GLOB_DAT
#define R_GENERIC_RELATIVE      R_ARM_RELATIVE
#define R_GENERIC_IRELATIVE     R_ARM_IRELATIVE
#define R_GENERIC_COPY          R_ARM_COPY
#define R_GENERIC_TLS_DTPMOD    R_ARM_TLS_DTPMOD32
#define R_GENERIC_TLS_DTPREL    R_ARM_TLS_DTPOFF32
#define R_GENERIC_TLS_TPREL     R_ARM_TLS_TPOFF32
#define R_GENERIC_TLSDESC       R_ARM_TLS_DESC

#elif defined (__i386__)

#define R_GENERIC_JUMP_SLOT     R_386_JMP_SLOT
#define R_GENERIC_ABSOLUTE      R_386_32
#define R_GENERIC_GLOB_DAT      R_386_GLOB_DAT
#define R_GENERIC_RELATIVE      R_386_RELATIVE
#define R_GENERIC_IRELATIVE     R_386_IRELATIVE
#define R_GENERIC_COPY          R_386_COPY
#define R_GENERIC_TLS_DTPMOD    R_386_TLS_DTPMOD32
#define R_GENERIC_TLS_DTPREL    R_386_TLS_DTPOFF32
#define R_GENERIC_TLS_TPREL     R_386_TLS_TPOFF
#define R_GENERIC_TLSDESC       R_386_TLS_DESC

#elif defined (__x86_64__)

#define R_GENERIC_JUMP_SLOT     R_X86_64_JUMP_SLOT
#define R_GENERIC_ABSOLUTE      R_X86_64_64
#define R_GENERIC_GLOB_DAT      R_X86_64_GLOB_DAT
#define R_GENERIC_RELATIVE      R_X86_64_RELATIVE
#define R_GENERIC_IRELATIVE     R_X86_64_IRELATIVE
#define R_GENERIC_COPY          R_X86_64_COPY
#define R_GENERIC_TLS_DTPMOD    R_X86_64_DTPMOD64
#define R_GENERIC_TLS_DTPREL    R_X86_64_DTPOFF64
#define R_GENERIC_TLS_TPREL     R_X86_64_TPOFF64
#define R_GENERIC_TLSDESC       R_X86_64_TLSDESC

#endif

#if defined(__LP64__)
#define ELFW(what) ELF64_ ## what
#define APK_NATIVE_LIB "lib/arm64-v8a"
#else
#define ELFW(what) ELF32_ ## what
#define APK_NATIVE_LIB "lib/armeabi-v7a"
#endif

enum class RelocMode {
    // Fast path for JUMP_SLOT relocations.
    JumpTable,
    // Fast path for typical relocations: ABSOLUTE, GLOB_DAT, or RELATIVE.
    Typical,
    // Handle all relocation types, relocations in text sections, and statistics/tracing.
    General,
};


#if defined(USE_RELA)
typedef ElfW(Rela) rel_t;
#else
typedef ElfW(Rel) rel_t;
#endif




class Relocator {
public:
    Relocator(const VersionTracker& version_tracker, const SymbolLookupList& lookup_list)
            : version_tracker(version_tracker), lookup_list(lookup_list)
    {}

    soinfo* si = nullptr;
    const char* si_strtab = nullptr;
    size_t si_strtab_size = 0;
    ElfW(Sym)* si_symtab = nullptr;

    const VersionTracker& version_tracker;
    const SymbolLookupList& lookup_list;

    // Cache key
    ElfW(Word) cache_sym_val = 0;
    // Cache value
    const ElfW(Sym)* cache_sym = nullptr;
    soinfo* cache_si = nullptr;

//    std::vector<TlsDynamicResolverArg>* tlsdesc_args;
//    std::vector<std::pair<TlsDescriptor*, size_t>> deferred_tlsdesc_relocs;
//    size_t tls_tp_base = 0;

    __attribute__((always_inline))
    const char* get_string(ElfW(Word) index);
};
__attribute__((noinline))
bool process_relocation_general(Relocator& relocator, const rel_t& reloc);

template <RelocMode OptMode, typename ...Args>
bool packed_relocate(Relocator& relocator, Args ...args);
