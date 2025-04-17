#ifndef TTENTRY_H
#define TTENTRY_H

#include "NodeType.h"

struct TTEntry {
    int depth;
    int from;
    int to;
    int score;
    NodeType type;

    TTEntry() : depth(0), from(-1), to(-1), score(0), type(NodeType::EXACT) {}

    TTEntry(int depth, int from, int to, int score, NodeType type)
        : depth(depth), from(from), to(to), score(score), type(type) {}
};

#endif
