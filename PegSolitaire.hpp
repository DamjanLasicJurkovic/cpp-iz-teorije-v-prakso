// Usually I use #pragma once, but it's not in the standard so using guards here
#ifndef PegSolitaire
#define PegSolitaire

#include <vector>
#include <cstdint>
#include <unordered_set>
#include <string>

namespace ps
{

// Piece of the board, None denotes a position not part of the board  
enum class Piece: uint8_t
{Peg, Hole, None};


// Direction of a move on the board
enum class Direction: uint8_t
{Up, Right, Down, Left};


// Position on the board
struct Position
{
  ptrdiff_t row;
  ptrdiff_t col;
};


// A struct describing a board move, from position to a direction
struct Move
{
  Position pos;
  Direction dir;
};
  
  
// The class that contains the Peg Solitaire board and handles moves on said
// board. The class also keeps a flattened version of the board, which is kept
// up-to-date whenever a move is made, and which is very handy for hashing and
// indexing. 
class PegBoard
{
public:
  PegBoard() = delete;
  // The board is constructed from a string representation of the board, where
  // spaces are unused positions, 1 is a peg and 0 is a hole, with newline
  // characters separating the rows. The board size is limited to a total of 64
  // pegs and/or holes, due to the flattened board implementation. The
  // constructor throws a runtime error is the input board is not valid. The
  // boards dimensions etc cannot be modified after the board is created.
  // Also, a std::string_view arg would be better suited if C++17 available.
  PegBoard(const std::string& board_string);
  
  // Gets the summed number of holes and pegs (but not Nones) on the board
  size_t get_n_positions() const;
  
  size_t get_n_rows() const;
  
  size_t get_n_cols() const;
  
  // The flattened value is a bitfield stored in a 64 bit integer. It represents
  // all the holes and peg positions on the board, if scanned from left to right
  // and up to down, starting in the bitfield with the least significant bit.
  // The pegs have a value of 1 and the holes of 0. The unused bits are always
  // 0, so the flattened value can be used to index a bitfield of size
  // 2^n_positions.
  uint64_t get_flattened_value() const;
  
  // Prints the board to std::cout
  void print() const;
  
  // Tries to make a move. If move is legal, updates the board state and returns
  // true. Else, returns false. It can also be used to make a reversed move,
  // which doest the same kind of checking and can be used to 'undo' a valid
  // move, bringing the board to the previous state.
  bool make_move(const Move& move, bool reversed = false);
      
  size_t count_pieces(const Piece piece) const;
  
private:
  size_t n_positions = 0;
  
  size_t n_rows = 0;
  
  size_t n_cols = 0;
  
  // The board state, n_rows of vectors of pieces of n_cols size
  std::vector<std::vector<Piece>> pieces;
  
  // As the vector of pieces, only including the index of the position in the
  // flattened_baord bitfield. Used to map any position to the bitfield pos.
  // This is handy because it makes setting the flattened board with a single
  // lookup rather than having to scan the board each time to determine the
  // position in the bitfield.
  std::vector<std::vector<size_t>> flattened_pos;
  
  // See comment for get_flattened_value
  uint64_t flattened_board = 0;
  
  // Checks whether a position is a legal position on board
  bool position_on_board(const Position& pos);
  
  Piece get(const Position& pos);
  
  void set(const Position& pos, const Piece piece);
  
  // Computes the position of target and middle locations for a move, but
  // doesn't check for validity, usually a call to position_on_board is done
  // after.
  void get_move_positions(const Move& move, Position& target, Position& middle);
};  
  
  
// Generates an instance of the english board  
PegBoard get_english_board();


// Prints a board to std::cout
void print(const PegBoard& board);
 
 
// Prints a starting boards game evolution through the supplied moves
void print_moves(const PegBoard& board_start, const std::vector<Move>& moves);


// How to save unwinnable boards during the backtracking algorithm.
enum class SolverBoardSaving: uint8_t
{
  // No saving, all states are considered winnable
  None,
  
  // Saves the unwinnable boards into a hashmap (std::unordered_set)
  Hashmap,
  
  // Saves the unwinnable boards in a bitfield (std::vector<bool>). Note that
  // for the english board, this would allocate at least a 1 GB array to store
  // all flattened board states (33 bits is 8 million values reduced to 1 GB
  // since using a bitfield). The advantage of this approach is to avoid
  // hashing, since we can use the flattened boards as indexes directly.
  Bitfield
};


// A class which saves the board states and provides info whether a board has
// been saved. The states here are the ones flattened to uint64_t.
class BoardSaver
{
public:
  BoardSaver() = delete;
  
  // The capacity argument is only needed for the Bitfield mode, as the
  // bitfield is in this case allocated in the constructor. 
  BoardSaver(size_t capacity,
    const SolverBoardSaving mode_in = SolverBoardSaving::Hashmap);
  
  void saveBoard(const uint64_t flattened_board);
  
  // Will return true if the board has been saved and is thus unwinnable
  bool checkBoardSaved(const uint64_t flattened_board);

private:
  SolverBoardSaving mode;

  // The hashmap used to save the boards in case of the Hashmap mode.
  std::unordered_set<uint64_t> boards_hashmap;

  // The bitfield which saves the boards in case of the Bitfield mode.
  // Generally we trust the compiler to optimise this to bitfield, otherwise
  // we'd have to manually calloc/new some memory and use bitshifts for
  // indexing. This member is left at size 0 in any other modes.
  std::vector<bool> boards_bitfield;
};
 
 
// A class which is able to solve provided Peg Solitaire boards. It does so
// using a simple backtracking algorithm, augmented by saving the determined
// unwinnable boards if desired. 
class BoardSolver
{ 
public:  
  BoardSolver() = delete;
  
  BoardSolver(const PegBoard& board,
      const SolverBoardSaving mode = SolverBoardSaving::Hashmap);
  
  // Tries to solve the board, and returns true if succeeds.
  bool solve_board();
  
  // Returns the reference to the vector of moves taken. The vector's size is
  // equal to the maximum number of needed moves for the board, which is the
  // number of pegs - 1.
  const std::vector<Move>& get_moves();
  
  // Returns the reference to the current board, which is a solved version if
  // solve_board succeeded, a last tried version if it failed, and the input
  // version if it hasn't been run yet.
  const PegBoard& get_current_board();

private:
  std::vector<Move> moves;
  
  // Would rather use std::optional as return here, but sometimes we don't
  // want to force C++17. This function finds the next move by scanning, in
  // order, the direction, column and row. Note that it will find any move,
  // and a PegBoard class is then responsible for checking whether the move
  // is legal.
  bool find_next_move(const Move& last_tried, Move& found_move);
  
  size_t last_move_index = 0;

  const PegBoard input_board;
  
  PegBoard board;
  
  BoardSaver board_saver;
  
  // Whether solve_board was already called
  bool solve_board_completed = false;
  
  // The status of the finished solve_board
  bool solve_board_status = false;
};

} // namespace ps

#endif
