//
// Created by chic on 2023/11/25.
//

#include "linker_mapped_file_fragment.h"
#include "linker_utils.h"
#include <inttypes.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

MappedFileFragment::MappedFileFragment() : map_start_(nullptr), map_size_(0),
                                           data_(nullptr), size_ (0)
{ }

MappedFileFragment::~MappedFileFragment() {
    if (map_start_ != nullptr) {
        munmap(map_start_, map_size_);
    }
}

bool MappedFileFragment::Map(int fd, off64_t base_offset, size_t elf_offset, size_t size) {
    off64_t offset;
    safe_add(&offset, base_offset, elf_offset);

    off64_t page_min = page_start(offset);
    off64_t end_offset;

    safe_add(&end_offset, offset, size);
    safe_add(&end_offset, end_offset, page_offset(offset));

    size_t map_size = static_cast<size_t>(end_offset - page_min);
//    CHECK(map_size >= size);

    uint8_t* map_start = static_cast<uint8_t*>(
            mmap64(nullptr, map_size, PROT_READ, MAP_PRIVATE, fd, page_min));

    if (map_start == MAP_FAILED) {
        return false;
    }

    map_start_ = map_start;
    map_size_ = map_size;

    data_ = map_start + page_offset(offset);
    size_ = size;

    return true;
}
