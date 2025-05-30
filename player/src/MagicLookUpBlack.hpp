#ifndef MAGIC_LOOK_UP_BLACK_HPP
#define MAGIC_LOOK_UP_BLACK_HPP

#include "Bitboard.hpp"
#include <array>
#include <cstdint>

constexpr int SQUARE_NUM_BLACK = 81;
constexpr int INDEX_MAX_BLACK = 131072;

extern std::array<Bitboard, SQUARE_NUM_BLACK> movesAloneBlack;
extern std::array<Bitboard, SQUARE_NUM_BLACK> magicNumbersBlack;
extern std::array<int, SQUARE_NUM_BLACK> magicShiftsBlack;
extern std::array<std::array<Bitboard, INDEX_MAX_BLACK>, SQUARE_NUM_BLACK> magicLookUpBlack;

void initMagicBlack();

#endif