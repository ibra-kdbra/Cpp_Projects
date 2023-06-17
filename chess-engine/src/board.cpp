#include "./board.hpp"

#include <iostream>
#include <algorithm>
#include <string>

#include "./utils.hpp"
#include "./bitboard.hpp"
#include "./move_gen.hpp"
#include "./zobrist.hpp"

using std::string;
using std::find_if;

namespace Chess
{

int Move::getval(const Board& b) const {
   // The values returned are just arbitrary numbers,
   // the important is for promotion to be greater than normal move etc.
   return b.is_promotion(from, to) ? 50
      : b.is_capture(to) ? 25
      : b.is_castle(from, to) ? 12
      : 1;
}
bool Move::less_than(const Move& other, const Board& b) const {
   return getval(b) < other.getval(b);
}

Board::Board (const string &FEN)
   : piecesBB{}, move_history{}, movelist{}, zobrist{}
{
   move_history.reserve(64);
   movelist.reserve(64);
   zobrist.reserve(64);
   load_fen(FEN);
}

BoardState Board::get_state() const {
   return state;
}

Bitboard Board::white_pieces() const {
   return piecesBB[WK] | piecesBB[WQ] | piecesBB[WR] | piecesBB[WB] | piecesBB[WN] | piecesBB[WP];
}

Bitboard Board::black_pieces() const {
   return piecesBB[BK] | piecesBB[BQ] | piecesBB[BR] | piecesBB[BB] | piecesBB[BN] | piecesBB[BP];
}

Bitboard Board::all_pieces() const {
   return white_pieces() | black_pieces();
}

Bitboard Board::get_pieces(const PieceColor c, const PieceType pt) const {
   const int i = c == White ? 0 : 6;
   switch (pt) {
      case King:   return piecesBB[WK + i];
      case Queen:  return piecesBB[WQ + i];
      case Rook:   return piecesBB[WR + i];
      case Bishop: return piecesBB[WB + i];
      case Knight: return piecesBB[WN + i];
      case Pawn:   return piecesBB[WP + i];
      case NoType: return c == White ? white_pieces() : black_pieces();
      default:     return 0;
   }
}

PieceType Board::get_piece_type(const Square sq) const {
      const Bitboard bb = sqbb(sq);
      return (bb & piecesBB[WP] || bb & piecesBB[BP]) ? Pawn
         : (bb & piecesBB[WB] || bb & piecesBB[BB]) ? Bishop
         : (bb & piecesBB[WN] || bb & piecesBB[BN]) ? Knight
         : (bb & piecesBB[WR] || bb & piecesBB[BR]) ? Rook
         : (bb & piecesBB[WQ] || bb & piecesBB[BQ]) ? Queen
         : (bb & piecesBB[WK] || bb & piecesBB[BK]) ? King
         : NoType;
}

PieceBBIndex Board::get_pieceBB_index(const Square sq) const {
   for (size_t i = 0; i < piecesBB.size(); ++i)
      if (sqbb(sq) & piecesBB.at(i)) return PieceBBIndex(i);
   return invalid_index;
}

PieceBBIndex Board::get_pieceBB_index(const PieceType pt, const PieceColor c) const {
   return PieceBBIndex(pt + (c == White ? 0 : 6));
}

PieceColor Board::get_piece_color(const Square sq) const {
   return sqbb(sq) & white_pieces() ? White : sqbb(sq) & black_pieces() ? Black : NoColor;
}

bool Board::is_square_occupied(const Square sq) const {
   return (sqbb(sq) & all_pieces()) != 0;
}

bool Board::in_check() const {
   return get_pieces(color_to_play, King) & attacked_by_enemy;
}

bool Board::is_capture(const Square to) const {
   return 0 != (to & all_pieces());
}

bool Board::is_castle(const Square from, const Square to) const {
   return get_piece_type(from) == King && abs((int) (to - from)) == 2;
}

bool Board::is_promotion(const Square from, const Square to) const {
   return (((sqbb(from) & piecesBB[WP]) && get_rank(to) == rank8)
    || ((sqbb(from) & piecesBB[BP]) && get_rank(to) == rank1)
   );
}

vector<Square> Board::get_possible_moves(const Square sq) const {
   auto piece = find_if(
      movelist.begin(),
      movelist.end(),
      [&](const PieceMoves &pm){ return pm.pos == sq; }
   );
   if (piece == movelist.end()) return {};

   std::vector<Square> res{};
   res.reserve(16);
   Bitboard possible_moves = piece->possible_moves;
   while (possible_moves) res.emplace_back(pop_lsb(possible_moves));
   return res;
}

void Board::remove_piece_at(const Square sq) {
   piecesBB[get_pieceBB_index(sq)] &= ~sqbb(sq);
}

MoveData Board::gen_move_data(const Square from, const Square to, const PieceType promote_to) const {
   MoveType mt = Normal;
   PieceType pt = get_piece_type(from);
   PieceType taken_pt = get_piece_type(to);
   if (is_promotion(from, to)) mt = Promotion;
   else if (pt == Pawn && to == enpassant_square) {
      mt = En_passant;
   }
   else if (is_castle(from, to)) {
      mt = Castling;
   }
   return new_md(from, to, promote_to, mt, taken_pt, cr, enpassant_square, fifty_move_counter);
}

Bitboard Board::attackers_of(const Square sq) const {
   Bitboard res = 0;
   const PieceColor myc = get_piece_color(sq);

   res |= gen_sliding_piece_moves(sq, Rook, all_pieces(), get_pieces(myc, NoType)) & (get_pieces(~myc, Rook) | get_pieces(~myc, Queen));
   res |= gen_sliding_piece_moves(sq, Bishop, all_pieces(), get_pieces(myc, NoType)) & (get_pieces(~myc, Bishop) | get_pieces(~myc, Queen));
   res |= gen_knight_moves(sq, all_pieces(), get_pieces(myc, NoType)) & get_pieces(~myc, Knight);
   res |= gen_pawn_attacks(sq, myc, all_pieces(), get_pieces(myc, NoType)) & get_pieces(~myc, Pawn);
   res |= gen_king_moves(sq, all_pieces(), get_pieces(myc, NoType), myc, NoCastling) & get_pieces(~myc, King);

   return res;
}

Bitboard Board::gen_piece_moves(const Square sq, const Bitboard occ, const Bitboard mypieces) const {
   Bitboard res = 0;
   const PieceType pt = get_piece_type(sq);
   const PieceColor myc = get_piece_color(sq);
   if (pt == Queen || pt == Rook || pt == Bishop) {
      res = gen_sliding_piece_moves(sq, pt, occ, mypieces);
   }
   else if (pt == Knight) {
      res = gen_knight_moves(sq, occ, mypieces);
   }
   else if (pt == King) {
      res = gen_king_moves(sq, occ, mypieces, myc, cr);
   }
   else if (pt == Pawn) {
      res = gen_pawn_attacks(sq, myc, occ, mypieces);
      if ((myc == White && get_rank(sq) == rank2)
         || (myc == Black && get_rank(sq) == rank7)
      ) {
         res |= gen_double_push(sq, myc, occ);
      }
      else {
         res |= gen_pawn_push(sq, myc, occ);
      }
   }
   return res;
}

void Board::limit_moves_of_pinned_pieces() {
   const Square king_sq = lsb(get_pieces(color_to_play, King));
   const array<Bitboard, 2> possible_pinners = {
      (rank_bb(king_sq) | file_bb(king_sq)) & (get_pieces(~color_to_play, Queen) | get_pieces(~color_to_play, Rook)),
      (diagonal_bb(king_sq) | anti_diagonal_bb(king_sq)) & (get_pieces(~color_to_play, Queen) | get_pieces(~color_to_play, Bishop))
   };
   for (auto i : possible_pinners) { // copy not reference
      while (i) {
         const Square pinner = pop_lsb(i);
         const Bitboard between = between_bb(king_sq, pinner);
         if (!more_than_one(between & all_pieces() & ~sqbb(pinner))) {
            auto pm = find_if(
               movelist.begin(),
               movelist.end(),
               [&](PieceMoves &pm){ return 0 != (sqbb(pm.pos) & between); }
            );
            if (pm != movelist.end()) pm->possible_moves &= between;
         }
      }
   }
}

void Board::gen_board_legal_moves() {
   movelist.clear();
   if (state == Playing && fifty_move_counter == 100) state = Draw;
   if (state == Draw || state == Checkmate) return;
   attacked_by_enemy = 0;
   checkers = 0;
   possible_moves = ~0ull;

   const Square king_sq = lsb(get_pieces(color_to_play, King));

   // Generated squares attacked by the enemy
   Bitboard pieces = get_pieces(~color_to_play);
   while (pieces) {
      const Square sq = pop_lsb(pieces);
      if (get_piece_type(sq) == Pawn) {
         attacked_by_enemy |= gen_piece_moves(sq, ~0, 0);
      }
      else {
         attacked_by_enemy |= gen_piece_moves(sq, all_pieces() & ~sqbb(king_sq), 0);
      }
   }
   // Checks
   const Bitboard attackers = attackers_of(king_sq);
   if (more_than_one(attackers)) possible_moves = 0;
   else if (attackers) possible_moves = between_bb(king_sq, lsb(attackers));

   // Handling en passant discovered check
   // TODO: improve this?
   const Bitboard ep_pawn = enpassant_square == NoSquare ? 0 : sqbb(enpassant_square - 8*pawn_direction(color_to_play));
   if (rank_bb(king_sq) & ep_pawn) {
      const Bitboard possible_attacker = rank_bb(king_sq) & (get_pieces(~color_to_play, Queen) | get_pieces(~color_to_play, Rook));
      if (possible_attacker) {
         const Square attacker_sq = lsb(possible_attacker);
         const Bitboard between = between_bb(king_sq, attacker_sq) & ~sqbb(attacker_sq);
         if (
            popcnt(between & all_pieces()) == 2
            && (ep_pawn & get_pieces(~color_to_play, Pawn))
            && ((ep_pawn >> 1 | ep_pawn << 1) & get_pieces(color_to_play, Pawn))
         ) {
            enpassant_square = NoSquare;
         }
      }
   }

   Bitboard mypieces = get_pieces(color_to_play);
   // Generate legal moves
   while (mypieces) {
      const Square sq = pop_lsb(mypieces);

      // En passant target is considered an occupied square in pawn move generation
      // to make it simpler
      const Bitboard ep = get_piece_type(sq) == Pawn ? sqbb(enpassant_square) : 0;
      PieceMoves pm{ sq, gen_piece_moves(sq, all_pieces() | ep, get_pieces(color_to_play)) };

      if (get_piece_type(sq) == Pawn && (((ep >> 8) & attackers) || ((ep << 8) & attackers))) {
         pm.possible_moves &= possible_moves | ep;
      }
      else if (sq != king_sq) {
         pm.possible_moves &= possible_moves;
      }
      else {
         pm.possible_moves &= ~attacked_by_enemy;
         const Bitboard sc = get_piece_color(sq) == White ? WKSCastlingSquaresBB : BKSCastlingSquaresBB;
         const Bitboard lc = sc == WKSCastlingSquaresBB ? WQSCastlingSquaresBB : BQSCastlingSquaresBB;
         // Long castle is illegal if the B file is occupied, but legal if it's attacked
         const Bitboard not_to_occupy_in_lc = sqbb(color_to_play == White ? B1 : B8);
         if ((cr & (color_to_play == White ? White_OO : Black_OO))
             && (all_pieces() & sc || attacked_by_enemy & sc)) {
            pm.possible_moves &= ~sqbb(sq+2);
         }
         if ((cr & (color_to_play == White ? White_OOO : Black_OOO))
             && (all_pieces() & (lc | not_to_occupy_in_lc) || attacked_by_enemy & lc)) {
            pm.possible_moves &= ~sqbb(sq-2);
         }
         if (in_check()) {
            pm.possible_moves &= ~sqbb(sq-2) & ~sqbb(sq+2);
         }
      }
      movelist.emplace_back(pm);
   }
   limit_moves_of_pinned_pieces();

   // Checkmate & Stalemate
   state = in_check() ? Checkmate : Draw;
   for (auto& pm: movelist) {
      if (pm.possible_moves) {
         state = Playing;
         break;
      }
   }
   if (state == Playing && (fifty_move_counter == 100 || is_draw_by_repetition())) state = Draw;
}

bool Board::is_valid_move(const Square from, const Square to) const {
   auto x = find_if(
      movelist.begin(),
      movelist.end(),
      [&](const PieceMoves &pm){ return pm.pos == from && (sqbb(to) & pm.possible_moves); }
   );
   return x != movelist.end();
}

vector<Move> Board::get_moves() const {
   vector<Move> moves{};
   moves.reserve(64);

   for (auto pm: movelist) { // copy to use pop_lsb
      while (pm.possible_moves) {
         Move m{ pm.pos, pop_lsb(pm.possible_moves), Queen };
         moves.emplace_back(m);
         // promotion to other pieces
         if (is_promotion(m.from, m.to)) {
            for (auto pt: { Rook, Bishop, Knight }) {
               Move move = m;
               move.pt = pt;
               moves.emplace_back(move);
            }
         }
      }
   }
   return moves;
}

void Board::change_piece_pos(Square from, Square to) {
   assert(is_ok(from) && is_ok(to));
   const int i = get_pieceBB_index(from);
   piecesBB.at(i) &= ~sqbb(from);
   piecesBB.at(i) |= sqbb(to);
}

void Board::handle_castling_rights_changes(const Square from, const Square to) {
   if (get_piece_type(from) == King) {
      cr &= ~(color_to_play == White ? WhiteCastling : BlackCastling);
   }
   else if (get_piece_type(from) == Rook) { // rook moved
      if (from == (color_to_play == White ? H1 : H8)) { // king side rook
         cr &= ~(color_to_play == White ? White_OO : Black_OO);
      }
      else if (from == (color_to_play == White ? A1 : A8)) { // queen side rook
         cr &= ~(color_to_play == White ? White_OOO : Black_OOO);
      }
   }
   if (get_piece_type(to) == Rook) { // rook was taken
      if (to == (~color_to_play == White ? H1 : H8)) { // king side rook
         cr &= ~(~color_to_play == White ? White_OO : Black_OO);
      }
      else if (to == (~color_to_play == White ? A1 : A8)) { // queen side rook
         cr &= ~(~color_to_play == White ? White_OOO : Black_OOO);
      }
   }
}

BoardErr Board::make_move(const Square from, const Square to, const PieceType promote_to) {
   if (!is_valid_move(from, to)) return InvalidMove;
   if (state == Draw || state == Checkmate) return GameOver;

   // Zobrist
   MoveData md      = gen_move_data(from, to, promote_to);
   enpassant_square = NoSquare;
   handle_castling_rights_changes(from, to);

   // Fifty move rule
   if (get_piece_type(from) == Pawn || get_piece_color(to) == ~color_to_play) {
      fifty_move_counter = 0;
      // size() because it hasn't been added yet
      last_irreversible_move = zobrist.size();
   }
   else fifty_move_counter++;

   // check if we should take a piece
   if (get_piece_color(to) == ~color_to_play) {
      remove_piece_at(to);
   }
   if (md_get_move_type(md) == En_passant) { // Take if en passant
      remove_piece_at(to - 8*pawn_direction(color_to_play));
   }
   // Check if we should set en passant square for the next move
   else if (get_piece_type(from) == Pawn && abs((int) (from - to)) == 8*2) {
      enpassant_square = from + 8*pawn_direction(color_to_play);
   }
   // Handle promotion
   else if (md_get_move_type(md) == Promotion) {
      assert(promote_to == Queen || promote_to == Rook || promote_to == Bishop || promote_to == Knight);
      remove_piece_at(from);
      piecesBB.at(get_pieceBB_index(promote_to, color_to_play)) |= sqbb(to);
   }
   // Handle caslting
   else if (md_get_move_type(md) == Castling) {
      last_irreversible_move = zobrist.size();
      // king side
      if (from < to) change_piece_pos(to + 1, to - 1);
      // queen side
      else change_piece_pos(to - 2, to + 1);
   }

   if (md_get_move_type(md) != Promotion) change_piece_pos(from, to);
   move_history.emplace_back(md);
   zobrist.emplace_back(calc_zobrist_key());

   color_to_play = ~color_to_play;
   gen_board_legal_moves();

   return NoErr;
}

BoardErr Board::unmake_move() {
   if (move_history.empty()) return NoMoveToUnmake;

   color_to_play = ~color_to_play;
   const int i = color_to_play == White ? 0 : 6;
   const MoveData md = move_history.back();
   move_history.pop_back();

   const Square from = md_get_square_from(md);
   const Square to = md_get_square_to(md);
   const PieceType taken_pt = md_get_taken_piece_type(md);

   if (md_get_move_type(md) == Promotion) {
      piecesBB[Pawn + i] |= sqbb(from);
      piecesBB[get_pieceBB_index(md_get_promotion_type(md), color_to_play)] &= ~sqbb(to);
   }
   else if (md_get_move_type(md) == En_passant) {
      change_piece_pos(to, from);
      piecesBB[get_pieceBB_index(Pawn, ~color_to_play)] |= sqbb(to - 8*pawn_direction(color_to_play));
      enpassant_square = to;
   }
   else if (md_get_move_type(md) == Castling) {
      if (to > from) { // king side
         change_piece_pos(to, from);
         change_piece_pos(to - 1, to + 1); // rook
      }
      else {
         change_piece_pos(to, from);
         change_piece_pos(to + 1, to - 2); // rook
      }
   }
   else {
      change_piece_pos(to, from);
   }
   if (taken_pt != NoType) {
      piecesBB[get_pieceBB_index(taken_pt, ~color_to_play)] |= sqbb(to);
   }
   cr = md_get_castling_rights(md);
   enpassant_square = md_get_ep_square(md);
   fifty_move_counter = md_get_fmrc(md);
   state = Playing;
   zobrist.pop_back();

   gen_board_legal_moves();

   return NoErr;
}

Key Board::calc_zobrist_key() {
   // TODO: this can be obtimized, there is no need to recalculate everything
   // after each move
   Key k{ color_to_play == Black ? Zobrist::side_to_move : 0 };
   // Enpassant
   if (enpassant_square != NoSquare) k ^= Zobrist::enpassant.at(get_file(enpassant_square));
   // Castling rights
   if (cr & White_OO)  k ^= Zobrist::castling_rights[0];
   if (cr & White_OOO) k ^= Zobrist::castling_rights[1];
   if (cr & Black_OO)  k ^= Zobrist::castling_rights[2];
   if (cr & Black_OOO) k ^= Zobrist::castling_rights[3];
   // Pieces
   Bitboard pieces = all_pieces();
   while (pieces) {
      const Square sq = pop_lsb(pieces);
      k ^= Zobrist::psq[get_pieceBB_index(sq)][sq];
   }
   return k;
}

bool Board::is_draw_by_repetition() const {
   const int lastkey = zobrist.size() - 1;
   int cnt = 0;
   for (int i = last_irreversible_move + 1; i < zobrist.size(); i++) {
      if (zobrist.at(i) == zobrist.at(lastkey) && ++cnt == 3) return true;
   }
   return false;
}
} // namespace Chess
