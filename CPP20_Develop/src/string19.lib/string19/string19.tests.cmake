
add_executable(string19-test)
file(GLOB string19_test_sources "${CMAKE_CURRENT_LIST_DIR}/*.test.cpp")
target_sources(string19-test
  PRIVATE ${string19_test_sources}
)
target_link_libraries(string19-test
  PRIVATE GTest::gtest_main
  PRIVATE CoCpp19::string19
)
add_test(NAME string19 COMMAND string19-test)
