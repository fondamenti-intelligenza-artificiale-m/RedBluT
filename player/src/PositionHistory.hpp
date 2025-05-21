#ifndef POSITIONHISTORY_H
#define POSITIONHISTORY_H

#include <vector>
#include <unordered_set>
#include <cstdint>
#include <cstddef>

class PositionHistory {
public:
    static PositionHistory& getInstance();

    bool contains(uint64_t zobristHash) const;
    void push(uint64_t zobristHash);
    void pop();
    void clear();
    bool empty() const;
    size_t size() const;

private:
    PositionHistory() = default;

    std::vector<uint64_t> stack;
    std::unordered_set<uint64_t> set;
};

#endif
