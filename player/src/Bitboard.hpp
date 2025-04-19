#ifndef BITBOARD_H
#define BITBOARD_H

#include <iostream>
#include <cstdint>
#include <bitset>

class Bitboard {
public:
    Bitboard(uint64_t upper = 0, uint64_t lower = 0) : upper(upper), lower(lower) {}

    bool get(int position) const;

    // By Value
    Bitboard setV(int position) const;
    Bitboard notV() const;
    Bitboard andV(const Bitboard& other) const;
    Bitboard orV(const Bitboard& other) const;
    Bitboard xorV(const Bitboard& other) const;
    Bitboard leftV(int amount) const;
    Bitboard rightV(int amount) const;

    // By Reference
    Bitboard& set(int position);
    Bitboard& notR();
    Bitboard& andR(const Bitboard& other);
    Bitboard& orR(const Bitboard& other);
    Bitboard& xorR(const Bitboard& other);
    Bitboard& leftR(int amount);
    Bitboard& rightR(int amount);

private:
    uint64_t upper;
    uint64_t lower;
};

#endif
