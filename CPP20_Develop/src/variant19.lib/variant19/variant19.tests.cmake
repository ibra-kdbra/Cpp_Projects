
add_executable(variant19-test)
file(GLOB variant19_test_sources "${CMAKE_CURRENT_LIST_DIR}/*.test.cpp")
target_sources(variant19-test
  PRIVATE ${variant19_test_sources}
)
target_link_libraries(variant19-test
  PRIVATE GTest::gtest_main
  PRIVATE CoCpp19::variant19
)
add_test(NAME variant19 COMMAND variant19-test)
