
add_executable(serialize19-test)
file(GLOB serialize19_test_sources "${CMAKE_CURRENT_LIST_DIR}/*.test.cpp")
target_sources(serialize19-test
  PRIVATE ${serialize19_test_sources}
)
target_link_libraries(serialize19-test
  PRIVATE GTest::gtest_main
  PRIVATE CoCpp19::serialize19
  PRIVATE CoCpp19::optional19
  PRIVATE CoCpp19::partial19
  PRIVATE CoCpp19::string19
  PRIVATE CoCpp19::tuple19
  PRIVATE CoCpp19::variant19
)
add_test(NAME serialize19 COMMAND serialize19-test)
