#include "State.hpp"

State::State() :
    black(Bitboard(0x11100000001000, 0x0000000000100000001110000011100000001000000000000010000000111000)),
    white(Bitboard(             0x0, 0x0000010000000010000001101100000010000000010000000000000000000000)),
    king( Bitboard(             0x0, 0x0000000000000000000000010000000000000000000000000000000000000000)),
    whiteTurn(true), whiteWinner(false), blackWinner(false), zobristHash(computeZobristHash()) {} //poi lo hardcodiamo

State::State(const Bitboard& black, const Bitboard& white, const Bitboard& king,
            bool whiteTurn, bool whiteWinner, bool blackWinner) :
    black(black), white(white), king(king), whiteTurn(whiteTurn),
    whiteWinner(whiteWinner), blackWinner(blackWinner), zobristHash(computeZobristHash()) {}

State::State(const Bitboard& black, const Bitboard& white, const Bitboard& king,
            bool whiteTurn, bool whiteWinner, bool blackWinner, uint64_t zobristHash) :
black(black), white(white), king(king), whiteTurn(whiteTurn),
whiteWinner(whiteWinner), blackWinner(blackWinner), zobristHash(zobristHash) {}

uint64_t State::getZobristHash() const { return zobristHash; }

bool State::isWhiteTurn() const { return whiteTurn; }
Bitboard State::getBlack() const { return black; }
Bitboard State::getWhite() const { return white; }
Bitboard State::getKing() const { return king; }
Bitboard State::getWhiteAndKing() const {return white.orV(king); }
Bitboard State::getPieces() const { return black.orV(getWhiteAndKing()); }

std::vector<int> State::getLegalMovesBlack(int from) const {
    uint64_t magicKey = ((getPieces() & movesAloneBlack[from]) * magicNumbersBlack[from]) >> magicShiftsBlack[from];
    return moveTableBlack[from][magicKey];
}

std::vector<int> State::getLegalMovesWhite(int from) const {
    uint64_t magicKey = ((getPieces() & movesAloneWhite[from]) * magicNumbersWhite[from]) >> magicShiftsWhite[from];
    return moveTableWhite[from][magicKey];
}

State State::moveBlack(int from, int to) const {
    Bitboard newBlack = black;
    uint64_t newZobristHash = zobristHash ^ zobristWhiteToMove;
    newBlack.clearR(from);
    newZobristHash ^= zobristTable[0][from];
    newBlack.setR(to);
    newZobristHash ^= zobristTable[0][to];

    //if (black.get(i)) black.clearR(from); cattura e condizione di vottoria

    return State(newBlack, white, king, !whiteTurn, whiteWinner, blackWinner, newZobristHash);
}

State State::moveWhite(int from, int to) const {
    Bitboard newWhite = white;
    Bitboard newKing = king;
    uint64_t newZobristHash = zobristHash ^ zobristWhiteToMove;
    if (king.get(from) & 1) {
        newKing.clearR(from);
        newZobristHash ^= zobristTable[2][from];
        newKing.setR(to);
        newZobristHash ^= zobristTable[2][to];
    } else {
        newWhite.clearR(from);
        newZobristHash ^= zobristTable[1][from];
        newWhite.setR(to);
        newZobristHash ^= zobristTable[1][to];
    }

    // cattura e condizione di vottoria

    return State(black, newWhite, newKing, !whiteTurn, whiteWinner, blackWinner, newZobristHash);
}

int State::evaluate() const {
    if (PositionHistory::getInstance().contains(zobristHash)) return 0; // Pareggio per ripetizione
    return 1; // Dummy, da migliorare più avanti
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