add_library(coro19 STATIC)
target_link_libraries(coro19
  PUBLIC CoCpp19::array19
)
target_include_directories(coro19
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
file(GLOB coro19_headers "${CMAKE_CURRENT_LIST_DIR}/*.h")
target_sources(coro19
  PRIVATE "coro19/coroutine.cpp"
  PUBLIC FILE_SET public_headers
    TYPE HEADERS
    FILES ${coro19_headers}
)

add_library(CoCpp19::coro19 ALIAS coro19)
install(TARGETS coro19
  EXPORT coro19Targets
  LIBRARY COMPONENT "coro19"
  ARCHIVE COMPONENT "coro19"
  RUNTIME COMPONENT "coro19"
  FILE_SET public_headers
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    COMPONENT "coro19"
)
install(EXPORT coro19Targets
  FILE "CoCpp19-coro19-targets.cmake"
  NAMESPACE "CoCpp19::"
  DESTINATION ${COCPP19_CMAKE_CONFIG_DESTINATION}
  COMPONENT "coro19"
)
