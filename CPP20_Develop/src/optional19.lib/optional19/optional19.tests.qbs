
Application {
    name: "optional19.tests"
    condition: googletest.present

    consoleApplication: true
    type: ["application", "autotest"]

    Depends { name: "optional19" }
    Depends { name: "googletest" }

    files: [
        "Optional.test.cpp",
        "PackedOptional.test.cpp",
        "PackedOptionalNan.test.cpp",
    ]
}
