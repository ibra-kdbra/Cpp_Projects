// Copyright (c) 2020-2023 David Sugar, Tycho Softworks
// This code is licensed under MIT license

#include "compiler.hpp"
#include <endian.hpp>
#include <cassert>

auto main([[maybe_unused]] int argc, [[maybe_unused]] char **argv) -> int {
    uint8_t bytes[] = {0x01, 0x02};

    assert(be_get16(bytes) == 258U);
    assert(le_get16(bytes) == 513U);
}


