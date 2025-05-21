#include "BestMove.hpp"

BestMove& BestMove::getInstance() {
    static BestMove instance;
    return instance;
}

void BestMove::propose(int f, int t) {
    std::lock_guard<std::mutex> lock(mtx);
    from = f;
    to = t;
}

void BestMove::play(int& f, int& t) {
    std::lock_guard<std::mutex> lock(mtx);
    f = from;
    t = to;
    from = -1;
    to = -1;
}
