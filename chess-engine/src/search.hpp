#pragma once

#include "./board.hpp"
#include "./bitboard.hpp"
#include "./evaluate.hpp"
#include "./debug.hpp"

#include <cstdint>
#include <vector>
#include <algorithm>
#include <atomic>

namespace Chess
{
struct MoveEval {
   Move move{};
   int eval = 0;
   bool operator<(const MoveEval& other) const { return eval < other.eval; }
   bool operator>(const MoveEval& other) const { return eval > other.eval; }
   bool operator<=(const MoveEval& other) const { return eval <= other.eval; }
   bool operator>=(const MoveEval& other) const { return eval >= other.eval; }
};

// Transposition Table Data
struct TTData {
   Key hash{};
   MoveEval best{};
   uint8_t depth{};
};

inline MoveEval alpha_beta_pruning(Board& b, const unsigned depth,
                                   const bool maximizing_player,
                                   int alpha, int beta) {
   std::vector<Move> moves{ b.get_moves() };
   // Sort according to priority
   std::sort(moves.begin(), moves.end(),
         [&](const Move& m1, const Move& m2){ return m1.less_than(m2, b); }
   );

   MoveEval res{ Move{}, maximizing_player ? -999 : 999 };
   for (auto& m: moves) {
      b.make_move(m.from, m.to, m.pt);
      MoveEval curr{m, };

      if (b.get_state() == Checkmate) {
         curr.eval = maximizing_player ? 999 : -999;
         res = curr;
         b.unmake_move();
         break;
      }
      else if (b.get_state() == Draw) {
         curr.eval = 0;
         res = maximizing_player ? std::max(res, curr) : std::min(res, curr);
      }
      else if (depth == 0) {
         curr.eval = evaluate(b);
         res = maximizing_player? std::max(res, curr) : std::min(res, curr);
      }
      else if (maximizing_player) {
         curr.eval = alpha_beta_pruning(b, depth - 1, false, alpha, beta).eval;
         res = std::max(res, curr);
         alpha = std::max(alpha, curr.eval);
         if (beta <= alpha) {
            b.unmake_move();
            break;
         }
      }
      else {
         curr.eval = alpha_beta_pruning(b, depth - 1, true, alpha, beta).eval;
         res = std::min(res, curr);
         beta = std::min(beta, curr.eval);
         if (beta <= alpha) {
            b.unmake_move();
            break;
         }
      }
      b.unmake_move();
   }
   return res;
}

inline MoveEval search(Board b,
         const unsigned depth) {
   return alpha_beta_pruning(b, depth, b.color_to_play == White, -999, 999);
}

} // namespace Chess
