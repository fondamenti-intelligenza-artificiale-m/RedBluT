# *RedBluT* Documentation

This documentation outlines the technical and design aspects of the *RedBluT* game engine, explaining key implementation choices and technologies. It serves both as a reference and a guide for anyone interested in understanding the system's inner workings.

## 📚 Table of Contents

- [🧵 Use of Parallelism](#use-of-parallelism)  
- [🧠 State Space Representation](#state-space-representation)  
- [🎯 Move Generation](#move-generation)  
- [🔍 Search Strategies](#search-strategies)  
- [⚖️ Evaluation and Heuristics](#evaluation-and-heuristics)

## Use of Parallelism

### Lazy SMP (Lazy Symmetric MultiProcessing)

The engine adopts a model known as **Lazy SMP**, which allows threads to work independently on different parts of the move tree, while sharing a central transposition table.

In the current configuration, with **4 available cores**, threads are assigned as follows:  
- 🕒 **1 thread** is dedicated exclusively to time management.  
    It signals when the search must stop in order to return a valid move within the time limit.  
- 🚀 **3 threads** handle the actual search process.  
    Each one works on one of the **top three moves**, ordered according to the heuristic function.

### LMR (Late Move Reductions)

Starting from the **fourth move** in heuristic order, the technique called **Late Move Reductions (LMR)** 😴 is applied.  
It reduces the search depth for moves considered less promising (i.e., those appearing later in the sorted list).  
The idea is that if a move is not among the first considered, it is probably less interesting, and can be explored at a reduced depth.  
Only if it still returns a value above a certain threshold, known as **fail-high** 📈, will it be re-searched at full depth.

## State Space Representation

The entire game state is represented in a compact and efficient way using **bitboards**, a 81-bit bitboard for each piece type. 🧠  
Each index corresponds to a square on the 9x9 board, and holds a 0 if the piece is absent, 1 if present.

```python
00  01  02  03  04  05  06  07  08
09  10  11  12  13  14  15  16  17
18  19  20  21  22  23  24  25  26
27  28  29  30  31  32  33  34  35
36  37  38  39  40  41  42  43  44
45  46  47  48  49  50  51  52  53
54  55  56  57  58  59  60  61  62
63  64  65  66  67  68  69  70  71
72  73  74  75  76  77  78  79  80
```

Specifically, the state structure includes:

- 🟤⚪ Two main bitboards – one for black pieces, one for white pieces.
- 👑 King bitboard – with a single active bit, since there is always exactly one king.
- 🔄 Turn bit – indicates which player is to move: `0` → white, `1` → black

In total, 244 bits are used to represent a position.  
This representation is highly efficient because bitwise operations allow for fast move generation, threat detection, and piece interactions.

## Move Generation

Move generation in a board game modeled with **bitboards** can be a complex yet highly optimizable process.  
A [dedicated subproject](../precompute) describes the details of the **move precomputation** process.

### Loneliness Bitboard

At initialization, we precompute **loneliness bitboard**.  
These bitboard define all possible moves for a piece of a specific side starting from a specific square.  
**Not all squares can be occupied by both sides**; for instance, camps and the central castle are restricted.  
Naturally, it makes no sense to generate a loneliness bitboard from a square that is unreachable for that side.  
Thus, the total number of movement bitboard is:  
(81 - 16) + (81 - 1) = **145**.

### Snag Bitboard

Other pieces on the board may block movement. For this reason, we use **snag bitboard**, small 16-bit bitboards indicating potential obstacles.  
Each piece can have at most 16 legal moves, so the snag bitboard only needs to represent 16 bits.  
These bitboard are also **precomputed**.

While in theory there could be 145 × 2¹⁶ = 9,502,720 possible combinations, the actual number is lower, **1,161,600**, thanks to forbidden squares.  
We do not consider pieces as snags if they are aligned on the same row or column but placed on a square not reachable in the corresponding loneliness bitboard.

### Sliding Pieces

Tablut pieces are **sliding pieces**, like queens, rooks, and bishops in chess, as opposed to draughts/checkers pieces.  
If not handled properly, a move might incorrectly “jump” over other pieces:

```python
moves_bitboard = loneliness_bitboard & ~snag_bitboard  # Mistake: unfortunately, the pieces are not equipped with Red Bull
```

### Move Lookup

We avoid computing legal moves one by one in every direction during gameplay.
Instead, moves are not generated, they are simply looked up.
For each side and each board position, all valid snag bitboard are precomputed.
A very compact index is used to access the lookup table:

```python
(color << 23) | (position << 16) | snag_bitboard
```

## Search Strategies

This section describes the techniques used to explore the move tree efficiently, combining classic pruning strategies with advanced optimizations inspired by modern engines.

### Iterative Deepening

The engine performs **Iterative Deepening Depth-First Search** to progressively deepen the search level, starting from shallow depths and reusing previous results to guide deeper iterations.  
This allows the use of early cutoffs, better move ordering, and timely interruption handling.

### Negamax

At the core of the search logic lies the **Negamax algorithm**, a simplified variant of Minimax that leverages the symmetry of zero-sum games.  
Rather than distinguishing between maximizing and minimizing players, Negamax assumes both players try to maximize their own score, and simply **negates the evaluation** when switching turns.

To improve efficiency, the algorithm is combined with **Alpha-Beta pruning**, which avoids exploring branches that cannot possibly influence the final decision:
- **Alpha (α)** is the best score the maximizing player is guaranteed so far.
- **Beta (β)** is the best score the minimizing player can force.

If the current evaluation exceeds β or falls below α, further exploration is cut off (pruned).

### Aspiration Window

Each search iteration begins with a narrow **aspiration window** around the previous score, typically enhancing pruning efficiency.  
If the actual score falls outside the expected range, the window is re-expanded, and the search is repeated with wider bounds.  
There are two possible outcomes that can trigger this re-expansion:
- 📈 **Fail-high**
- 📉 **Fail-low**

### Transposition Table

The state space is a graph, since identical positions can be reached through different sequences of moves.  
The **Transposition Table** allows the engine to treat the state space as a tree.  
It stores previously evaluated positions using a hash-based index, in particular, each entry may contain:
1. a depth value (used to decide whether to overwrite),
1. a best move (for move ordering),
1. a score,
1. a flag indicating whether the value is:
  - **exact**
  - **lower bound** (fail-high)
  - **upper bound** (fail-low)

### Zobrist Hashing

**Zobrist hashing** is used to efficiently generate a unique identifier for each game position, which serves as a key in the Transposition Table.  
Each possible game feature, such as a specific piece on a specific square or the current player to move, is assigned a random 64-bit number.  
The overall hash is computed by XOR-ing the values of all active features in the current position.  
Because only a few features change with each move, the hash can be updated incrementally, making it both fast and practical.  
The probability of a hash collision depends on the number of explored positions `m` and the hash size `b = 64`.  
The following approximation clearly demonstrates that the probability of a collision is negligible.

$$
P_{\text{collision}} \approx 1 - \exp\left( -\frac{m^2}{2 \cdot 2^b} \right)
$$

### Repetition Stack

To handle **draw by repetition**, the engine maintains a **Repetition Stack**: a history of Zobrist hashes for previously visited positions.  
If the current hash is already in the stack, the position is immediately evaluated as a draw (score `0`) and not searched further.

## Evaluation and Heuristics

Work in Progress... Luca and Francesco are cooking.
