//
// Created by chic on 2024/2/19.
//

#pragma once
#include "stdint.h"

typedef uintptr_t soinfo;
soinfo* find_all_library_byname(const char* soname);

