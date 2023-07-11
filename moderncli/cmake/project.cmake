# Project settings and common targets.
# Copyright (C) 2020-2021 David Sugar <tychosoft@gmail.com>.
#
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without
# modifications, as long as this notice is preserved.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

string(TOLOWER "${PROJECT_NAME}" PROJECT_ARCHIVE)
include(GNUInstallDirs)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

source_group("Documentation" REGULAR_EXPRESSION  ".*\\.(1|2|3|4|5|6|8|md|tex)")
source_group("Generated Files" REGULAR_EXPRESSION  ".*\\.(in)")
source_group("Header Files" REGULAR_EXPRESSION  ".*\\.(hpp|h)")
source_group("Source Files" REGULAR_EXPRESSION  ".*\\.(cpp|c)")
source_group("Config Files" REGULAR_EXPRESSION ".*\\.(conf|ini|reg)")
source_group("Script Files" REGULAR_EXPRESSION ".*\\.(sh|py|rb|js)")

if(CMAKE_BUILD_TYPE MATCHES "Debug")
    set(BUILD_TESTING TRUE)
    if(NOT MSVC)
        list(APPEND CMAKE_CXX_FLAGS "-Wall")
    endif()
endif()

if(RELEASE AND NOT PROJECT_RELEASE)
    set(PROJECT_RELEASE "${RELEASE}")
elseif(NOT PROJECT_RELEASE)
    set(PROJECT_RELEASE "1")
endif()

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    if(EXISTS "/usr/local/lib/")
        include_directories("/usr/local/include")
        link_directories("/usr/local/lib")
    endif()
    if(EXISTS "/usr/pkg/lib/")
        include_directories("/usr/pkg/include")
        link_directories("/usr/pkg/lib")
    endif()
endif()

if(NOT DEFINED OPEN_BROWSER)
    set(OPEN_BROWSER "xdg-open")
endif()

# Common tarball distribution
add_custom_target(dist
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    COMMAND "${CMAKE_COMMAND}" -E remove -F "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_ARCHIVE}-*.tar.gz"
    COMMAND git archive -o "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_ARCHIVE}-${PROJECT_VERSION}.tar.gz" --format tar.gz --prefix="${PROJECT_ARCHIVE}-${PROJECT_VERSION}/" "v${PROJECT_VERSION}" 2>/dev/null || git archive -o "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_ARCHIVE}-${PROJECT_VERSION}.tar.gz" --format tar.gz --prefix="${PROJECT_ARCHIVE}-${PROJECT_VERSION}/" HEAD
)

# Documentation generation
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile")
    find_package(Doxygen COMPONENTS dot doxygen)
    if(DOXYGEN_FOUND)
        add_custom_target(docgen
            WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
            COMMAND ${DOXYGEN_EXECUTABLE} Doxyfile
            COMMAND ${OPEN_BROWSER} out/html/index.html
        )
    endif()
endif()
