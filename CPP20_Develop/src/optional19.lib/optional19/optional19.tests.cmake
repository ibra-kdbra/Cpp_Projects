
add_executable(optional19-test)
file(GLOB optional19_test_sources "${CMAKE_CURRENT_LIST_DIR}/*.test.cpp")
target_sources(optional19-test
  PRIVATE ${optional19_test_sources}
)
target_link_libraries(optional19-test
  PRIVATE GTest::gtest_main
  PRIVATE CoCpp19::optional19
)
add_test(NAME optional19 COMMAND optional19-test)
