#ifndef STATE_H
#define STATE_H

#include <vector>
#include "Bitboard.h"
#include "PositionHistory.h"

class State {
public:
    State();
    State(const Bitboard& black, const Bitboard& white, const Bitboard& king,
          bool whiteTurn, bool whiteWinner = false, bool blackWinner = false,
          uint64_t zobristHash = 0);

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
    Bitboard black;
    Bitboard white;
    Bitboard king;
    bool whiteTurn;
    bool whiteWinner;
    bool blackWinner;
    uint64_t zobristHash;

    bool capture(const Bitboard& attacker, const Bitboard& defender) const;
};

#endif
