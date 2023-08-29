
add_library(serialize19 INTERFACE)
target_link_libraries(serialize19
  INTERFACE CoCpp19::meta19
)
target_include_directories(serialize19
  INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
file(GLOB serialize19_headers "${CMAKE_CURRENT_LIST_DIR}/*.h")
target_sources(serialize19
  INTERFACE FILE_SET public_headers
    TYPE HEADERS
    FILES ${serialize19_headers}
)

add_library(CoCpp19::serialize19 ALIAS serialize19)
install(TARGETS serialize19
  EXPORT serialize19Targets
  FILE_SET public_headers
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    COMPONENT "serialize19"
)
install(EXPORT serialize19Targets
  FILE "CoCpp19-serialize19-targets.cmake"
  NAMESPACE "CoCpp19::"
  DESTINATION ${COCPP19_CMAKE_CONFIG_DESTINATION}
  COMPONENT "serialize19"
)
