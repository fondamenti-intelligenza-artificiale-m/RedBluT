#ifndef STATE_H
#define STATE_H

#include "ZobristParams.hpp"
#include "MagicLookUpBlack.hpp"
#include "MagicLookUpWhite.hpp"
#include "Bitboard.hpp"
#include "PositionHistory.hpp"
#include <vector>

class State {
public:
    State();
    State(const Bitboard& black, const Bitboard& white, const Bitboard& king,
        bool whiteTurn, bool whiteWinner = false, bool blackWinner = false, uint64_t zobristHash = 0); //temporaneo ancora da hardcodare
    
    uint64_t getZobristHash() const;

    bool isWhiteTurn() const;
    Bitboard getBlack() const;
    Bitboard getWhite() const;
    Bitboard getKing() const;
    Bitboard getWhiteAndKing() const;
    Bitboard getPieces() const;

    Bitboard getLegalMovesBlack(int from) const;
    Bitboard getLegalMovesWhite(int from) const;

    State moveBlack(int from, int to) const;
    State moveWhite(int from, int to) const;

    int evaluate() const;
    bool isWhiteWinner() const;
    bool isBlackWinner() const;

    int getKingPosition() const;

private:

    int checkDiagonal() const;
    int whiteEdges() const;
    int blackNearKing() const;
    int countPieces() const;
    int checkFreeWay() const;
    int kingMobility() const;

    static int directions[4];
    Bitboard camps;
    Bitboard lookout;
    Bitboard castle;

    uint64_t zobristHash;
    Bitboard black;
    Bitboard white;
    Bitboard king;
    bool whiteTurn;
    bool whiteWinner;
    bool blackWinner;

    static const Bitboard diagonals;
    static const Bitboard goal;

    static const Bitboard row3;
    static const Bitboard row7;
    static const Bitboard columnC;
    static const Bitboard columnG;

    void computeZobristHash();
};

int State::directions[4] = {-9, -1, 1, 9}; //direzioni in cui si può muovere il re

//diagonali vantaggiose per il nero
const Bitboard State::diagonals(0b100010,0b1000001000000000000000000000000000001000001000100010000000000);

const Bitboard State::goal(0b01100011010000000,0b1100000001000000000000000000000000000100000001100000001011000110);

const Bitboard State::row3(0,0b0000000000000000000000000000000000000111111111000000000000000000);
const Bitboard State::row7(0,0b0111111111000000000000000000000000000000000000000000000000000000);
const Bitboard State::columnC(0b10000000010,
                              0b0000000100000000100000000100000000100000000100000000100000000100);
const Bitboard State::columnG(0b100000000100000,
                              0b0001000000001000000001000000001000000001000000001000000001000000);

#endif
