#include "Archive.h"
#include "ReadArchive.h"
#include "dynamicWrite.h"

#include <gtest/gtest.h>

using namespace serialize19;

enum class Profession : uint16_t { Coder, Guru, Shopper, Gamer };

struct Person {
    std::string name{};
    int birthYear{};
    Profession profession{};

    bool operator==(const Person&) const = default;
};

template<class A> void serialize(A& a, std::string& s) {
    auto size = s.size();
    serialize(a, size);
    if (a.mode == ArchiveMode::Read) {
        s.resize(size);
        for (auto i = 0U; i < size; i++) serialize(a, s[i]);
    }
    else {
        for (auto c : s) serialize(a, c);
    }
}

template<class A> void serialize(A& a, Person& p) {
    serialize(a, p.name);
    serialize(a, p.birthYear);
    serialize(a, p.profession);
}

TEST(Codec, Person) {
    auto input = Person{"Santa Clause", -100, Profession::Guru};
    auto buffer = dynamicWrite(input);

    auto reader = ReadArchive{buffer.slice()};
    auto output = Person{};
    serialize(reader, output);

    EXPECT_EQ(input, output);
}
