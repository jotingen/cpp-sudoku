#include <fmt/format.h>
#include <sudoku/sudoku.h>

#include <algorithm>
#include <array>
#include <sstream>
#include <string>

namespace sudoku {

  Sudoku::Sudoku(std::string initial_state_str) {
    if (initial_state_str.size() != 81) {
      throw std::invalid_argument(
          fmt::format("Sudoku string was {}, expected 81", initial_state_str.size()));
    }

    std::array<int, 81> initial_state = {};
    std::replace(initial_state_str.begin(), initial_state_str.end(), '.', '0');
    for (int i = 0; i < 81; i++) {
      initial_state[i] = initial_state_str[i] - '0';  // Convert char to int
    }
    state.push_back(initial_state);
  }

  std::string Sudoku::toTable() const {
    std::ostringstream out;

    for (int row = 0; row < 9; row++) {
      if (row % 3 == 0 && row != 0) {
        out << "------+-------+------\n";  // horizontal separator
      }

      for (int col = 0; col < 9; col++) {
        if (col % 3 == 0 && col != 0) {
          out << "| ";  // vertical separator
        }

        int val = state.back()[row * 9 + col];
        if (val == 0) {
          out << ". ";  // empty cell
        } else {
          out << val << " ";
        }
      }
      out << "\n";
    }

    return out.str();  // return the whole table as a string
  }

  std::string Sudoku::toString() const {
    std::string s;
    s.reserve(81);  // avoid reallocations

    for (int i = 0; i < 81; i++) {
      if (state.back()[i] == 0) {
        s.push_back('.');  // use '.' for empty cells
      } else {
        s.push_back(static_cast<char>('0' + state.back()[i]));
      }
    }

    return s;
  }

  // Definition of the overloaded << operator
  std::ostream& operator<<(std::ostream& os, const Sudoku& s) {
    os << s.toString();
    return os;
  }

  bool Sudoku::makeMove(unsigned int val, unsigned int row, unsigned int col) {
    if (val > 9 || row > 8 || col > 8) {
      return false;
    }
    return true;
  }

}  // namespace sudoku
