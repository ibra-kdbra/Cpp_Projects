cmake_minimum_required(VERSION 3.14)
project(chess-engine CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# raylib
find_package(raylib)
if (NOT raylib_FOUND)
    include(FetchContent)
    FetchContent_Declare(
        raylib
        GIT_REPOSITORY https://github.com/raysan5/raylib.git
        GIT_TAG 4.2.0
    )
    FetchContent_MakeAvailable(raylib)
endif()

#fmt
find_package(fmt)
if (NOT fmt_FOUND)
   include(FetchContent)
   FetchContent_Declare(
      fmt
      GIT_REPOSITORY https://github.com/fmtlib/fmt.git
      GIT_TAG master
   )
   FetchContent_MakeAvailable(fmt)
endif()

include_directories(include/)

set(chess_engine_sources
  src/zobrist.cpp
  src/bitboard.cpp
  src/pawn_move_gen.cpp
  src/move_gen.cpp
  src/load_fen.cpp
  src/board.cpp
)
add_library(chess_engine SHARED ${chess_engine_sources})

set(tests_sources src/tests.cpp)
add_executable(chess_tests ${tests_sources})
target_link_libraries(chess_tests chess_engine fmt)

set(perft_sources src/perft.cpp)
add_executable(chess_perft ${perft_sources})
target_link_libraries(chess_perft chess_engine)

set(gui_sources src/gui_chess.cpp)
add_executable(chess_gui ${gui_sources})
target_link_libraries(chess_gui chess_engine raylib)