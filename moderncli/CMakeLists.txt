# Copyright (C) 2022-2023 David Sugar <tychosoft@gmail.com>.
#
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without
# modifications, as long as this notice is preserved.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

cmake_minimum_required(VERSION 3.16.0)
project(ModernCLI VERSION 0.5.1 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)

file(GLOB LINT_SOURCES RELATIVE ${PROJECT_SOURCE_DIR} headers/*.hpp test/*.cpp)

include(cmake/custom.cmake OPTIONAL)
include(cmake/project.cmake)
include(cmake/features.cmake)
include(cmake/coverage.cmake)
include(cmake/linting.cmake)

file(GLOB markdown *.md)
file(GLOB headers headers/*.hpp)

# Setup headers

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/headers
    ${CMAKE_CURRENT_SOURCE_DIR}/utils
)

configure_file(config.hpp.in config.hpp)
configure_file(moderncli.pc.in moderncli.pc)

# Testing
enable_testing()
add_executable(test_shell  test/shell.cpp headers/print.hpp headers/args.hpp)
add_test(NAME test-shell COMMAND test_shell)

add_executable(test_keyfile  test/keyfile.cpp headers/keyfile.hpp test/test.conf)
add_test(NAME test-keyfile COMMAND test_keyfile)

add_executable(test_strings  test/strings.cpp headers/strings.hpp)
add_test(NAME test-strings COMMAND test_strings)

add_executable(test_sync  test/sync.cpp headers/sync.hpp)
add_test(NAME test-sync COMMAND test_sync)

add_executable(test_endian  test/endian.cpp headers/endian.hpp)
add_test(NAME test-endian COMMAND test_endian)

add_executable(test_serial  test/serial.cpp headers/serial.hpp)
add_test(NAME test-serial COMMAND test_serial)

if(OPENSSL_FOUND)
    add_executable(test_digest  test/digest.cpp headers/digest.hpp)
    target_link_libraries(test_digest ${CRYPTO_LIBS})
    add_test(NAME test-digest COMMAND test_digest)
endif()

# Extras...

add_custom_target(header-files SOURCES ${headers})
add_custom_target(support-files SOURCES config.hpp.in moderncli.pc.in Doxyfile ${markdown})

install(FILES ${headers} DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/moderncli")
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/moderncli.pc DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/pkgconfig")
