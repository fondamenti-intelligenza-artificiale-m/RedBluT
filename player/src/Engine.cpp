#include "Engine.hpp"

Engine::Engine(int threads) : threads(threads) {}

void Engine::go(int seconds) {
    State initialState; //ci dovrebbe essere un ascolto

    //std::vector<std::future<void>> futures;
    //for (int i = 0; i < threads; ++i) {
        //futures.emplace_back(std::async(std::launch::async, [&, i]() {
            int bestFrom = -1, bestTo = -1;
            iterativeDeepening(initialState, bestFrom, bestTo);
        //}));
    //}
    //std::this_thread::sleep_for(std::chrono::seconds(seconds - 1));

    int from, to;
    BestMove::getInstance().play(from, to);
    std::cout << "Mossa scelta: da " << from << " a " << to << std::endl; //ci vorrà un messaggio
}

void Engine::iterativeDeepening(const State& initialState, int& bestFrom, int& bestTo) {
    int depth = 1;
    while (true) {
        int from = -1, to = -1;
        negaMaxAspirationWindow(initialState, depth++, from, to);
        BestMove::getInstance().propose(from, to);
    }
}

int Engine::negaMaxAspiration(const State& state, int depth, int& bestFrom, int& bestTo) {
    int guess = state.evaluate();
    int window = 10;
    int alpha = guess - window;
    int beta  = guess + window;
    while (true) {
        int newFrom = -1, newTo = -1;
        int score = negaMax(state, depth, alpha, beta, newFrom, newTo);
        if (score <= alpha) alpha -= window;
        else if (score >= beta) beta += window;
        else {
            bestFrom = newFrom;
            bestTo = newTo;
            return score;
        }
        window *= 2;
    }
}

int Engine::negaMax(const State& state, int depth, int alpha, int beta, int& bestFrom, int& bestTo) {
    if (depth == 0 || state.isWhiteWinner() || state.isBlackWinner()) return state.evaluate();
    uint64_t hash = state.getZobristHash();
    TTEntry tte;
    TranspositionTable& tt = TranspositionTable::getInstance();
    if (tt.lookup(hash, tte) && tte.depth >= depth) {
        switch (tte.type) {
            case NodeType::EXACT:
                bestFrom = tte.from;
                bestTo = tte.to;
                return tte.score;
            case NodeType::LOWERBOUND:
                alpha = std::max(alpha, tte.score);
                break;
            case NodeType::UPPERBOUND:
                beta = std::min(beta, tte.score);
                break;
        }
        if (alpha >= beta) {
            bestFrom = tte.from;
            bestTo = tte.to;
            return tte.score;
        }
    }
    int maxEval = -1000000;
    int localBestFrom = -1, localBestTo = -1;
    Bitboard pieces = isWhiteToMove ? state.getWhite().orV(state.getKing()) : state.getBlack().orV(state.getKing());
    for (int from = 0; from < 81; ++from) {
        if (!pieces.get(from)) continue;
        Bitboard moves = isWhiteToMove ? state.getLegalMovesWhite(from) : state.getLegalMovesBlack(from);
        for (int to : moves) {
            State child = isWhiteToMove ? state.moveWhite(from, to) : state.moveBlack(from, to);
            int childFrom, childTo;
            int eval = -negaMax(child, depth - 1, -beta, -alpha, !isWhiteToMove, childFrom, childTo);
            if (eval > maxEval) {
                maxEval = eval;
                localBestFrom = from;
                localBestTo = to;
            }
            alpha = std::max(alpha, eval);
            if (alpha >= beta) break;
        }
    }
    NodeType nodeType = NodeType::EXACT;
    if (maxEval <= alpha) nodeType = NodeType::UPPERBOUND;
    else if (maxEval >= beta) nodeType = NodeType::LOWERBOUND;
    tt.store(hash, {maxEval, depth, nodeType, localBestFrom, localBestTo});
    bestFrom = localBestFrom;
    bestTo = localBestTo;
    return maxEval;
}
