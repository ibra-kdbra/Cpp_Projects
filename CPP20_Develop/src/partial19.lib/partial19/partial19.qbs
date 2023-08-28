
Product {
    Depends { name: "meta19" }

    Export {
        Depends { name: "cpp" }
        cpp.includePaths: [".."]
        Depends { name: "meta19" }
    }

    files: [
        "Bitset.h",
        "Partial.equals.h",
        "Partial.h",
        "Partial.ostream.h",
        "align.h",
    ]
}
