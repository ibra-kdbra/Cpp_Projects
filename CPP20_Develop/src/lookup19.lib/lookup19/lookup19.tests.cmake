
add_executable(lookup19-test)
file(GLOB lookup19_test_sources "${CMAKE_CURRENT_LIST_DIR}/*.test.cpp")
target_sources(lookup19-test
  PRIVATE ${lookup19_test_sources}
)
target_link_libraries(lookup19-test
  PRIVATE GTest::gtest_main
  PRIVATE CoCpp19::lookup19
)
add_test(NAME lookup19 COMMAND lookup19-test)
