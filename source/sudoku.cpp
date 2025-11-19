#include <fmt/format.h>
#include <sudoku/sudoku.h>

namespace sudoku {

  Sudoku::Sudoku(std::string _sudoku) : sudoku(std::move(_sudoku)) {
    if (sudoku.size() != 81) {
      throw std::invalid_argument(fmt::format("Sudoku string was {}, expected 81", sudoku.size()));
    }
  }

  std::string Sudoku::toString() const { return sudoku; }

  // Definition of the overloaded << operator
  std::ostream& operator<<(std::ostream& os, const Sudoku& s) {
    os << s.sudoku;
    return os;
  }

}  // namespace sudoku
