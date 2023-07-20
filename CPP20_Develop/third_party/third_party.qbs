
Project {

    references: [
        "googletest.qbs",
    ]

    Product {
        name: "third_party/[Extra Files]"
        files: [
            "git_clone_googletest.sh",
        ]
    }

}
