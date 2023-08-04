
add_library(meta19 INTERFACE)
target_link_libraries(meta19
  INTERFACE CoCpp19::array19
)
target_include_directories(meta19
  INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
file(GLOB meta19_headers "${CMAKE_CURRENT_LIST_DIR}/*.h")
target_sources(meta19
  INTERFACE FILE_SET public_headers
    TYPE HEADERS
    FILES ${meta19_headers}
)

add_library(CoCpp19::meta19 ALIAS meta19)
install(TARGETS meta19
  EXPORT meta19Targets
  FILE_SET public_headers
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    COMPONENT "meta19"
)
install(EXPORT meta19Targets
  FILE "CoCpp19-meta19-targets.cmake"
  NAMESPACE "CoCpp19::"
  DESTINATION ${COCPP19_CMAKE_CONFIG_DESTINATION}
  COMPONENT "meta19"
)
