
Product {
    Depends { name: "tuple19" }
    Depends { name: "fmt"; required: false }

    Export {
        Depends { name: "cpp" }
        cpp.includePaths: [".."]
        Depends { name: "tuple19" }
    }

    Group {
        name: "Rope"
        files: [
            "Rope.Decimal.h",
            "Rope.Rope.h",
            "Rope.StringStore.h",
            "Rope.StringView.h",
            "Rope.char.h",
            "Rope.h",
            "Rope.join.h",
            "Rope.store.h",
        ]
    }
    Group {
        name: "StringStore"
        files: [
            "StringStore.h",
            "StringStore.literal.h",
            "StringStore.range.h",
        ]
    }
    Group {
        name: "StringView"
        files: [
            "StringView.equals.h",
            "StringView.fmt.h",
            "StringView.h",
            "StringView.literal.h",
            "StringView.ostream.h",
            "StringView.range.h",
            "StringView.std.h",
            "StringView.store.h",
        ]
    }
    files: [
        "ADL.h",
        "DynamicString.h",
    ]
}
