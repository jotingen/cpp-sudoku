#pragma once

#include <array>
#include <iostream>
#include <optional>
#include <string>
#include <vector>

namespace sudoku {

  /**
   * @brief A class for saying hello in multiple languages
   */
  class Sudoku {
  private:
    std::vector<std::array<int, 81>> state;

  public:
    /**
     * @brief Creates a new sudoku
     * @param name the name to greet
     */
    explicit Sudoku(std::string initial_state_str);

    /**
     * @brief Creates a table
     * @return a string containing the greeting
     */
    std::string toTable() const;

    /**
     * @brief Creates a table
     * @return a string containing the greeting
     */
    std::string toString() const;

    // Friend function to overload <<
    friend std::ostream& operator<<(std::ostream& os, const Sudoku& s);

    /**
     * @brief Takes a move and if legal stores to state and returns true
     * @return true if valid
     */
    bool makeMove(unsigned int val, unsigned int row, unsigned int col);
  };

}  // namespace sudoku
