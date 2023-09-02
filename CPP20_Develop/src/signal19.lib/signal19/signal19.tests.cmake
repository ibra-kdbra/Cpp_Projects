
add_executable(signal19-test)
file(GLOB signal19_test_sources "${CMAKE_CURRENT_LIST_DIR}/*.test.cpp")
target_sources(signal19-test
  PRIVATE ${signal19_test_sources}
)
target_link_libraries(signal19-test
  PRIVATE GTest::gtest_main
  PRIVATE CoCpp19::signal19
)
add_test(NAME signal19 COMMAND signal19-test)
