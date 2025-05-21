#ifndef STATE_H
#define STATE_H

#include "ZobristParams.hpp"
#include "MagicLookUpWhite.hpp"
#include "MagicLookUpBlack.hpp"
#include "Bitboard.hpp"
#include "PositionHistory.hpp"
#include <vector>

class State {
public:
    State();
    State(const Bitboard& black, const Bitboard& white, const Bitboard& king,
        bool whiteTurn, bool whiteWinner = false, bool blackWinner = false);
    State(const Bitboard& black, const Bitboard& white, const Bitboard& king,
        bool whiteTurn, bool whiteWinner = false, bool blackWinner = false, uint64_t zobristHash);
    
    uint64_t getZobristHash() const;

    bool isWhiteTurn() const;
    Bitboard getBlack() const;
    Bitboard getWhite() const;
    Bitboard getKing() const;
    Bitboard getWhiteAndKing() const;
    Bitboard getPieces() const;

    std::vector<int> getLegalMovesBlack(int from) const;
    std::vector<int> getLegalMovesWhite(int from) const;

    State moveBlack(int from, int to) const;
    State moveWhite(int from, int to) const;

    int evaluate() const;
    bool isWhiteWinner() const;
    bool isBlackWinner() const;

private:
    int directions[4] = {1, 9, -1, -9};
    Bitboard camps  (0x11100000001000, 0x0000000000100000001110000011100000001000000000000010000000111000);
    Bitboard lookout(             0x0, 0x0000010000000010000001101100000010000000010000000000000000000000);
    Bitboard castle (             0x0, 0x0000000000000000000000010000000000000000000000000000000000000000);

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
