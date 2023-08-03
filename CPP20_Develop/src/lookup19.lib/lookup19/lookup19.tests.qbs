
Application {
    name: "lookup19.tests"
    condition: googletest.present

    consoleApplication: true
    type: ["application", "autotest"]

    Depends { name: "lookup19" }
    Depends { name: "googletest" }

    // Depends { name: "cpp" }
    // cpp.combineCxxSources: true

    files: [
        "OrderedSetOf.test.cpp",
    ]
}
