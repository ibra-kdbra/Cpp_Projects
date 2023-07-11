// Copyright (c) 2020-2023 David Sugar, Tycho Softworks
// This code is licensed under MIT license

#include "compiler.hpp"
#include <sync.hpp>
#include <cassert>

struct test {
    int v1{2};
    int v2{7};
};

static unique_sync<int> counter(3);
static shared_sync<struct test> testing;

auto main([[maybe_unused]] int argc, [[maybe_unused]] char **argv) -> int {
    sync_ptr<int> count(counter);
    assert(*count == 3);
    ++*count;
    assert(*count == 4);

    const reader_ptr<struct test> tester(testing);
    assert(tester->v1 == 2);
}


