#include "Engine.hpp"

void Engine::start(std::string color, std::string ip_referee)
{
    struct sockaddr_in s{};
    s.sin_family = AF_INET;
    for (char &c : color)
        c = std::toupper(static_cast<unsigned char>(c));
    s.sin_port = htons(color == "WHITE" ? whitePort : blackPort);
    if (inet_pton(AF_INET, ip_referee.c_str(), &s.sin_addr) <= 0)
        exit(EXIT_FAILURE);
    sd = socket(AF_INET, SOCK_STREAM, 0);
    if (sd < 0)
        exit(EXIT_FAILURE);
    if (connect(sd, (struct sockaddr *)&s, sizeof(s)) < 0)
    {
        close(sd);
        exit(EXIT_FAILURE);
    }
    uint32_t length = htonl(name.size());
    ssize_t sent = write(sd, &length, sizeof(length));
    if (sent != sizeof(length))
    {
        close(sd);
        exit(EXIT_FAILURE);
    }
    sent = write(sd, name.c_str(), name.size());
    if (sent != (ssize_t)name.size())
    {
        close(sd);
        exit(EXIT_FAILURE);
    }
}

int Engine::negaMax(const State &state, int depth, int alpha, int beta, int &bestFrom, int &bestTo)
{
    if (depth == 0)
        return state.evaluate();
    if (state.isWhiteWinner())
        return 10000;
    if (state.isBlackWinner())
        return -10000;
    PositionHistory &ph = PositionHistory::getInstance();
    uint64_t hash = state.getZobristHash();
    if (ph.contains(hash))
    {
        bestFrom = -1;
        bestTo = -1;
        return 0;
    }
    ph.push(hash);
    TTEntry tte;
    TranspositionTable &tt = TranspositionTable::getInstance();
    if (tt.lookup(hash, tte) && tte.depth >= depth)
    {
        switch (tte.type)
        {
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
        if (alpha >= beta)
        {
            bestFrom = tte.from;
            bestTo = tte.to;
            ph.pop();
            return tte.score;
        }
    }
    int maxEval = -10000;
    int localBestFrom = -1, localBestTo = -1;
    Bitboard pieces = state.isWhiteTurn() ? state.getWhite().orV(state.getKing()) : state.getBlack();
    for (int from = 0; from < 81; from++)
    {
        if (!pieces.get(from))
            continue;
        //Bitboard moves = state.getLegalMoves(from);
        for (int to : state.getLegalMoves(from))
        {
            State child = state.move(from, to);
            int childFrom, childTo;
            int eval = -negaMax(child, depth - 1, -beta, -alpha, childFrom, childTo);
            if (eval > maxEval)
            {
                maxEval = eval;
                localBestFrom = from;
                localBestTo = to;
            }
            alpha = std::max(alpha, eval);
            if (alpha >= beta)
                break;
        }
        if (alpha >= beta)
            break;
    }
    ph.pop();
    NodeType nodeType = NodeType::EXACT;
    if (maxEval <= alpha)
        nodeType = NodeType::UPPERBOUND;
    else if (maxEval >= beta)
        nodeType = NodeType::LOWERBOUND;
    tt.store(hash, {depth, localBestFrom, localBestTo, maxEval, nodeType});
    bestFrom = localBestFrom;
    bestTo = localBestTo;
    return maxEval;
}

int Engine::negaMaxAspirationWindow(const State &state, int depth, int &bestFrom, int &bestTo)
{
    int guess = state.evaluate();
    int window = 300;
    int alpha = guess - window;
    int beta = guess + window;
    while (true)
    {
        int newFrom = -1, newTo = -1;
        int score = negaMax(state, depth, alpha, beta, newFrom, newTo);
        if (score <= alpha)
            alpha -= window;
        else if (score >= beta)
            beta += window;
        else
        {
            bestFrom = newFrom;
            bestTo = newTo;
            return score;
        }
        window += 200;
    }
}

void *Engine::iterativeDeepening(void *args)
{
    ThreadArgs *tas = static_cast<ThreadArgs *>(args);
    Engine *engine = tas->engine;
    const State &initialState = *tas->state;
    int depth = 1;
    while (true)
    {
        int from = -1, to = -1;
        engine->negaMaxAspirationWindow(initialState, depth++, from, to);
        pthread_barrier_wait(&engine->barrier);
        if (pthread_self() == engine->searchers[tas->id])
            BestMove::getInstance().propose(from, to);
        pthread_barrier_wait(&engine->barrier);
    }
    return nullptr;
}

void Engine::playTurn(const State &initialSatate, int seconds)
{
    PositionHistory::getInstance().push(initialSatate.getZobristHash());
    pthread_barrier_init(&barrier, NULL, 3);
    ThreadArgs *tas = new ThreadArgs[3];
    for (int t = 0; t < 3; t++)
    {
        tas[t].id = t;
        tas[t].state = &initialSatate;
        tas[t].engine = this;
        pthread_create(&searchers[t], NULL, Engine::iterativeDeepening, &tas[t]);
    }
    sleep(seconds - 2);
    for (int t = 0; t < 3; t++)
        pthread_cancel(searchers[t]);
    delete[] tas;
    pthread_barrier_destroy(&barrier);
    int from, to;
    BestMove::getInstance().play(from, to);
    PositionHistory::getInstance().push(initialSatate.move(from, to).getZobristHash());
}

std::string Engine::indexToCoordinate(int index)
{
    if (index < 0 || index >= 81)
        return "Invalid";

    int row = index / 9; // da 0 a 8
    int col = index % 9; // da 0 a 8

    char colChar = 'A' + col; // 'A'..'I'
    int rowNum = row + 1;     // 1..9

    return std::string(1, colChar) + std::to_string(rowNum);
}

void Engine::end()
{
    close(sd);
}

void Engine::go(std::string color, int seconds, std::string ip_referee)
{
    this->start(color, ip_referee);
    const int bufferSize = 4096;
    char buffer[bufferSize];
    std::string json;
    ssize_t bytesRead;
    while (bytesRead = read(sd, buffer, 1) > 0)
    {
        bytesRead = read(sd, buffer, 1);
        bytesRead = read(sd, buffer, 1);
        bytesRead = read(sd, buffer, 1);
        // this read is needed to remove stuff from the buffer

        bytesRead = read(sd, buffer, bufferSize - 1); // this is the read that actually reads the json

        buffer[bytesRead] = '\0';
        json = buffer;
        printf("JSON from Server: %s\n", json.c_str());

        uint64_t whiteBitmap_low = 0ULL;
        uint64_t whiteBitmap_high = 0ULL;
        uint64_t blackBitmap_low = 0ULL;
        uint64_t blackBitmap_high = 0ULL;
        uint64_t kingBitmap_low = 0ULL;
        uint64_t kingBitmap_high = 0ULL;
        std::string turn = "";
        bool blackTurn = false;
        bool whiteTurn = false;
        bool blackWin = false;
        bool whiteWin = false;
        bool draw = false;

        // PARSING JSON
        size_t boardPos = json.find("\"board\":");
        if (boardPos == std::string::npos)
            return;

        // Trova inizio e fine board
        size_t startBoard = json.find("[[", boardPos);
        size_t endBoard = json.find("]]", startBoard);
        if (startBoard == std::string::npos || endBoard == std::string::npos)
            return;

        // Puntatore che scorre le stringhe interne
        size_t pos = startBoard + 2;

        int bitIndex = 0; // posizione bit da 0 a 80

        while (pos < endBoard)
        {
            // Cerca inizio stringa cella: "
            size_t startQuote = json.find('\"', pos);
            if (startQuote == std::string::npos || startQuote > endBoard)
                break;

            // Cerca fine stringa cella: "
            size_t endQuote = json.find('\"', startQuote + 1);
            if (endQuote == std::string::npos || endQuote > endBoard)
                break;

            std::string cell = json.substr(startQuote + 1, endQuote - startQuote - 1);

            // Aggiorna bit nelle bitmap
            uint64_t mask = 1ULL << (bitIndex % 64);

            if (bitIndex < 64)
            {
                if (cell == "WHITE")
                    whiteBitmap_low |= mask;
                else if (cell == "BLACK")
                    blackBitmap_low |= mask;
                else if (cell == "KING")
                    kingBitmap_low |= mask;
                // THRONE o EMPTY → nessun bit set
            }
            else
            {
                if (cell == "WHITE")
                    whiteBitmap_high |= mask;
                else if (cell == "BLACK")
                    blackBitmap_high |= mask;
                else if (cell == "KING")
                    kingBitmap_high |= mask;
            }

            bitIndex++;
            pos = endQuote + 1;
        }

        // Cerca turn
        size_t turnPos = json.find("\"turn\":\"");
        if (turnPos != std::string::npos)
        {
            turnPos += 8;
            size_t turnEnd = json.find('\"', turnPos);
            if (turnEnd != std::string::npos)
            {
                turn = json.substr(turnPos, turnEnd - turnPos);
            }
            if (turn == "BLACK")
                blackTurn = true;
            if (turn == "WHITE")
                whiteTurn = true;
            if (turn == "BLACKWIN")
                blackWin = true;
            if (turn == "WHITEWIN")
                whiteWin = true;
            if (turn == "DRAW")
                draw = true;
        }

        // CREATE BITBOARDS
        Bitboard white(whiteBitmap_low, whiteBitmap_high);
        Bitboard black(blackBitmap_low, blackBitmap_high);
        Bitboard king(kingBitmap_low, kingBitmap_high);

        // CREATE INITIAL STATE
        State initialState(white, black, king, whiteTurn, whiteWin, blackWin, 0);

        // FIND MOVE
        this->playTurn(initialState, seconds);

        int from, to;
        BestMove::getInstance().play(from, to);
        initialState.move(from, to);

        // SEND MOVE
        char moveString[100];
        //"Turn: " + this.turn + " " + "Pawn from " + from + " to " + to;
        sprintf(moveString, "{\"from\":\"%s\",\"to\":\"%s\",\"turn\":\"%s\"}\n", this->indexToCoordinate(from).c_str(), this->indexToCoordinate(to).c_str(), turn.c_str());
        int stringLength = htonl(strlen(moveString));
        write(sd, &stringLength, sizeof(stringLength));
        write(sd, moveString, sizeof(char) * strlen(moveString));
    }
    this->end();
}
