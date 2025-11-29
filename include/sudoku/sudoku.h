#pragma once

#include <array>
#include <iostream>
#include <optional>
#include <string>
#include <vector>

namespace sudoku {

  using Cell = std::vector<int>;
  using CellGroup = std::vector<Cell>;

  using Board = std::array<std::array<Cell, 9>, 9>;

  struct IndexedCell {
    size_t row;
    size_t col;
    Cell& cell;
    IndexedCell(size_t r, size_t c, Cell& cref) : row(r), col(c), cell(cref) {}
  };
  using IndexedCellGroup = std::vector<IndexedCell>;

  /**
   * @brief A class for saying hello in multiple languages
   */
  class Sudoku {
  private:
    std::vector<Board> state;

    auto solveRulePenciling() -> bool;
    auto solveRulePencilingCell(size_t row, size_t col, Cell& cell) -> bool;
    auto solveRulePencilingCellWithGroup(size_t row, size_t col, Cell& cell,
                                         const IndexedCellGroup& group) -> bool;

    auto solveRulePointingGroups(const IndexedCellGroup group0, const IndexedCellGroup group1)
        -> bool;
    auto solveRulePointing() -> bool;

    auto solveRuleHiddenPairsGroup(IndexedCellGroup cellGroup) -> bool;
    auto solveRuleHiddenPairs() -> bool;

    auto solveRuleHiddenTuplesGroup(IndexedCellGroup cellGroup) -> bool;
    auto solveRuleHiddenTuples() -> bool;

    auto solveRuleNakedPairs() -> bool;

    auto solveRuleNakedTriples() -> bool;

    auto solveRuleXWingCells(size_t row0, size_t row1, size_t col0, size_t col1) -> bool;
    auto solveRuleXWing() -> bool;

    static auto convertRCtoI(size_t row, size_t col) -> size_t;

    auto getCell(size_t row, size_t col) -> Cell&;
    auto getCell(size_t row, size_t col) const -> const Cell&;
    auto getRow(size_t row) -> IndexedCellGroup;
    auto getCol(size_t col) -> IndexedCellGroup;
    auto getBlock(size_t row, size_t col) -> IndexedCellGroup;

  public:
    /**
     * @brief Creates a new sudoku
     * @param name the name to greet
     */
    explicit Sudoku(std::string initial_state_str);

    // Return number of snapshots (steps taken)
    auto stepsTaken() const -> size_t;

    auto solved() const -> bool;

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
