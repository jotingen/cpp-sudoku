#pragma once

#include <array>
#include <iostream>
#include <optional>
#include <string>
#include <vector>

namespace sudoku {

  using Cell = std::vector<int>;

  using Board = std::array<std::array<Cell, 9>, 9>;

  struct IndexedCell {
    size_t row;
    size_t col;
    Cell& cell;
    IndexedCell(size_t r, size_t c, Cell& cref) : row(r), col(c), cell(cref) {}
  };

  /**
   * @brief A class for saying hello in multiple languages
   */
  class Sudoku {
  private:
    std::vector<Board> state;

    bool solveRulePenciling();
    bool solveRulePencilingCell(size_t row, size_t col, Cell& cell);

    bool solveRulePointing();

    bool solveRuleHiddenPairs();

    bool solveRuleHiddenTriples();

    bool solveRuleNakedPairs();

    bool solveRuleNakedTriples();

    size_t convertRCtoI(size_t row, size_t col) const;

    Cell& getCell(size_t row, size_t col);
    const Cell& getCell(size_t row, size_t col) const;
    std::vector<IndexedCell> getRow(size_t row);
    std::vector<IndexedCell> getCol(size_t col);
    std::vector<IndexedCell> getBlock(size_t row, size_t col);

  public:
    /**
     * @brief Creates a new sudoku
     * @param name the name to greet
     */
    explicit Sudoku(std::string initial_state_str);

    // Return number of snapshots (steps taken)
    size_t stepsTaken() const;

    bool solved() const;

    /**
     * @brief Creates a table
     * @return a string containing the greeting
     */
    std::string toTable() const;

    std::string toDebug();
    std::string toDebugTable() const;
    /**
     * @brief Creates a table
     * @return a string containing the greeting
     */
    std::string toString() const;

    // Friend function to overload <<
    friend std::ostream& operator<<(std::ostream& os, const Sudoku& s);

    bool solveStep();
  };

}  // namespace sudoku
