//
// Created by thehepta on 2023/12/17.
//

#include "linker_version.h"
#include "linker_debug.h"




struct soinfo;


template <typename F>
static bool for_each_verdef(const soinfo* si, F functor) {
    if (!si->has_min_version(2)) {
        return true;
    }

    uintptr_t verdef_ptr = si->get_verdef_ptr();
    if (verdef_ptr == 0) {
        return true;
    }

    size_t offset = 0;

    size_t verdef_cnt = si->get_verdef_cnt();
    for (size_t i = 0; i<verdef_cnt; ++i) {
        const ElfW(Verdef)* verdef = reinterpret_cast<ElfW(Verdef)*>(verdef_ptr + offset);
        size_t verdaux_offset = offset + verdef->vd_aux;
        offset += verdef->vd_next;

        if (verdef->vd_version != 1) {
            DL_ERR("unsupported verdef[%zd] vd_version: %d (expected 1) library: %s",
                   i, verdef->vd_version, si->get_realpath());
            return false;
        }

        if ((verdef->vd_flags & VER_FLG_BASE) != 0) {
            // "this is the version of the file itself.  It must not be used for
            //  matching a symbol. It can be used to match references."
            //
            // http://www.akkadia.org/drepper/symbol-versioning
            continue;
        }

        if (verdef->vd_cnt == 0) {
            DL_ERR("invalid verdef[%zd] vd_cnt == 0 (version without a name)", i);
            return false;
        }

        const ElfW(Verdaux)* verdaux = reinterpret_cast<ElfW(Verdaux)*>(verdef_ptr + verdaux_offset);

        if (functor(i, verdef, verdaux) == true) {
            break;
        }
    }

    return true;
}



ElfW(Versym) find_verdef_version_index(const soinfo* si, const version_info* vi) {
    if (vi == nullptr) {
        return kVersymNotNeeded;
    }

    ElfW(Versym) result = kVersymGlobal;

    if (!for_each_verdef(si,
                         [&](size_t, const ElfW(Verdef)* verdef, const ElfW(Verdaux)* verdaux) {
                             if (verdef->vd_hash == vi->elf_hash &&
                                 strcmp(vi->name, si->get_string(verdaux->vda_name)) == 0) {
                                 result = verdef->vd_ndx;
                                 return true;
                             }

                             return false;
                         }
    )) {
        // verdef should have already been validated in prelink_image.
        LOGE("invalid verdef after prelinking: %s",si->get_realpath());
    }

    return result;
}



bool VersionTracker::init(const soinfo *si_from, const SymbolLookupList &list) {
    if (!si_from->has_min_version(2)) {
        return true;
    }

    return init_verneed(si_from, list) && init_verdef(si_from);
}


bool VersionTracker::init_verdef(const soinfo* si_from) {
    return for_each_verdef(si_from,
                           [&](size_t, const ElfW(Verdef)* verdef, const ElfW(Verdaux)* verdaux) {
                               add_version_info(verdef->vd_ndx, verdef->vd_hash,
                                                si_from->get_string(verdaux->vda_name), si_from);
                               return false;
                           }
    );
}



bool VersionTracker::init_verneed(const soinfo *si_from,  SymbolLookupList list) {
    uintptr_t verneed_ptr = si_from->get_verneed_ptr();

    if (verneed_ptr == 0) {
        return true;
    }

    size_t verneed_cnt = si_from->get_verneed_cnt();

    for (size_t i = 0, offset = 0; i<verneed_cnt; ++i) {
        const ElfW(Verneed)* verneed = reinterpret_cast<ElfW(Verneed)*>(verneed_ptr + offset);
        size_t vernaux_offset = offset + verneed->vn_aux;
        offset += verneed->vn_next;

        if (verneed->vn_version != 1) {
            DL_ERR("unsupported verneed[%zd] vn_version: %d (expected 1)", i, verneed->vn_version);
            return false;
        }

        const char* target_soname = si_from->get_string(verneed->vn_file);
//        // find it in dependencies
//        soinfo* target_si = si_from->get_children().find_if([&](const soinfo* si) {
//            return si->get_soname() != nullptr && strcmp(si->get_soname(), target_soname) == 0;
//        });
        soinfo* target_si = nullptr;
        std::vector<SymbolLookupLib>::iterator iter;
        for (iter = list.getVectorSymLib().begin(); iter != list.getVectorSymLib().end(); iter++) {
            if(iter->si_->get_soname()!= nullptr&& strcmp(iter->si_->get_soname(), target_soname) == 0){
                target_si = iter->si_ ;
                break;
            }
        }

        if (target_si == nullptr) {
            DL_ERR("cannot find \"%s\" from verneed[%zd] in DT_NEEDED list for \"%s\"",
                   target_soname, i, si_from->get_realpath());
            return false;
        }

        for (size_t j = 0; j<verneed->vn_cnt; ++j) {
            const ElfW(Vernaux)* vernaux = reinterpret_cast<ElfW(Vernaux)*>(verneed_ptr + vernaux_offset);
            vernaux_offset += vernaux->vna_next;

            const ElfW(Word) elf_hash = vernaux->vna_hash;
            const char* ver_name = si_from->get_string(vernaux->vna_name);
            ElfW(Half) source_index = vernaux->vna_other;

            add_version_info(source_index, elf_hash, ver_name, target_si);
        }
    }

    return true;
}

void VersionTracker::add_version_info(size_t source_index,
                                      ElfW(Word) elf_hash,
                                      const char* ver_name,
                                      const soinfo* target_si) {
    if (source_index >= version_infos.size()) {
        version_infos.resize(source_index+1);
    }

    version_infos[source_index].elf_hash = elf_hash;
    version_infos[source_index].name = ver_name;
    version_infos[source_index].target_si = target_si;
}

const version_info* VersionTracker::get_version_info(ElfW(Versym) source_symver) const {
    if (source_symver < 2 ||
        source_symver >= version_infos.size() ||
        version_infos[source_symver].name == nullptr) {
        return nullptr;
    }

    return &version_infos[source_symver];
}