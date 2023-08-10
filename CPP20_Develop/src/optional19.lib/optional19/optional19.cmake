
add_library(optional19 INTERFACE)
target_link_libraries(optional19
  INTERFACE CoCpp19::meta19
)
target_include_directories(optional19
  INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
file(GLOB optional19_headers "${CMAKE_CURRENT_LIST_DIR}/*.h")
target_sources(optional19
  INTERFACE FILE_SET public_headers
    TYPE HEADERS
    FILES ${optional19_headers}
)

add_library(CoCpp19::optional19 ALIAS optional19)
install(TARGETS optional19
  EXPORT optional19Targets
  FILE_SET public_headers
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    COMPONENT "optional19"
)
install(EXPORT optional19Targets
  FILE "CoCpp19-optional19-targets.cmake"
  NAMESPACE "CoCpp19::"
  DESTINATION ${COCPP19_CMAKE_CONFIG_DESTINATION}
  COMPONENT "optional19"
)
