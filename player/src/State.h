#ifndef STATE_H
#define STATE_H

#include <vector>
#include "Bitboard.h"
#include "PositionHistory.h"

class State {
public:
    State();
    State(const Bitboard& black, const Bitboard& white, const Bitboard& king,
        bool whiteTurn, bool whiteWinner = false, bool blackWinner = false);
    State(const Bitboard& black, const Bitboard& white, const Bitboard& king,
        bool whiteTurn, bool whiteWinner = false, bool blackWinner = false, uint64_t zobristHash);
    uint64_t getZobristHash() const;

    Bitboard getBlack() const;
    Bitboard getWhite() const;
    Bitboard getKing() const;
    bool isWhiteTurn() const;

    std::vector<int> getLegalMovesBlack(int from) const;
    std::vector<int> getLegalMovesWhite(int from) const;

    State moveBlack(int from, int to) const;
    State moveWhite(int from, int to) const;

    int getScore() const;
    bool isWhiteWinner() const;
    bool isBlackWinner() const;

private:
    const uint64_t zobristTable[3][81] = {0}; //per ora
    const uint64_t zobristWhiteToMove = 0; //per ora

    uint64_t zobristHash;
    Bitboard black;
    Bitboard white;
    Bitboard king;
    bool whiteTurn;
    bool whiteWinner;
    bool blackWinner;

    void computeZobristHash();
    bool capture(const Bitboard& attacker, const Bitboard& defender) const;
};

#endif
