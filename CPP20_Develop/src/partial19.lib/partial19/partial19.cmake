
add_library(partial19 INTERFACE)
target_link_libraries(partial19
  INTERFACE CoCpp19::meta19
)
target_include_directories(partial19
  INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
file(GLOB partial19_headers "${CMAKE_CURRENT_LIST_DIR}/*.h")
target_sources(partial19
  INTERFACE FILE_SET public_headers
    TYPE HEADERS
    FILES ${partial19_headers}
)

add_library(CoCpp19::partial19 ALIAS partial19)
install(TARGETS partial19
  EXPORT partial19Targets
  FILE_SET public_headers
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    COMPONENT "partial19"
)
install(EXPORT partial19Targets
  FILE "CoCpp19-partial19-targets.cmake"
  NAMESPACE "CoCpp19::"
  DESTINATION ${COCPP19_CMAKE_CONFIG_DESTINATION}
  COMPONENT "partial19"
)
