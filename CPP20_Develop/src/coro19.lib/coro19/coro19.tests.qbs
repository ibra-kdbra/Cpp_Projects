
Application {
    name: "coro19.tests"
    condition: googletest.present

    consoleApplication: true
    type: ["application", "autotest"]

    Depends { name: "coro19" }
    Depends { name: "googletest" }

    files: [
        "CoEnumerator.rang.test.cpp",
        "CoEnumerator.test.cpp",
    ]
}
