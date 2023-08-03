
Product {
    Depends { name: "cpp19" }
    Depends { name: "array19" }

    Export {
        Depends { name: "cpp" }
        cpp.includePaths: [".."]
        Depends { name: "cpp19" }

        Depends { name: "array19" }
    }

    files: [
        "OrderedSetOf.h",
        "OrderedSliceOf.h",
    ]
}
