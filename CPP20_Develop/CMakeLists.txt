cmake_minimum_required(VERSION 3.24 FATAL_ERROR)
project (co-cpp19 LANGUAGES CXX)
enable_testing()

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
set(COCPP19_CMAKE_CONFIG_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/CoCpp19")

find_package(GTest QUIET)

add_subdirectory(third_party)
add_subdirectory(src)

configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/CoCpp19Config.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/CoCpp19Config.cmake"
  INSTALL_DESTINATION ${COCPP19_CMAKE_CONFIG_DESTINATION}
)
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/CoCpp19ConfigVersion.cmake"
  VERSION "1.0.0"
  COMPATIBILITY AnyNewerVersion
)
install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/CoCpp19Config.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/CoCpp19ConfigVersion.cmake"
  DESTINATION ${COCPP19_CMAKE_CONFIG_DESTINATION}
  COMPONENT cocpp19
)
