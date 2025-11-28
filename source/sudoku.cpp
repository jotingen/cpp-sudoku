#include <fmt/format.h>
#include <spdlog/spdlog.h>
#include <sudoku/sudoku.h>

#include <algorithm>
#include <array>
#include <iostream>
#include <print>
#include <ranges>
#include <set>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace sudoku {

  const size_t ROWS = 9;
  const size_t COLS = 9;

  Sudoku::Sudoku(std::string initial_state_str) {
    if (initial_state_str.size() != ROWS * COLS) {
      throw std::invalid_argument(
          fmt::format("Sudoku string was {}, expected {}", initial_state_str.size(), ROWS * COLS));
    }

    Board initial_state = {};
    std::ranges::replace(initial_state_str, '.', '0');
    for (size_t i = 0; i < ROWS; i++) {
      for (size_t j = 0; j < COLS; j++) {
        if (initial_state_str[convertRCtoI(i, j)] == '0') {
          initial_state[i][j] = {1, 2, 3, 4, 5, 6, 7, 8, 9};  // NOLINT(readability-magic-numbers)
        } else {
          initial_state[i][j]
              = {initial_state_str[convertRCtoI(i, j)] - '0'};  // Convert char to int
        }
      }
    }
    state.push_back(initial_state);
    spdlog::debug("Sudoku instance created");
  }

  // Return number of snapshots (steps taken)
  auto Sudoku::stepsTaken() const -> size_t { return state.size(); }

  auto Sudoku::toString() const -> std::string {
    std::string s;
    s.reserve(ROWS * COLS);  // avoid reallocations

    for (size_t i = 0; i < ROWS * COLS; i++) {
      for (size_t j = 0; j < COLS; j++) {
        const auto& cell = state.back()[i][j];
        if (cell.size() == 1) {
          s.push_back(static_cast<char>('0' + cell[0]));
        } else {
          s.push_back('.');
        }
      }
    }

    return s;
  }

  auto Sudoku::toTable() const -> std::string {
    std::ostringstream out;

    for (size_t row = 0; row < ROWS; row++) {
      if (row % 3 == 0 && row != 0) {
        out << "------+-------+------\n";  // horizontal separator
      }

      for (size_t col = 0; col < COLS; col++) {
        if (col % 3 == 0 && col != 0) {
          out << "| ";  // vertical separator
        }

        const auto& cell = state.back()[row][col];
        if (cell.size() == 1) {
          int val = cell[0];
          out << val << " ";
        } else {
          out << ". ";
        }
      }
      out << "\n";
    }

    return out.str();  // return the whole table as a string
  }

  auto Sudoku::toDebug() -> std::string {
    std::ostringstream out;

    for (size_t row = 0; row < ROWS; row++) {
      for (size_t col = 0; col < COLS; col++) {
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

  auto Sudoku::toDebugTable() const -> std::string {
    std::ostringstream out;

    out << toTable();
    for (size_t row = 0; row < ROWS; row++) {
      if (row % 3 == 0 && row != 0) {
        out << "------------+-------------+------------\n";
        out << "            |             |            \n";
      }

      for (size_t colRow = 0; colRow < 3; colRow++) {
        for (size_t col = 0; col < COLS; col++) {
          if (col % 3 == 0 && col != 0) {
            out << "| ";
          }

          const Cell& cell = state.back()[row][col];
          if (std::ranges::find(cell, (3 * colRow) + 1) != cell.end()) {
            out << (3 * colRow) + 1;
          } else {
            out << ".";
          }
          if (std::ranges::find(cell, (3 * colRow) + 2) != cell.end()) {
            out << (3 * colRow) + 2;
          } else {
            out << ".";
          }
          if (std::ranges::find(cell, (3 * colRow) + 3) != cell.end()) {
            out << (3 * colRow) + 3;
          } else {
            out << ".";
          }
          out << " ";
        }
        out << "\n";
        if (colRow == 2 && row != 8) {
          out << "            |             |            \n";
        }
      }
    }

    return out.str();  // return the whole table as a string
  }

  // Definition of the overloaded << operator
  std::ostream& operator<<(std::ostream& os, const Sudoku& s) {
    os << s.toString();
    return os;
  }

  auto Sudoku::convertRCtoI(size_t row, size_t col) -> size_t { return (row * COLS) + col; }

  auto Sudoku::solved() const -> bool {
    for (size_t i = 0; i < ROWS; i++) {
      for (size_t j = 0; j < COLS; j++) {
        if (state.back()[i][j].size() != 1) {
          return false;
        }
      }
    }

    return true;
  }

  auto Sudoku::getCell(size_t row, size_t col) -> Cell& { return state.back()[row][col]; }
  auto Sudoku::getCell(size_t row, size_t col) const -> const Cell& {
    return state.back()[row][col];
  }

  auto Sudoku::getRow(size_t row) -> IndexedCellGroup {
    IndexedCellGroup rowCells = {};
    for (size_t col = 0; col < COLS; ++col) {
      rowCells.emplace_back(row, col, state.back()[row][col]);
    }
    return rowCells;
  }

  auto Sudoku::getCol(size_t col) -> IndexedCellGroup {
    IndexedCellGroup colCells = {};
    for (size_t row = 0; row < ROWS; ++row) {
      colCells.emplace_back(row, col, state.back()[row][col]);
    }
    return colCells;
  }

  auto Sudoku::getBlock(size_t row, size_t col) -> IndexedCellGroup {
    IndexedCellGroup blockCells = {};
    size_t r0 = (row / 3) * 3;
    size_t c0 = (col / 3) * 3;
    for (size_t dr = 0; dr < 3; ++dr) {
      for (size_t dc = 0; dc < 3; ++dc) {
        blockCells.emplace_back(r0 + dr, c0 + dc, state.back()[r0 + dr][c0 + dc]);
      }
    }
    return blockCells;
  }

  auto Sudoku::solveRulePencilingCellWithGroup(size_t row, size_t col, Cell& cell,
                                               const IndexedCellGroup& group) -> bool {
    for (const IndexedCell& groupCell : group) {
      int valueNdx = 0;
      while (valueNdx < int(cell.size())) {
        if (groupCell.cell.size() == 1 && (groupCell.row != row || groupCell.col != col)
            && groupCell.cell.front() == cell.at(valueNdx)) {
          spdlog::debug("Penciling: Removing possible value {} from ({},{})", cell.at(valueNdx),
                        row, col);
          cell.erase(cell.begin() + valueNdx);
          if (cell.size() == 1) {
            spdlog::debug("Penciling: Solved cell with value {} from ({},{})", cell.front(), row,
                          col);
          }
          return true;
        }
        valueNdx++;
      }
    }
    return false;
  }

  auto Sudoku::solveRulePencilingCell(size_t row, size_t col, Cell& cell) -> bool {
    if (cell.size() > 1) {
      std::vector<IndexedCellGroup> groups = {getRow(row), getCol(col), getBlock(row, col)};
      for (const auto& group : groups) {
        bool cellUpdated = solveRulePencilingCellWithGroup(row, col, getCell(row, col), group);
        if (cellUpdated) {
          return true;
        }
      }
    }
    return false;
  }

  auto Sudoku::solveRulePenciling() -> bool {
    bool updatedLoop;
    bool updated = false;
    do {
      updatedLoop = false;
      for (size_t row = 0; row < ROWS; row++) {
        for (size_t col = 0; col < COLS; col++) {
          bool cellUpdated = solveRulePencilingCell(row, col, getCell(row, col));
          if (cellUpdated) {
            updated = true;
            updatedLoop = true;
          }
        }
      }
    } while (updatedLoop);
    if (updated) {
      spdlog::debug("\n{}", toDebugTable());
      return true;
    }

    return false;
  }

  auto Sudoku::solveRulePointingGroups(const IndexedCellGroup group0, const IndexedCellGroup group1)
      -> bool {
    IndexedCellGroup sharedCells;
    std::set<int> sharedValues;
    for (const auto& g0Cell : group0) {
      for (const auto& g1Cell : group1) {
        if (g0Cell.row == g1Cell.row && g0Cell.col == g1Cell.col) {
          sharedCells.push_back(g0Cell);
          if (g0Cell.cell.size() > 1) {
            for (auto value : g0Cell.cell) {
              sharedValues.insert(value);
            }
          }
          if (g1Cell.cell.size() > 1) {
            for (auto value : g1Cell.cell) {
              sharedValues.insert(value);
            }
          }
        }
      }
    }
    spdlog::trace("  Found {} shared cells", sharedCells.size());

    IndexedCellGroup unsharedGroup0Cells;
    for (const auto& g0Cell : group0) {
      bool isShared = false;
      for (const auto& sharedCell : sharedCells) {
        if (g0Cell.row == sharedCell.row && g0Cell.col == sharedCell.col) {
          isShared = true;
        }
      }
      if (!isShared) {
        unsharedGroup0Cells.push_back(g0Cell);
      }
    }
    spdlog::trace("  Found {} unshared group0 cells", unsharedGroup0Cells.size());

    IndexedCellGroup unsharedGroup1Cells;
    for (const auto& g1Cell : group1) {
      bool isShared = false;
      for (const auto& sharedCell : sharedCells) {
        if (g1Cell.row == sharedCell.row && g1Cell.col == sharedCell.col) {
          isShared = true;
        }
      }
      if (!isShared) {
        unsharedGroup1Cells.push_back(g1Cell);
      }
    }
    spdlog::trace("  Found {} unshared group1 cells", unsharedGroup1Cells.size());

    spdlog::trace("  Found {} shared values", sharedValues.size());
    bool updated = false;
    if (sharedValues.size() > 0) {
      for (auto sharedValue : sharedValues) {
        bool valueFoundOutsideOfSharedGroup0 = false;
        bool valueFoundOutsideOfSharedGroup1 = false;
        spdlog::trace("    {}", sharedValue);
        //  check if in unshared group0 cells
        for (const auto& unsharedGroup0Cell : unsharedGroup0Cells) {
          if (std::ranges::find(unsharedGroup0Cell.cell, sharedValue)
              != unsharedGroup0Cell.cell.end()) {
            valueFoundOutsideOfSharedGroup0 = true;
            spdlog::trace("    {} found outside of shared cells in group0", sharedValue);
            break;
          }
        }
        // check if in unshared group1 cells
        for (const auto& unsharedGroup1Cell : unsharedGroup1Cells) {
          if (std::ranges::find(unsharedGroup1Cell.cell, sharedValue)
              != unsharedGroup1Cell.cell.end()) {
            valueFoundOutsideOfSharedGroup1 = true;
            spdlog::trace("    {} found outside of shared cells in group1", sharedValue);
            break;
          }
        }
        if (valueFoundOutsideOfSharedGroup0 && !valueFoundOutsideOfSharedGroup1) {
          spdlog::trace("\n{}", toDebugTable());
          spdlog::trace("    {} only in found in unshared cells in group0", sharedValue);
          for (const auto& unsharedGroup0Cell : unsharedGroup0Cells) {
            spdlog::debug("Pointing: Removing possible value {} from ({},{})", sharedValue,
                          unsharedGroup0Cell.row, unsharedGroup0Cell.col);
            unsharedGroup0Cell.cell.erase(
                std::ranges::remove(unsharedGroup0Cell.cell, sharedValue).begin(),
                unsharedGroup0Cell.cell.end());
          }
          updated = true;
        } else if (!valueFoundOutsideOfSharedGroup0 && valueFoundOutsideOfSharedGroup1) {
          spdlog::trace("\n{}", toDebugTable());
          spdlog::trace("    {} only in found in unshared cells in group1", sharedValue);
          for (const auto& unsharedGroup1Cell : unsharedGroup1Cells) {
            spdlog::debug("Pointing: Removing possible value {} from ({},{})", sharedValue,
                          unsharedGroup1Cell.row, unsharedGroup1Cell.col);
            unsharedGroup1Cell.cell.erase(
                std::ranges::remove(unsharedGroup1Cell.cell, sharedValue).begin(),
                unsharedGroup1Cell.cell.end());
          }
          updated = true;
        } else if (valueFoundOutsideOfSharedGroup0 && valueFoundOutsideOfSharedGroup1) {
          spdlog::trace("   {} found in unshared cells in both groups", sharedValue);
        } else {
          spdlog::trace("   {} not found in unshared cells in either group", sharedValue);
        }
      }
    }

    if (updated) {
      spdlog::debug("\n{}", toDebugTable());
      return true;
    }

    return false;
  }

  auto Sudoku::solveRulePointing() -> bool {
    for (size_t row = 0; row < ROWS; row++) {
      for (size_t col = 0; col < COLS; col += 3) {
        spdlog::trace("Pointing - Row - ({},{})", row, col);
        if (solveRulePointingGroups(getRow(row), getBlock(row, col))) {
          return true;
        }
      }
    }
    for (size_t col = 0; col < COLS; col++) {
      for (size_t row = 0; row < ROWS; row += 3) {
        spdlog::trace("Pointing - Col - ({},{})", row, col);
        if (solveRulePointingGroups(getCol(col), getBlock(row, col))) {
          return true;
        }
      }
    }
    return false;
  }

  std::vector<std::pair<int, int>> makeCandidatePairs(const std::set<int>& candidates) {
    std::vector<int> vals(candidates.begin(), candidates.end());

    auto indices = std::views::iota(0, (int)vals.size());

    std::vector<std::pair<int, int>> result;
    for (int i : indices) {
      for (int j : std::views::iota(i + 1, (int)vals.size())) {
        result.emplace_back(vals[i], vals[j]);
      }
    }
    return result;
  }

  std::vector<std::tuple<int, int, int>> makeCandidateTuples(const std::set<int>& candidates) {
    std::vector<int> vals(candidates.begin(), candidates.end());

    std::vector<std::tuple<int, int, int>> result;
    for (size_t i = 0; i < vals.size(); ++i) {
      for (size_t j = i + 1; j < vals.size(); ++j) {
        for (size_t k = j + 1; k < vals.size(); ++k) {
          result.emplace_back(vals[i], vals[j], vals[k]);
        }
      }
    }

    return result;
  }

  auto Sudoku::solveRuleHiddenPairsGroup(IndexedCellGroup cellGroup) -> bool {
    spdlog::trace("solveRuleHiddenPairsGroup");

    // Get all candidates in group and generate pairs
    std::set<int> candidates;
    for (const auto& cell : cellGroup) {
      if (cell.cell.size() > 1) {
        for (auto candidate : cell.cell) {
          candidates.insert(candidate);
        }
      }
    }

    // Pair up candidates and check if only two cells have them as a pair
    for (auto [a, b] : makeCandidatePairs(candidates)) {
      spdlog::trace("  Candidate pair: ({}, {})", a, b);
      bool invalidated = false;
      IndexedCellGroup candidateCells = {};
      for (const auto& cell : cellGroup) {
        bool cellContainsFirstValue = std::ranges::find(cell.cell, a) != cell.cell.end();
        bool cellContainsSecondValue = std::ranges::find(cell.cell, b) != cell.cell.end();
        if (cellContainsFirstValue && cellContainsSecondValue) {
          candidateCells.push_back(cell);
        } else if (cellContainsFirstValue || cellContainsSecondValue) {
          invalidated = true;
          break;
        }
      }
      if (invalidated) {
        continue;
      }
      // Check if there are only two candidates
      // and only process if a cell has mroe then 2 candidates
      if (candidateCells.size() == 2
          && (candidateCells[0].cell.size() > 2 || candidateCells[1].cell.size() > 2)) {
        spdlog::debug("Hidden Pairs: Found {} and {} paired in cells ({},{}) and ({},{})", a, b,
                      candidateCells[0].row, candidateCells[0].col, candidateCells[1].row,
                      candidateCells[1].col);
        for (const auto& candidateCell : candidateCells) {
          candidateCell.cell.erase(
              std::remove_if(candidateCell.cell.begin(), candidateCell.cell.end(),
                             [=](int x) { return x != a && x != b; }),
              candidateCell.cell.end());
        }
        spdlog::debug("\n{}", toDebugTable());
        return true;
      }
    }
    return false;
  }

  auto Sudoku::solveRuleHiddenPairs() -> bool {
    spdlog::trace("solveRuleHiddenPairs");
    for (size_t row = 0; row < ROWS; row++) {
      if (solveRuleHiddenPairsGroup(getRow(row))) {
        return true;
      }
    }
    for (size_t col = 0; col < COLS; col++) {
      if (solveRuleHiddenPairsGroup(getCol(col))) {
        return true;
      }
    }
    for (size_t row = 0; row < ROWS; row += 3) {
      for (size_t col = 0; col < COLS; col += 3) {
        if (solveRuleHiddenPairsGroup(getBlock(row, col))) {
          return true;
        }
      }
    }
    return false;
  }

  auto Sudoku::solveRuleHiddenTuplesGroup(IndexedCellGroup cellGroup) -> bool {
    spdlog::trace("solveRuleHiddenTuplesGroup");

    // Get all candidates in group and generate tuples
    std::set<int> candidates;
    for (const auto& cell : cellGroup) {
      if (cell.cell.size() > 1) {
        for (auto candidate : cell.cell) {
          candidates.insert(candidate);
        }
      }
    }

    // Tuple up candidates and check if only three cells have them
    for (auto [a, b, c] : makeCandidateTuples(candidates)) {
      spdlog::trace("  Candidate tuple: ({}, {}, {})", a, b, c);
      bool invalidated = false;
      IndexedCellGroup candidateCells = {};
      for (const auto& cell : cellGroup) {
        bool cellContainsFirstValue = std::ranges::find(cell.cell, a) != cell.cell.end();
        bool cellContainsSecondValue = std::ranges::find(cell.cell, b) != cell.cell.end();
        bool cellContainsThirdValue = std::ranges::find(cell.cell, c) != cell.cell.end();
        if ((cellContainsFirstValue && cellContainsSecondValue)
            || (cellContainsFirstValue && cellContainsThirdValue)
            || (cellContainsSecondValue && cellContainsThirdValue)) {
          candidateCells.push_back(cell);
        } else if (cellContainsFirstValue || cellContainsSecondValue || cellContainsThirdValue) {
          invalidated = true;
          break;
        }
      }
      if (invalidated) {
        continue;
      }
      // Check if there are only three candidates
      // and only process if a cell has more then 3 candidates
      if (candidateCells.size() == 3
          && (candidateCells[0].cell.size() > 3 || candidateCells[1].cell.size() > 3
              || candidateCells[2].cell.size() > 3)) {
        spdlog::debug(
            "Hidden Tuples: Found {}, {}, and {} paired in cells ({},{}), ({},{}) and ({},{})", a,
            b, c, candidateCells[0].row, candidateCells[0].col, candidateCells[1].row,
            candidateCells[1].col, candidateCells[2].row, candidateCells[2].col);
        for (const auto& candidateCell : candidateCells) {
          candidateCell.cell.erase(
              std::remove_if(candidateCell.cell.begin(), candidateCell.cell.end(),
                             [=](int x) { return x != a && x != b && x != c; }),
              candidateCell.cell.end());
        }
        spdlog::debug("\n{}", toDebugTable());
        return true;
      }
    }
    return false;
  }

  auto Sudoku::solveRuleHiddenTuples() -> bool {
    spdlog::trace("solveRuleHiddenTuples");
    for (size_t row = 0; row < ROWS; row++) {
      if (solveRuleHiddenTuplesGroup(getRow(row))) {
        return true;
      }
    }
    for (size_t col = 0; col < COLS; col++) {
      if (solveRuleHiddenTuplesGroup(getCol(col))) {
        return true;
      }
    }
    for (size_t row = 0; row < ROWS; row += 3) {
      for (size_t col = 0; col < COLS; col += 3) {
        if (solveRuleHiddenTuplesGroup(getBlock(row, col))) {
          return true;
        }
      }
    }
    return false;
  }

  auto Sudoku::solveStep() -> bool {
    spdlog::trace("SolveStep");
    using Step = bool (Sudoku::*)();

    std::vector<Step> rules = {&Sudoku::solveRulePenciling, &Sudoku::solveRulePointing,
                               &Sudoku::solveRuleHiddenPairs, &Sudoku::solveRuleHiddenTuples};

    state.push_back(state.back());

    return std::ranges::any_of(rules, [this](const Step& rule) { return (this->*rule)(); });
  }

}  // namespace sudoku
