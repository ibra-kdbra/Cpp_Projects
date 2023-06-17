#pragma once

#include <cstdint>
#include <array>

namespace Chess
{

#define ENABLE_BASE_OPERATORS_ON(T)                                \
constexpr T operator+(T d1, int d2) { return T(int(d1) + d2); }    \
constexpr T operator-(T d1, int d2) { return T(int(d1) - d2); }    \
constexpr T operator-(T d) { return T(-int(d)); }                  \
inline T& operator+=(T& d1, int d2) { return d1 = d1 + d2; }       \
inline T& operator-=(T& d1, int d2) { return d1 = d1 - d2; }

#define ENABLE_INCR_OPERATORS_ON(T)                                \
inline T& operator++(T& d) { return d = T(int(d) + 1); }           \
inline T& operator--(T& d) { return d = T(int(d) - 1); }

#define ENABLE_OPERATORS_ON(T)                  \
   ENABLE_BASE_OPERATORS_ON(T)                  \
   ENABLE_INCR_OPERATORS_ON(T)

using std::size_t;
using std::array;

using Key      = uint64_t; // For Zobrist
using Bitboard = uint64_t;
using PiecePos = uint64_t;

enum PieceColor { White, Black, NoColor };
inline constexpr PieceColor operator~(PieceColor c) {
   return PieceColor(c ^ 1);
}
inline constexpr int pawn_direction(PieceColor c) {
   return c == White ? 1 : -1;
}

enum PieceType { Knight, Bishop, Rook, Queen, King, Pawn, PieceTypeNum = 6, NoType };

enum Square {
  A1, B1, C1, D1, E1, F1, G1, H1,
  A2, B2, C2, D2, E2, F2, G2, H2,
  A3, B3, C3, D3, E3, F3, G3, H3,
  A4, B4, C4, D4, E4, F4, G4, H4,
  A5, B5, C5, D5, E5, F5, G5, H5,
  A6, B6, C6, D6, E6, F6, G6, H6,
  A7, B7, C7, D7, E7, F7, G7, H7,
  A8, B8, C8, D8, E8, F8, G8, H8,
  SqNum = 64, NoSquare
};
inline constexpr bool is_ok(const Square sq) { return sq >= A1 && sq <= H8; };
inline constexpr Bitboard sqbb(Square sq) { return sq < A1 || sq > H8 ? 0 : 1ull << sq; };
ENABLE_OPERATORS_ON(Square);

enum File {
   fileA, fileB, fileC, fileD, fileE, fileF, fileG, fileH
};
inline constexpr Bitboard _fileA = 0x0101010101010101;
inline constexpr File get_file(Square sq) { return File(sq % 8); }
inline constexpr Bitboard file_bb(File file) { return _fileA << file; }
inline constexpr Bitboard file_bb(Square sq) { return file_bb(get_file(sq)); }
inline constexpr bool squares_on_same_file(Square sq1, Square sq2) { return get_file(sq1) == get_file(sq2); }

enum Rank { rank1, rank2, rank3, rank4, rank5, rank6, rank7, rank8 };

inline constexpr Bitboard _rank1 = 255;
inline constexpr Rank get_rank(Square sq) { return Rank(sq / 8); }
inline constexpr Bitboard rank_bb(Rank rank) { return _rank1 << (8*rank); }
inline constexpr Bitboard rank_bb(Square sq) { return rank_bb(get_rank(sq)); }
inline constexpr bool squares_on_same_rank(Square sq1, Square sq2) { return get_rank(sq1) == get_rank(sq2); }

enum Direction {
   North,
   South,
   East,
   West,
   NoEa,
   NoWe,
   SoEa,
   SoWe,
   DirNum = 8
};

static constexpr array<int, DirNum> dir = {
    // Rook directions
    8, -8, 1, -1,
    // Bishop directions
    9, 7, -7, -9,
};
// Useful indices
inline constexpr size_t rook_dir_start    = 0;
inline constexpr size_t rook_dir_end      = 3;
inline constexpr size_t bishop_dir_start  = 4;
inline constexpr size_t bishop_dir_end    = 7;

using CastlingRights = uint8_t;
enum {
   NoCastling,
   White_OO,
   Black_OO       = 1 << 1,
   White_OOO      = 1 << 2,
   Black_OOO      = 1 << 3,
   KingSide      = White_OO  | Black_OO,
   QueenSide     = White_OOO | Black_OOO,
   WhiteCastling = White_OO  | White_OOO,
   BlackCastling = Black_OO  | Black_OOO,
   AnyCastling   = WhiteCastling | BlackCastling,
};

inline constexpr Bitboard WKSCastlingSquaresBB = (1ull << F1) | (1ull << G1);
inline constexpr Bitboard BKSCastlingSquaresBB = (1ull << F8) | (1ull << G8);
inline constexpr Bitboard WQSCastlingSquaresBB = (1ull << C1) | (1ull << D1);
inline constexpr Bitboard BQSCastlingSquaresBB = (1ull << C8) | (1ull << D8);

/*
  bit 0-5: the origin square
  bit 6-11: the target square
  bit 12-13: promotion type
  bit 14-15: special move flag: promotion (1), en passant (2), castling (3)
  bit 16-18: type of piece taken if there is any
  bit 19-22: castling rights before making the move
  bit 23-29: en passant target (6 bits because +1 for NoSquare)
  bit 30-36: value of 50 move rule counter
*/
using MoveData = uint64_t;
enum MoveType {
   Normal,
   Promotion,
   En_passant,
   Castling,
};
// masks
inline constexpr Bitboard md_from             = 63;
inline constexpr Bitboard md_to               = 63 << 6;
inline constexpr Bitboard md_promotion_type   = 3 << 12;
inline constexpr Bitboard md_move_type        = 3 << 14;
inline constexpr Bitboard md_taken_piece_type = 7 << 16;
inline constexpr Bitboard md_castling_rights  = 15 << 19;
inline constexpr Bitboard md_ep_square        = 127 << 23;
inline constexpr Bitboard md_fmrc             = 127ul << 30;

inline constexpr MoveData new_md(Square from, Square to, PieceType pt, MoveType mt, PieceType taken,
                                 CastlingRights cr, Square ep_square, size_t fmrc) {
   return ((fmrc & 127) << 30) | (ep_square << 23) | (cr << 19) | (taken << 16) | (mt << 14) | (pt << 12) | (to << 6) | from;
}
inline constexpr Square md_get_square_from(MoveData m) {
   return Square(m & md_from);
}
inline constexpr Square md_get_square_to(MoveData m) {
   return Square((m & md_to) >> 6);
}
inline constexpr PieceType md_get_promotion_type(MoveData m) {
   return PieceType((m & md_promotion_type) >> 12);
}
inline constexpr MoveType md_get_move_type(MoveData m) {
   return MoveType((m & md_move_type) >> 14);
}
inline constexpr PieceType md_get_taken_piece_type(MoveData m) {
   return PieceType((m & md_taken_piece_type) >> 16);
}
inline constexpr CastlingRights md_get_castling_rights(MoveData m) {
   return CastlingRights((m & md_castling_rights) >> 19);
}
inline constexpr Square md_get_ep_square(MoveData m) {
   return Square((m & md_ep_square) >> 23);
}

inline constexpr size_t md_get_fmrc(MoveData m) {
   return (m & md_fmrc) >> 30;
}

} // namespace Chess

#undef ENABLE_OPERATORS_ON
#undef ENABLE_BASE_OPERATORS_ON
#undef ENABLE_INCR_OPERATORS_ON
