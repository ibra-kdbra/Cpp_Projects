#pragma once

#include "./utils.hpp"

using namespace Chess;

/*!
   Generate (pseudo-legal) moves of sliding pieces (queen, rook and bishop).
   @param s: the position of the piece.
   @param t: type of the piece (Queen/Rook/Bishop).

   @return a Bitboard containing the moves.
*/
Bitboard gen_sliding_piece_moves(const Square sq, const PieceType t, const Bitboard occ, const Bitboard mypieces);

/*!
   Generate legal moves of king.
   @param s: the position of the piece.

   @return a Bitboard containing the moves.
*/
Bitboard gen_king_moves(const Square sq, const Bitboard occ, const Bitboard mypieces, const PieceColor c, const CastlingRights cr);

/*!
   Generate (pseudo-legal) moves of knight.
   @param s: the position of the piece.

   @return a Bitboard containing the moves.
*/
Bitboard gen_knight_moves(const Square sq, const Bitboard occ, const Bitboard mypieces);

/*!
   Generate pawn push if the target square is empty.
   @param s: the position of the pawn.

   @return a Bitboard containing the moves.
*/
Bitboard gen_pawn_push(const Square sq, const PieceColor c, const Bitboard occ);

/*!
   Generate pawn push and double push if the target square is empty.
   @param s: the position of the pawn.

   @return a Bitboard containing the moves.
*/
Bitboard gen_double_push(const Square sq, const PieceColor c, const Bitboard occ);

/*!
   Generates pawn attacks if corresponding positions are occupied by enemy.
   @param s: the position of the pawn.

   @return a Bitboard containing the moves.
*/
Bitboard gen_pawn_attacks(const Square sq, const PieceColor c, const Bitboard occ, const Bitboard mypieces);
