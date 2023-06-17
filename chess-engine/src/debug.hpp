#pragma once

#include <chrono>
#include <iostream>

#include "./board.hpp"
#include "./bitboard.hpp"

//************************************************
//
// ** Run perft and return the number of nodes
// Bitboard perft(Board& b, const int depth);
// ** Run perft and print time to stdout
// ** and return the number of nodes
// Bitboard timed_perft(Board &b, int depth);
//
//************************************************

using std::chrono::high_resolution_clock;
using std::chrono::duration;
using std::chrono::seconds;
using std::milli;

namespace Chess
{

inline std::ostream& operator<<(std::ostream& os, PieceColor c) {
   return os << (c == White ? "White" : c == Black ? "Black" : "NoColor");
}

inline string pt_to_str(const PieceType pt) {
   if (pt >= 6) return "";
   return array{
      "Knight", "Bishop", "Rook",
      "Queen", "King", "Pawn"
   }[pt];
}

inline std::ostream& operator<<(std::ostream& os, Square sq) {
   if (sq == NoSquare) {
      return os << "No Square";
   }
   else {
      char str[3] = {0};
      str[0] = 'A' + (sq%8);
      str[1] = '1' + (sq/8);
      return os << str;
   }
}
[[maybe_unused]]
inline void print_bb(Bitboard bb) {
   for (int i=7; i >= 0; --i) {
      for (unsigned j=0; j < 8; ++j) {
         std::cout << ((bb & sqbb( Square(i*8+j) )) ? '1' : '.');
      }
      std::cout << '\n';
   }
   std::cout << '\n';
}

static const char pieces_char[] = {'N', 'B', 'R', 'Q', 'K', 'P'};

[[maybe_unused]]
static void print_board(Board& b, PieceColor p = NoColor) {
   for (int i = 7; i >= 0; --i) {
      for (int j = 0; j < 8; ++j) {
         const Square sq = Square(i*8 + j);
         PieceType t = b.get_piece_type(sq);
         if (b.is_square_occupied(sq) && (p == NoColor || b.get_piece_color(sq) == p)) {
            char c = pieces_char[t] + (b.get_piece_color(sq) == White ? 0 : 32);
            std::cout << c;
         }
         else std::cout << '.';
      }
      std::cout << '\n';
   }
   std::cout << '\n';
}

[[maybe_unused]]
inline string sqstr(const Square sq) {
   string str = "a1";
   str[0] += sq%8;
   str[1] += sq/8;
   return str;
}

[[maybe_unused]]
inline string str_repeat(const string &str, const int n) {
   string res = "";
   for (int i = 0; i < n; ++i) res += str;
   return res;
}

[[maybe_unused]]
inline Bitboard perft(Board& b, const int depth, const int original_depth = 0) {
   if (depth == 0) return 1ull;

   int pos_num = 0;
   const vector<Move> moves{ b.get_moves() };

   [[maybe_unused]] int x = 0;
   for (size_t i = 0; i < moves.size(); ++i) {
      string m = sqstr(moves[i].from) + sqstr(moves[i].to);
      if (b.get_piece_type(moves[i].from) == Pawn && (moves[i].to <= H1 || moves[i].to >= A8)) {
         m += pieces_char[moves[i].pt];
      }
      if (b.make_move(moves[i].from, moves[i].to, moves[i].pt) == Chess::InvalidMove) {
         std::cerr << "perft() invalid move: "
              << sqstr(moves[i].from) << " -> " << sqstr(moves[i].to) << '\n';
         print_board(b);
         exit(1);
      };
      int j = perft(b, depth - 1, original_depth == 0 ? depth : original_depth);
      x = j;
      pos_num += j;
      b.unmake_move();
      //if (depth > 1) std::cout << str_repeat("-- ", depth) << ' ' << m << ' ' << x << '\n';
   }
   return pos_num;
}

inline Bitboard timed_perft(Board &b, int depth) {
   auto t1 = high_resolution_clock::now();
   Bitboard nodes = perft(b, depth);
   auto t2 = high_resolution_clock::now();
   duration<double, milli> exec_time_ms = t2 - t1;
   duration<double> exec_time_s = exec_time_ms;
   std::cout << "Nodes: " << nodes << '\n';
   std::cout << "Calculated in: " << exec_time_s.count() << "s, or " << exec_time_ms.count() << "ms\n" ;
   return nodes;
}

} // namespace Chess
