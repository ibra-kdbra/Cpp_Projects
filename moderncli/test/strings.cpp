// Copyright (c) 2020-2023 David Sugar, Tycho Softworks
// This code is licensed under MIT license

#include "compiler.hpp"
#include <strings.hpp>
#include <cassert>

auto main([[maybe_unused]] int argc, [[maybe_unused]] char **argv) -> int {
    const std::string text = "hi,bye,gone";
    auto list = split(text, ",");

    assert(list.size() == 3);
    assert(list[0] == "hi");
    assert(list[1] == "bye");
    assert(list[2] == "gone");

    assert(upper_case("Hi There") == "HI THERE");
    assert(lower_case<std::string>("Hi There") == "hi there");
    assert(strip("   testing ") == "testing");
    assert(begins_with<std::string>("belong", "be"));
    assert(ends_with("belong", "ong"));

    assert(unquote("'able '") == "able ");
    assert(unquote("'able ") == "'able ");
}


