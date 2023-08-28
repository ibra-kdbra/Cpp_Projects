
Application {
    name: "partial19.tests"
    condition: googletest.present

    consoleApplication: true
    type: ["application", "autotest"]

    Depends { name: "partial19" }
    Depends { name: "googletest" }

    files: [
        "Bitset.test.cpp",
        "Partial.test.cpp",
    ]
}
