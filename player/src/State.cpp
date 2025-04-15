#include "State.h"

State::State()
    : black(), white(), king(), whiteTurn(true),
      whiteWinner(false), blackWinner(false), zobristHash(0) {}

State::State(const Bitboard& black, const Bitboard& white, const Bitboard& king,
             bool whiteTurn, bool whiteWinner, bool blackWinner, uint64_t zobristHash)
    : black(black), white(white), king(king), whiteTurn(whiteTurn),
      whiteWinner(whiteWinner), blackWinner(blackWinner), zobristHash(zobristHash) {}

Bitboard State::getBlack() const { return black; }
Bitboard State::getWhite() const { return white; }
Bitboard State::getKing() const { return king; }
bool State::isWhiteTurn() const { return whiteTurn; }

std::vector<int> State::getLegalMovesBlack(int from) const {
    return {};
}

std::vector<int> State::getLegalMovesWhite(int from) const {
    return {};
}

State State::moveBlack(int from, int to) const {
    // TODO: logica di movimento
    uint64_t newZobrist = 0; // TODO: calcolare nuovo hash
    return State(black, white, king, !whiteTurn, whiteWinner, blackWinner, newZobrist);
}

State State::moveWhite(int from, int to) const {
    // TODO: logica di movimento
    uint64_t newZobrist = 0; // TODO: calcolare nuovo hash
    return State(black, white, king, !whiteTurn, whiteWinner, blackWinner, newZobrist);
}

int State::getScore() const {
    if (PositionHistory::getInstance().contains(zobristHash)) return 0; // Pareggio per ripetizione
    return 1; // Dummy, da migliorare più avanti
}

bool State::isWhiteWinner() const { return whiteWinner; }
bool State::isBlackWinner() const { return blackWinner; }

bool State::capture(const Bitboard& attacker, const Bitboard& defender) const {
    return false;
}
