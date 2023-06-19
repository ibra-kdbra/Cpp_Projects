#include "./bitboard.hpp"
#include "./move_gen.hpp"

namespace Chess {

static array<array<Bitboard, 64>, 64> _between_bb_all() {
   array<array<Bitboard, 64>, 64> arr{};
   for (Square sq1 = A1; sq1 <= H8; ++sq1) {
      for (auto pt: { Rook, Bishop }) {
         for (Square sq2 = A1; sq2 <= H8; ++sq2) {
            if (gen_sliding_piece_moves(sq1, pt, 0, 0) & sqbb(sq2)) {
               arr[sq1][sq2] = gen_sliding_piece_moves(sq1, pt, sqbb(sq2), 0) & gen_sliding_piece_moves(sq2, pt, sqbb(sq1), 0);
            }
            arr[sq1][sq2] |= sqbb(sq2);
         }
      }
   }
   return arr;
}

const array<array<Bitboard, 64>, 64> _betweenBB = _between_bb_all();

} // namespace Chess
