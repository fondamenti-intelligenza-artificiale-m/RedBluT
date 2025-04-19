#include "Engine.hpp"

Engine::Engine(int threads) : threads(threads) {}

void Engine::go(int seconds) {
    State initialState; //ci dovrebbe essere un ascolto

    std::vector<std::future<void>> futures;
    for (int i = 0; i < threads; ++i) {
        futures.emplace_back(std::async(std::launch::async, [&, i]() {
            int bestFrom = -1, bestTo = -1;
            iterativeDeepening(initialState, bestFrom, bestTo);
        }));
    }
    std::this_thread::sleep_for(std::chrono::seconds(seconds - 1));

    int from, to;
    BestMove::getInstance().play(from, to);
    std::cout << "Mossa scelta: da " << from << " a " << to << std::endl; //ci vorrà un messaggio
}

void Engine::iterativeDeepening(const State& initialState, int& bestFrom, int& bestTo) {
    int depth = 1;
    while (true) {
        int from = -1, to = -1, bestScore = 0;
        negaMaxAspirationWindow(initialState, depth++, -100000, 100000, bestScore, from, to);
        BestMove::getInstance().propose(from, to);
    }
}

void Engine::negaMaxAspirationWindow(const State& rootState, int maxDepth, int alpha, int beta, int& bestScore, int& bestFrom, int& bestTo) {
    struct StackEntry {
        State state;
        int depth;
        int alpha, beta;
        int moveFrom, moveTo;
        int childIndex = 0;
        std::vector<std::pair<int, int>> moves;
        int bestScore = -1000000;
        int bestFrom = -1;
        int bestTo = -1;
    };
    std::vector<StackEntry> stack;
    stack.push_back({rootState, maxDepth, alpha, beta, -1, -1, 0});
    PositionHistory& ph = PositionHistory::getInstance();
    TranspositionTable& tt = TranspositionTable::getInstance();

    auto updateParent = [&](StackEntry& parent, int score, int from, int to) {
        int flippedScore = -score;
        if (flippedScore > parent.bestScore) {
            parent.bestScore = flippedScore;
            parent.bestFrom = from;
            parent.bestTo = to;
        }
        parent.alpha = std::max(parent.alpha, parent.bestScore);
    };

    while (!stack.empty()) {
        StackEntry& entry = stack.back();

        // --- Terminal node ---
        if (entry.depth == 0 || state.isWhiteWinner() || state.isBlackWinner()) {
            entry.bestScore = entry.state.evaluate();
            stack.pop_back();

            if (!stack.empty()) {
                updateParent(stack.back(), entry.bestScore, entry.moveFrom, entry.moveTo);
            } else {
                bestScore = entry.bestScore;
                bestFrom = entry.bestFrom;
                bestTo = entry.bestTo;
            }
            continue;
        }

        // --- TT lookup ---
        if (entry.childIndex == 0) {
            uint64_t hash = entry.state.getZobristHash();
            TTEntry tte;
            if (tt.lookup(hash, tte) && tte.depth >= entry.depth) {
                bool cutoff = false;
                switch (tte.type) {
                    case NodeType::EXACT:
                        entry.bestScore = tte.score;
                        cutoff = true;
                        break;
                    case NodeType::LOWERBOUND:
                        if (tte.score >= entry.beta) { entry.bestScore = tte.score; cutoff = true; }
                        break;
                    case NodeType::UPPERBOUND:
                        if (tte.score <= entry.alpha) { entry.bestScore = tte.score; cutoff = true; }
                        break;
                }
                if (cutoff) {
                    stack.pop_back();
                    if (!stack.empty()) updateParent(stack.back(), entry.bestScore, entry.moveFrom, entry.moveTo);
                    else {
                        bestScore = entry.bestScore;
                        bestFrom = entry.bestFrom;
                        bestTo = entry.bestTo;
                    }
                    continue;
                }

                // --- Generate moves (first time only) ---
                Bitboard whitePieces = entry.state.getWhite().orC(entry.state.getKing());
                for (int from = 0; from < 81; ++from) {
                    if (whitePieces.get(from)) {
                        std::vector<int> destinations = entry.state.getLegalMovesWhite(from);
                        for (int to : destinations) {
                            entry.moves.emplace_back(from, to);
                        }
                    }
                }
            }
        }

        // --- No more children to explore: backtrack + TT store ---
        if (entry.childIndex >= entry.moves.size()) {
            TTEntry tte;
            tte.score = entry.bestScore;
            tte.depth = entry.depth;

            if (entry.bestScore <= entry.alpha) tte.type = NodeType::UPPERBOUND;
            else if (entry.bestScore >= entry.beta) tte.type = NodeType::LOWERBOUND;
            else tte.type = NodeType::EXACT;

            tte.from = entry.bestFrom;
            tte.to = entry.bestTo;

            tt.store(entry.state.getZobristHash(), tte);
            stack.pop_back();

            if (!stack.empty()) {
                updateParent(stack.back(), entry.bestScore, entry.moveFrom, entry.moveTo);
                if (stack.back().alpha >= stack.back().beta)
                    stack.pop_back(); // alpha-beta cutoff
            } else {
                bestScore = entry.bestScore;
                bestFrom = entry.bestFrom;
                bestTo = entry.bestTo;
            }

            continue;
        }

        // --- Explore next child ---
        auto [from, to] = entry.moves[entry.childIndex++];
        State child = entry.state.moveWhite(from, to);
        stack.push_back({child, entry.depth - 1, -entry.beta, -entry.alpha, from, to, 0});
    }
}
