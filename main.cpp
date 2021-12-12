#include "PegSolitaire.hpp"

#include <iostream> 
#include <chrono>

int main()
{
  using namespace std::chrono;
  auto const english_board = ps::get_english_board();

  std::cout << "Start solving:\n";
  ps::print(english_board);
  std::cout << "---------------\n";

  auto const start = high_resolution_clock::now();

  ps::BoardSolver solver(english_board);
  solver.solve_board();

  auto end = high_resolution_clock::now();
  auto duration_ms =
    duration_cast<duration<double, std::milli>>(end - start).count();

  ps::print_moves(english_board, solver.get_moves());
  
  std::cout << "Solving took: " << duration_ms << " ms\n";

  return 0;
}
