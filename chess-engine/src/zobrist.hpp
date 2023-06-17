#pragma once

#include "./utils.hpp"

#include <array>

using std::array;

namespace Chess
{
namespace Zobrist
{

extern const array<array<Key, 64>, 12> psq;
extern const array<Key, 8> enpassant;
extern const array<Key, 16> castling_rights;
extern const Key side_to_move;

} // namespace Zobrist
} // namespace Chess
