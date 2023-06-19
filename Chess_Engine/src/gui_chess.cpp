#include <iostream>
#include <array>
#include <vector>
#include <thread>
#include <atomic>

#include <raylib.h>

#include "./board.hpp"
#include "./debug.hpp"
#include "./bitboard.hpp"
#include "./search.hpp"

namespace consts {
   // size of image = size of image to avoid complicating stuff
   constexpr int pieceSize = 60;
   constexpr int winW = pieceSize*8;
   constexpr int winH = pieceSize*8;
}

using TexturesArr = std::array<Texture, 12>;

Vector2 sq_to_v(const Chess::Square sq);
Chess::Square v_to_sq(const Vector2 &v);
TexturesArr get_textures();
void draw_board(const Chess::Board &b, TexturesArr &txtrs);
void draw_text(const std::string &text);
Chess::PieceType get_promotion_type();

int main() {
   InitWindow(consts::winW, consts::winH, "Chess");
   SetTargetFPS(60);

   Texture2D board_txtr = LoadTexture("../assets/img/board.png");
   TexturesArr txtrs = get_textures();

   Chess::Board b{ Chess::standard_chess };
   // Saves the piece to move
   Chess::Square from = Chess::NoSquare;
   // Saves the piece to promote to
   Chess::PieceType pt = Chess::Queen;

   while (!WindowShouldClose()) {
      // Mouse pos
      const Vector2 mp = GetMousePosition();

      if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
         if (from == Chess::NoSquare) {
            from = v_to_sq(mp);
         }
         else {
            const Chess::Square to = v_to_sq(mp);
            // Get piece to promote to
            if (b.is_valid_move(from, to) && b.is_promotion(from, to))
               pt = get_promotion_type();
            if (b.make_move(from, to, pt) == Chess::NoErr) {
               // Play move for the engine
               Chess::MoveEval best = search(b, 2);
               if (b.make_move(best.move.from, best.move.to) == Chess::InvalidMove) {
                  std::cout << "Invalid Move\n";
                  std::cout << sqstr(best.move.from) << " -> " << sqstr(best.move.to) << '\n';
                  std::cout << best.eval << '\n';
               }
            }
            // Reset variables
            from = Chess::NoSquare;
            pt = Chess::Queen;
         }
      }
      BeginDrawing();
      ClearBackground(RAYWHITE);

      // Draw board
      DrawTexture(board_txtr, 0, 0, RAYWHITE);
      // Highlight selected piece
      if (from != Chess::NoSquare) {
         const Vector2 v = sq_to_v(from);
         DrawRectangle(v.x, v.y, consts::pieceSize, consts::pieceSize,
                        Color{255, 0, 0, 100});
         std::vector<Chess::Square> possible_moves{ b.get_possible_moves(from) };
         for (auto sq: possible_moves) {
            const Vector2 tmp = sq_to_v(sq);
            DrawRectangle(tmp.x, tmp.y, consts::pieceSize, consts::pieceSize,
                        Color{255, 0, 0, 100});
         }
      }
      draw_board(b, txtrs);

      // Draw text when game ends
      if (b.get_state() == Chess::Checkmate) draw_text("Checkmate!");
      if (b.get_state() == Chess::Draw) draw_text("Draw!");

      EndDrawing();
   }

   UnloadTexture(board_txtr);
   for (auto& txtr: txtrs) UnloadTexture(txtr);
   CloseWindow();
}

Vector2 sq_to_v(const Chess::Square sq) {
   return (Vector2) {
      float(sq%8) * consts::pieceSize,
      float(7 - int(sq/8)) * consts::pieceSize,
   };
}

Chess::Square v_to_sq(const Vector2 &v) {
   Chess::Square sq = Chess::Square(v.x / consts::pieceSize);
   sq +=  8*(7 - int(v.y / consts::pieceSize));
   return sq;
}

TexturesArr get_textures() {
   using namespace Chess;
   TexturesArr txtrs{};
   txtrs[WP] = LoadTexture("../assets/img/pieces/wp.png");
   txtrs[WN] = LoadTexture("../assets/img/pieces/wn.png");
   txtrs[WB] = LoadTexture("../assets/img/pieces/wb.png");
   txtrs[WR] = LoadTexture("../assets/img/pieces/wr.png");
   txtrs[WQ] = LoadTexture("../assets/img/pieces/wq.png");
   txtrs[WK] = LoadTexture("../assets/img/pieces/wk.png");

   txtrs[BP] = LoadTexture("../assets/img/pieces/bp.png");
   txtrs[BN] = LoadTexture("../assets/img/pieces/bn.png");
   txtrs[BB] = LoadTexture("../assets/img/pieces/bb.png");
   txtrs[BR] = LoadTexture("../assets/img/pieces/br.png");
   txtrs[BQ] = LoadTexture("../assets/img/pieces/bq.png");
   txtrs[BK] = LoadTexture("../assets/img/pieces/bk.png");

   return txtrs;
}

void draw_board(const Chess::Board &b, TexturesArr &txtrs) {
   Chess::Bitboard piecesBB = b.all_pieces();
   while (piecesBB) {
      const Chess::Square sq = Chess::pop_lsb(piecesBB);
      const Vector2 v = sq_to_v(sq);
      const auto i = b.get_pieceBB_index(sq);
      DrawTexture(txtrs[i], v.x, v.y, RAYWHITE);
   }
}

void draw_text(const std::string &text) {
   constexpr int boxW = 300;
   constexpr int boxH = 200;
   constexpr int boxX = consts::winW/2 - boxW/2;
   constexpr int boxY = consts::winH/2 - boxH/2;

   DrawRectangle(boxX, boxY, boxW, boxH, Color{0, 0, 0, 150});
   DrawText(text.c_str(),  boxX + 60, boxY + 50, 28, RAYWHITE);
}

Chess::PieceType get_promotion_type() {
   using namespace Chess;
   PieceType pt = Queen;
   constexpr int boxW = 250;
   constexpr int boxH = 200;
   constexpr int boxX = consts::winW/2 - boxW/2;
   constexpr int boxY = consts::winH/2 - boxH/2;

   BeginDrawing();
      DrawRectangle(boxX, boxY, boxW, boxH, Color{0, 0, 0, 150});
      DrawText("<Q> Queen",  boxX + 60, boxY + 20,  24, RAYWHITE);
      DrawText("<R> Rook",   boxX + 60, boxY + 50,  24, RAYWHITE);
      DrawText("<B> Bishop", boxX + 60, boxY + 80,  24, RAYWHITE);
      DrawText("<N> Knight", boxX + 60, boxY + 110, 24, RAYWHITE);
   EndDrawing();

   while (!WindowShouldClose()) {
      if (IsKeyPressed(KEY_Q)) break;
      else if (IsKeyPressed(KEY_B)) { pt = Bishop; break; }
      else if (IsKeyPressed(KEY_R)) { pt = Rook;   break; }
      else if (IsKeyPressed(KEY_N)) { pt = Knight; break; }
      BeginDrawing();
      EndDrawing();
   }
   return pt;
}