#include <iostream>
#include <chrono>

#include "./board.hpp"
#include "./debug.hpp"

int main(int argc, char **argv) {
   // first arg is depth, and second is fen string
   if (argc >= 2) {
      std::string FEN = "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8";
      if (argc == 3) FEN = argv[1];
      Chess::Board b{ FEN };
      int depth = argv[2][0] - '0';
      timed_perft(b, depth);
      return 0;
   }

   // Perft test
   std::string fen{};
   std::cout << "Enter a fen string (std for standard chess): ";
   std::getline(std::cin, fen);
   if (fen == "std") {
      fen = Chess::standard_chess;
   }
   Chess::Board b{ fen };

   while (true) {
      int depth = 0;
      std::cout << "Depth (-1 to exit): ";
      std::cin >> depth;
      if (depth == -1) {
         break;
      }
      Chess::timed_perft(b, depth);
   }

   return 0;
}
