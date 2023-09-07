
Application {
    name: "string19.tests"
    condition: googletest.present

    consoleApplication: true
    type: ["application", "autotest"]

    Depends { name: "string19" }
    Depends { name: "googletest" }

    files: [
        "Rope.decimal.test.cpp",
        "Rope.join.test.cpp",
        "Rope.test.cpp",
        "StringStore.test.cpp",
        "StringView.test.cpp",
    ]
}
