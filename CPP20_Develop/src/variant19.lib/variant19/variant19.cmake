
add_library(variant19 INTERFACE)
target_link_libraries(variant19
  INTERFACE CoCpp19::meta19
)
target_include_directories(variant19
  INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
file(GLOB variant19_headers "${CMAKE_CURRENT_LIST_DIR}/*.h")
target_sources(variant19
  INTERFACE FILE_SET public_headers
    TYPE HEADERS
    FILES ${variant19_headers}
)

add_library(CoCpp19::variant19 ALIAS variant19)
install(TARGETS variant19
  EXPORT variant19Targets
  FILE_SET public_headers
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    COMPONENT "variant19"
)
install(EXPORT variant19Targets
  FILE "CoCpp19-variant19-targets.cmake"
  NAMESPACE "CoCpp19::"
  DESTINATION ${COCPP19_CMAKE_CONFIG_DESTINATION}
  COMPONENT "variant19"
)
