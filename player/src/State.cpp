#include "State.hpp"

State::State() :
    camps  (0b11100000001000, 0b0000000000100000001110000011100000001000000000000010000000111000),
    lookout(             0b0, 0b0000010000000010000001101100000010000000010000000000000000000000),
    castle (             0b0, 0b0000000000000000000000010000000000000000000000000000000000000000),
    black(camps), white(lookout), king(castle),
    whiteTurn(true), whiteWinner(false), blackWinner(false),
    zobristHash(0) //questo invece lo cambieremo col valore hardcodato invece di calcolarelo
{
    computeZobristHash(); //questo nella versione finale lo togliamo
} 

State::State(const Bitboard& white, const Bitboard& black, const Bitboard& kings,
             bool whiteTurn, bool whiteWinner, bool blackWinner, uint64_t zobristHash) :
    camps  (0b11100000001000, 0b0000000000100000001110000011100000001000000000000010000000111000),
    lookout(             0b0, 0b0000010000000010000001101100000010000000010000000000000000000000),
    castle (             0b0, 0b0000000000000000000000010000000000000000000000000000000000000000),   
    white(white), black(black), king(kings),
    whiteTurn(whiteTurn), whiteWinner(whiteWinner), blackWinner(blackWinner), zobristHash(zobristHash)
{
    if (zobristHash == -1) computeZobristHash();
}

uint64_t State::getZobristHash() const { return zobristHash; }

bool State::isWhiteTurn() const { return whiteTurn; }
Bitboard State::getBlack() const { return black; }
Bitboard State::getWhite() const { return white; }
Bitboard State::getKing() const { return king; }
Bitboard State::getPieces() const { return black.orV(white).orV(king); }

Bitboard State::getLegalMovesBlack(int from) const {
    return magicLookUpBlack[from]
        [getPieces()
            .andV(movesAloneBlack[from])
            .mulV(magicNumbersBlack[from])
            .rightV(magicShiftsBlack[from])
            .lower];
}

Bitboard State::getLegalMovesWhite(int from) const {
    return magicLookUpWhite[from]
        [getPieces()
            .andV(movesAloneWhite[from])
            .mulV(magicNumbersWhite[from])
            .rightV(magicShiftsWhite[from])
            .lower];
}

State State::moveBlack(int from, int to) const {
    Bitboard newBlack = black;
    Bitboard newWhite = white;
    Bitboard newKing = king;
    bool newBlackWinner = false;
    uint64_t newZobristHash = zobristHash ^ zobristWhiteToMove;
    newBlack.clearR(from);
    newZobristHash ^= zobristTable[0][from];
    newBlack.setR(to);
    newZobristHash ^= zobristTable[0][to];
    if (newBlack.get(31) && newBlack.get(39) && newBlack.get(41) && newBlack.get(49) && newKing.get(40)) {
        newKing.clearR(40);
        newZobristHash ^= zobristTable[2][40];
        newBlackWinner = true;
    }
    if ((newBlack.get(22) && newBlack.get(30) && newBlack.get(32) && newKing.get(31))) {
        newKing.clearR(31);
        newZobristHash ^= zobristTable[2][31];
        newBlackWinner = true;
    }
    if (newBlack.get(30) && newBlack.get(38) && newBlack.get(48) && newKing.get(39)) {
        newKing.clearR(39);
        newZobristHash ^= zobristTable[2][39];
        newBlackWinner = true;
    }
    if (newBlack.get(32) && newBlack.get(42) && newBlack.get(50) && newKing.get(41)) {
        newKing.clearR(41);
        newZobristHash ^= zobristTable[2][41];
        newBlackWinner = true;
    }
    if (newBlack.get(48) && newBlack.get(50) && newBlack.get(58) && newKing.get(49)) {
        newKing.clearR(49);
        newZobristHash ^= zobristTable[2][49];
        newBlackWinner = true;
    }
    for (int dir = 0; dir < 4; ++dir) {
        int maybeCaptured = to + directions[dir];
        int nedded = maybeCaptured + directions[dir];
        if (nedded < 0 || nedded > 80) continue;
        if (!(newWhite.orV(newKing).get(maybeCaptured) & 1)) continue;
        if (!(newBlack.orV(camps).orV(castle).get(nedded) & 1)) continue;
        if ((dir == -1 || dir == 1)) {
            if (!(to / 9 == maybeCaptured / 9 && to / 9 == nedded / 9)) continue;
        } else {
            if (!(to % 9 == maybeCaptured % 9 && to % 9 == nedded % 9)) continue;
        }
        if (newWhite.get(maybeCaptured) & 1) {
            newWhite.clearR(maybeCaptured);
            newZobristHash ^= zobristTable[1][maybeCaptured];
        } else if (newKing.get(maybeCaptured) & 1) {
            newKing.clearR(maybeCaptured);
            newZobristHash ^= zobristTable[2][40];
        }
    }
    return State(newBlack, newWhite, newKing, !whiteTurn, whiteWinner, newBlackWinner, newZobristHash);
}

State State::moveWhite(int from, int to) const {
    Bitboard newBlack = black;
    Bitboard newWhite = white;
    Bitboard newKing = king;
    bool newWhiteWinner = false;
    uint64_t newZobristHash = zobristHash ^ zobristWhiteToMove;
    if (king.get(from) & 1) {
        newKing.clearR(from);
        newZobristHash ^= zobristTable[2][from];
        newKing.setR(to);
        newZobristHash ^= zobristTable[2][to];
        if ((to / 9 == 0) || (to / 9 == 8) || (to % 9 == 0) || (to % 9 == 8)) newWhiteWinner = true;
    } else {
        newWhite.clearR(from);
        newZobristHash ^= zobristTable[1][from];
        newWhite.setR(to);
        newZobristHash ^= zobristTable[1][to];
    }
    for (int dir = 0; dir < 4; ++dir) {
        int maybeCaptured = to + directions[dir];
        int nedded = maybeCaptured + directions[dir];
        if (nedded < 0 || nedded > 80) continue;
        if (!(newBlack.get(maybeCaptured) & 1)) continue;
        if (!(white.orV(king).orV(camps).orV(castle).get(nedded) & 1)) continue;
        if ((dir == -1 || dir == 1)) {
            if (!(to / 9 == maybeCaptured / 9 && to / 9 == nedded / 9)) continue;
        } else {
            if (!(to % 9 == maybeCaptured % 9 && to % 9 == nedded % 9)) continue;
        }
        newBlack.clearR(maybeCaptured);
        newZobristHash ^= zobristTable[0][maybeCaptured];
    }
    return State(newBlack, newWhite, newKing, !whiteTurn, newWhiteWinner, blackWinner, newZobristHash);
}

int State::evaluate() const {
    if (isWhiteWinner()) return 100;
    if (isBlackWinner()) return -100;
    return 0; // Dummy, da migliorare più avanti
}

bool State::isWhiteWinner() const { return whiteWinner; }
bool State::isBlackWinner() const { return blackWinner; }

void State::computeZobristHash() {
    zobristHash = 0;
    if (isWhiteTurn()) zobristHash ^= zobristWhiteToMove;
    for (int i = 0; i < 81; ++i) { 
        if (black.get(i)) zobristHash ^= zobristTable[0][i];
        if (white.get(i)) zobristHash ^= zobristTable[1][i];
        if (king.get(i)) zobristHash ^= zobristTable[2][i];
    }
}