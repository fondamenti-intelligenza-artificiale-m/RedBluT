#ifndef ENGINE_H
#define ENGINE_H

#include "NodeType.hpp"
#include "TTEntry.hpp"
#include "TranspositionTable.hpp"
#include "BestMove.hpp"
#include "State.hpp"
#include "ThreadArgs.hpp"
#include <iostream>
#include <atomic>
#include <thread>
#include <sys/socket.h>
#include <arpa/inet.h>    // per inet_pton
#include <unistd.h>       // per close, write, sleep
#include <netinet/in.h>
#include <string>
#include <cctype>

class Engine {
public:
    Engine();
    void start(std::string color, std::string ip_referee);
    int negaMax(const State& state, int depth, int alpha, int beta, int& bestFrom, int& bestTo);
    int negaMaxAspirationWindow(const State& state, int depth, int& bestFrom, int& bestTo);
    static void* iterativeDeepening(void* args);
    void playTurn(const State &initialSatate, int seconds);
    void end();
    void go(std::string color, int seconds, std::string ip_referee);

private:
    std::string name = "RedBlut";
    int whitePort = 5800;
    int blackPort = 5801;
    int sd;
    pthread_t searchers [3];
    pthread_barrier_t barrier;
    void iterativeDeepening(const State& initialState, int& bestFrom, int& bestTo);
    void negaMaxAspirationWindow(const State& state, int depth, int alpha, int beta,
                                 int& bestScore, int& bestFrom, int& bestTo);
};

#endif
