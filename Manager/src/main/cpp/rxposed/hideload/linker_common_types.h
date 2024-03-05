//
// Created by chic on 2023/11/28.
//

#pragma once

#include "linked_list.h"
#include "android/dlext.h"

#if defined(__LP64__)
#define USE_RELA 1
#endif


struct soinfo;
class SoinfoListAllocator {
public:
    static LinkedListEntry<soinfo>* alloc();
    static void free(LinkedListEntry<soinfo>* entry);

private:
    // unconstructable
    DISALLOW_IMPLICIT_CONSTRUCTORS(SoinfoListAllocator);
};

class NamespaceListAllocator {
public:
    static LinkedListEntry<android_namespace_t>* alloc();
    static void free(LinkedListEntry<android_namespace_t>* entry);

private:
    // unconstructable
    DISALLOW_IMPLICIT_CONSTRUCTORS(NamespaceListAllocator);
};

typedef LinkedList<soinfo, SoinfoListAllocator> soinfo_list_t;
typedef LinkedList<android_namespace_t, NamespaceListAllocator> android_namespace_list_t;
