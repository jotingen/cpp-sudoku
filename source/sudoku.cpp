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

  Sudoku::Sudoku(std::string initial_state_str) {
    if (initial_state_str.size() != ROWS * COLS) {
      throw std::invalid_argument(
          fmt::format("Sudoku string was {}, expected {}", initial_state_str.size(), ROWS * COLS));
    }

    Board initial_state = {};
    std::ranges::replace(initial_state_str, '.', '0');
    for (size_t i = 0; i < ROWS; i++) {
      for (size_t j = 0; j < COLS; j++) {
        int value = initial_state_str[convertRCtoI(i, j)] - '0';
        if (value != 0) {
          initial_state.getCell(i, j).keepOnly(value);
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
        const auto& cell = state.back().getCell(i, j);
        if (cell.candidateCount() == 1) {
          s.push_back(static_cast<char>('0' + cell.toVector()[0]));
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

        const auto& cell = state.back().getCell(row, col);
        if (cell.candidateCount() == 1) {
          int val = cell.toVector()[0];
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
        for (int value : cell.toVector()) {
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

          const Cell& cell = state.back().getCell(row, col);
          if (cell.hasCandidate((3 * colRow) + 1)) {
            out << (3 * colRow) + 1;
          } else {
            out << ".";
          }
          if (cell.hasCandidate((3 * colRow) + 2)) {
            out << (3 * colRow) + 2;
          } else {
            out << ".";
          }
          if (cell.hasCandidate((3 * colRow) + 3)) {
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
        if (state.back().getCell(i, j).candidateCount() != 1) {
          return false;
        }
      }
    }

    return true;
  }

  auto Sudoku::getCell(size_t row, size_t col) -> Cell& { return state.back().getCell(row, col); }
  auto Sudoku::getCell(size_t row, size_t col) const -> const Cell& {
    return state.back().getCell(row, col);
  }

  auto Sudoku::getRow(size_t row) -> Group {
    return state.back().getRow(row);
  }

  auto Sudoku::getCol(size_t col) -> Group {
    return state.back().getCol(col);
  }

  auto Sudoku::getBlock(size_t row, size_t col) -> Group {
    return state.back().getBlock(row,col);
  }

  auto Sudoku::solveRulePencilingCellWithGroup(Cell& cell, const Group& group) -> bool {
    for (const Cell& groupCell : group) {
      for (auto candidate : cell.toVector()) {
        if (groupCell.candidateCount() == 1
            && (groupCell.row != cell.row || groupCell.col != cell.col)
            && groupCell.hasCandidate(candidate)
            && cell.hasCandidate(candidate)) {
          spdlog::debug("Penciling: Removing possible value {} from ({},{})", candidate, cell.row,
                        cell.col);
          cell.removeCandidate(candidate);
          if (cell.candidateCount() == 1) {
            spdlog::debug("Penciling: Solved cell with value {} from ({},{})", cell.toVector().front(), cell.row,
                          cell.col);
          }
          return true;
        }
      }
    }
    return false;
  }

  auto Sudoku::solveRulePencilingCell(Cell& cell) -> bool {
    if (cell.candidateCount() > 1) {
      std::vector<Group> groups = {state.back().getRow(cell.row), state.back().getCol(cell.col),
                                   state.back().getBlock(cell.row, cell.col)};
      for (const auto& group : groups) {
        if (solveRulePencilingCellWithGroup(cell, group)) {
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
          if (solveRulePencilingCell(getCell(row, col))) {
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

  auto Sudoku::solveRulePointingGroups(Group group0, Group group1) -> bool {
    Group sharedCells;
    std::set<int> sharedValues;
    for (auto& g0Cell : group0) {
      for (auto& g1Cell : group1) {
        if (g0Cell.row == g1Cell.row && g0Cell.col == g1Cell.col) {
          sharedCells.add(g0Cell);
          if (g0Cell.candidateCount() > 1) {
            for (auto value : g0Cell.toVector()) {
              sharedValues.insert(value);
            }
          }
          if (g1Cell.candidateCount() > 1) {
            for (auto value : g1Cell.toVector()) {
              sharedValues.insert(value);
            }
          }
        }
      }
    }
    spdlog::trace("  Found {} shared cells", sharedCells.size());

    Group unsharedGroup0Cells;
    for (auto& g0Cell : group0) {
      bool isShared = false;
      for (const auto& sharedCell : sharedCells) {
        if (g0Cell.row == sharedCell.row && g0Cell.col == sharedCell.col) {
          isShared = true;
        }
      }
      if (!isShared) {
        unsharedGroup0Cells.add(g0Cell);
      }
    }
    spdlog::trace("  Found {} unshared group0 cells", unsharedGroup0Cells.size());

    Group unsharedGroup1Cells;
    for (auto& g1Cell : group1) {
      bool isShared = false;
      for (const auto& sharedCell : sharedCells) {
        if (g1Cell.row == sharedCell.row && g1Cell.col == sharedCell.col) {
          isShared = true;
        }
      }
      if (!isShared) {
        unsharedGroup1Cells.add(g1Cell);
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
          if (unsharedGroup0Cell.hasCandidate(sharedValue)) {
            valueFoundOutsideOfSharedGroup0 = true;
            spdlog::trace("    {} found outside of shared cells in group0", sharedValue);
            break;
          }
        }
        // check if in unshared group1 cells
        for (const auto& unsharedGroup1Cell : unsharedGroup1Cells) {
          if (unsharedGroup1Cell.hasCandidate(sharedValue)) {
            valueFoundOutsideOfSharedGroup1 = true;
            spdlog::trace("    {} found outside of shared cells in group1", sharedValue);
            break;
          }
        }
        if (valueFoundOutsideOfSharedGroup0 && !valueFoundOutsideOfSharedGroup1) {
          spdlog::trace("\n{}", toDebugTable());
          spdlog::trace("    {} only in found in unshared cells in group0", sharedValue);
          for (auto& unsharedGroup0Cell : unsharedGroup0Cells) {
            spdlog::debug("Pointing: Removing possible value {} from ({},{})", sharedValue,
                          unsharedGroup0Cell.row, unsharedGroup0Cell.col);
            unsharedGroup0Cell.removeCandidate(sharedValue);
          }
          updated = true;
        } else if (!valueFoundOutsideOfSharedGroup0 && valueFoundOutsideOfSharedGroup1) {
          spdlog::trace("\n{}", toDebugTable());
          spdlog::trace("    {} only in found in unshared cells in group1", sharedValue);
          for (auto& unsharedGroup1Cell : unsharedGroup1Cells) {
            spdlog::debug("Pointing: Removing possible value {} from ({},{})", sharedValue,
                          unsharedGroup1Cell.row, unsharedGroup1Cell.col);
            unsharedGroup1Cell.removeCandidate(sharedValue);
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

  auto Sudoku::solveRuleHiddenPairsGroup(Group cellGroup) -> bool {
    spdlog::trace("solveRuleHiddenPairsGroup");

    // Get all candidates in group and generate pairs
    std::set<int> candidates;
    for (auto& cell : cellGroup) {
      if (cell.candidateCount() > 1) {
        for (auto candidate : cell.toVector()) {
          candidates.insert(candidate);
        }
      }
    }

    // Pair up candidates and check if only two cells have them as a pair
    for (auto [a, b] : makeCandidatePairs(candidates)) {
      spdlog::trace("  Candidate pair: ({}, {})", a, b);
      bool invalidated = false;
      Group candidateCells = {};
      for (auto& cell : cellGroup) {
        bool cellContainsFirstValue = cell.hasCandidate(a);
        bool cellContainsSecondValue = cell.hasCandidate(b);
        if (cellContainsFirstValue && cellContainsSecondValue) {
          candidateCells.add(cell);
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
          && (candidateCells[0].candidateCount() > 2 || candidateCells[1].candidateCount() > 2)) {
        spdlog::debug("Hidden Pairs: Found {} and {} paired in cells ({},{}) and ({},{})", a, b,
                      candidateCells[0].row, candidateCells[0].col, candidateCells[1].row,
                      candidateCells[1].col);
        for (auto& candidateCell : candidateCells) {
          candidateCell.keepOnly({a, b});
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

  auto Sudoku::solveRuleHiddenTuplesGroup(Group cellGroup) -> bool {
    spdlog::trace("solveRuleHiddenTuplesGroup");

    // Get all candidates in group and generate tuples
    std::set<int> candidates;
    for (const auto& cell : cellGroup) {
      if (cell.candidateCount() > 1) {
        for (auto candidate : cell.toVector()) {
          candidates.insert(candidate);
        }
      }
    }

    // Tuple up candidates and check if only three cells have them
    for (auto [a, b, c] : makeCandidateTuples(candidates)) {
      spdlog::trace("  Candidate tuple: ({}, {}, {})", a, b, c);
      bool invalidated = false;
      Group candidateCells = {};
      for (auto& cell : cellGroup) {
        bool cellContainsFirstValue = cell.hasCandidate(a);
        bool cellContainsSecondValue = cell.hasCandidate(b);
        bool cellContainsThirdValue = cell.hasCandidate(c);
        if ((cellContainsFirstValue && cellContainsSecondValue)
            || (cellContainsFirstValue && cellContainsThirdValue)
            || (cellContainsSecondValue && cellContainsThirdValue)) {
          candidateCells.add(cell);
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
          && (candidateCells[0].candidateCount() > 3 || candidateCells[1].candidateCount() > 3
              || candidateCells[2].candidateCount() > 3)) {
        spdlog::debug(
            "Hidden Tuples: Found {}, {}, and {} paired in cells ({},{}), ({},{}) and ({},{})", a,
            b, c, candidateCells[0].row, candidateCells[0].col, candidateCells[1].row,
            candidateCells[1].col, candidateCells[2].row, candidateCells[2].col);
        for (auto& candidateCell : candidateCells) {
          candidateCell.keepOnly({a, b, c});
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

  auto Sudoku::solveRuleXWingCells(size_t row0, size_t row1, size_t col0, size_t col1) -> bool {
    Group candidateCells;
    candidateCells.add(state.back().getCell(row0, col0));
    candidateCells.add(state.back().getCell(row0, col1));
    candidateCells.add(state.back().getCell(row1, col0));
    candidateCells.add(state.back().getCell(row1, col1));
    // Get all candidates in group
    std::set<int> candidates;
    for (const auto& cell : candidateCells) {
      if (cell.candidateCount() == 1) {
        return false;
      }
      for (auto candidate : cell.toVector()) {
        candidates.insert(candidate);
      }
    }

    for (auto candidate : candidates) {
      if (candidateCells[0].hasCandidate(candidate)
          && candidateCells[1].hasCandidate(   candidate)
          && candidateCells[2].hasCandidate(   candidate)
          && candidateCells[3].hasCandidate(   candidate)) {
        spdlog::trace("{} shared amoungst ({},{}) ({},{}), ({},{}), ({},{})", candidate, row0, col0,
                      row0, col1, row1, col0, row1, col1);
        bool unique;
        bool others;

        // Check rows
        unique = true;
        others = false;
        // Check that other cells in row dont contain candidate
        for (auto row : {getRow(row0), getRow(row1)}) {
          for (auto cell : row) {
            if (cell.hasCandidate(candidate)
                && cell.col != col0 && cell.col != col1) {
              unique = false;
            }
          }
        }
        // Check that other cells in rows contain candidate
        for (auto col : {getCol(col0), getCol(col1)}) {
          for (auto cell : col) {
            if (cell.hasCandidate(candidate)
                && cell.row != row0 && cell.row != row1) {
              others = true;
            }
          }
        }
        if (unique && others) {
          spdlog::debug("\n{}", toDebugTable());
          spdlog::debug("{} is unique across rows for ({},{}) ({},{}), ({},{}), ({},{})", candidate,
                        row0, col0, row0, col1, row1, col0, row1, col1);
          for (auto col : {getCol(col0), getCol(col1)}) {
            for (auto cell : col) {
              if (cell.hasCandidate(candidate)
                  && cell.row != row0 && cell.row != row1) {
                cell.removeCandidate(candidate);
              }
            }
          }
          spdlog::debug("\n{}", toDebugTable());
          return true;
        }

        // Check columns
        unique = true;
        others = false;
        // Check that other cells in column dont contain candidate
        for (auto col : {getCol(col0), getCol(col1)}) {
          for (auto cell : col) {
            if (cell.hasCandidate(candidate)
                && cell.row != row0 && cell.row != row1) {
              unique = false;
            }
          }
        }
        // Check that other cells in row dont contain candidate
        for (auto row : {getRow(row0), getRow(row1)}) {
          for (auto& cell : row) {
            if (cell.hasCandidate(candidate)
                && cell.col != col0 && cell.col != col1) {
              others = true;
            }
          }
        }
        if (unique && others) {
          spdlog::debug("\n{}", toDebugTable());
          spdlog::debug("{} is unique across cols for ({},{}) ({},{}), ({},{}), ({},{})", candidate,
                        row0, col0, row0, col1, row1, col0, row1, col1);
          for (auto row : {getRow(row0), getRow(row1)}) {
            for (auto& cell : row) {
              if (cell.hasCandidate(candidate)
                  && cell.col != col0 && cell.col != col1) {
                cell.removeCandidate(candidate);
              }
            }
          }
          spdlog::debug("\n{}", toDebugTable());
          return true;
        }
      }
    }

    return false;
  }

  auto Sudoku::solveRuleXWing() -> bool {
    spdlog::trace("solveRuleXWing");
    for (size_t row0 = 0; row0 < ROWS; row0++) {
      for (size_t col0 = 0; col0 < COLS; col0++) {
        for (size_t row1 = row0 + 1; row1 < ROWS; row1++) {
          for (size_t col1 = col0 + 1; col1 < COLS; col1++) {
            if (row0 / 3 != row1 / 3 || col0 / 3 != col1 / 3) {
              if (solveRuleXWingCells(row0, row1, col0, col1)) {
                return true;
              }
            }
          }
        }
      }
    }
    return false;
  }

  auto Sudoku::solveStep() -> bool {
    spdlog::trace("SolveStep");
    using Step = bool (Sudoku::*)();

    std::vector<Step> rules
        = {&Sudoku::solveRulePenciling, &Sudoku::solveRulePointing, &Sudoku::solveRuleHiddenPairs,
           &Sudoku::solveRuleHiddenTuples, &Sudoku::solveRuleXWing};

    state.push_back(state.back());

    return std::ranges::any_of(rules, [this](const Step& rule) { return (this->*rule)(); });
  }

}  // namespace sudoku
