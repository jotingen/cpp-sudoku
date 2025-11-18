#include <fmt/format.h>
#include <sudoku/sudoku.h>

namespace sudoku {

  Sudoku::Sudoku(std::string _sudoku) : sudoku(std::move(_sudoku)) {}

  std::string Sudoku::toString() const { return sudoku; }

  // Definition of the overloaded << operator
  std::ostream& operator<<(std::ostream& os, const Sudoku& s) {
    os << s.sudoku;
    return os;
  }

}  // namespace sudoku
