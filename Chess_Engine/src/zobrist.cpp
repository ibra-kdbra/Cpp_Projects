#include "zobrist.hpp"

#include <random>

namespace Chess
{
static Key randn() {
   static std::random_device rd;
   static std::mt19937_64 generator(rd());
   static std::uniform_int_distribution<Key> random_number(1, UINT64_MAX);
   return random_number(generator);
}
static auto fill_psq() {
   array<array<Key, 64>, 12> res;
   for(auto& line: res) for (auto& n: line) n = randn();
   return res;
}

static auto fill_ep() {
   array<Key, 8> res;
   for (auto& n: res) n = randn();
   return res;
}

static auto fill_cr() {
   array<Key, 16> res;
   for(auto& n: res) n = randn();
   return res;
}

namespace Zobrist
{

const array<array<Key, 64>, 12> psq = fill_psq();
const array<Key, 8> enpassant = fill_ep();
const array<Key, 16> castling_rights = fill_cr();
const Key side_to_move = randn();

} // namespace Zobrist

} // namespace Chess
