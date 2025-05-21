#ifndef ENGINE_H
#define ENGINE_H

#include "NodeType.hpp"
#include "TTEntry.hpp"
#include "TranspositionTable.hpp"
#include "BestMove.hpp"
#include "State.hpp"
#include "SearchersArgs.hpp"
#include <iostream>
#include <atomic>
#include <thread>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string>
#include <cctype>

class Engine {
public:
    Engine(int threads = 4);
    void go(int seconds = 60);

private:
    String name = "RedBlut";
    int whitePort = 5800;
    int blackPort = 5801;
    int sd;
    int threads;
    void iterativeDeepening(const State& initialState, int& bestFrom, int& bestTo);
    void negaMaxAspirationWindow(const State& state, int depth, int alpha, int beta,
                                 int& bestScore, int& bestFrom, int& bestTo);
};

#endif
