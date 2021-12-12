# cpp-iz-teorije-v-prakso
Napredna delavnica: C++ iz teorije v prakso

C++ program solving Peg solitaire game
========================================

About Peg Solitaire.
----------------------
Peg solitaire (or Solo Noble) is a single player board game involving movement of pegs on a board with holes. In the United Kingdom the game is referred to as Solitaire while the card games are called Patience. The game is also known as Brainvita (especially in India). The first evidence of the game can be traced back to 1697.  More: http://en.wikipedia.org/wiki/Peg_solitaire

Game rules.
--------------------
The rules of Peg solitaire are similar to Checkers, in that a peg can jump over another peg as long as there is an empty hole on the other side.  Once a peg has been jumped over, it is removed from the board.  

The game starts with all the holes on the board filled with a peg except the hole in the center, which is empty. The first move begins with a peg jumping over another peg into an empty hole. A peg can jump left, right, up or down but cannot jump diagonally.  A peg can only jump one peg at a time, but as with Checkers, a peg can make a series of multiple jumps if there are open holes. Play continues with a peg jumping over another peg into an empty hole until there are no possible moves left.   

Winning the game.
----------------------
The game is won when all of the pegs are removed except one (the position of the last peg can be on arbitrary field). 
More: http://www.ehow.com/list_6181682_peg-solitaire-rules.html

Problem complexity
----------------------
For the version with 33 holes and 32 pegs (The English Board) of Peg Solitaire there are 577,116,156,815,309,849,672 different game sequences (577+ quintillion possible games), with 40,861,647,040,079,968 solutions. (Source: http://www.durangobill.com/Peg33.html).

An algorithm which blindly checks all the possibilities will not work quickly, and even with a very fast machine it would take years to solve.

```
  Start solving:
  111
  111
1111111
1110111
1111111
  111
  111
---------------
  111
  111
1111111
1110111
1111111
  111
  111

  111
  111
1111111
1111001
1111111
  111
  111

[ ... omitted ...]

  000
  000
0000000
0000000
0001000
  010
  000

  000
  000
0000000
0000000
0000000
  000
  010
```
Screenshots from the program and one of a solution:


Other useful links:
---------------------
Online game: http://www.coolmath-games.com/0-pegsolitaire/ (can be used for testing the solution)
How to solve the game YouTUbe video: https://www.youtube.com/watch?v=33UgfA3jt7s

Solution algorithm.
----------------------
We can describe all of the possible board games with a graph, where each node is a game state and each edge is a move. Obviously, as stated above, this graph is huge, and we cannot afford to check all of the possible variations of the game.

The most basic approach would be a simple backtracking algorithm (basically a depth-first search on the graph). This is, in essence, still a brute-force solution, but luckily there are enough possible solutions that it is possible to find one in reasonable time this way. The idea is to start at the first node, then find the first possible move and advance, taking a step back if all possibilities were exhausted. Since in this approach, we expect to come accross the same board state at many different nodes, the algorithm can be improved by noting when a board state is bad, so we can easily identify one when we come upon it the next time, skipping analysing that sub-tree altogether.

Further possible optimizations could include some fast way of determining obviously non-winnable boards and skipping them, as well as some heuristics to guess the most-likely-to-be-best move on each step. Both of these would require additional calculations on each step, so I'm not sure whether they would improve performance, and I didn't test them.

Implementation.
----------------------
The main library is coded in PegSolitaire.hpp and PegSolitaire.cpp. Generally, I would rather split this into multiple sources (and commits), but decided to keep this structure for such a small project. In addition, there's a test.cpp where some high level tests are performed. In production code, I'd include more thorough unit tests and use a dedicated framework like GoogleTest.

The library includes three main classes.

PegBoard contains the board and includes the logic for checking and making moves, as well as printing the board state. The class keeps the board as a vector of vectors of pieces, and separately keeps a flattened version of the board, which is a bitfield stored in a uint64_t integer. The latter can be quickly uodated during each move and is very handy as a unique identifier for hashing and indexing when saving the board as unwinnable.

BoardSaver class is just a structure that saves a given board, and provides information whether a given board has been saved. The default approach here is to use a hashmap (std::unordered_set), but there was an alternative that I wanted to test which is a bitfield that can be directly addressed by the flattened board integer, removing the need to perform any hashing. Since this method requires lots of memory (1 GB for the english board), it is not really an example of production-friendly code, but an interesting exercise in efficiency none the less. The three modes of operation for this class are None (never saving boards and always identifing them as not noted), Hashmap (utilizing the hashmap saving), and Bitfield (as described above). Note that the latter uses std::vector<bool>, whcih I expect the compiler to compress to a bitfield. Otherwise, we'd implement it as an array of integers, then setting and getting specific bits using bitshifted masks.

BoardSolver is class used for solving peg solitaire boards. It is initialized with a certain board and the desired board saving mode, and contains a method for solving the board.

The code contains various helper functions and enums as well.

Performance.
----------------------
For a very crude measurement of performance, compare_times.cpp was used to measure the running times. A better evaluation could be achieved by using a dedicated profiler.

Since for the bitfield board saving approach, the initial memory allocation and clearing time can be significant, the timing was done in two variants, one included both the BoardSolver initialization as well as its solve_board method, while the other included only the latter, e.g. skipping the potentially long memory allocation step.

The following times were measured on my machine (Intel i7-4790 CPU, 8 GB RAM, 64bit Windows 10, g++ v8.1.0 via MinGW-w64, compiler flags -O3 -march=native)

English board solving times
| BoardSolver initialization | No board saving | Hashmap | Bitfield |
| Not measured | 14.0 s | 46.9 ms | 15.6 ms |
| Measured | 13.6 s | 46.9 ms | 297 ms |

The bitfield approach shows a better performance in solving the algorithm, indicating that the hashing time is the bottleneck for the hashmap approach at this level. However, the bitfield approach takes much longer to also allocate the large amount of memory (1 GB). The latter time should probably be highly dependent on the OS memory managment, especially in cases where the OS can provide some pre-zeroed-out memory pages so the allocation is faster.

Code style.
----------------------
I went with the general style introduced in the main function of the project (2 space indenting, snake case names). Elsewhere, I tried to follow the Cosylab's guidelines (except for the m_ prefix for class members, as they look confusing with snake case names).