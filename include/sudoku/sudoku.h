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
    std::vector<std::array<std::vector<int>, 81>> state;

  public:
    /**
     * @brief Creates a new sudoku
     * @param name the name to greet
     */
    explicit Sudoku(std::string initial_state_str);

    // Return number of snapshots (steps taken)
    size_t stepsTaken() const;

    /**
     * @brief Creates a table
     * @return a string containing the greeting
     */
    std::string toTable() const;

    std::string toDebug();
    /**
     * @brief Creates a table
     * @return a string containing the greeting
     */
    std::string toString() const;

    // Friend function to overload <<
    friend std::ostream& operator<<(std::ostream& os, const Sudoku& s);

    bool solveStep();
    bool solveRuleUniqueRow();
    bool solveRuleUniqueCol();
    bool solveRuleUniqueBlock();

    int convertRCtoI(int row, int col) const;
    std::vector<int>& getCell(int row, int col);
    std::array<std::reference_wrapper<std::vector<int>>, 9> getRow(int row);
    std::array<std::reference_wrapper<std::vector<int>>, 9> getCol(int col);
    std::array<std::reference_wrapper<std::vector<int>>, 9> getBlock(int row, int col);
  };

}  // namespace sudoku
