
add_executable(coro19-test)
file(GLOB coro19_test_sources "${CMAKE_CURRENT_LIST_DIR}/*.test.cpp")
target_sources(coro19-test
  PRIVATE ${coro19_test_sources}
)
target_link_libraries(coro19-test
  PRIVATE GTest::gtest_main
  PRIVATE CoCpp19::coro19
)
add_test(NAME coro19 COMMAND coro19-test)
