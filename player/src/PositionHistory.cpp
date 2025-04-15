#include "PositionHistory.h"

PositionHistory& PositionHistory::getInstance() {
    static PositionHistory instance;
    return instance;
}

bool PositionHistory::contains(uint64_t zobristHash) const {
    return set.find(zobristHash) != set.end();
}

void PositionHistory::push(uint64_t zobristHash) {
    stack.push_back(zobristHash);
    set.insert(zobristHash);
}

void PositionHistory::pop() {
    if (!stack.empty()) {
        uint64_t top = stack.back();
        set.erase(top);
        stack.pop_back();
    }
}

void PositionHistory::clear() {
    stack.clear();
    set.clear();
}

bool PositionHistory::empty() const {
    return stack.empty();
}

size_t PositionHistory::size() const {
    return stack.size();
}
