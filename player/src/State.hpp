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

    State move(int from, int to) const;
    State moveBlack(int from, int to) const;
    State moveWhite(int from, int to) const;

    int evaluate() const;
    bool isWhiteWinner() const;
    bool isBlackWinner() const;

private:
    int directions[4] = {1, 9, -1, -9};
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
