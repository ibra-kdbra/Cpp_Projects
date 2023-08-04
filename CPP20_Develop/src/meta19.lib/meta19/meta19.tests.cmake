
add_executable(meta19-test)
file(GLOB meta19_test_sources "${CMAKE_CURRENT_LIST_DIR}/*.test.cpp")
target_sources(meta19-test
  PRIVATE ${meta19_test_sources}
)
target_link_libraries(meta19-test
  PRIVATE GTest::gtest_main
  PRIVATE CoCpp19::meta19
)
add_test(NAME meta19 COMMAND meta19-test)
