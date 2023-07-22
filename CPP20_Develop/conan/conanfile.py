from conans import ConanFile
from conan.tools.cmake import CMake, CMakeToolchain, cmake_layout
from conan.tools.files import apply_conandata_patches, export_conandata_patches, get, replace_in_file, rm, rmdir
from conans import tools

import os

class Cocpp19Conan(ConanFile):
    name = "co-cpp19"
    version = "1.0"
    description = "C++20 Library with the fastest runtime and compile times"
    author = "Hicknhack Software"
    license = "MIT License"
    url = "https://github.com/basicpp17/co-cpp19"
    homepage = "https://github.com/basicpp17/co-cpp19"
    topics = ("algorithm", "container", "common", "utility")

    settings = "os", "arch", "compiler", "build_type"
    options = {
        "shared": [True, False],
        "fPIC": [True, False]
    }
    default_options = {
        "shared": False,
        "fPIC": True
    }
    requires = [("gtest/1.11.0", "private")]
    generators = "CMakeDeps"

    def layout(self):
        self.folders.root = ".."
        cmake_layout(self)

    def export_sources(self):
        self.copy("CMakeLists.txt", src="..")
        self.copy("CoCpp19Config.cmake.in", src="..")
        self.copy("LICENSE", src="..")
        self.copy("src/*", src="..", excludes="*.qbs")
        self.copy("third_party/CMakeLists.txt", src="..")
        self.copy("third_party/googletest.cmake", src="..")

    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

    def validate(self):
        tools.check_min_cppstd(self, "20")

    def generate(self):
        tc = CMakeToolchain(self)
        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        self.copy("LICENSE")
        cmake = CMake(self)
        cmake.install()
        rmdir(self, os.path.join(self.package_folder, "lib", "pkgconfig"))
        rmdir(self, os.path.join(self.package_folder, "lib", "cmake"))

    def package_info(self):
        self.cpp_info.set_property("cmake_find_mode", "both")
        self.cpp_info.set_property("cmake_file_name", "CoCpp19")

        components = [
            {"name": "array19"},
            {"name": "coro19", "requires": ["array19"], "libs": ["coro19"]},
            {"name": "enum19", "requires": ["string19"]},
            {"name": "lookup19", "requires": ["array19"]},
            {"name": "meta19", "requires": ["array19"]},
            {"name": "optional19", "requires": ["meta19"]},
            {"name": "partial19", "requires": ["meta19"]},
            {"name": "serialize19", "requires": ["meta19"]},
            {"name": "signal19", "requires": ["array19"]},
            {"name": "string19", "requires": ["tuple19"]},
            {"name": "strong19", "requires": ["string19"]},
            {"name": "tuple19", "requires": ["meta19"]},
            {"name": "variant19", "requires": ["meta19"]}
        ]
        for comp in components:
            name = comp["name"]
            self.cpp_info.components[name].set_property("cmake_target_name", f"CoCpp19::{name}")
            self.cpp_info.components[name].set_property("pkg_config_name", name)
            if "requires" in comp:
                self.cpp_info.components[name].requires = comp["requires"]
            if "libs" in comp:
                self.cpp_info.components[name].libs = comp["libs"]

        self.cpp_info.names["cmake_find_package"] = "CoCpp19"
        self.cpp_info.names["cmake_find_package_multi"] = "CoCpp19"

    def test(self):
        cmake = CMake(self)
        cmake.test(output_on_failure=True)
