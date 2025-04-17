#ifndef BITBOARD_H
#define BITBOARD_H

#include <iostream>
#include <cstdint>
#include <bitset>

class Bitboard {
public:
    // Costruttore che inizializza le due metà (upper e lower) della bitboard
    Bitboard(uint64_t upper = 0, uint64_t lower = 0) : upper(upper), lower(lower) {}

    // Getter per ottenere il bit alla posizione specifica (costante)
    bool get(int position) const;

    // Operazioni che non modificano la bitboard, ma ne restituiscono una nuova
    Bitboard setC(int position) const;
    Bitboard notC() const;
    Bitboard andC(const Bitboard& other) const;
    Bitboard orC(const Bitboard& other) const;
    Bitboard xorC(const Bitboard& other) const;
    Bitboard leftC(int positions) const;
    Bitboard rightC(int positions) const;

    // Operazioni che modificano la bitboard in loco (e ritornano il riferimento a sé stessa)
    Bitboard& set(int position);
    Bitboard& notR();  // Negazione della bitboard
    Bitboard& andR(const Bitboard& other);  // AND tra due bitboard
    Bitboard& orR(const Bitboard& other);   // OR tra due bitboard
    Bitboard& xorR(const Bitboard& other);  // XOR tra due bitboard
    Bitboard& leftR(int positions);  // Shift a sinistra
    Bitboard& rightR(int positions); // Shift a destra

private:
    uint64_t upper;  // Parte superiore della bitboard
    uint64_t lower;  // Parte inferiore della bitboard
};

#endif
