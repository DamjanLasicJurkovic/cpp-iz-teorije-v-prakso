#include "PegSolitaire.hpp"

#include <iostream>
#include <chrono>

// Runs the solve on an input board using the provided mode and prints the time
void run_test_on_board(const ps::PegBoard& board, ps::SolverBoardSaving mode);

int main()
{
  std::cout << "----- Input board:\n";
  ps::get_english_board().print();
  
  std::cout << "\n----- Solving with no board saving\n";
  run_test_on_board(ps::get_english_board(), ps::SolverBoardSaving::None);
    
  std::cout << "\n----- Solving with hashmap board saving\n";
  run_test_on_board(ps::get_english_board(), ps::SolverBoardSaving::Hashmap);
  
  std::cout << "\n----- Solving with bitfield board saving\n";
  run_test_on_board(ps::get_english_board(), ps::SolverBoardSaving::Bitfield);
  
  return 0;
}

void run_test_on_board(const ps::PegBoard& board, ps::SolverBoardSaving mode)
{  
  
  // Start timing to include solver initialization (important for Bitfield mode)
  //auto const start = std::chrono::high_resolution_clock::now();
  
  ps::BoardSolver solver(board, mode);
  
  // Starting here only times the solving routine
  auto const start = std::chrono::high_resolution_clock::now();
  
  bool success = solver.solve_board();
  
  auto end = std::chrono::high_resolution_clock::now();
  auto duration_ms =
    std::chrono::duration_cast<std::chrono::duration<double, std::milli>>
    (end - start).count();
  
  std::cout << "Success: " << success << ", time elapsed: " << duration_ms << " ms" << std::endl;
  
  if (success)
  {
    std::cout << "Final board:\n";
    auto final_board = solver.get_current_board();
    final_board.print(); 
  }
}
