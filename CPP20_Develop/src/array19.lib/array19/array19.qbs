
Product {
    Depends { name: "cpp19" }

    Export {
        Depends { name: "cpp" }
        cpp.includePaths: [".."]
        Depends { name: "cpp19" }
    }

    files: [
        "AllocatedArrayOf.equals.h",
        "AllocatedArrayOf.h",
        "AllocatedArrayOf.ostream.h",
        "AllocatedArrayUtils.h",
        "Array.h",
        "Array.ostream.h",
        "DynamicArrayOf.equals.h",
        "DynamicArrayOf.h",
        "DynamicArrayOf.ostream.h",
        "DynamicSortedSet.h",
        "MoveSliceOf.h",
        "MoveSliceOf.single.h",
        "SliceOf.carray.h",
        "SliceOf.equals.h",
        "SliceOf.h",
        "SliceOf.max.h",
        "SliceOf.ostream.h",
        "SliceOf.single.h",
        "SliceOf.store.h",
        "WithIndex.h",
        "Zip.h",
        "adlRange.h",
    ]
}
