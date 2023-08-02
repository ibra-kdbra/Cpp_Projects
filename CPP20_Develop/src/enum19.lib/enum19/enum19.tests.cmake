
add_executable(enum19-test)
file(GLOB enum19_test_sources "${CMAKE_CURRENT_LIST_DIR}/*.test.cpp")
target_sources(enum19-test
  PRIVATE ${enum19_test_sources}
)
target_link_libraries(enum19-test
  PRIVATE GTest::gtest_main
  PRIVATE CoCpp19::enum19
)
add_test(NAME enum19 COMMAND enum19-test)
