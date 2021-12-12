#include "PegSolitaire.hpp"
#include <iostream>
#include <sstream>
#include <stdexcept>

namespace ps
{
    
PegBoard get_english_board()
{
  std::string board_string(
  "  111  \n"
  "  111  \n"
  "1111111\n"
  "1110111\n"
  "1111111\n"
  "  111  \n"
  "  111  ");

  return PegBoard(board_string);
}
  
  
void print(const PegBoard& board)
{
  board.print();
}


void print_moves(const PegBoard& board_start, const std::vector<Move>& moves)
{
  PegBoard board(board_start);

  for (const auto& move: moves)
  {
    board.make_move(move);
    board.print();
  }
}


PegBoard::PegBoard(const std::string& board_string)
{
  // We generate the board from the input string. We do this by parsing it line
  // by line, and the lines char by char. An exception is thrown if an invalid
  // character appears.
  size_t row_index = 0;
  size_t flat_index = 0;
  std::stringstream ss(board_string);  
  std::string line;
  
  while(std::getline(ss, line))
  {
    pieces.emplace_back();
    flattened_pos.emplace_back();
    for (char c: line)
    {
      switch (c)
      {
      case ' ':
        pieces[row_index].push_back(Piece::None);
        // Since operations on None pieces are never translated to the
        // flattened board, these indexes can be set to any value
        flattened_pos[row_index].push_back(0);
        break;
        
      case '1':
        pieces[row_index].push_back(Piece::Peg);
        
        // Set the flattened board bit (only handle here, if hole leave 0)
        if (flat_index == 63)
        {
          throw std::runtime_error("Board cannot have more than 64 piece "
            "positions");
        }
        flattened_pos[row_index].push_back(flat_index);
        flattened_board = flattened_board | (uint64_t(1) << flat_index);
        
        flat_index++;
        break;
        
      case '0':
        pieces[row_index].push_back(Piece::Hole);
        flattened_pos[row_index].push_back(flat_index);
        flat_index++;
        break;
        
      default:
        throw std::runtime_error("Invalid character in board string");
      
      }
    }
    
    row_index++;
  }
  
  n_positions = flat_index;    
  n_rows = pieces.size();
  
  if (n_rows == 0)
  {
    throw std::runtime_error("Board of size 0");
  }
  
  n_cols = pieces[0].size();
  
  if (n_cols == 0 || n_positions == 0)
  {
    throw std::runtime_error("Board of size 0");
  }
  
  for (size_t i = 0; i < n_rows; ++i)
  {
    if (pieces[i].size() != n_cols)
    {
      throw std::runtime_error("All board rows must be of the same size");
    }
  }
}


void PegBoard::get_move_positions(const Move& move, Position& target,
  Position& middle)
{
  // We just increment or decrement the position dimensions, whether the
  // positions are inside or outside the board is left to be determined by the
  // caller
  target = move.pos;
  middle = move.pos;
  switch (move.dir)
  {
  case Direction::Up:
    target.row -= 2;
    middle.row -= 1;
    break;
    
  case Direction::Right:
    target.col += 2;
    middle.col += 1;
    break;
      
  case Direction::Down:
    target.row += 2;
    middle.row += 1;
    break;
      
  case Direction::Left:
    target.col -= 2;
    middle.col -= 1;
    break;
  }
}


bool PegBoard::make_move(const Move& move, bool reversed)
{
  // Find target and mid position and check if both source and target on board
  Position target, middle;
  get_move_positions(move, target, middle);
  
  if (!position_on_board(move.pos) || !position_on_board(target))
  {
    return false;
  }

  // Handle the reversed-ness
  Piece piece_target = reversed ? Piece::Peg : Piece::Hole;
  Piece piece_other = reversed ? Piece::Hole : Piece::Peg;

  // Check that all three positions are the required types
  if (get(move.pos) != piece_other ||  get(middle) != piece_other ||
    get(target) != piece_target)
  {
    return false;
  }

  // Now can make legal move
  set(target, piece_other);
  set(move.pos, piece_target);
  set(middle, piece_target);

  return true;
}


bool PegBoard::position_on_board(const Position& pos)
{
  return (pos.col >= 0) && (pos.col < static_cast<ptrdiff_t>(n_cols)) &&
    (pos.row >= 0) && (pos.row < static_cast<ptrdiff_t>(n_rows));
}


Piece PegBoard::get(const Position& pos)
{
  return pieces[pos.row][pos.col];
}


void PegBoard::set(const Position& pos, const Piece piece)
{
  // Set the piece on the uncompressed board
  pieces[pos.row][pos.col] = piece;
  
  // Set the piece on the flattened board
  size_t flattened_index = flattened_pos[pos.row][pos.col];
  uint64_t flattened_mask = uint64_t(1) << flattened_index;
  
  // If the piece is set to Peg, we set the corresponding bit to 1, else to 0
  if (piece == Piece::Peg)
  {
    flattened_board = flattened_board | flattened_mask;
  }
  else
  {
    flattened_board = flattened_board & ~flattened_mask;
  }
}


size_t PegBoard::count_pieces(const Piece piece) const
{
  size_t count = 0;
  
  for (auto& row: pieces)
  {
    for (auto& col: row)
    {
      if (col == piece) count++;  
    }
  }
  
  return count;
}


size_t PegBoard::get_n_positions() const
{
  return n_positions;
}
    
    
uint64_t PegBoard::get_flattened_value() const
{
  return flattened_board;
}


size_t PegBoard::get_n_rows() const
{
  return n_rows;
}
    
    
size_t PegBoard::get_n_cols() const
{
  return n_cols;
}
    

void PegBoard::print() const
{
  for (auto& row: pieces)
  {
    for (auto& col: row)
    {
      char c = '0';
      switch (col)
      {
        case Piece::Hole:
        c = '0';
        break;
        case Piece::Peg:
        c = '1';
        break;
        case Piece::None:
        c = ' ';
        break;
      }
      std::cout << c;
    }
    std::cout << '\n';
  }
  std::cout << std::endl;    
}
  
  
BoardSolver::BoardSolver(const PegBoard& board, const SolverBoardSaving mode)
: input_board(board), board(board), board_saver(board.get_n_positions(), mode)
{
  // The maximum amount of moves is n_pegs-1, so we just set the move vector
  // size once
  size_t n_pegs = board.count_pieces(Piece::Peg);
  moves.resize(n_pegs-1);  
}
  
  
const PegBoard& BoardSolver::get_current_board() {return board;}


const std::vector<Move>& BoardSolver::get_moves()
{
  return moves;
}


bool BoardSolver::find_next_move(const Move& last_tried, Move& found_move)
{
  found_move = last_tried;
  
  if (found_move.dir != Direction::Left)
  {
    // We just increment the enum via some casting (since we confirmed not max)
    found_move.dir =
      static_cast<Direction>(static_cast<uint8_t>(found_move.dir)+1);
    return true;
  }
  
  // Directions exhausted, change position, and return false if all exhausted
  found_move.dir = Direction::Up;
  if (found_move.pos.col == static_cast<ptrdiff_t>(board.get_n_cols() - 1))
  {
    if (found_move.pos.row == static_cast<ptrdiff_t>(board.get_n_rows() - 1))
    {
      return false;
    }
    
    found_move.pos.col = 0;
    found_move.pos.row++;
  }
  else
  {
    found_move.pos.col++;
  }
  
  return true;
}
 
 
bool BoardSolver::solve_board() {
  // If the board was already solved, just return the saved value (state and
  // moves are already final)
  if (solve_board_completed)
  {
    return solve_board_status;
  }
  solve_board_completed = true;
  
  // The solving loop, a backtracking algorithm with possible bad board saving
  while (true)
  {
    bool success = board.make_move(moves[last_move_index]);
    
    // If resulting board is unwinnable we just revert this step and note no
    // success. We don't have to check for retval of the make move here.
    if (board_saver.checkBoardSaved(board.get_flattened_value()))
    {
      board.make_move(moves[last_move_index], true);
      success = false;
    }
    
    if (success)
    {
      // If this was the last move, we have solved the board
      if (last_move_index == moves.size() - 1)
      {
        return solve_board_status = true;
      }
            
      // If succeeded, we go to the next move in the vector, and initialize it
      // to default (first) move
      last_move_index++;
      moves[last_move_index] = {{0, 0}, Direction::Up};
    }
    else
    {
      // Move failed, try next move. If all moves for current board state
      // exhausted, return to previous state and decrement the counter. Needs
      // a loop in case of nested fails
      while (true)
      {
        Move next;
        if (find_next_move(moves[last_move_index], next)) 
        {
          moves[last_move_index] = next;
          break;
        }
        else
        {
          // All moves exhausted. If we are at starting pos, the algorithm has
          // failed.
          if (last_move_index == 0)
          {
            return solve_board_status = false;
          }
          
          // Save board as unwinnable and go to previous move
          board_saver.saveBoard(board.get_flattened_value());
          board.make_move(moves[last_move_index - 1], true);
          last_move_index--; 
        }
      }
    } // Quite some nesting, could probably use a refactor :)
  }
} 


BoardSaver::BoardSaver(size_t capacity, const SolverBoardSaving mode_in)
: mode(mode_in)
{
  // Capacity means number of positions, so we have to allocate 2^capacity bits
  // Generally, we expect std::vector<bool> to implement a bitfield, requiring
  // 1 GB of storage for the english board, and not 8 GB as would a char array.
  // This is still a lot of memory, so in case this alloc fails, we default to
  // the hashmap mode.
  if (mode == SolverBoardSaving::Bitfield)
  {
    try
    {
      boards_bitfield.resize(uint64_t(1) << capacity);
    }
    catch (const std::bad_alloc& e)
    {
      std::cout << "Memory allocation for the Bitfield BoardSaver failed. Using"
        " the Hashmap mode instead" << std::endl;
      mode = SolverBoardSaving::Hashmap;
    }
  }
}
  
  
void BoardSaver::saveBoard(const uint64_t flattened_board)
{
  if (mode == SolverBoardSaving::Hashmap)
  {
    boards_hashmap.insert(flattened_board);
  }
  else if (mode == SolverBoardSaving::Bitfield)
  {
    boards_bitfield[flattened_board] = true;
  }
}
  
  
bool BoardSaver::checkBoardSaved(const uint64_t flattened_board)
{
  if (mode == SolverBoardSaving::Hashmap)
  {
    return boards_hashmap.count(flattened_board);
  }
  else if (mode == SolverBoardSaving::Bitfield)
  {
    return boards_bitfield[flattened_board];
  }
  
  // When mode is None we always return false (and the boards are not saved)
  return false;
}
  
} // namespace ps
