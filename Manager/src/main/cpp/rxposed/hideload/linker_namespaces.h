//
// Created by chic on 2023/11/28.
//

#pragma once
#include "user_system.h"
#include "string"
#include "vector"
#include "linker_common_types.h"
#include <unordered_set>
#include "android/dlext.h"

//struct android_namespace_t;


struct android_namespace_link_t {

private:
    android_namespace_t* const linked_namespace_;
    const std::unordered_set<std::string> shared_lib_sonames_;
    bool allow_all_shared_libs_;
};



//struct android_namespace_t {
//private:
//    std::string name_;
//    bool is_isolated_;
//    bool is_greylist_enabled_;
//    bool is_also_used_as_anonymous_;
//    std::vector<std::string> ld_library_paths_;
//    std::vector<std::string> default_library_paths_;
//    std::vector<std::string> permitted_paths_;
//    std::vector<std::string> whitelisted_libs_;
//    // Loader looks into linked namespace if it was not able
//    // to find a library in this namespace. Note that library
//    // lookup in linked namespaces are limited by the list of
//    // shared sonames.
//    std::vector<android_namespace_link_t> linked_namespaces_;
//    soinfo_list_t soinfo_list_;
//
//    DISALLOW_COPY_AND_ASSIGN(android_namespace_t);
//};
