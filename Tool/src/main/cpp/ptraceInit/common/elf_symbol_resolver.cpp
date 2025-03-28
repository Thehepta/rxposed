
#include <dlfcn.h>
#include <linux/elf.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <dlfcn.h>
#include <sys/mman.h>
#include <fstream>
#include "vector"
#include <elf.h>
#include <dlfcn.h>
#include <link.h>
#include <sys/mman.h>
#include <string>
#include <vector>
#include <algorithm>
#include <arm_neon.h>
#include <stdint.h>
#include <stdlib.h>
#include "elf_symbol_resolver.h"
#define LINE_MAX 2048

#include <utility>
#if defined(__arm__) || defined(__aarch64__)
#define USE_GNU_HASH_NEON 1
#else
#define USE_GNU_HASH_NEON 0
#endif


#ifdef __LP64__
#define __PRI_64_prefix "l"
#define __PRI_PTR_prefix "l"
#else
#define __PRI_64_prefix "ll"
#define __PRI_PTR_prefix
#endif

#define PRIxPTR __PRI_PTR_prefix "x" /* uintptr_t */

#define FLAG_GNU_HASH         0x00000040 // uses gnu hash
#define powerof2(x)     ((((x) - 1) & (x)) == 0)



typedef struct elfctx {
    void *header;
    ElfW(Addr) base;
    size_t phnum;
    const ElfW(Phdr)* phdr;

    ElfW(Addr) load_bias;

    ElfW(Dyn)* dynamic;

    ElfW(Shdr) * sym_sh_;
    ElfW(Shdr) * dynsym_sh_;

    const char *strtab_;
    ElfW(Sym) * symtab_;

    const char *dynstrtab_;
    ElfW(Sym) * dynsymtab_;

    size_t nbucket_;
    size_t nchain_;
    uint32_t *bucket_;
    uint32_t *chain_;
    uint32_t flags_;
    size_t strtab_size_;

    size_t gnu_nbucket_;
    uint32_t *gnu_bucket_;
    uint32_t *gnu_chain_;
    uint32_t gnu_maskwords_;
    uint32_t gnu_shift2_;
    ElfW(Addr) * gnu_bloom_filter_;

    bool is_gnu_hash() const {
        return (flags_ & FLAG_GNU_HASH) != 0;
    }

    void transform(ElfW(Ehdr) *ehdr) {
        this->base = reinterpret_cast<ElfW(Addr)>(ehdr);
//    this->size = lib_si->size;
        this->flags_ = 0;
        this->phnum = ehdr->e_phnum;
        this->phdr = reinterpret_cast<const ElfW(Phdr)   *>(this->base + ehdr->e_phoff);
        this->load_bias = reinterpret_cast<ElfW(Addr)>(ehdr);
        this->prelink_image();
    }
    const ElfW(Sym) * find_symbol_by_name(SymbolName &symbol_name) const {
        return is_gnu_hash() ? gnu_lookup(symbol_name) : elf_lookup(symbol_name);
    }

    const char* get_string(ElfW(Word) index) const {
        if ((index >= strtab_size_)) {
//            LOGE("%s: strtab out of bounds error; STRSZ=%zd, name=%d",
//                 get_realpath(), strtab_size_, index);
            return nullptr;
        }

        return strtab_ + index;
    }


    bool is_symbol_global_and_defined(ElfW(Sym)* s) const {
        if (__predict_true(ELF_ST_BIND(s->st_info) == STB_GLOBAL ||
                           ELF_ST_BIND(s->st_info) == STB_WEAK)) {
            return s->st_shndx != SHN_UNDEF;
        } else if (__predict_false(ELF_ST_BIND(s->st_info) != STB_LOCAL)) {
//            DL_WARN("Warning: unexpected ST_BIND value: %d for \"%s\" in \"%s\" (ignoring)",
//                    ELF_ST_BIND(s->st_info), this->get_string(s->st_name), this->get_realpath());
        }
        return false;
    }


    const ElfW(Sym)* elf_lookup(SymbolName& symbol_name) const {
        uint32_t hash = symbol_name.elf_hash();

//        LOGE( "SEARCH %s in %s@%p h=%x(elf) %zd",
//              symbol_name.get_name(), get_realpath(),
//              reinterpret_cast<void*>(base), hash, hash % nbucket_);

        for (uint32_t n = bucket_[hash % nbucket_]; n != 0; n = chain_[n]) {
            ElfW(Sym)* s = symtab_ + n;

            if (strcmp(get_string(s->st_name), symbol_name.get_name()) == 0 &&
                is_symbol_global_and_defined(s)) {
//                LOGE("FOUND %s in %s (%p) %zd",
//                     symbol_name.get_name(), get_realpath(),
//                     reinterpret_cast<void*>(s->st_value),
//                     static_cast<size_t>(s->st_size));
                return symtab_ + n;
            }
        }

//        LOGE( "NOT FOUND %s in %s@%p %x %zd",
//              symbol_name.get_name(), get_realpath(),
//              reinterpret_cast<void*>(base), hash, hash % nbucket_);

        return nullptr;
    }


    ElfW(Sym)* gnu_lookup(SymbolName& symbol_name) const {
        const uint32_t hash = symbol_name.gnu_hash();

        constexpr uint32_t kBloomMaskBits = sizeof(ElfW(Addr)) * 8;
        const uint32_t word_num = (hash / kBloomMaskBits) & gnu_maskwords_;
        const ElfW(Addr) bloom_word = gnu_bloom_filter_[word_num];
        const uint32_t h1 = hash % kBloomMaskBits;
        const uint32_t h2 = (hash >> gnu_shift2_) % kBloomMaskBits;

//        LOGE( "SEARCH %s in %s@%p (gnu)",
//              symbol_name.get_name(), get_realpath(), reinterpret_cast<void*>(base));

        // test against bloom filter
        if ((1 & (bloom_word >> h1) & (bloom_word >> h2)) == 0) {
//            LOGE( "NOT FOUND %s in %s@%p",
//                  symbol_name.get_name(), get_realpath(), reinterpret_cast<void*>(base));
            return nullptr;
        }

        // bloom test says "probably yes"...
        uint32_t n = gnu_bucket_[hash % gnu_nbucket_];

        if (n == 0) {
//            LOGE( "NOT FOUND %s in %s@%p",
//                  symbol_name.get_name(), get_realpath(), reinterpret_cast<void*>(base));
            return nullptr;
        }

        do {
            ElfW(Sym)* s = symtab_ + n;
            if (((gnu_chain_[n] ^ hash) >> 1) == 0 &&
                strcmp(get_string(s->st_name), symbol_name.get_name()) == 0 && is_symbol_global_and_defined(s)) {
//                LOGE( "FOUND %s in %s (%p) %zd",
//                      symbol_name.get_name(), get_realpath(), reinterpret_cast<void*>(s->st_value),
//                      static_cast<size_t>(s->st_size));
                return symtab_ + n;
            }
        } while ((gnu_chain_[n++] & 1) == 0);
//        LOGE( "NOT FOUND %s in %s@%p",
//              symbol_name.get_name(), get_realpath(), reinterpret_cast<void*>(base));
        return nullptr;
    }



    bool prelink_image(){
        ElfW(Word) dynamic_flags = 0;
        for (size_t i = 0; i<this->phnum; ++i) {
            const ElfW(Phdr)& phdr = this->phdr[i];
            if (phdr.p_type == PT_DYNAMIC) {
                this->dynamic = reinterpret_cast<ElfW(Dyn)*>(load_bias + phdr.p_vaddr);
                if (dynamic_flags) {
                    dynamic_flags = phdr.p_flags;
                }
            }
        }
        if (dynamic == nullptr) {
            return false;
        }

        for (ElfW(Dyn) *d = dynamic; d->d_tag != DT_NULL; ++d) {
            switch (d->d_tag) {

                case DT_HASH:
                    nbucket_ = reinterpret_cast<uint32_t *>(load_bias + d->d_un.d_ptr)[0];
                    nchain_ = reinterpret_cast<uint32_t *>(load_bias + d->d_un.d_ptr)[1];
                    bucket_ = reinterpret_cast<uint32_t *>(load_bias + d->d_un.d_ptr + 8);
                    chain_ = reinterpret_cast<uint32_t *>(load_bias + d->d_un.d_ptr + 8 + nbucket_ * 4);
                    break;
                case DT_GNU_HASH:

                    gnu_nbucket_ = reinterpret_cast<uint32_t *>(load_bias + d->d_un.d_ptr)[0];
                    // skip symndx
                    gnu_maskwords_ = reinterpret_cast<uint32_t *>(load_bias + d->d_un.d_ptr)[2];
                    gnu_shift2_ = reinterpret_cast<uint32_t *>(load_bias + d->d_un.d_ptr)[3];

                    gnu_bloom_filter_ = reinterpret_cast<ElfW(Addr) *>(load_bias + d->d_un.d_ptr + 16);
                    gnu_bucket_ = reinterpret_cast<uint32_t *>(gnu_bloom_filter_ + gnu_maskwords_);
                    // amend chain for symndx = header[1]
                    gnu_chain_ = gnu_bucket_ + gnu_nbucket_ -
                                 reinterpret_cast<uint32_t *>(load_bias + d->d_un.d_ptr)[1];

                    if (!powerof2(gnu_maskwords_)) {
//                        DL_ERR("invalid maskwords for gnu_hash = 0x%x, in \"%s\" expecting power to two",
//                               gnu_maskwords_, get_realpath());
                        return false;
                    }
                    --gnu_maskwords_;

                    flags_ |= FLAG_GNU_HASH;
                    break;
                case DT_STRTAB:
                    strtab_ = reinterpret_cast<const char *>(load_bias + d->d_un.d_ptr);
                    break;

                case DT_STRSZ:
                    strtab_size_ = d->d_un.d_val;
                    break;

                case DT_SYMTAB:
                    symtab_ = reinterpret_cast<ElfW(Sym) *>(load_bias + d->d_un.d_ptr);
                    break;

            }

        }
        return true;

    }


} soinfo;



typedef struct _RuntimeModule {
    char path[1024];
    void *load_address;
} RuntimeModule;

typedef uintptr_t addr_t;

static std::vector<RuntimeModule> *modules;

static std::vector<RuntimeModule> & getProcessMapWithProcMaps(){
    if (modules == nullptr) {
        modules = new std::vector<RuntimeModule>();
    }

    FILE *fp = fopen("/proc/self/maps", "r");
    if (fp == nullptr)
        return *modules;

    while (!feof(fp)) {
        char line_buffer[LINE_MAX + 1];
        fgets(line_buffer, LINE_MAX, fp);

        // ignore the rest of characters
        if (strlen(line_buffer) == LINE_MAX && line_buffer[LINE_MAX] != '\n') {
            // Entry not describing executable data. Skip to end of line to set up
            // reading the next entry.
            int c;
            do {
                c = getc(fp);
            } while ((c != EOF) && (c != '\n'));
            if (c == EOF)
                break;
        }

        addr_t region_start, region_end;
        addr_t region_offset;
        char permissions[5] = {'\0'}; // Ensure NUL-terminated string.
        uint8_t dev_major = 0;
        uint8_t dev_minor = 0;
        long inode = 0;
        int path_index = 0;

        if (sscanf(line_buffer,
                   "%" PRIxPTR "-%" PRIxPTR " %4c "
                   "%" PRIxPTR " %hhx:%hhx %ld %n",
                   &region_start, &region_end, permissions, &region_offset, &dev_major, &dev_minor, &inode,
                   &path_index) < 7) {
            printf("/proc/self/maps parse failed!");
            fclose(fp);
            return *modules;
        }

        // check header section permission
        if (strcmp(permissions, "r--p") != 0 && strcmp(permissions, "r-xp") != 0)
            continue;

        // check elf magic number
        ElfW(Ehdr) *header = (ElfW(Ehdr) *)region_start;
        if (memcmp(header->e_ident, ELFMAG, SELFMAG) != 0) {
            continue;
        }

        char *path_buffer = line_buffer + path_index;
        if (*path_buffer == 0 || *path_buffer == '\n' || *path_buffer == '[')
            continue;
        RuntimeModule module;

        // strip
        if (path_buffer[strlen(path_buffer) - 1] == '\n') {
            path_buffer[strlen(path_buffer) - 1] = 0;
        }
        strncpy(module.path, path_buffer, sizeof(module.path) - 1);
        module.load_address = (void *)region_start;
        modules->push_back(module);

    }

    fclose(fp);
    return *modules;
}

RuntimeModule GetProcessMaps(const char *name) {
    auto modules = getProcessMapWithProcMaps();
    for (auto module : modules) {
        if (strstr(module.path, name) != 0) {
            return module;
        }
    }
    return RuntimeModule{0};
}


int linkerElfCtxInit(soinfo *ctx, void *header_) {
    ElfW(Ehdr) *ehdr = (ElfW(Ehdr) *)header_;
    ctx->header = ehdr;

    ElfW(Addr) ehdr_addr = (ElfW(Addr))ehdr;

    // Handle dynamic segment
    {
        ElfW(Addr) addr = 0;
        ElfW(Dyn) *dyn = NULL;
        ElfW(Phdr) *phdr = reinterpret_cast<ElfW(Phdr) *>(ehdr_addr + ehdr->e_phoff);
        for (size_t i = 0; i < ehdr->e_phnum; i++) {
            if (phdr[i].p_type == PT_DYNAMIC) {
                dyn = reinterpret_cast<ElfW(Dyn) *>(ehdr_addr + phdr[i].p_offset);
            } else if (phdr[i].p_type == PT_LOAD) {
                addr = ehdr_addr + phdr[i].p_offset - phdr[i].p_vaddr;
                if (ctx->load_bias == 0)
                    ctx->load_bias = ehdr_addr - (phdr[i].p_vaddr - phdr[i].p_offset);
            } else if (phdr[i].p_type == PT_PHDR) {
                ctx->load_bias = (ElfW(Addr))phdr - phdr[i].p_vaddr;
            }
        }
    }

    // Handle section
    {
        ElfW(Shdr) * dynsym_sh, *dynstr_sh;
        ElfW(Shdr) * sym_sh, *str_sh;

        ElfW(Shdr) *shdr = reinterpret_cast<ElfW(Shdr) *>(ehdr_addr + ehdr->e_shoff);

        ElfW(Shdr) *shstr_sh = NULL;
        shstr_sh = &shdr[ehdr->e_shstrndx];
        char *shstrtab = NULL;
        shstrtab = (char *)((addr_t)ehdr_addr + shstr_sh->sh_offset);

        for (size_t i = 0; i < ehdr->e_shnum; i++) {
            if (shdr[i].sh_type == SHT_SYMTAB) {
                sym_sh = &shdr[i];
                ctx->sym_sh_ = sym_sh;
                ctx->symtab_ = (ElfW(Sym) *)(ehdr_addr + shdr[i].sh_offset);
            } else if (shdr[i].sh_type == SHT_STRTAB && strcmp(shstrtab + shdr[i].sh_name, ".strtab") == 0) {
                str_sh = &shdr[i];
                ctx->strtab_ = (const char *)(ehdr_addr + shdr[i].sh_offset);
            } else if (shdr[i].sh_type == SHT_DYNSYM) {
                dynsym_sh = &shdr[i];
                ctx->dynsym_sh_ = dynsym_sh;
                ctx->dynsymtab_ = (ElfW(Sym) *)(ehdr_addr + shdr[i].sh_offset);
            } else if (shdr[i].sh_type == SHT_STRTAB && strcmp(shstrtab + shdr[i].sh_name, ".dynstr") == 0) {
                dynstr_sh = &shdr[i];
                ctx->dynstrtab_ = (const char *)(ehdr_addr + shdr[i].sh_offset);
            }
        }
    }

    return 0;
}


static void *iterateSymbolTableImpl(const char *symbol_name, ElfW(Sym) * symtab, const char *strtab, int count) {
    for (int i = 0; i < count; ++i) {
        ElfW(Sym) *sym = symtab + i;
        const char *symbol_name_ = strtab + sym->st_name;
        if (strcmp(symbol_name_, symbol_name) == 0) {
            return (void *)sym->st_value;
        }
    }
    return NULL;
}

void *linkerElfCtxIterateSymbolTable(soinfo *ctx, const char *symbol_name) {
    void *result = NULL;
    if (ctx->symtab_ && ctx->strtab_) {
        size_t count = ctx->sym_sh_->sh_size / sizeof(ElfW(Sym));
        result = iterateSymbolTableImpl(symbol_name, ctx->symtab_, ctx->strtab_, count);
        if (result)
            return result;
    }

    if (ctx->dynsymtab_ && ctx->dynstrtab_) {
        size_t count = ctx->dynsym_sh_->sh_size / sizeof(ElfW(Sym));
        result = iterateSymbolTableImpl(symbol_name, ctx->dynsymtab_, ctx->dynstrtab_, count);
        if (result)
            return result;
    }
    return NULL;
}

uintptr_t get_libFile_Symbol_off(char *lib_path,char *fun_name){

    uintptr_t result = NULL;

    soinfo ctx;
    memset(&ctx, 0, sizeof(soinfo));
    size_t file_size = 0;
    {
        struct stat s;
        int rt = stat(lib_path, &s);
        if (rt != 0) {
//            printf("mmap %s failed\n", lib_path);
            return NULL;
        }
        file_size = s.st_size;
    }
    int fd = open(lib_path, O_RDONLY, 0);
    if (fd < 0) {
//        printf("%s open failed\n", lib_path);
        return NULL;
    }

    // auto align
    auto mmap_buffer = (uint8_t *)mmap(0, file_size, PROT_READ | PROT_WRITE, MAP_FILE | MAP_PRIVATE, fd, 0);
    if (mmap_buffer == MAP_FAILED) {
//        printf("mmap %s failed\n", lib_path);
        return NULL;
    }


    linkerElfCtxInit(&ctx, mmap_buffer);
    result = reinterpret_cast<uintptr_t>(linkerElfCtxIterateSymbolTable(&ctx, fun_name));
    close(fd);
    return result;
}
void *get_remote_load_Sym_Addr(const char *library_name, const char *symbol_name) {

}

void *get_self_load_Sym_Addr(const char *library_name, const char *symbol_name) {
    ElfW(Addr) result = NULL;

    soinfo si;
    memset(&si, 0, sizeof(soinfo));
    RuntimeModule module = GetProcessMaps(library_name);
    if(module.load_address){
        si.transform((ElfW(Ehdr)*)module.load_address);
        SymbolName symbol_JNI_OnLoad(symbol_name);
        const ElfW(Sym)* sym = si.find_symbol_by_name(symbol_JNI_OnLoad);
        if(sym!= nullptr){
            result = sym->st_value + si.load_bias;
        }
    }
    return reinterpret_cast<void *>(result);
}




uint32_t calculate_elf_hash(const char* name) {
    const uint8_t* name_bytes = reinterpret_cast<const uint8_t*>(name);
    uint32_t h = 0, g;

    while (*name_bytes) {
        h = (h << 4) + *name_bytes++;
        g = h & 0xf0000000;
        h ^= g;
        h ^= g >> 24;
    }

    return h;
}

struct __attribute__((aligned(8))) GnuHashInitEntry {
    uint64_t ignore_mask;
    uint32_t accum;
};

constexpr uint32_t kStep0 = 1;
constexpr uint32_t kStep1 = kStep0 * 33;
constexpr uint32_t kStep2 = kStep1 * 33;
constexpr uint32_t kStep3 = kStep2 * 33;
constexpr uint32_t kStep4 = kStep3 * 33;
constexpr uint32_t kStep5 = kStep4 * 33;
constexpr uint32_t kStep6 = kStep5 * 33;
constexpr uint32_t kStep7 = kStep6 * 33;
constexpr uint32_t kStep8 = kStep7 * 33;
constexpr uint32_t kStep9 = kStep8 * 33;
constexpr uint32_t kStep10 = kStep9 * 33;
constexpr uint32_t kStep11 = kStep10 * 33;

// Step by -1 through -7:  33 * 0x3e0f83e1 == 1 (mod 2**32)
constexpr uint32_t kStepN1 = kStep0 * 0x3e0f83e1;
constexpr uint32_t kStepN2 = kStepN1 * 0x3e0f83e1;
constexpr uint32_t kStepN3 = kStepN2 * 0x3e0f83e1;
constexpr uint32_t kStepN4 = kStepN3 * 0x3e0f83e1;
constexpr uint32_t kStepN5 = kStepN4 * 0x3e0f83e1;
constexpr uint32_t kStepN6 = kStepN5 * 0x3e0f83e1;
constexpr uint32_t kStepN7 = kStepN6 * 0x3e0f83e1;

// Calculate the GNU hash and string length of the symbol name.
//
// The hash calculation is an optimized version of this function:
//
//    uint32_t calculate_gnu_hash(const uint8_t* name) {
//      uint32_t h = 5381;
//      for (; *name != '\0'; ++name) {
//        h *= 33;
//        h += *name;
//      }
//      return h;
//    }
//
std::pair<uint32_t, uint32_t> calculate_gnu_hash_neon(const char* name) {

    // The input string may be misaligned by 0-7 bytes (K). This function loads the first aligned
    // 8-byte chunk, then counteracts the misalignment:
    //  - The initial K bytes are set to 0xff in the working chunk vector.
    //  - The accumulator is initialized to 5381 * modinv(33)**K.
    //  - The accumulator also cancels out each initial 0xff byte.
    // If we could set bytes to NUL instead, then the accumulator wouldn't need to cancel out the
    // 0xff values, but this would break the NUL check.

    static const struct GnuHashInitEntry kInitTable[] = {
            { // (addr&7) == 0
                    0ull,
                    5381u*kStep0,
            }, { // (addr&7) == 1
                    0xffull,
                    5381u*kStepN1 - 0xffu*kStepN1,
            }, { // (addr&7) == 2
                    0xffffull,
                    5381u*kStepN2 - 0xffu*kStepN1 - 0xffu*kStepN2,
            }, { // (addr&7) == 3
                    0xffffffull,
                    5381u*kStepN3 - 0xffu*kStepN1 - 0xffu*kStepN2 - 0xffu*kStepN3,
            }, { // (addr&7) == 4
                    0xffffffffull,
                    5381u*kStepN4 - 0xffu*kStepN1 - 0xffu*kStepN2 - 0xffu*kStepN3 - 0xffu*kStepN4,
            }, { // (addr&7) == 5
                    0xffffffffffull,
                    5381u*kStepN5 - 0xffu*kStepN1 - 0xffu*kStepN2 - 0xffu*kStepN3 - 0xffu*kStepN4 - 0xffu*kStepN5,
            }, { // (addr&7) == 6
                    0xffffffffffffull,
                    5381u*kStepN6 - 0xffu*kStepN1 - 0xffu*kStepN2 - 0xffu*kStepN3 - 0xffu*kStepN4 - 0xffu*kStepN5 - 0xffu*kStepN6,
            }, { // (addr&7) == 7
                    0xffffffffffffffull,
                    5381u*kStepN7 - 0xffu*kStepN1 - 0xffu*kStepN2 - 0xffu*kStepN3 - 0xffu*kStepN4 - 0xffu*kStepN5 - 0xffu*kStepN6 - 0xffu*kStepN7,
            },
    };

    uint8_t offset = reinterpret_cast<uintptr_t>(name) & 7;
    const uint64_t* chunk_ptr = reinterpret_cast<const uint64_t*>(reinterpret_cast<uintptr_t>(name) & ~7);
    const struct GnuHashInitEntry* entry = &kInitTable[offset];

    uint8x8_t chunk = vld1_u8(reinterpret_cast<const uint8_t*>(chunk_ptr));
    chunk |= vld1_u8(reinterpret_cast<const uint8_t*>(&entry->ignore_mask));

    uint32x4_t accum_lo = { 0 };
    uint32x4_t accum_hi = { entry->accum, 0, 0, 0 };
    const uint16x4_t kInclineVec = { kStep3, kStep2, kStep1, kStep0 };
    const uint32x4_t kStep8Vec = vdupq_n_u32(kStep8);
    uint8x8_t is_nul;
    uint16x8_t expand;

    while (1) {
        // Exit the loop if any of the 8 bytes is NUL.
        is_nul = vceq_u8(chunk, (uint8x8_t){ 0 });
        expand = vmovl_u8(chunk);
        uint64x1_t is_nul_64 = vreinterpret_u64_u8(is_nul);
        if (vget_lane_u64(is_nul_64, 0)) break;

        // Multiply both accumulators by 33**8.
        accum_lo = vmulq_u32(accum_lo, kStep8Vec);
        accum_hi = vmulq_u32(accum_hi, kStep8Vec);

        // Multiply each 4-piece subchunk by (33**3, 33**2, 33*1, 1), then accumulate the result. The lo
        // accumulator will be behind by 33**4 until the very end of the computation.
        accum_lo = vmlal_u16(accum_lo, vget_low_u16(expand), kInclineVec);
        accum_hi = vmlal_u16(accum_hi, vget_high_u16(expand), kInclineVec);

        // Load the next chunk.
        chunk = vld1_u8(reinterpret_cast<const uint8_t*>(++chunk_ptr));
    }

    // Reverse the is-NUL vector so we can use clz to count the number of remaining bytes.
    is_nul = vrev64_u8(is_nul);
    const uint64_t is_nul_u64 = vget_lane_u64(vreinterpret_u64_u8(is_nul), 0);
    const uint32_t num_valid_bits = __builtin_clzll(is_nul_u64);

    const uint32_t name_len = reinterpret_cast<const char*>(chunk_ptr) - name + (num_valid_bits >> 3);

    static const uint32_t kFinalStepTable[] = {
            kStep4, kStep0,   // 0 remaining bytes
            kStep5, kStep1,   // 1 remaining byte
            kStep6, kStep2,   // 2 remaining bytes
            kStep7, kStep3,   // 3 remaining bytes
            kStep8, kStep4,   // 4 remaining bytes
            kStep9, kStep5,   // 5 remaining bytes
            kStep10, kStep6,  // 6 remaining bytes
            kStep11, kStep7,  // 7 remaining bytes
    };

    // Advance the lo/hi accumulators appropriately for the number of remaining bytes. Multiply 33**4
    // into the lo accumulator to catch it up with the hi accumulator.
    const uint32_t* final_step = &kFinalStepTable[num_valid_bits >> 2];
    accum_lo = vmulq_u32(accum_lo, vdupq_n_u32(final_step[0]));
    accum_lo = vmlaq_u32(accum_lo, accum_hi, vdupq_n_u32(final_step[1]));

    static const uint32_t kFinalInclineTable[] = {
            0,      kStep6, kStep5, kStep4, kStep3, kStep2, kStep1, kStep0,
            0,      0,      0,      0,      0,      0,      0,      0,
    };

    // Prepare a vector to multiply powers of 33 into each of the remaining bytes.
    const uint32_t* const incline = &kFinalInclineTable[8 - (num_valid_bits >> 3)];
    const uint32x4_t incline_lo = vld1q_u32(incline);
    const uint32x4_t incline_hi = vld1q_u32(incline + 4);

    // Multiply 33 into each of the remaining 4-piece vectors, then accumulate everything into
    // accum_lo. Combine everything into a single 32-bit result.
    accum_lo = vmlaq_u32(accum_lo, vmovl_u16(vget_low_u16(expand)), incline_lo);
    accum_lo = vmlaq_u32(accum_lo, vmovl_u16(vget_high_u16(expand)), incline_hi);

    uint32x2_t sum = vadd_u32(vget_low_u32(accum_lo), vget_high_u32(accum_lo));
    const uint32_t hash = sum[0] + sum[1];

    return { hash, name_len };
}


static inline std::pair<uint32_t, uint32_t> calculate_gnu_hash(const char* name) {
#if USE_GNU_HASH_NEON
    return calculate_gnu_hash_neon(name);
#else
    return calculate_gnu_hash_simple(name);
#endif
}


uint32_t SymbolName::elf_hash() {
    if (!has_elf_hash_) {
        elf_hash_ = calculate_elf_hash(name_);
        has_elf_hash_ = true;
    }

    return elf_hash_;
}
uint32_t SymbolName::gnu_hash() {
    if (!has_gnu_hash_) {
        gnu_hash_ = calculate_gnu_hash(name_).first;
        has_gnu_hash_ = true;
    }

    return gnu_hash_;
}


