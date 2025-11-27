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

    static size_t convertRCtoI(size_t row, size_t col);

    Cell& getCell(size_t row, size_t col);
    const Cell& getCell(size_t row, size_t col) const;
    auto getRow(size_t row) -> std::vector<IndexedCell>;
    auto getCol(size_t col) -> std::vector<IndexedCell>;
    auto getBlock(size_t row, size_t col) -> std::vector<IndexedCell>;

  public:
    /**
     * @brief Creates a new sudoku
     * @param name the name to greet
     */
    explicit Sudoku(std::string initial_state_str);

    // Return number of snapshots (steps taken)
    auto stepsTaken() const -> size_t;

    bool solved() const;

    /**
     * @brief Creates a table
     * @return a string containing the greeting
     */
    auto toTable() const -> std::string;

    auto toDebug() -> std::string;
    auto toDebugTable() const -> std::string;
    /**
     * @brief Creates a table
     * @return a string containing the greeting
     */
    auto toString() const -> std::string;

    // Friend function to overload <<
    friend std::ostream& operator<<(std::ostream& os, const Sudoku& s);

    bool solveStep();
  };

}  // namespace sudoku
