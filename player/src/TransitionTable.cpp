#include "TranspositionTable.hpp"

TranspositionTable& TranspositionTable::getInstance() {
    static TranspositionTable instance;
    return instance;
}

void TranspositionTable::store(uint64_t zobristHash, const TTEntry& entry) {
    table[zobristHash] = entry;
}

bool TranspositionTable::lookup(uint64_t zobristHash, TTEntry& entry) const {
    auto it = table.find(zobristHash);
    if (it != table.end()) {
        entry = it->second;
        return true;
    }
    return false;
}

void TranspositionTable::clear() {
    table.clear();
}
