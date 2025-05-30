#ifndef MAGIC_LOOK_UP_WHITE_HPP
#define MAGIC_LOOK_UP_WHITE_HPP

#include "Bitboard.hpp"
#include <array>
#include <cstdint>

constexpr int SQUARE_NUM_WHITE = 81;
constexpr int INDEX_MAX_WHITE = 131072;

extern std::array<Bitboard, SQUARE_NUM_WHITE> movesAloneWhite;
extern std::array<Bitboard, SQUARE_NUM_WHITE> magicNumbersWhite;
extern std::array<int, SQUARE_NUM_WHITE> magicShiftsWhite;
extern std::array<std::array<Bitboard, INDEX_MAX_WHITE>, SQUARE_NUM_WHITE> magicLookUpWhite;

void initMagicWhite();

#endif