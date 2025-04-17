#include "Engine.h"

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
        int alpha;
        int beta;
        int moveFrom;
        int moveTo;
        int childIndex;
        std::vector<std::pair<int, int>> moves;
        int bestScore;
        int bestFrom;
        int bestTo;
    };

    std::vector<StackEntry> stack;
    stack.push_back({rootState, maxDepth, alpha, beta, -1, -1, 0, {}, -1000000, -1, -1});

    TranspositionTable& tt = TranspositionTable::getInstance();

    while (!stack.empty()) {
        StackEntry& entry = stack.back();

        if (entry.depth == 0) {
            entry.bestScore = entry.state.getScore();
            stack.pop_back();

            if (!stack.empty()) {
                StackEntry& parent = stack.back();
                int score = -entry.bestScore;
                if (score > parent.bestScore) {
                    parent.bestScore = score;
                    parent.bestFrom = entry.moveFrom;
                    parent.bestTo = entry.moveTo;
                }
                parent.alpha = std::max(parent.alpha, parent.bestScore);
            } else {
                bestScore = entry.bestScore;
                bestFrom = entry.bestFrom;
                bestTo = entry.bestTo;
            }

            continue;
        }

        uint64_t hash = entry.state.getZobristHash();

        if (entry.childIndex == 0) {
            TTEntry tte;
            if (tt.lookup(hash, tte) && tte.depth >= entry.depth) {
                if (tte.type == NodeType::EXACT) {
                    entry.bestScore = tte.score;
                    stack.pop_back();

                    if (!stack.empty()) {
                        StackEntry& parent = stack.back();
                        int score = -entry.bestScore;
                        if (score > parent.bestScore) {
                            parent.bestScore = score;
                            parent.bestFrom = entry.moveFrom;
                            parent.bestTo = entry.moveTo;
                        }
                        parent.alpha = std::max(parent.alpha, parent.bestScore);
                    } else {
                        bestScore = entry.bestScore;
                        bestFrom = entry.bestFrom;
                        bestTo = entry.bestTo;
                    }

                    continue;
                } else if (tte.type == NodeType::LOWERBOUND && tte.score <= entry.alpha) {
                    entry.bestScore = tte.score;
                    stack.pop_back();
                    continue;
                } else if (tte.type == NodeType::UPPERBOUND && tte.score >= entry.beta) {
                    entry.bestScore = tte.score;
                    stack.pop_back();
                    continue;
                }
            }

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

        if (entry.childIndex >= entry.moves.size()) {
            TTEntry tte;
            tte.score = entry.bestScore;
            tte.depth = entry.depth;

            if (entry.bestScore <= entry.alpha) {
                tte.type = NodeType::UPPERBOUND;
            } else if (entry.bestScore >= entry.beta) {
                tte.type = NodeType::LOWERBOUND;
            } else {
                tte.type = NodeType::EXACT;
            }

            tte.from = entry.bestFrom;
            tte.to = entry.bestTo;
            tt.store(hash, tte);

            stack.pop_back();

            if (!stack.empty()) {
                StackEntry& parent = stack.back();
                int score = -entry.bestScore;
                if (score > parent.bestScore) {
                    parent.bestScore = score;
                    parent.bestFrom = entry.moveFrom;
                    parent.bestTo = entry.moveTo;
                }
                parent.alpha = std::max(parent.alpha, parent.bestScore);
                if (parent.alpha >= parent.beta) {
                    stack.pop_back();
                }
            } else {
                bestScore = entry.bestScore;
                bestFrom = entry.bestFrom;
                bestTo = entry.bestTo;
            }

            continue;
        }

        auto [from, to] = entry.moves[entry.childIndex++];
        State childState = entry.state.moveWhite(from, to);

        stack.push_back({childState, entry.depth - 1, -entry.beta, -entry.alpha, from, to, 0, {}, -1000000, -1, -1});
    }
}
