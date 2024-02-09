//
// Created by thehepta on 2023/6/7.
//

#pragma once

#include <link.h>

namespace rxposed {

    void *resolve_elf_internal_symbol(const char *library_name, const char *symbol_name);
}


