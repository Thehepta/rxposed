//
// Created by chic on 2023/12/17.
//

#pragma once
#include <stdint.h>
#include <limits.h>


// Helper classes for decoding LEB128, used in packed relocation data.
// http://en.wikipedia.org/wiki/LEB128

class sleb128_decoder {
public:
    sleb128_decoder(const uint8_t* buffer, size_t count)
            : current_(buffer), end_(buffer+count) { }

    size_t pop_front() {
        size_t value = 0;
        static const size_t size = CHAR_BIT * sizeof(value);

        size_t shift = 0;
        uint8_t byte;

        do {
            if (current_ >= end_) {
//                LOGE("sleb128_decoder ran out of bounds");
            }
            byte = *current_++;
            value |= (static_cast<size_t>(byte & 127) << shift);
            shift += 7;
        } while (byte & 128);

        if (shift < size && (byte & 64)) {
            value |= -(static_cast<size_t>(1) << shift);
        }

        return value;
    }

private:
    const uint8_t* current_;
    const uint8_t* const end_;
};
