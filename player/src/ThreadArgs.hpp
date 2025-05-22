#ifndef THREADARGS_HPP
#define THREADARGS_HPP

#include "State.hpp"

class Engine;

struct ThreadArgs {
    int id;
    const State* state;
    Engine* engine;
};

#endif
