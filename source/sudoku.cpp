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

  int Sudoku::convertRCtoI(int row, int col) { return row * 9 + col; }

  int Sudoku::getCell(int row, int col) { return state.back()[convertRCtoI(row, col)]; }

  std::array<int, 9> Sudoku::getRow(int row) {
    return {getCell(row, 0), getCell(row, 1), getCell(row, 2), getCell(row, 3), getCell(row, 4),
            getCell(row, 5), getCell(row, 6), getCell(row, 7), getCell(row, 8)};
  }

  std::array<int, 9> Sudoku::getCol(int col) {
    return {getCell(0, col), getCell(1, col), getCell(2, col), getCell(3, col), getCell(4, col),
            getCell(5, col), getCell(6, col), getCell(7, col), getCell(8, col)};
  }

  std::array<int, 9> Sudoku::getBlock(int row, int col) {
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

  bool Sudoku::solveRuleLoneStep() {
    std::array<int, 81> new_state = state.back();

    // Loop through all cells and check if there are no other possible solutions
    for (int row = 0; row < 9; row++) {
      for (int col = 0; col < 9; col++) {
        if (state.back()[convertRCtoI(row, col)] == 0) {
          std::vector<int> remainder_row = {1, 2, 3, 4, 5, 6, 7, 8, 9};
          std::vector<int> remainder_col = {1, 2, 3, 4, 5, 6, 7, 8, 9};
          std::vector<int> remainder_block = {1, 2, 3, 4, 5, 6, 7, 8, 9};

          for (int value : getRow(row)) {
            int r_i = 0;
            while (r_i < 9) {
              if (value == remainder_row[r_i]) {
                remainder_row.erase(remainder_row.begin() + r_i);
                break;
              }
              r_i++;
            }
          }

          for (int value : getCol(col)) {
            int c_i = 0;
            while (c_i < 9) {
              if (value == remainder_col[c_i]) {
                remainder_col.erase(remainder_col.begin() + c_i);
                break;
              }
              c_i++;
            }
          }

          for (int value : getBlock(row, col)) {
            int b_i = 0;
            while (b_i < 9) {
              if (value == remainder_block[b_i]) {
                remainder_block.erase(remainder_block.begin() + b_i);
                break;
              }
              b_i++;
            }
          }

          // std::cout << row << ":" << col << std::endl;
          // std::cout << "Row    ";
          // for (int c : getRow(row)) {
          //   std::cout <<  c << " ";
          // }
          // std::cout << std::endl;
          // std::cout << "Col    ";
          // for (int c : getCol(row)) {
          //   std::cout <<  c << " ";
          // }
          // std::cout << std::endl;
          // std::cout << "Block  ";
          // for (int c : getBlock(row, col)) {
          //   std::cout <<  c << " ";
          // }
          // std::cout << std::endl;

          // std::cout << "Possible Row Values  ";
          // for (int remainder : remainder_row) {
          //   std::cout <<  remainder << " ";
          // }
          // std::cout << std::endl;

          // std::cout << "Possible Col Values  ";
          // for (int remainder : remainder_col) {
          //   std::cout <<  remainder << " ";
          // }
          // std::cout << std::endl;

          // std::cout << "Possible Block Values ";
          // for (int remainder : remainder_block) {
          //   std::cout <<  remainder << " ";
          // }
          // std::cout << std::endl;

          std::vector<int> tmp;
          std::set_intersection(remainder_row.begin(), remainder_row.end(), remainder_col.begin(),
                                remainder_col.end(), std::back_inserter(tmp));

          std::vector<int> remainders;
          std::set_intersection(tmp.begin(), tmp.end(), remainder_block.begin(),
                                remainder_block.end(), std::back_inserter(remainders));

          //`std::cout << "Possible Values       ";
          //`for (int remainder : remainders) {
          //`  std::cout << "  " << remainder << " ";
          //`}
          //`std::cout << std::endl;

          if (remainders.size() == 1) {
            std::cout << row << ":" << col << " -> " << remainders[0] << std::endl;
            new_state[convertRCtoI(row, col)] = remainders[0];
            state.push_back(new_state);
            return true;
          }
        }
      }
    }

    return false;
  }

  bool Sudoku::solveStep() {
    using Step = bool (Sudoku::*)();

    std::vector<Step> steps = {&Sudoku::solveRuleLoneStep};
    for (size_t i = 0; i < steps.size(); i++) {
      if ((this->*steps[i])()) {
        std::cout << "Rule:" << i << std::endl;
        return true;
      }
    }
    return false;
  }

}  // namespace sudoku
