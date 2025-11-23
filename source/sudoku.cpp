#include <fmt/format.h>
#include <sudoku/sudoku.h>

#include <algorithm>
#include <array>
#include <iostream>
#include <sstream>
#include <string>

namespace sudoku {

  Sudoku::Sudoku(std::string initial_state_str) {
    if (initial_state_str.size() != 81) {
      throw std::invalid_argument(
          fmt::format("Sudoku string was {}, expected 81", initial_state_str.size()));
    }

    std::array<std::vector<int>, 81> initial_state = {};
    std::replace(initial_state_str.begin(), initial_state_str.end(), '.', '0');
    for (int i = 0; i < 81; i++) {
      if (initial_state_str[i] == '0') {
        initial_state[i] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
      } else {
        initial_state[i] = {initial_state_str[i] - '0'};  // Convert char to int
      }
    }
    state.push_back(initial_state);
  }

  // Return number of snapshots (steps taken)
  size_t Sudoku::stepsTaken() const { return state.size(); }

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

        if (state.back()[row * 9 + col].size() == 1) {
          int val = state.back()[row * 9 + col][0];
          out << val << " ";
        } else {
          out << ". ";
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
      if (state.back()[i].size() == 1) {
        s.push_back(static_cast<char>('0' + state.back()[i][0]));
      } else {
        s.push_back('.');
      }
    }

    return s;
  }

  std::string Sudoku::toDebug() {
    std::ostringstream out;

    for (int row = 0; row < 9; row++) {
      for (int col = 0; col < 9; col++) {
        out << " (" << row << "," << col << "): ";
        const auto& cell = getCell(row, col);
        for (int value : cell) {
          out << value;
        }
        out << "\n";
      }
    }

    return out.str();  // return the whole table as a string
  }

  // Definition of the overloaded << operator
  std::ostream& operator<<(std::ostream& os, const Sudoku& s) {
    os << s.toString();
    return os;
  }

  int Sudoku::convertRCtoI(int row, int col) const { return row * 9 + col; }

  bool Sudoku::solved() const {

    for (int i = 0; i < 81; i++) {
      if (state.back()[i].size() != 1) {
        return false;
      }
    }

    return true;
  }

  std::vector<int>& Sudoku::getCell(int row, int col) {
    return state.back()[convertRCtoI(row, col)];
  }

  std::array<std::reference_wrapper<std::vector<int>>, 9> Sudoku::getRow(int row) {
    return {getCell(row, 0), getCell(row, 1), getCell(row, 2), getCell(row, 3), getCell(row, 4),
            getCell(row, 5), getCell(row, 6), getCell(row, 7), getCell(row, 8)};
  }

  std::array<std::reference_wrapper<std::vector<int>>, 9> Sudoku::getCol(int col) {
    return {getCell(0, col), getCell(1, col), getCell(2, col), getCell(3, col), getCell(4, col),
            getCell(5, col), getCell(6, col), getCell(7, col), getCell(8, col)};
  }

  std::array<std::reference_wrapper<std::vector<int>>, 9> Sudoku::getBlock(int row, int col) {
    return {getCell((row / 3) * 3 + 0, (col / 3) * 3 + 0),
            getCell((row / 3) * 3 + 0, (col / 3) * 3 + 1),
            getCell((row / 3) * 3 + 0, (col / 3) * 3 + 2),
            getCell((row / 3) * 3 + 1, (col / 3) * 3 + 0),
            getCell((row / 3) * 3 + 1, (col / 3) * 3 + 1),
            getCell((row / 3) * 3 + 1, (col / 3) * 3 + 2),
            getCell((row / 3) * 3 + 2, (col / 3) * 3 + 0),
            getCell((row / 3) * 3 + 2, (col / 3) * 3 + 1),
            getCell((row / 3) * 3 + 2, (col / 3) * 3 + 2)};
  }

  bool Sudoku::solveRuleUniqueRow() {
    for (int row = 0; row < 9; row++) {
      for (int col = 0; col < 9; col++) {
        // std::cout << "Processing (" << row << "," << col <<"):"<<std::endl;
        auto& cell = getCell(row, col);
        if (cell.size() > 1) {
          for (auto& value : getRow(row)) {
            int i = 0;
            while (i < int(cell.size())) {
              if (value.get().size() == 1 && &cell != &value.get()
                  && value.get().front() == cell.at(i)) {
                // std::cout << "  UniqRow - Removing " << cell.at(i) << " from (" << row << "," <<
                // col
                //           << ")" << std::endl;
                cell.erase(cell.begin() + i);
                break;
              }
              i++;
            }
          }
          if (cell.size() == 1) {
            // std::cout << "UniqRow - (" << row << "," << col << "): " << cell.front() <<
            // std::endl;
            return true;
          }
        }
      }
    }
    return false;
  }

  bool Sudoku::solveRuleUniqueCol() {
    for (int row = 0; row < 9; row++) {
      for (int col = 0; col < 9; col++) {
        // std::cout << "Processing (" << row << "," << col <<"):"<<std::endl;
        auto& cell = getCell(row, col);
        if (cell.size() > 1) {
          for (auto& value : getCol(col)) {
            int i = 0;
            while (i < int(cell.size())) {
              if (value.get().size() == 1 && &cell != &value.get()
                  && value.get().front() == cell.at(i)) {
                // std::cout << "  UniqCol - Removing " << cell.at(i) << " from (" << row << "," <<
                // col
                //           << ")" << std::endl;
                cell.erase(cell.begin() + i);
                break;
              }
              i++;
            }
          }
          if (cell.size() == 1) {
            // std::cout << "(" << row << "," << col << "): " << cell.front() << std::endl;
            return true;
          }
        }
      }
    }
    return false;
  }

  bool Sudoku::solveRuleUniqueBlock() {
    for (int row = 0; row < 9; row++) {
      for (int col = 0; col < 9; col++) {
        // std::cout << "Processing (" << row << "," << col <<"):"<<std::endl;
        auto& cell = getCell(row, col);
        if (cell.size() > 1) {
          for (auto& value : getBlock(row, col)) {
            int i = 0;
            while (i < int(cell.size())) {
              if (value.get().size() == 1 && &cell != &value.get()
                  && value.get().front() == cell.at(i)) {
                // std::cout << "  Removing " << cell.at(i) << std::endl;
                cell.erase(cell.begin() + i);
                break;
              }
              i++;
            }
          }
          if (cell.size() == 1) {
            // std::cout << "(" << row << "," << col << "): " << cell.front() << std::endl;
            return true;
          }
        }
      }
    }
    return false;
  }

  bool Sudoku::solveStep() {
    using Step = bool (Sudoku::*)();

    std::vector<Step> rules
        = {&Sudoku::solveRuleUniqueRow, &Sudoku::solveRuleUniqueCol, &Sudoku::solveRuleUniqueBlock};

    state.push_back(state.back());

    for (Step rule : rules) {
      if ((this->*rule)()) {
        std::cout << "Rule:" << rule << std::endl;
        return true;
      }
    }
    return false;
  }

}  // namespace sudoku
