
Product {
    Depends { name: "string19" }
    Depends { name: "fmt"; required: false }
    Depends { name: "array19"; required: false }

    Export {
        Depends { name: "cpp" }
        cpp.includePaths: [".."]
        Depends { name: "string19" }

        Properties {
            condition: qbs.toolchain.contains('clang')
            cpp.cxxFlags: base.concat(
                "-Wno-gnu-zero-variadic-macro-arguments" // accept this extensions for opaque strong types
            )
        }
    }

    files: [
        "ADL.h",
        "Macro.h",
        "SortedStrongSet.h",
        "Strong.extras.ostream.h",
        "Strong.fmt.h",
        "Strong.h",
        "Strong.hash.h",
        "Strong.less.h",
        "Strong.ostream.h",
        "Strong.traits.h",
        "WeakOf.h",
    ]
}
