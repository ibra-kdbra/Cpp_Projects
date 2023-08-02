add_library(enum19 INTERFACE)
target_link_libraries(enum19
  INTERFACE CoCpp19::string19
)
target_include_directories(enum19
  INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
file(GLOB enum19_headers "${CMAKE_CURRENT_LIST_DIR}/*.h")
target_sources(enum19
  INTERFACE FILE_SET public_headers
    TYPE HEADERS
    FILES ${enum19_headers}
)

add_library(CoCpp19::enum19 ALIAS enum19)
install(TARGETS enum19
  EXPORT enum19Targets
  FILE_SET public_headers
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    COMPONENT "enum19"
)
install(EXPORT enum19Targets
  FILE "CoCpp19-enum19-targets.cmake"
  NAMESPACE "CoCpp19::"
  DESTINATION ${COCPP19_CMAKE_CONFIG_DESTINATION}
  COMPONENT "enum19"
)
