#pragma once

#include <array>
#include <vector>
#include <string>

#include "./utils.hpp"

namespace Chess {

using std::string;
using std::array;
using std::vector;

// enum to access pieces' positions
enum PieceBBIndex : size_t { WN, WB, WR, WQ, WK, WP, BN, BB, BR, BQ, BK, BP, invalid_index = 99999999999 };

struct Board;
struct Move {
   Square from;
   Square to;
   PieceType pt;
   Move(Square from, Square to, PieceType pt = Queen) : from{ from }, to{ to }, pt{ pt } {};
   Move() : from{ NoSquare }, to{ NoSquare }, pt{ Queen } {};
   int getval(const Board& b) const;
   bool less_than(const Move& other, const Board& b) const;
};

struct PieceMoves {
   Square pos;
   Bitboard possible_moves;
   PieceMoves() : pos{ NoSquare }, possible_moves{} {};
   PieceMoves(Square sq, Bitboard bb) : pos{ sq }, possible_moves{ bb } {};
};

inline const string standard_chess = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

enum BoardState { None, Playing, Draw, Checkmate };
enum BoardErr { NoErr, InvalidMove, NoMoveToUnmake, GameOver };

struct Board {
public:
   BoardState state = None;
   // A list of bitboards, one for each color-piece combination
   array<Bitboard, 12> piecesBB;
   // A list of previously played moves
   vector<MoveData> move_history;
   // The list of moves each piece (with color to play) can make
   vector<PieceMoves> movelist;
   // Save en passant target square if it exists
   Square enpassant_square = NoSquare;
   // The color of the pieces to play
   PieceColor color_to_play = White;
   // Castling rights
   CastlingRights cr = 0;
   // A bitboard containing the squares attacked by the enemy
   Bitboard attacked_by_enemy = 0;
   // A bitboard containing the pieces putting the king in check
   Bitboard checkers = 0;
   // Used to restrict movement of pieces when in check
   Bitboard possible_moves = ~0;
   // 50 move rule counter
   size_t fifty_move_counter = 0;
   // Zobrist
   vector<Key> zobrist;
   // To optimize checking for draw by repetition
   int last_irreversible_move = 0;
public:
   /*!
     Default constructor.
     @param FEN: a FEN notation string.

     WARNING: the string MUST be a valid FEN notation, otherwise the engine aborts.
   */
   Board(const string &FEN);

   /*!
     Load a FEN string into a board and generate legal moves.
   */
   void load_fen(const string& FEN);

   /*!
     Return the state of the board.
   */
   BoardState get_state() const;

   /*!
     Returns a Bitboard containing all white pieces.
   */
   Bitboard white_pieces() const;

   /*!
     Returns a Bitboard containing all black pieces.
   */
   Bitboard black_pieces() const;

   /*!
     Returns a Bitboard containing all pieces.
   */
   Bitboard all_pieces() const;

   /*
      TODO
   */
   Bitboard get_pieces(const PieceColor c, const PieceType pt = NoType) const;

   /*!
      Get the type of a piece.
      If the square is empty, `NoType` is returned.
   */
   PieceType get_piece_type(const Square sq) const;

   /*!
      Return the index of the bitboard containing the piece if it exists.
   */
   PieceBBIndex get_pieceBB_index(const Square sq) const;
   PieceBBIndex get_pieceBB_index(const PieceType pt, const PieceColor c) const;

   /*!
      Get color of a piece given its positions.
      If the square is empty, `NoColor` is returned.
   */
   PieceColor get_piece_color(const Square sq) const;

   /*!
     Check if a square is occupied.
   */
   bool is_square_occupied(const Square sq) const;

   /*!
      TODO
   */
   bool is_capture(const Square to) const;
   bool is_castle(const Square from, const Square to) const;
   bool is_promotion(const Square from, const Square to) const;

   /*!
      TODO
   */
   vector<Square> get_possible_moves(const Square sq) const;

   /*
     Check if king to play is in check.
   */
   bool in_check() const;

   //! Remove a piece from the board.
   void remove_piece_at(const Square sq);

   /*!
      Generate all possible moves of a piece.
      It is just a wrapper for `gen_knight_moves()` and other similar functions.
   */
   Bitboard gen_piece_moves(const Square sq, const Bitboard occ, const Bitboard mypieces) const;

   /*
      TODO
   */
   void limit_moves_of_pinned_pieces();

   /*!
      Generate all legal moves at a given position, the result is stored
      in `Board::movelist`.
   */
   void gen_board_legal_moves();

   /*
      Check if a move is valid.
   */
   bool is_valid_move(const Square from, const Square to) const;

   /*
      TODO
   */
   vector<Move> get_moves() const;

   /*!
      TODO
   */
   MoveData gen_move_data(const Square from, const Square to, const PieceType promote_to) const;

   /*!
      Gets attackers of a square.
   */
   Bitboard attackers_of(const Square sq) const;

   /*
      Change position of a piece, no checks are made and pieces
      from enemy are not taken.
   */
   void change_piece_pos(Square from, Square to);

   /*!
     Handle castling rights changes.
   */
   void handle_castling_rights_changes(const Square from, const Square to);

   /*!
      Make a move.
      @return `InvalidMove` if move is invalid, `GameOver` if the game ends,
      otherwise `NoErr`.
   */
   BoardErr make_move(const Square from, const Square to, const PieceType promote_to = Queen);

   /*
      Unmake a move.
      @return `NoMoveToUnmake` if there is no move to unmake, `NoErr` otherwise.
   */
   BoardErr unmake_move();

   /*
      Calculate Zobrist key of the current position.
   */
   Key calc_zobrist_key();

   /*
      Check if draw by repetition.
   */
   bool is_draw_by_repetition() const;

};

} // namesapce Chess
