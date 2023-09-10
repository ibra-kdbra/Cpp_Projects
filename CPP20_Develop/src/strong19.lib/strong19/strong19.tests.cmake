
add_executable(strong19-test)
file(GLOB strong19_test_sources "${CMAKE_CURRENT_LIST_DIR}/*.test.cpp")
target_sources(strong19-test
  PRIVATE ${strong19_test_sources}
)
target_link_libraries(strong19-test
  PRIVATE GTest::gtest_main
  PRIVATE CoCpp19::strong19
)
add_test(NAME strong19 COMMAND strong19-test)
