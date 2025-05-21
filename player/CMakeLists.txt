cmake_minimum_required(VERSION 3.10)
project(player)

# Usa C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Aggiungi tutti i file sorgente
add_executable(player
    src/BestMove.cpp
    src/PositionHistory.cpp
    src/TranspositionTable.cpp
    src/Bitboard.cpp
    src/State.cpp
    src/Engine.cpp
    src/main.cpp
)

# Se hai header, li puoi includere con:
target_include_directories(player PRIVATE src)
