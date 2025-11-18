#include <doctest/doctest.h>
#include <sudoku/sudoku.h>
#include <sudoku/version.h>

#include <string>

TEST_CASE("Sudoku") {
  using namespace sudoku;

  Sudoku sudoku("Tests");

  //CHECK(sudoku.greet(LanguageCode::EN) == "Hello, Tests!");
  //CHECK(sudoku.greet(LanguageCode::DE) == "Hallo Tests!");
  //CHECK(sudoku.greet(LanguageCode::ES) == "¡Hola Tests!");
  //CHECK(sudoku.greet(LanguageCode::FR) == "Bonjour Tests!");
}

TEST_CASE("Sudoku version") {
  static_assert(std::string_view(SUDOKU_VERSION) == std::string_view("1.0"));
  CHECK(std::string(SUDOKU_VERSION) == std::string("1.0"));
}
