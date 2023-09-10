
Application {
    name: "strong19.tests"
    condition: googletest.present

    consoleApplication: true
    type: ["application", "autotest"]

    Depends { name: "strong19" }
    Depends { name: "googletest" }

    files: [
        "Strong.extras.ostream.test.cpp",
        "Strong.hash.test.cpp",
        "Strong.test.cpp",
    ]
}
