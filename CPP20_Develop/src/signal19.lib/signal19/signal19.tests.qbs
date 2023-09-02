
Application {
    name: "signal19.tests"
    condition: googletest.present

    consoleApplication: true
    type: ["application", "autotest"]

    Depends { name: "googletest" }
    Depends { name: "signal19" }

    files: [
        "SignalWith.test.cpp",
        "Subscriptions.test.cpp",
    ]
}
