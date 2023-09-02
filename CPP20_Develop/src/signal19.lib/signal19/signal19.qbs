
Product {
    Depends { name: "cpp" }
    Depends { name: "meta19" }

    Export {
        Depends { name: "cpp" }
        cpp.includePaths: [".."]
        Depends { name: "meta19" }
    }

    files: [
        "SignalWith.h",
        "Subscription.h",
        "Subscriptions.h",
    ]
}
