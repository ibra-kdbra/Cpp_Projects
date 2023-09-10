
add_library(strong19 INTERFACE)
target_link_libraries(strong19
  INTERFACE CoCpp19::string19
)
target_include_directories(strong19
  INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
file(GLOB strong19_headers "${CMAKE_CURRENT_LIST_DIR}/*.h")
target_sources(strong19
  INTERFACE FILE_SET public_headers
    TYPE HEADERS
    FILES ${strong19_headers}
)

add_library(CoCpp19::strong19 ALIAS strong19)
install(TARGETS strong19
  EXPORT strong19Targets
  FILE_SET public_headers
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    COMPONENT "strong19"
)
install(EXPORT strong19Targets
  FILE "CoCpp19-strong19-targets.cmake"
  NAMESPACE "CoCpp19::"
  DESTINATION ${COCPP19_CMAKE_CONFIG_DESTINATION}
  COMPONENT "strong19"
)
