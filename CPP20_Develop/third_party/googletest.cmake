include(FetchContent)
find_package(Threads REQUIRED)
set(THREADS_PREFER_PTHREAD_FLAG ON)

if(NOT ${GTest_FOUND})
  FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        release-1.11.0
  )
  FetchContent_GetProperties(googletest)
  if(NOT googletest_POPULATED)
    FetchContent_Populate(googletest)

    set(gtest_SOURCE_DIR "${googletest_SOURCE_DIR}/googletest")
    add_library(gtest STATIC
      "${gtest_SOURCE_DIR}/src/gtest-all.cc"
    )
    target_include_directories(gtest
      PRIVATE "${gtest_SOURCE_DIR}"
      PUBLIC "${gtest_SOURCE_DIR}/include"
    )
    if(NOT ${CMAKE_USE_PTHREADS_INIT})
      target_compile_definitions(gtest
        PUBLIC GTEST_HAS_PTHREAD=0
      )
    endif()
    target_link_libraries(gtest
      PRIVATE Threads::Threads
    )
    add_library(GTest::gtest ALIAS gtest)

    add_library(gtest_main STATIC
      "${gtest_SOURCE_DIR}/src/gtest_main.cc"
    )
    target_link_libraries(gtest_main
      PUBLIC gtest
    )
    add_library(GTest::gtest_main ALIAS gtest_main)
  endif()
endif()
