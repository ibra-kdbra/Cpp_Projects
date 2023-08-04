
Application {
    name: "meta19.tests"

    Depends { name: "meta19" }
    // all tests are compile time only!

    Depends { name: "cpp" }
    // cpp.combineCxxSources: true

    files: [
        "*.test.cpp",
    ]
}
