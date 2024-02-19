//
// Created by chic on 2023/11/28.
//

#pragma once


#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
    TypeName(const TypeName&) = delete;      \
    void operator=(const TypeName&) = delete


#define DISALLOW_IMPLICIT_CONSTRUCTORS(TypeName) \
    TypeName() = delete;                           \
    DISALLOW_COPY_AND_ASSIGN(TypeName)

// Android uses RELA for LP64.
#if defined(__LP64__)
#define USE_RELA 1
#endif
