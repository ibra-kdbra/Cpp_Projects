
Application {
    name: "serialize19.tests"
    condition: googletest.present

    consoleApplication: true
    type: ["application", "autotest"]

    Depends { name: "googletest" }
    Depends { name: "optional19" }
    Depends { name: "partial19" }
    Depends { name: "serialize19" }
    Depends { name: "tuple19" }
    Depends { name: "variant19" }

    Group {
        name: "std"
        files: [
            "serialize.std_bitset.test.cpp",
            "serialize.std_optional.test.cpp",
            "serialize.std_tuple.test.cpp",
            "serialize.std_variant.test.cpp",
            "serialize.std_vector.test.cpp",
        ]
    }
    Group {
        name: "co-cpp19"
        files: [
            "serialize.BufferSlice.test.cpp",
            "serialize.DynamicArrayOf.test.cpp",
            "serialize.Optional.test.cpp",
            "serialize.PackedOptional.test.cpp",
            "serialize.Partial.test.cpp",
            "serialize.Tuple.test.cpp",
            "serialize.Variant.test.cpp",
        ]
    }
    files: [
        "Codec.test.cpp",
        "Endianness.test.cpp",
    ]
}
