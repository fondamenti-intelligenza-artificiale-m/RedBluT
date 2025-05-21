#ifndef TRANSPOSITION_TABLE_H
#define TRANSPOSITION_TABLE_H

#include "TTEntry.hpp"
#include <unordered_map>
#include <cstdint>

class TranspositionTable {
public:
    static TranspositionTable& getInstance();

    void store(uint64_t zobristHash, const TTEntry& entry);
    bool lookup(uint64_t zobristHash, TTEntry& entry) const;
    void clear();

private:
    TranspositionTable() = default;
    std::unordered_map<uint64_t, TTEntry> table;
};

#endif
