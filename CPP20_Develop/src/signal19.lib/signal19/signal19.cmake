
add_library(signal19 INTERFACE)
target_link_libraries(signal19
  INTERFACE CoCpp19::array19
)
target_include_directories(signal19
  INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
file(GLOB signal19_headers "${CMAKE_CURRENT_LIST_DIR}/*.h")
target_sources(signal19
  INTERFACE FILE_SET public_headers
    TYPE HEADERS
    FILES ${signal19_headers}
)

add_library(CoCpp19::signal19 ALIAS signal19)
install(TARGETS signal19
  EXPORT signal19Targets
  FILE_SET public_headers
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    COMPONENT "signal19"
)
install(EXPORT signal19Targets
  FILE "CoCpp19-signal19-targets.cmake"
  NAMESPACE "CoCpp19::"
  DESTINATION ${COCPP19_CMAKE_CONFIG_DESTINATION}
  COMPONENT "signal19"
)
