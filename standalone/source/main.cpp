#include <sudoku/sudoku.h>
#include <sudoku/version.h>

#include <cxxopts.hpp>
#include <iostream>
#include <string>
#include <unordered_map>

auto main(int argc, char** argv) -> int {
  cxxopts::Options options(*argv, "A Sudoku Solver");

  std::string filename;
  std::vector<std::string> sudokus;

  // clang-format off
  options.add_options()
    ("h,help", "Show help")
    ("v,version", "Print the current version number")
    ("f,file", "File of sudokus to solve, one per line", cxxopts::value(filename))
    ("sudokus", "Sudokus to solve", cxxopts::value(sudokus))
  ;
  // clang-format on
  options.parse_positional({"sudokus"});
  options.positional_help("<Sudokus to solve>");

  auto result = options.parse(argc, argv);

  if (result["help"].as<bool>()) {
    std::cout << options.help() << std::endl;
    return 0;
  }

  if (result["version"].as<bool>()) {
    std::cout << "Sudoku, version " << SUDOKU_VERSION << std::endl;
    return 0;
  }

  for (uint i = 0; i < sudokus.size(); i++) {
    try {
      sudoku::Sudoku game = sudoku::Sudoku(sudokus[i]);
      std::cout << game << std::endl;
      std::cout << game.toString() << std::endl;
      std::cout << game.makeMove(0, 0, 0) << std::endl;
      std::cout << game.toTable() << std::endl;
    } catch (const std::invalid_argument& e) {
      std::cerr << e.what() << std::endl;
      continue;
    }
  }

  return 0;
}
