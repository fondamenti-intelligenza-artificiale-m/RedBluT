#ifndef TTENTRY_H
#define TTENTRY_H

enum class NodeType { EXACT, LOWERBOUND, UPPERBOUND };

struct TTEntry {
    int depth;
    int from;
    int to;
    int score;
    NodeType type;

    TTEntry(int depth, int from, int to, int score, NodeType type)
        : depth(depth), from(from), to(to), score(score), type(type) {}
};

#endif
