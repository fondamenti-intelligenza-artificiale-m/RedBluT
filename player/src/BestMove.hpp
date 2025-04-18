#ifndef BEST_MOVE_H
#define BEST_MOVE_H

#include <mutex>

class BestMove {
public:
    static BestMove& getInstance();

    void propose(int from, int to);
    void play(int& from, int& to);

private:
    BestMove() : from(-1), to(-1) {}

    int from;
    int to;
    std::mutex mtx;
};

#endif
