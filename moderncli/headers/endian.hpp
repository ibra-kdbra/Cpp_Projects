// Copyright (C) 2020-2023 David Sugar, Tycho Softworks
// This code is licensed under MIT license

#ifndef ENDIAN_HPP_
#define ENDIAN_HPP_

#include <cstdint>

namespace tycho {
inline auto be_get8(const uint8_t *ptr) {
    return *ptr;
}

inline auto be_get16(const uint8_t *ptr) {
    return ((uint16_t)(ptr[0]) << 8) | (uint16_t)(ptr[1]);
}

inline auto be_get32(const uint8_t *ptr) {
    return ((uint32_t)(ptr[0]) << 24) | ((uint32_t)(ptr[1]) << 16) |
        ((uint32_t)(ptr[2]) << 8) | (uint32_t)(ptr[3]);
}

inline auto be_get64(const uint8_t *ptr) {
    return ((uint64_t)(ptr[0]) << 56) | ((uint64_t)(ptr[1]) << 48) |
        ((uint64_t)(ptr[2]) << 40) | ((uint64_t)(ptr[3]) << 32) |
        ((uint64_t)(ptr[4]) << 24) | ((uint64_t)(ptr[5]) << 16) |
        ((uint64_t)(ptr[6]) << 8) | (uint64_t)(ptr[7]);
}

inline auto le_get8(const uint8_t *ptr) {
    return *ptr;
}

inline auto le_get16(const uint8_t *ptr) {
    return ((uint16_t)(ptr[1]) << 8) | (uint16_t)(ptr[0]);
}

inline auto le_get32(const uint8_t *ptr) {
    return ((uint32_t)(ptr[3]) << 24) | ((uint32_t)(ptr[2]) << 16) |
        ((uint32_t)(ptr[1]) << 8) | (uint32_t)(ptr[0]);
}

inline auto le_get64(const uint8_t *ptr) {
    return ((uint64_t)(ptr[7]) << 56) | ((uint64_t)(ptr[6]) << 48) |
        ((uint64_t)(ptr[5]) << 40) | ((uint64_t)(ptr[4]) << 32) |
        ((uint64_t)(ptr[3]) << 24) | ((uint64_t)(ptr[2]) << 16) |
        ((uint64_t)(ptr[1]) << 8) | (uint64_t)(ptr[0]);
}

inline void be_set8(uint8_t *ptr, uint8_t value) {
    *ptr = value;
}

inline void be_set16(uint8_t *ptr, uint16_t value) {
    ptr[0] = (value & 0xff00) >> 8;
    ptr[1] = value & 0xff;
}

inline void be_set32(uint8_t *ptr, uint32_t value) {
    ptr[0] = (value & 0xff000000) >> 24;
    ptr[1] = (value & 0x00ff0000) >> 16;
    ptr[2] = (value & 0x0000ff00) >> 8;
    ptr[3] = value & 0xff;
}

inline void be_set64(uint8_t *ptr, uint64_t value) {
    ptr[0] = (value & 0xff00000000000000) >> 56;
    ptr[1] = (value & 0x00ff000000000000) >> 48;
    ptr[2] = (value & 0x0000ff0000000000) >> 40;
    ptr[3] = (value & 0x000000ff00000000) >> 32;
    ptr[4] = (value & 0x00000000ff000000) >> 24;
    ptr[5] = (value & 0x0000000000ff0000) >> 16;
    ptr[6] = (value & 0x000000000000ff00) >> 8;
    ptr[7] = value & 0xff;
}

inline void le_set8(uint8_t *ptr, uint8_t value) {
    *ptr = value;
}

inline void le_set16(uint8_t *ptr, uint16_t value) {
    ptr[1] = (value & 0xff00) >> 8;
    ptr[0] = value & 0xff;
}

inline void le_set32(uint8_t *ptr, uint32_t value) {
    ptr[3] = (value & 0xff000000) >> 24;
    ptr[2] = (value & 0x00ff0000) >> 16;
    ptr[1] = (value & 0x0000ff00) >> 8;
    ptr[0] = value & 0xff;
}

inline void le_set64(uint8_t *ptr, uint64_t value) {
    ptr[7] = (value & 0xff00000000000000) >> 56;
    ptr[6] = (value & 0x00ff000000000000) >> 48;
    ptr[5] = (value & 0x0000ff0000000000) >> 40;
    ptr[4] = (value & 0x000000ff00000000) >> 32;
    ptr[3] = (value & 0x00000000ff000000) >> 24;
    ptr[2] = (value & 0x0000000000ff0000) >> 16;
    ptr[1] = (value & 0x000000000000ff00) >> 8;
    ptr[0] = value & 0xff;
}
} // end namespace

/*!
 * Endian ordering of binary data.
 * \file endian.hpp
 */
#endif
