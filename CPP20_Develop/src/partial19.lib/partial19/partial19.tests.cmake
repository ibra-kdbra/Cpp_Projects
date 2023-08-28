
add_executable(partial19-test)
file(GLOB partial19_test_sources "${CMAKE_CURRENT_LIST_DIR}/*.test.cpp")
target_sources(partial19-test
  PRIVATE ${partial19_test_sources}
)
target_link_libraries(partial19-test
  PRIVATE GTest::gtest_main
  PRIVATE CoCpp19::partial19
)
add_test(NAME partial19 COMMAND partial19-test)
