#ifndef STATE_H
#define STATE_H

#include "ZobristParams.hpp"
#include "Bitboard.hpp"
//#include "MagicLookUpBlack.hpp"
//#include "MagicLookUpWhite.hpp"
#include "PositionHistory.hpp"
#include <vector>
#include <stdio.h>

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

    std::vector<int> getLegalMoves(int from) const;
    //Bitboard getLegalMoves(int from) const;
    //Bitboard getLegalMovesBlack(int from) const;
    //Bitboard getLegalMovesWhite(int from) const;

    State move(int from, int to) const;
    State moveBlack(int from, int to) const;
    State moveWhite(int from, int to) const;

    int evaluate() const;
    bool isWhiteWinner() const;
    bool isBlackWinner() const;

    int getKingPosition() const;

private:
    static int directions[4];
    static const Bitboard diagonals;
    static const Bitboard goal;
    static const Bitboard row3;
    static const Bitboard row7;
    static const Bitboard columnC;
    static const Bitboard columnG;

    int checkDiagonal() const;
    int whiteEdges() const;
    int blackNearKing() const;
    int countPieces() const;
    int checkFreeWay() const;
    int kingMobility() const;

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

    void computeZobristHash();
};

#endif
