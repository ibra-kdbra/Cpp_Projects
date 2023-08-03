
add_library(lookup19 INTERFACE)
target_link_libraries(lookup19
  INTERFACE CoCpp19::array19
)
target_include_directories(lookup19
  INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
file(GLOB lookup19_headers "${CMAKE_CURRENT_LIST_DIR}/*.h")
target_sources(lookup19
  INTERFACE FILE_SET public_headers
    TYPE HEADERS
    FILES ${lookup19_headers}
)

add_library(CoCpp19::lookup19 ALIAS lookup19)
install(TARGETS lookup19
  EXPORT lookup19Targets
  FILE_SET public_headers
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    COMPONENT "lookup19"
)
install(EXPORT lookup19Targets
  FILE "CoCpp19-lookup19-targets.cmake"
  NAMESPACE "CoCpp19::"
  DESTINATION ${COCPP19_CMAKE_CONFIG_DESTINATION}
  COMPONENT "lookup19"
)
