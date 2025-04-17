#ifndef ENGINE_H
#define ENGINE_H

#include "NodeType.h"
#include "TTEntry.h"
#include "TranspositionTable.h"

#include "State.h"
#include "BestMove.h"
#include <iostream>
#include <chrono>
#include <atomic>
#include <thread>
#include <vector>
#include <future>

class Engine {
public:
    Engine(int threads = 4);

    void go(int seconds = 60);

private:
    int threads;

    void iterativeDeepening(const State& initialState, int& bestFrom, int& bestTo);

    void negaMaxAspirationWindow(const State& state, int depth, int alpha, int beta,
                                 int& bestScore, int& bestFrom, int& bestTo);
};

#endif
