#pragma once

#include <array>
#include "./board.hpp"

namespace Chess
{
/*!
   Search function, for now it just counts material.
*/
inline int evaluate(const Board& b) {
   using namespace Chess;
   // { WN, WB, WR, WQ, WK, WP, BN, BB, BR, BQ, BK, BP }
   static constexpr array<int, 12> piece_value = {3, 3, 5, 9, 0, 1, -3, -3, -5, -9, 0, -1};
   int res = 0;
   for (size_t i = 0; i < b.piecesBB.size(); ++i) {
      res += piece_value.at(i) * popcnt(b.piecesBB.at(i));
   }
   return res;
}
} // namespace Chess