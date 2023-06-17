#include "./move_gen.hpp"

using namespace Chess;

Bitboard gen_pawn_push(const Square sq, const PieceColor c, const Bitboard occ) {
   Square pushpos = sq + 8*pawn_direction(c);
   if (pushpos < 0 || pushpos > 63 || occ & sqbb(pushpos)) {
      return 0;
   }
   return sqbb(pushpos);
}

Bitboard gen_double_push(const Square sq, const PieceColor c, const Bitboard occ) {
   Bitboard res = 0;
   res |= gen_pawn_push(sq, c, occ);
   if (res != 0) {
      res |= gen_pawn_push(sq+8*pawn_direction(c), c, occ);
   }
   return res;
}

Bitboard gen_pawn_attacks(const Square sq, const PieceColor c, const Bitboard occ, const Bitboard mypieces) {
   Bitboard res = 0;
   constexpr int targets[2] = {9, 7};
   const Bitboard masks[2] {
      c == White ? ~(file_bb(fileA) | rank_bb(rank1)) : ~(file_bb(fileH) | rank_bb(rank8)),
      c == White ? ~(file_bb(fileH) | rank_bb(rank1)) : ~(file_bb(fileA) | rank_bb(rank8)),
   };
   for (int i = 0; i < 2; ++i) {
      const Square target = sq + targets[i]*pawn_direction(c);
      if (sqbb(target) & occ) {
         res |= sqbb(target) & masks[i];
      }
   }
   return res & ~mypieces;
}
