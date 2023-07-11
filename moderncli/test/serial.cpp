// Copyright (c) 2020-2023 David Sugar, Tycho Softworks
// This code is licensed under MIT license

#include "compiler.hpp"
#include <serial.hpp>
#include <cassert>

auto main([[maybe_unused]] int argc, [[maybe_unused]] char **argv) -> int {
    const serial_t serial;
    assert(is(serial) == false);
}


