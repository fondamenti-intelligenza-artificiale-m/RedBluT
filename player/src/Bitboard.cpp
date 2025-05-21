#include "Bitboard.hpp"

bool Bitboard::get(int position) const {
    if (position < 64) return (lower >> position) & 1;
    return (upper >> (position - 64)) & 1;
}

Bitboard Bitboard::clearV(int position) const {
    Bitboard newBoard = *this;
    if (position < 64) newBoard.lower &= (0ULL << position);
    else newBoard.upper &= (0ULL << (position - 64));
    return newBoard;
}

Bitboard Bitboard::setV(int position) const {
    Bitboard newBoard = *this;
    if (position < 64) newBoard.lower |= (1ULL << position);
    else newBoard.upper |= (1ULL << (position - 64));
    return newBoard;
}

Bitboard Bitboard::notV() const {
    Bitboard newBoard = *this;
    newBoard.upper = ~newBoard.upper;
    newBoard.lower = ~newBoard.lower;
    return newBoard;
}

Bitboard Bitboard::andV(const Bitboard& other) const {
    Bitboard newBoard = *this;
    newBoard.upper &= other.upper;
    newBoard.lower &= other.lower;
    return newBoard;
}

Bitboard Bitboard::orV(const Bitboard& other) const {
    Bitboard newBoard = *this;
    newBoard.upper |= other.upper;
    newBoard.lower |= other.lower;
    return newBoard;
}

Bitboard Bitboard::xorV(const Bitboard& other) const {
    Bitboard newBoard = *this;
    newBoard.upper ^= other.upper;
    newBoard.lower ^= other.lower;
    return newBoard;
}

Bitboard Bitboard::leftV(int amount) const {
    Bitboard newBoard = *this;
    if (amount < 64) {
        newBoard.upper = (newBoard.upper << amount) | (newBoard.lower >> (64 - amount));
        newBoard.lower = newBoard.lower << amount;
    } else {
        newBoard.upper = newBoard.lower << (amount - 64);
        newBoard.lower = 0;
    }
    return newBoard;
}

Bitboard Bitboard::rightV(int amount) const {
    Bitboard newBoard = *this;
    if (amount < 64) {
        newBoard.lower = (newBoard.lower >> amount) | (newBoard.upper << (64 - amount));
        newBoard.upper = newBoard.upper >> amount;
    } else {
        newBoard.lower = upper >> (amount - 64);
        newBoard.upper = 0;
    }
    return newBoard;
}

Bitboard& Bitboard::clearR(int position) {
    if (position < 64) lower &= (0ULL << position);
    else upper &= (0ULL << (position - 64));
    return *this;
}

Bitboard& Bitboard::setR(int position) {
    if (position < 64) lower |= (1ULL << position);
    else upper |= (1ULL << (position - 64));
    return *this;
}

Bitboard& Bitboard::notR() {
    upper = ~upper;
    lower = ~lower;
    return *this;
}

Bitboard& Bitboard::andR(const Bitboard& other) {
    upper &= other.upper;
    lower &= other.lower;
    return *this;
}

Bitboard& Bitboard::orR(const Bitboard& other) {
    upper |= other.upper;
    lower |= other.lower;
    return *this;
}

Bitboard& Bitboard::xorR(const Bitboard& other) {
    upper ^= other.upper;
    lower ^= other.lower;
    return *this;
}

Bitboard& Bitboard::leftR(int amount) {
    if (amount < 64) {
        upper = (upper << amount) | (lower >> (64 - amount));
        lower = lower << amount;
    } else {
        upper = lower << (amount - 64);
        lower = 0;
    }
    return *this;
}

Bitboard& Bitboard::rightR(int amount) {
    if (amount < 64) {
        lower = (lower >> amount) | (upper << (64 - amount));
        upper = upper >> amount;
    } else {
        lower = upper >> (amount - 64);
        upper = 0;
    }
    return *this;
}
