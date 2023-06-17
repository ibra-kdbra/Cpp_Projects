#include "./move_gen.hpp"

#include <array>

using std::array;
using namespace Chess;

// When iterating through directions, if (sqbb(target_square) & corresponding_mask != 0),
// it means the target_square is in the edge of the board
static constexpr array<Bitboard, 8> dir_masks {
   // Rook directions
   rank_bb(rank8),
   rank_bb(rank1),
   file_bb(fileH),
   file_bb(fileA),
   // Bishop directions
   file_bb(fileH) | rank_bb(rank8),
   file_bb(fileA) | rank_bb(rank8),
   file_bb(fileH) | rank_bb(rank1),
   file_bb(fileA) | rank_bb(rank1),
};

static constexpr array<int, 8> knight_moves {
   6, 10, 15, 17, -6, -10, -15, -17,
};

// Masks to avoid wrap arounds
static constexpr array<Bitboard, 8> knight_moves_corresponding_mask {
   ~(rank_bb(rank1) | file_bb(fileG) | file_bb(fileH)),
   ~(rank_bb(rank1) | file_bb(fileA) | file_bb(fileB)),
   ~(rank_bb(rank1) | rank_bb(rank2) | file_bb(fileH)),
   ~(rank_bb(rank1) | rank_bb(rank2) | file_bb(fileA)),
   ~(rank_bb(rank8) | file_bb(fileB) | file_bb(fileA)),
   ~(rank_bb(rank8) | file_bb(fileH) | file_bb(fileG)),
   ~(rank_bb(rank8) | rank_bb(rank7) | file_bb(fileA)),
   ~(rank_bb(rank8) | rank_bb(rank7) | file_bb(fileH)),
};

Bitboard gen_sliding_piece_moves(const Square sq, const PieceType t, const Bitboard occ, const Bitboard mypieces) {
   Bitboard res = 0;
   const unsigned start = (t == Bishop) ? bishop_dir_start : rook_dir_start;
   const unsigned end = (t == Rook) ? rook_dir_end : bishop_dir_end;
   for (unsigned i = start; i <= end; ++i) {
      if (sqbb(sq) & dir_masks.at(i)) {
         continue;
      }
      Square pos = sq + dir.at(i);
      while (pos >= A1 && pos <= H8) {
         if (sqbb(pos) & occ) {
            res |= sqbb(pos);
            break;
         }
         res |= sqbb(pos);
         if (sqbb(pos) & dir_masks.at(i)) {
            break;
         }
         pos += dir.at(i);
      }
   }
   return res & ~mypieces;
}

Bitboard gen_knight_moves(const Square sq, const Bitboard occ, const Bitboard mypieces) {
   Bitboard res = 0;
   for (int i = 0; i < 8; ++i) {
      const Square target = sq + knight_moves.at(i);
      res |= sqbb(target) & knight_moves_corresponding_mask.at(i);
   }
   return res & ~mypieces;
}

Bitboard gen_king_moves(const Square sq, const Bitboard occ, const Bitboard mypieces, const PieceColor c, const CastlingRights cr) {
   Bitboard res = 0;
   for (int i = 0; i < 8; ++i) {
      if (sqbb(sq) & dir_masks.at(i)) {
         continue;
      }
      const Square target = sq + dir.at(i);
      res |= sqbb(target);
   }

   // TODO: replace castlingSquaresBB with between_bb() for more flexibility
   const Bitboard KSCastlingSquaresBB = (c == White ? WKSCastlingSquaresBB : BKSCastlingSquaresBB);
   const Bitboard QSCastlingSquaresBB = (c == White ? WQSCastlingSquaresBB : BQSCastlingSquaresBB);
   // King side castling
   if (cr & (c == White ? White_OO : Black_OO)) {
      if (!(occ & KSCastlingSquaresBB)) {
         res |= sqbb(sq + 2);
      }
   }
   // Queen side castling
   if (cr & (c == White ? White_OOO : Black_OOO)) {
      if (!(occ & QSCastlingSquaresBB)) {
         res |= sqbb(sq - 2);
      }
   }
   return res & ~mypieces;
}

