#include "PegSolitaire.hpp"

#include <iostream>
#include <vector>
#include <string>

// These are just some high-level tests. More detailed unit tests would be used
// in production code. Also I'd use GoogleTest, but won't include it for such a
// small excercise project.

bool print_solve_board(const std::string& board_string);

int main()
{
  std::vector<std::string> solvable_boards = {    
    "11010101010",
    
    "11010101010\n"
    "          1\n"
    "01010101010",
    
    "0010\n"
    "0010\n"
    "0000\n"
    "0000",
    
    // English board
    "  111  \n"
    "  111  \n"
    "1111111\n"
    "1110111\n"
    "1111111\n"
    "  111  \n"
    "  111  ",
    
    // Expanded assymetric
    "  111   \n"
    "  111   \n"
    "  111   \n"
    "11111111\n"
    "11101111\n"
    "11111111\n"
    "  111   \n"
    "  111   "
    };
   
  std::vector<std::string> not_solvable_boards = {
    "1111\n"
    "1111\n"
    "1111\n"
    "1111",
    
    "01010101010",
    
    "11010101010\n"
    "          0\n"
    "01010101010"
    };
  
  std::vector<std::string> invalid_boards = {
    // Invalid chars
    "123",
    
    // Empty
    "   ",
    
    // Invalid chars
    "  123  ",
    
    // Empty
    "\n \n\n",
    
    // Row sizes not equal
    "111\n101\n11",
    
    // Row sizes not equal
    "1111\n"
    "111\n"
    "1111\n"
    "1111",
    
    // more than 64 positions
    "111111111111111\n"
    "111111111111111\n"
    "111111111111111\n"
    "111111111111111\n"
    "111111111111111\n"
    "111111111111111"
    };
  
  // Test all solvable boards
  std::cout << "--- Testing solvable boards" << std::endl;
  bool failed_solvable = false;
  for (const auto& board_string: solvable_boards)
  {
    if (!print_solve_board(board_string))
    {
      failed_solvable = true;
    }    
  }
  
  // Test all unsolvable boards
  std::cout << "--- Testing unsolvable boards" << std::endl;
  bool failed_unsolvable = false;
  for (const auto& board_string: not_solvable_boards)
  {
    if (print_solve_board(board_string))
    {
      failed_unsolvable = true;
    }        
  }
  
  // Test invalid boards for exceptions
  std::cout << "--- Testing invalid boards" << std::endl;
  bool failed_invalid = false;
  for (const auto& board_string: invalid_boards)
  {
    try
    {
      ps::PegBoard board(board_string);
      failed_invalid = true;
    }   
    catch(...)
    {}
  }
  
  std::cout << "\n - Solvable board test set " <<
    (failed_solvable ? "failed." : "passed.") << std::endl;
  std::cout << "\n - Unsolvable board test set " << 
    (failed_unsolvable ? "failed." : "passed.") << std::endl;
  std::cout << "\n - Invalid board test set " <<
    (failed_invalid ? "failed." : "passed.") << std::endl;
  
  return 0;
}

bool print_solve_board(const std::string& board_string)
{
  ps::PegBoard board(board_string);
  ps::BoardSolver solver(board);

  std::cout << "Trying board:\n";
  board.print();
  
  return solver.solve_board();
}
