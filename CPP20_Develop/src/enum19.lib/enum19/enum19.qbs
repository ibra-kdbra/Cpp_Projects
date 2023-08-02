
Product {
    Depends { name: "cpp" }
    Depends { name: "string19" }
    Depends { name: "fmt"; required: false }

    Export {
        Depends { name: "cpp" }
        cpp.includePaths: [".."]
        Depends { name: "string19" }
    }

    files: [
        "ADL.h",
        "Enum.extras.ostream.h",
        "Enum.fmt.h",
        "Enum.h",
        "Enum.max.h",
        "Enum.names.h",
        "Enum.ostream.h",
        "visitEnumMemberNames.h",
    ]
}
