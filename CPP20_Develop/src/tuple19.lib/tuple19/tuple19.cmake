
add_library(tuple19 INTERFACE)
target_link_libraries(tuple19
  INTERFACE CoCpp19::meta19
)
target_include_directories(tuple19
  INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
file(GLOB tuple19_headers "${CMAKE_CURRENT_LIST_DIR}/*.h")
target_sources(tuple19
  INTERFACE FILE_SET public_headers
    TYPE HEADERS
    FILES ${tuple19_headers}
)

add_library(CoCpp19::tuple19 ALIAS tuple19)
install(TARGETS tuple19
  EXPORT tuple19Targets
  FILE_SET public_headers
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    COMPONENT "tuple19"
)
install(EXPORT tuple19Targets
  FILE "CoCpp19-tuple19-targets.cmake"
  NAMESPACE "CoCpp19::"
  DESTINATION ${COCPP19_CMAKE_CONFIG_DESTINATION}
  COMPONENT "tuple19"
)
