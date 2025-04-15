#include "Bitboard.h"

Bitboard& Bitboard::setBit(int position) {
    if (position < 64) {
        upper |= (1ULL << position);  // Set bit in the upper part
    } else {
        lower |= (1ULL << (position - 64));  // Set bit in the lower part
    }
    return *this;  // Return the current object for fluent interface
}

Bitboard& Bitboard::clearBit(int position) {
    if (position < 64) {
        upper &= ~(1ULL << position);  // Clear bit in the upper part
    } else {
        lower &= ~(1ULL << (position - 64));  // Clear bit in the lower part
    }
    return *this;  // Return the current object for fluent interface
}

bool Bitboard::getBit(int position) const {
    if (position < 64) {
        return (upper & (1ULL << position)) != 0;
    } else {
        return (lower & (1ULL << (position - 64))) != 0;
    }
}

Bitboard& Bitboard::notOp() {
    upper = ~upper;
    lower = ~lower;
    return *this;  // Return the current object for fluent interface
}

Bitboard& Bitboard::andOp(const Bitboard& other) {
    upper &= other.upper;
    lower &= other.lower;
    return *this;  // Return the current object for fluent interface
}

Bitboard& Bitboard::orOp(const Bitboard& other) {
    upper |= other.upper;
    lower |= other.lower;
    return *this;  // Return the current object for fluent interface
}

Bitboard& Bitboard::xorOp(const Bitboard& other) {
    upper ^= other.upper;
    lower ^= other.lower;
    return *this;  // Return the current object for fluent interface
}

Bitboard& Bitboard::shiftLeft(int positions) {
    if (positions < 64) {
        upper <<= positions;
        upper |= (lower >> (64 - positions));
        lower <<= positions;
    } else {
        lower = upper << (positions - 64);
        upper = 0;
    }
    return *this;  // Return the current object for fluent interface
}

Bitboard& Bitboard::shiftRight(int positions) {
    if (positions < 64) {
        lower >>= positions;
        lower |= (upper << (64 - positions));
        upper >>= positions;
    } else {
        upper = lower >> (positions - 64);
        lower = 0;
    }
    return *this;  // Return the current object for fluent interface
}

void Bitboard::print() const {
    std::cout << "Upper: " << std::bitset<64>(upper) << ", Lower: " << std::bitset<64>(lower) << std::endl;
}
