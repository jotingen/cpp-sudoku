#include <doctest/doctest.h>
#include <sudoku/sudoku.h>
#include <sudoku/version.h>

#include <string>

TEST_CASE("Simple") {
  using namespace sudoku;

  Sudoku game("53..7....6..195....98....6.8...6...34..8.3..17...2...6.6....28....419..5....8..79");

  bool updated = true;
  std::cout << "Initial:\n" << game.toTable() << std::endl;
  while (updated) {
    updated = game.solveStep();
  }
  std::cout << "Final:\n" << game.toTable() << std::endl;

  CHECK(game.solved() == true);
  // CHECK(sudoku.greet(LanguageCode::DE) == "Hallo Tests!");
  // CHECK(sudoku.greet(LanguageCode::ES) == "¡Hola Tests!");
  // CHECK(sudoku.greet(LanguageCode::FR) == "Bonjour Tests!");
}

TEST_CASE("Sudoku version") {
  static_assert(std::string_view(SUDOKU_VERSION) == std::string_view("1.0"));
  CHECK(std::string(SUDOKU_VERSION) == std::string("1.0"));
}
