#include "Engine.hpp"

Engine::Engine(int threads) : threads(threads) {}

void Engine::start(string color, string ip_referee) {
    struct sockeraddr_in s{};
    s.sin_family = AF_INET;
    for (char& c : color) c = std::toupper(static_cast<unsigned char>(c));
    s.sin_port = htons(color == "WHITE" ? whitePort : blackPort);
    if (inet_pton(AF_INET, ip_referee.c_str(), &s.sin_addr) <= 0) exit(EXIT_FAILURE);
    sd = socket(AF_INET, SOCK_STREAM, 0);
    if (sd < 0) exit(EXIT_FAILURE);
    if (connect(sd, (struct sockaddr*)&s, sizeof(s)) < 0) {
        close(sd);
        exit(EXIT_FAILURE);
    }
    uint32_t length = htonl(name.size());
    ssize_t sent = write(sd, &length, sizeof(length));
    if (sent != sizeof(length)) {
        close(sd);
        exit(EXIT_FAILURE);
    }
    sent = write(sd, name.c_str(), name.size());
    if (sent != (ssize_t)name.size()) {
        close(sd);
        exit(EXIT_FAILURE);
    }
}

int Engine::negaMax(const State& state, int depth, int alpha, int beta, int& bestFrom, int& bestTo) {
    if (depth == 0) return state.evaluate();
    if (state.isWhiteWinner()) return 10000;
    if (state.isBlackWinner()) return -10000;
    PositionHistory& ph = PositionHistory::getInstance();
    uint64_t hash = state.getZobristHash();
    if (ph.contains(hash)) {
        bestFrom = -1;
        bestTo = -1;
        return 0;
    }
    ph.push(hash);
    TTEntry tte;
    TranspositionTable& tt = TranspositionTable::getInstance();
    if (tt.lookup(hash, tte) && tte.depth >= depth) {
        switch (tte.type) {
            case NodeType::EXACT:
                bestFrom = tte.from;
                bestTo = tte.to;
                ph.pop();
                return tte.score;
            case NodeType::LOWERBOUND:
                alpha = std::max(alpha, tte.score);
                break;
            case NodeType::UPPERBOUND:
                beta = std::min(beta, tte.score);
                break;
        }
        if (alpha >= beta) {
            bestFrom = tte.from;
            bestTo = tte.to;
            ph.pop();
            return tte.score;
        }
    }
    int maxEval = -10000;
    int localBestFrom = -1, localBestTo = -1;
    Bitboard pieces = state.isWhiteTurn() ? state.getWhite().orV(state.getKing()) : state.getBlack();
    for (int from = 0; from < 81; from++) {
        if (!pieces.get(from)) continue;
        Bitboard moves = state.getLegalMoves(from);
        for (int to : moves.toIndexVector()) {
            State child = state.move(from, to);
            int childFrom, childTo;
            int eval = -negaMax(child, depth - 1, -beta, -alpha, childFrom, childTo);
            if (eval > maxEval) {
                maxEval = eval;
                localBestFrom = from;
                localBestTo = to;
            }
            alpha = std::max(alpha, eval);
            if (alpha >= beta) break;
        }
        if (alpha >= beta) break;
    }
    ph.pop();
    NodeType nodeType = NodeType::EXACT;
    if (maxEval <= alpha) nodeType = NodeType::UPPERBOUND;
    else if (maxEval >= beta) nodeType = NodeType::LOWERBOUND;
    tt.store(hash, {depth, localBestFrom, localBestTo, maxEval, nodeType});
    bestFrom = localBestFrom;
    bestTo = localBestTo;
    return maxEval;
}

int Engine::negaMaxAspirationWindow(const State& state, int depth, int& bestFrom, int& bestTo) {
    int guess = state.evaluate();
    int window = 300;
    int alpha = guess - window;
    int beta  = guess + window;
    while (true) {
        int newFrom = -1, newTo = -1;
        int score = negaMax(state, depth, alpha, beta, newFrom, newTo);
        if (score <= alpha) alpha -= window;
        else if (score >= beta) beta += window;
        else {
            bestFrom = newFrom;
            bestTo = newTo;
            return score;
        }
        window += 200;
    }
}

void* Engine::iterativeDeepening(void* args) {
    ThreadArgs* tas = static_cast<ThreadArgs*>(args);
    Engine* engine = tas->engine;
    const State& initialState = *tas->state;
    int depth = 1;
    while (true) {
        int from = -1, to = -1;
        engine->negaMaxAspirationWindow(initialState, depth++, from, to);
        pthread_barrier_wait(&engine->barrier);
        if (pthread_self() == engine->searchers[tas->id]) BestMove::getInstance().propose(from, to);
        pthread_barrier_wait(&engine->barrier);
    }
    return nullptr;
}

void Engine::playTurn(const State &initialSatate, int seconds) {
    PositionHistory::getInstance().push(initialSatate.getZobristHash());
    pthread_barrier_init(&barrier, NULL, 3);
    ThreadArgs* tas = new ThreadArgs[3];
    for (int t = 0; t < 3; t++) {
        tas[t].id = t;
        tas[t].state = &initialSatate;
        tas[t].engine = this;
        pthread_create(&searchers[t], NULL, Engine::iterativeDeepening, &tas[t]);
    }
    sleep(seconds - 2);
    for (int t = 0; t < 3; t++) pthread_cancel(searchers[t]);
    delete[] tas;
    pthread_barrier_destroy(&barrier);
    int from, to;
    BestMove::getInstance().play(from, to);
    PositionHistory::getInstance().push(initialSatate.move(from, to).getZobristHash());
}

void Engine::end() {
    close(sd);
}

void Engine::go(std::string color, int seconds, std::string ip_referee) {
    /*this->start(color, ip_referee);
    while (true) {
        const int bufferSize = 4096;
        char buffer[bufferSize];
        std::string json;
        ssize_t bytesRead;
        while ((bytesRead = read(sd, buffer, bufferSize - 1)) > 0) {
            buffer[bytesRead] = '\0';
            json += buffer;
            if (json.back() == '}') break;
        }

        


        //leggo la stringa
        //controllo se indica il fine partita e nel caso ritorno
        //se no interpreto la stringa come un nuovo initial state
        if (...) this->end();
        State initialState; //ci dovrebbe essere un ascolto //gioca color chiaramente
        this->playTurn(initialState, seconds);
        //send(sd, from.c_str(), from.size(), 0);
        //send(sd, to.c_str(), to.size(), 0);
        //std::cout << "Mossa scelta: da " << from << " a " << to << std::endl;
    }*/
}
