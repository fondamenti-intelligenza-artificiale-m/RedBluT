#include "Bitboard.hpp"

// Getter per ottenere il bit alla posizione specifica (costante)
bool Bitboard::get(int position) const {
    if (position < 64) {
        return (lower >> position) & 1;
    } else {
        return (upper >> (position - 64)) & 1;
    }
}

// Operazione immutabile: Imposta il bit alla posizione specificata e restituisce una nuova bitboard
Bitboard Bitboard::setC(int position) const {
    Bitboard newBoard = *this;
    if (position < 64) {
        newBoard.lower |= (1ULL << position);
    } else {
        newBoard.upper |= (1ULL << (position - 64));
    }
    return newBoard;
}

// Operazione immutabile: Negazione della bitboard e restituisce una nuova bitboard
Bitboard Bitboard::notC() const {
    Bitboard newBoard = *this;
    newBoard.upper = ~newBoard.upper;
    newBoard.lower = ~newBoard.lower;
    return newBoard;
}

// Operazione immutabile: AND tra due bitboard e restituisce una nuova bitboard
Bitboard Bitboard::andC(const Bitboard& other) const {
    Bitboard newBoard = *this;
    newBoard.upper &= other.upper;
    newBoard.lower &= other.lower;
    return newBoard;
}

// Operazione immutabile: OR tra due bitboard e restituisce una nuova bitboard
Bitboard Bitboard::orC(const Bitboard& other) const {
    Bitboard newBoard = *this;
    newBoard.upper |= other.upper;
    newBoard.lower |= other.lower;
    return newBoard;
}

// Operazione immutabile: XOR tra due bitboard e restituisce una nuova bitboard
Bitboard Bitboard::xorC(const Bitboard& other) const {
    Bitboard newBoard = *this;
    newBoard.upper ^= other.upper;
    newBoard.lower ^= other.lower;
    return newBoard;
}

// Shift a sinistra (immutabile): restituisce una nuova bitboard con i bit spostati
Bitboard Bitboard::leftC(int positions) const {
    Bitboard newBoard = *this;
    if (positions < 64) {
        // Shift della parte lower
        newBoard.lower = (newBoard.lower << positions) | (newBoard.upper >> (64 - positions));
        // Shift della parte upper
        newBoard.upper = newBoard.upper << positions;
    } else {
        // Se lo shift è maggiore di 64, tutto va nell'upper
        newBoard.upper = newBoard.lower << (positions - 64);
        newBoard.lower = 0;
    }
    return newBoard;
}

// Shift a sinistra (mutabile): modifica la bitboard in loco
Bitboard& Bitboard::leftR(int positions) {
    if (positions < 64) {
        // Shift della parte lower
        lower = (lower << positions) | (upper >> (64 - positions));
        // Shift della parte upper
        upper = upper << positions;
    } else {
        // Se lo shift è maggiore di 64, tutto va nell'upper
        upper = lower << (positions - 64);
        lower = 0;
    }
    return *this;
}

// Operazione mutabile: Imposta il bit alla posizione specificata e modifica la bitboard in loco
Bitboard& Bitboard::set(int position) {
    if (position < 64) {
        lower |= (1ULL << position);
    } else {
        upper |= (1ULL << (position - 64));
    }
    return *this;
}

// Operazione mutabile: Negazione della bitboard e modifica la bitboard in loco
Bitboard& Bitboard::notR() {
    upper = ~upper;
    lower = ~lower;
    return *this;
}

// Operazione mutabile: AND tra due bitboard e modifica la bitboard in loco
Bitboard& Bitboard::andR(const Bitboard& other) {
    upper &= other.upper;
    lower &= other.lower;
    return *this;
}

// Operazione mutabile: OR tra due bitboard e modifica la bitboard in loco
Bitboard& Bitboard::orR(const Bitboard& other) {
    upper |= other.upper;
    lower |= other.lower;
    return *this;
}

// Operazione mutabile: XOR tra due bitboard e modifica la bitboard in loco
Bitboard& Bitboard::xorR(const Bitboard& other) {
    upper ^= other.upper;
    lower ^= other.lower;
    return *this;
}

// Shift a destra (immutabile): restituisce una nuova bitboard con i bit spostati
Bitboard Bitboard::rightC(int positions) const {
    Bitboard newBoard = *this;
    if (positions < 64) {
        // Shift della parte upper
        newBoard.upper = (newBoard.upper >> positions) | (newBoard.lower << (64 - positions));
        // Shift della parte lower
        newBoard.lower = newBoard.lower >> positions;
    } else {
        // Se lo shift è maggiore di 64, tutto va nel lower
        newBoard.lower = upper >> (positions - 64);
        newBoard.upper = 0;
    }
    return newBoard;
}

// Shift a destra (mutabile): modifica la bitboard in loco
Bitboard& Bitboard::rightR(int positions) {
    if (positions < 64) {
        // Shift della parte upper
        upper = (upper >> positions) | (lower << (64 - positions));
        // Shift della parte lower
        lower = lower >> positions;
    } else {
        // Se lo shift è maggiore di 64, tutto va nel lower
        lower = upper >> (positions - 64);
        upper = 0;
    }
    return *this;
}
