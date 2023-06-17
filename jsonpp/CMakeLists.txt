cmake_minimum_required(VERSION 3.10)

project(ccdJsonpp VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

option(BUILD_SHARED_LIBS "Build as shared library" ON)

set(include_dest "include/ccdJsonpp-${ccdJsonpp_VERSION}")
set(lib_dest "lib/ccdJsonpp-${ccdJsonpp_VERSION}")
set(cmake_dest "${lib_dest}/cmake")

add_subdirectory(src)

configure_file(ccdJsonppConfigVersion.cmake.in 
	${CMAKE_CURRENT_BINARY_DIR}/ccdJsonppConfigVersion.cmake
	@ONLY)

install(EXPORT ccdJsonpp
	DESTINATION "${cmake_dest}")

install(FILES ccdJsonppConfig.cmake 
	${CMAKE_CURRENT_BINARY_DIR}/ccdJsonppConfigVersion.cmake
	DESTINATION ${cmake_dest})
