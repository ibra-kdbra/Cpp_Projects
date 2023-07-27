
Application {
    name: "array19.tests"
    condition: googletest.present

    consoleApplication: true
    type: ["application", "autotest"]

    Depends { name: "array19" }
    Depends { name: "googletest" }

    // Depends { name: "cpp" }
    // cpp.combineCxxSources: true

    files: [
        "*.test.cpp",
    ]
}
