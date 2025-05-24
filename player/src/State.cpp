#include "State.hpp"

int State::directions[4] = {-9, -1, 1, 9};
const Bitboard State::diagonals(0b100010,0b1000001000000000000000000000000000001000001000100010000000000);
const Bitboard State::goal     (0b01100011010000000,0b1100000001000000000000000000000000000100000001100000001011000110);
const Bitboard State::row3     (0,0b0000000000000000000000000000000000000111111111000000000000000000);
const Bitboard State::row7     (0,0b0111111111000000000000000000000000000000000000000000000000000000);
const Bitboard State::columnC  (0b10000000010, 0b0000000100000000100000000100000000100000000100000000100000000100);
const Bitboard State::columnG  (0b100000000100000, 0b0001000000001000000001000000001000000001000000001000000001000000);

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
    if (zobristHash == 0) computeZobristHash();
}

uint64_t State::getZobristHash() const { return zobristHash; }

bool State::isWhiteTurn() const { return whiteTurn; }
Bitboard State::getBlack() const { return black; }
Bitboard State::getWhite() const { return white; }
Bitboard State::getKing() const { return king; }
Bitboard State::getPieces() const { return black.orV(white).orV(king); }

/*
Bitboard State::getLegalMoves(int from) const {
    return this->isWhiteTurn() ? this->getLegalMovesWhite(from) : this->getLegalMovesBlack(from);
}

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
}*/

std::vector<int> State::getLegalMoves(int from) const {
    std::vector<int> legalMoves;
    Bitboard prohibited = getPieces().orV(camps.orV(castle));
    for (int dir : directions) {
        int to = from + dir;
        while (
            to >= 0 && to <= 80 && 
            (((dir == 1 || dir == -1) && (from / 9 == to / 9)) || (dir == 9 || dir == -9))
        ) {
            if (prohibited.get(to)) break;
            legalMoves.push_back(to);
            to += dir;
        }
    }
    return legalMoves;
}

State State::move(int from, int to) const {
    return this->isWhiteTurn() ? this->moveWhite(from, to) : this->moveBlack(from, to);
}

State State::moveBlack(int from, int to) const {
    Bitboard newBlack = black;
    Bitboard newWhite = white;
    Bitboard newKing = king;
    bool newBlackWinner = false;
    //uint64_t newZobristHash = zobristHash ^ zobristWhiteToMove;
    newBlack.clearR(from);
    //newZobristHash ^= zobristTable[0][from];
    newBlack.setR(to);
    //newZobristHash ^= zobristTable[0][to];
    if (newBlack.get(31) && newBlack.get(39) && newBlack.get(41) && newBlack.get(49) && newKing.get(40)) {
        newKing.clearR(40);
        //newZobristHash ^= zobristTable[2][40];
        newBlackWinner = true;
    }
    if ((newBlack.get(22) && newBlack.get(30) && newBlack.get(32) && newKing.get(31))) {
        newKing.clearR(31);
        //newZobristHash ^= zobristTable[2][31];
        newBlackWinner = true;
    }
    if (newBlack.get(30) && newBlack.get(38) && newBlack.get(48) && newKing.get(39)) {
        newKing.clearR(39);
        //newZobristHash ^= zobristTable[2][39];
        newBlackWinner = true;
    }
    if (newBlack.get(32) && newBlack.get(42) && newBlack.get(50) && newKing.get(41)) {
        newKing.clearR(41);
        //newZobristHash ^= zobristTable[2][41];
        newBlackWinner = true;
    }
    if (newBlack.get(48) && newBlack.get(50) && newBlack.get(58) && newKing.get(49)) {
        newKing.clearR(49);
        //newZobristHash ^= zobristTable[2][49];
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
            //newZobristHash ^= zobristTable[1][maybeCaptured];
        } else if (newKing.get(maybeCaptured) & 1) {
            newKing.clearR(maybeCaptured);
            //newZobristHash ^= zobristTable[2][40];
        }
    }
    return State(newBlack, newWhite, newKing, !whiteTurn, whiteWinner, newBlackWinner, 0);
}

State State::moveWhite(int from, int to) const {
    Bitboard newBlack = black;
    Bitboard newWhite = white;
    Bitboard newKing = king;
    bool newWhiteWinner = false;
    //uint64_t newZobristHash = zobristHash ^ zobristWhiteToMove;
    if (king.get(from) & 1) {
        newKing.clearR(from);
        //newZobristHash ^= zobristTable[2][from];
        newKing.setR(to);
        //newZobristHash ^= zobristTable[2][to];
        if ((to / 9 == 0) || (to / 9 == 8) || (to % 9 == 0) || (to % 9 == 8)) newWhiteWinner = true;
    } else {
        newWhite.clearR(from);
        //newZobristHash ^= zobristTable[1][from];
        newWhite.setR(to);
        //newZobristHash ^= zobristTable[1][to];
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
        //newZobristHash ^= zobristTable[0][maybeCaptured];
    }
    return State(newBlack, newWhite, newKing, !whiteTurn, newWhiteWinner, blackWinner, 0);
}

int State::getKingPosition() const {
    for (int i = 0; i < 81; ++i) {
        if (king.get(i)) return i;
    }
    return -1; // King not found
}

int State::evaluate() const {
    if (isWhiteWinner()) return 10000;
    if (isBlackWinner()) return -10000;
    int result = 0;
    result += checkDiagonal();
    result += whiteEdges();
    result += blackNearKing();
    result += countPieces();
    result += checkFreeWay();
    result += kingMobility();

    return result; 
}


// Valutare se diagonale completa
int State::checkDiagonal() const {
    const int weight = -100;
    int count = black.andV(diagonals).countOnes();
    return count * weight;
}

int State::whiteEdges() const {
    int result = 0;

    const int weight = 100;

    if (white.get(1) || white.get(2))
        result += weight;
    if (white.get(6) || white.get(7))
        result += weight;
    if (white.get(9) || white.get(18))
        result += weight;
    if (white.get(17) || white.get(26))
        result += weight;
    if (white.get(54) || white.get(63))
        result += weight;
    if (white.get(62) || white.get(71))
        result += weight;
    if (white.get(78) || white.get(79))
        result += weight;
    if (white.get(73) || white.get(74))
        result += weight;
    return result;
}

int State::blackNearKing() const {
    int result = 0;
    const int weight = 250;
    if (king.get(40)){
        if(black.get(31))
            result -= weight;
        if(black.get(39))
            result -= weight;
        if(black.get(41))
            result -= weight;
        if(black.get(49))
            result -= weight;    
    }else if(king.get(31)) {
        if (black.get(22))
            result -= weight;
        if (black.get(30))
            result -= weight;
        if (black.get(32))
            result -= weight; 
    }else if (king.get(39)) {
        if (black.get(30))
            result -= weight;
        if (black.get(38))
            result -= weight;
        if (black.get(48))
            result -= weight; 
    }else if (king.get(41)) {
        if (black.get(32))
            result -= weight;
        if (black.get(42))
            result -= weight;
        if (black.get(50))
            result -= weight; 
    }else if (king.get(49)) {
        if (black.get(48))
            result -= weight;
        if (black.get(50))
            result -= weight;
        if (black.get(58))
            result -= weight; 
    }else {
        int kingPos = getKingPosition();
        for(int i = 0; i < 4; ++i) {
            if (black.get(kingPos + directions[i])) {
                result -= weight;
                //break; //Il re potrebbe piazzarsi in mezzo a due pedine nere già presenti
            }
        }
        // re vicino all'angolo delle caserme
        if (kingPos==12 || kingPos==14 || kingPos==28 || kingPos==34 || kingPos==46 || kingPos==52 || kingPos==66 || kingPos==68)
            result -= weight*2;
        else if (kingPos==22 || kingPos==38 || kingPos==42 || kingPos==58)
            result -= weight;
    }
    return result;
}


int State::checkFreeWay() const{
    int result = 0;
    bool found = false;

    if(black.orV(white).andV(row3).isEmpty()){
        result += 30;
        if(!king.andV(row3).isEmpty())
            result += 1000;
    }
    if(black.orV(white).andV(row7).isEmpty()){
        result += 30;
        if(!found && !king.andV(row7).isEmpty())
            result += 1000;
    }
    if(black.orV(white).andV(columnC).isEmpty()){
        result += 30;
        if(!found && !king.andV(columnC).isEmpty())
            result += 1000;
    }
    if(black.orV(white).andV(columnG).isEmpty()){
        result += 30;
        if(!found && !king.andV(columnG).isEmpty())
            result += 1000;
    }
    return result;
}

int State::countPieces() const{
    return white.countOnes()*50 - black.countOnes()*25;
}


// Lasciamo commentato il check sul goal
int State::kingMobility() const {
    int result = 0;
    int weight = +10;
    //int goalweight = 25;
    int kingPos = getKingPosition();
    result += getLegalMoves(kingPos).size()*weight;

    /*Bitboard movesKing = movesAloneWhite[kingPos];
    int piecesInTrajectory = movesKing.andV(black).countOnes() + movesKing.andV(white).countOnes();
    result += piecesInTrajectory*weight;*/
    //int goalInTrajectory = movesKing.andV(goal).countOnes();
    //result += goalInTrajectory*goalweight;

    return result;
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