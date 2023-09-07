
add_library(string19 INTERFACE)
target_link_libraries(string19
  INTERFACE CoCpp19::tuple19
)
target_include_directories(string19
  INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
file(GLOB string19_headers "${CMAKE_CURRENT_LIST_DIR}/*.h")
target_sources(string19
  INTERFACE FILE_SET public_headers
    TYPE HEADERS
    FILES ${string19_headers}
)

add_library(CoCpp19::string19 ALIAS string19)
install(TARGETS string19
  EXPORT string19Targets
  FILE_SET public_headers
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    COMPONENT "string19"
)
install(EXPORT string19Targets
  FILE "CoCpp19-string19-targets.cmake"
  NAMESPACE "CoCpp19::"
  DESTINATION ${COCPP19_CMAKE_CONFIG_DESTINATION}
  COMPONENT "string19"
)
