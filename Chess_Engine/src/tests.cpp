#include "./board.hpp"
#include "./debug.hpp"

#include <fmt/color.h>

void help();
void assure(const int testID, Chess::Board& b, const int depth, const size_t expected, const bool print_time);

int main(int argc, char **argv) {
   [[maybe_unused]] bool extended_tests{ false };
   for (int i = 1; i < argc; ++i) {
      if (std::string(argv[i]) == "-h" || std::string(argv[i]) == "--help")
         help(), exit(0);
      else if (std::string(argv[i]) == "-e" || std::string(argv[i]) == "--extended")
         extended_tests = true;
      else help(), exit(1);
   }

   if (!extended_tests) fmt::print("Expected time (release build): ~30 seconds\n\n");

   { // Test 1
      Chess::Board b{ "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8" };
      assure(1, b, 3, 62'379, true);
   }
   { // Test 2
      Chess::Board b{ "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10" };
      assure(2, b, 3, 89'890, true);
   }
   { // Test 3
      Chess::Board b{ "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1" };
      assure(3, b, 4, 422'333, true);
   }
   { // Test 4
      Chess::Board b{ "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - -" };
      assure(4, b, 5, 674'624, true);
   }
   { // Test 5
      Chess::Board b{ "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -" };
      assure(5, b, 3, 97'862, true);
   }
   { // Test 6
      Chess::Board b{ Chess::standard_chess };
      assure(6, b, 4, 197'281, true);
   }
   fmt::print(fg(fmt::color::light_green) | fmt::emphasis::bold, "All test passed!\n");
}

void help() {
   fmt::print("Usage: ./tests [-e|--extended]\n");
   fmt::print("\t-h, --help display this message and exit\n");
   fmt::print("\t-e, --extended run more test (takes more time)\n");
}

void assure(const int testID, Chess::Board& b, const int depth, const size_t expected, const bool print_time) {
   const auto generated{ print_time ? Chess::timed_perft(b, depth) : Chess::perft(b, depth) };
   if (generated != expected) {
      fmt::print(stderr, "Test {}: ", testID);
      fmt::print(stderr, fg(fmt::color::red) | fmt::emphasis::bold, "failed! ");
      fmt::print(stderr, "({} moves generated, {} were expected)\n", generated, expected);
      exit(1);
   }
   else {
      fmt::print(stderr, "Test {}: ", testID);
      fmt::print(stderr, fg(fmt::color::light_green),"good!\n\n");
   }
}
