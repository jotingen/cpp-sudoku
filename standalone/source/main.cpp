#include <sudoku/sudoku.h>
#include <sudoku/version.h>

#include <cxxopts.hpp>
#include <iostream>
#include <string>
#include <unordered_map>

auto main(int argc, char** argv) -> int {
  const std::unordered_map<std::string, sudoku::LanguageCode> languages{
      {"en", sudoku::LanguageCode::EN},
      {"de", sudoku::LanguageCode::DE},
      {"es", sudoku::LanguageCode::ES},
      {"fr", sudoku::LanguageCode::FR},
  };

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

  sudoku::Sudoku sudoku(filename);
  // std::cout << sudoku.greet(langIt->second) << std::endl;
  std::cout << sudokus[0] << std::endl;

  return 0;
}
