
add_executable(array19-test)
file(GLOB array19_test_sources "${CMAKE_CURRENT_LIST_DIR}/*.test.cpp")
target_sources(array19-test
  PRIVATE ${array19_test_sources}
)
target_link_libraries(array19-test
  PRIVATE GTest::gtest_main
  PRIVATE CoCpp19::array19
)
add_test(NAME array19 COMMAND array19-test)
