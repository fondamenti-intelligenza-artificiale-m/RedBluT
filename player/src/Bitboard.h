#ifndef BITBOARD_H
#define BITBOARD_H

#include <iostream>
#include <cstdint>
#include <bitset>

class Bitboard {
public:
    Bitboard(uint64_t upper = 0, uint64_t lower = 0) : upper(upper), lower(lower) {}

    Bitboard& setBit(int position);
    Bitboard& clearBit(int position);
    bool getBit(int position) const;
    
    Bitboard& notOp();
    Bitboard& andOp(const Bitboard& other);
    Bitboard& orOp(const Bitboard& other);
    Bitboard& xorOp(const Bitboard& other);

    Bitboard& shiftLeft(int positions);
    Bitboard& shiftRight(int positions);

    void print() const;

private:
    uint64_t upper;
    uint64_t lower;
};

#endif
