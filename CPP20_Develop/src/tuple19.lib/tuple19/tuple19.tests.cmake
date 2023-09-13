
add_executable(tuple19-test)
file(GLOB tuple19_test_sources "${CMAKE_CURRENT_LIST_DIR}/*.test.cpp")
target_sources(tuple19-test
  PRIVATE ${tuple19_test_sources}
)
target_link_libraries(tuple19-test
  PRIVATE GTest::gtest_main
  PRIVATE CoCpp19::tuple19
)
add_test(NAME tuple19 COMMAND tuple19-test)
