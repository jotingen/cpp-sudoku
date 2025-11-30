#pragma once

#include <array>
#include <bitset>
#include <iostream>
#include <optional>
#include <string>
#include <vector>

namespace sudoku {

  const size_t ROWS = 9;
  const size_t COLS = 9;

  using Candidates = std::bitset<9>;

  struct Cell {
    size_t row;
    size_t col;
    Candidates candidates;

    // Default for Board initialization, note row and col are 0
    Cell() : row(0), col(0), candidates{} { candidates.set(); }

    // Construct with no candidates given (set all)
    Cell(size_t r, size_t c) : row(r), col(c), candidates{} { candidates.set(); }

    // Construct with given candidates
    Cell(size_t r, size_t c, Candidates candidates) : row(r), col(c), candidates(candidates) {}

    // Add a candidate (digit 1–9)
    void addCandidate(int digit) { candidates.set(digit - 1); }

    // Remove a candidate
    void removeCandidate(int digit) { candidates.reset(digit - 1); }

    // Check if a candidate is present
    bool hasCandidate(int digit) const { return candidates.test(digit - 1); }

    // Get number of candidates
    int candidateCount() const { return candidates.count(); }

    // Return all candidates as a vector of ints
    std::vector<int> toVector() const {
      std::vector<int> result;
      for (int i = 0; i < 9; ++i) {
        if (candidates.test(i)) {
          result.push_back(i + 1);
        }
      }
      return result;
    }

    // Keep only a specific set of digits
    void keepOnly(const std::vector<int>& digits) {
      Candidates newCandidates;
    for (int d : digits) {
        if (candidates.test(d - 1)) {
            newCandidates.set(d - 1);
        }
    }
    candidates = newCandidates;
    }
    void keepOnly(int digit) {
      Candidates newCandidates;
        if (candidates.test(digit - 1)) {
            newCandidates.set(digit - 1);
        }
    candidates = newCandidates;
    }

  };

  struct Group {
    std::vector<Cell*> cells;

    // Add a cell reference
    void add(Cell& cell) { cells.push_back(&cell); }

    // iteration
    struct iterator {
      using base_iter = std::vector<Cell*>::iterator;
      base_iter it;
      iterator(base_iter i) : it(i) {}
      Cell& operator*() const { return **it; }
      iterator& operator++() {
        ++it;
        return *this;
      }
      bool operator!=(const iterator& other) const { return it != other.it; }
    };

    auto begin() { return iterator(cells.begin()); }
    auto end() { return iterator(cells.end()); }

    // const iteration
    struct const_iterator {
      using base_iter = std::vector<Cell*>::const_iterator;
      base_iter it;
      const_iterator(base_iter i) : it(i) {}
      const Cell& operator*() const { return **it; }
      const_iterator& operator++() {
        ++it;
        return *this;
      }
      bool operator!=(const const_iterator& other) const { return it != other.it; }
    };

    auto begin() const { return const_iterator(cells.begin()); }
    auto end() const { return const_iterator(cells.end()); }

    // size function
    size_t size() const { return cells.size(); }

    // operator[] returns reference
    Cell& operator[](size_t i) { return *cells[i]; }
    const Cell& operator[](size_t i) const { return *cells[i]; }
  };

  struct Board {
    std::array<std::array<Cell, 9>, 9> cells;

    Board() : cells{} {
      cells = {};
      for (size_t i = 0; i < ROWS; i++) {
        for (size_t j = 0; j < COLS; j++) {
          cells[i][j].row = i;
          cells[i][j].col = j;
        }
      }
    }

    auto getCell(size_t row, size_t col) -> Cell& { return cells[row][col]; }
    auto getCell(size_t row, size_t col) const -> const Cell& { return cells[row][col]; }

    auto getRow(size_t row) -> Group {
      Group rowCells;
      for (size_t col = 0; col < COLS; ++col) {
        rowCells.add(cells[row][col]);
      }
      return rowCells;
    }

    auto getCol(size_t col) -> Group {
      Group colCells;
      for (size_t row = 0; row < ROWS; ++row) {
        colCells.add(cells[row][col]);
      }
      return colCells;
    }

    auto getBlock(size_t row, size_t col) -> Group {
      Group blockCells;
      size_t r0 = (row / 3) * 3;
      size_t c0 = (col / 3) * 3;
      for (size_t dr = 0; dr < 3; ++dr) {
        for (size_t dc = 0; dc < 3; ++dc) {
          blockCells.add(cells[r0 + dr][c0 + dc]);
        }
      }
      return blockCells;
    }
  };

  /**
   * @brief A class for saying hello in multiple languages
   */
  class Sudoku {
  private:
    std::vector<Board> state;

    auto solveRulePenciling() -> bool;
    auto solveRulePencilingCell(Cell& cell) -> bool;
    auto solveRulePencilingCellWithGroup(Cell& cell, const Group& group) -> bool;

    auto solveRulePointingGroups(Group group0, Group group1) -> bool;
    auto solveRulePointing() -> bool;

    auto solveRuleHiddenPairsGroup(Group cellGroup) -> bool;
    auto solveRuleHiddenPairs() -> bool;

    auto solveRuleHiddenTuplesGroup(Group cellGroup) -> bool;
    auto solveRuleHiddenTuples() -> bool;

    auto solveRuleNakedPairs() -> bool;

    auto solveRuleNakedTriples() -> bool;

    auto solveRuleXWingCells(size_t row0, size_t row1, size_t col0, size_t col1) -> bool;
    auto solveRuleXWing() -> bool;

    static auto convertRCtoI(size_t row, size_t col) -> size_t;

    auto getCell(size_t row, size_t col) -> Cell&;
    auto getCell(size_t row, size_t col) const -> const Cell&;
    auto getRow(size_t row) -> Group;
    auto getCol(size_t col) -> Group;
    auto getBlock(size_t row, size_t col) -> Group;

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
