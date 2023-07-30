import qbs.Utilities

StaticLibrary {
    Depends { name: "cpp" }
    Depends { name: "array19" }
    Properties {
        condition: qbs.toolchain.contains('msvc') && !qbs.toolchain.contains('clang-cl') && (Utilities.versionCompare(cpp.compilerVersion, '19.28.29333') < 0)
        cpp.cxxFlags: base.concat("/await") // enable coroutine-ts
    }
    Properties {
        condition: qbs.toolchain.contains('msvc') && qbs.toolchain.contains('clang-cl')
        cpp.cxxFlags: base.concat("-Xclang", "-fcoroutines-ts") // enable coroutine-ts
    }
    Properties {
        condition: qbs.toolchain.contains('clang')
        cpp.cxxFlags: base.concat("-fcoroutines-ts") // enable coroutine-ts
    }

    files: [
        "CoEnumerator.h",
        "CoEnumerator.range.h",
        "coroutine.cpp",
        "coroutine.h",
    ]

    Export {
        Depends { name: "cpp" }
        cpp.includePaths: [".."]
        Depends { name: "array19" }

        Properties {
            condition: qbs.toolchain.contains('msvc') && !qbs.toolchain.contains('clang-cl') && (Utilities.versionCompare(cpp.compilerVersion, '19.28.29333') < 0)
            cpp.cxxFlags: base.concat("/await") // enable coroutine-ts
        }
        Properties {
            condition: qbs.toolchain.contains('msvc') && qbs.toolchain.contains('clang-cl')
            cpp.cxxFlags: base.concat("-Xclang", "-fcoroutines-ts") // enable coroutine-ts
        }
        Properties {
            condition: qbs.toolchain.contains('clang') && cpp.compilerVersionMajor < 11
            cpp.cxxFlags: base.concat("-fcoroutines-ts") // enable coroutine-ts
        }
    }
}
