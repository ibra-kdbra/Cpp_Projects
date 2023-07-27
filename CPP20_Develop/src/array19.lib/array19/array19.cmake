
add_library(array19 INTERFACE)
target_compile_features(array19
  INTERFACE cxx_std_20
)
target_include_directories(array19
  INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
file(GLOB array19_headers "${CMAKE_CURRENT_LIST_DIR}/*.h")
target_sources(array19
  INTERFACE FILE_SET public_headers
    TYPE HEADERS
    FILES ${array19_headers}
)

add_library(CoCpp19::array19 ALIAS array19)
install(TARGETS array19
  EXPORT array19Targets
  FILE_SET public_headers
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    COMPONENT "array19"
)
install(EXPORT array19Targets
  FILE "CoCpp19-array19-targets.cmake"
  NAMESPACE "CoCpp19::"
  DESTINATION ${COCPP19_CMAKE_CONFIG_DESTINATION}
  COMPONENT "array19"
)
