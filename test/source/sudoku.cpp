#include <doctest/doctest.h>
#include <spdlog/spdlog.h>
#include <sudoku/sudoku.h>
#include <sudoku/version.h>

#include <string>

TEST_CASE("Simple") {
  using namespace sudoku;

  spdlog::debug("Simple");
  Sudoku game("53..7....6..195....98....6.8...6...34..8.3..17...2...6.6....28....419..5....8..79");
  spdlog::debug("\n{}", game.toTable());
  spdlog::debug("\n{}", game.toDebugTable());

  bool updated = true;
  std::cout << "Initial:\n" << game.toTable() << std::endl;
  while (updated) {
    updated = game.solveStep();
  }
  std::cout << "Final:\n" << game.toTable() << std::endl;
  if (!game.solved()) {
    spdlog::debug("\n{}", game.toTable());
    spdlog::debug("\n{}", game.toDebugTable());
  }
  CHECK(game.solved() == true);
}

TEST_CASE("Hidden Pairs") {
  using namespace sudoku;

  spdlog::debug("Hidden Pairs");
  Sudoku game("1794...3.65..1.7..82...76..56....87.438672...79........87..9.5.9.5.8.3.7..675.9..");
  spdlog::debug("\n{}", game.toTable());
  spdlog::debug("\n{}", game.toDebugTable());

  bool updated = true;
  std::cout << "Initial:\n" << game.toTable() << std::endl;
  while (updated) {
    updated = game.solveStep();
  }
  std::cout << "Final:\n" << game.toTable() << std::endl;
  if (!game.solved()) {
    spdlog::debug("\n{}", game.toTable());
    spdlog::debug("\n{}", game.toDebugTable());
  }
  CHECK(game.solved() == true);
}

TEST_CASE("World's Hardest Sudoku") {
  using namespace sudoku;

  spdlog::debug("World's Hardest Sudoku");
  Sudoku game("8.........36......7..9.2...5...7.......457......1...3...1....68..85...1..9....4..");
  spdlog::debug("\n{}", game.toTable());
  spdlog::debug("\n{}", game.toDebugTable());

  bool updated = true;
  std::cout << "Initial:\n" << game.toTable() << std::endl;
  while (updated) {
    updated = game.solveStep();
  }
  std::cout << "Final:\n" << game.toTable() << std::endl;
  if (!game.solved()) {
    spdlog::debug("\n{}", game.toTable());
    spdlog::debug("\n{}", game.toDebugTable());
  }
  CHECK(game.solved() == true);
}

TEST_CASE("17-clue Minimal") {
  using namespace sudoku;

  spdlog::debug("17-clue Minimal");
  Sudoku game("1....7..9....3..5...........2..1..8...........5..9..3...........4..8....7..2....6");
  spdlog::debug("\n{}", game.toTable());
  spdlog::debug("\n{}", game.toDebugTable());

  bool updated = true;
  std::cout << "Initial:\n" << game.toTable() << std::endl;
  while (updated) {
    updated = game.solveStep();
  }
  std::cout << "Final:\n" << game.toTable() << std::endl;
  if (!game.solved()) {
    spdlog::debug("\n{}", game.toTable());
    spdlog::debug("\n{}", game.toDebugTable());
  }
  CHECK(game.solved() == true);
}

TEST_CASE("X Puzzle") {
  using namespace sudoku;

  spdlog::debug("X Puzzle");
  Sudoku game(".4........2..6.......7..1.......9..5...5...3...8..2......6..4.......1..9.......7.");
  spdlog::debug("\n{}", game.toTable());
  spdlog::debug("\n{}", game.toDebugTable());

  bool updated = true;
  std::cout << "Initial:\n" << game.toTable() << std::endl;
  while (updated) {
    updated = game.solveStep();
  }
  std::cout << "Final:\n" << game.toTable() << std::endl;
  if (!game.solved()) {
    spdlog::debug("\n{}", game.toTable());
    spdlog::debug("\n{}", game.toDebugTable());
  }
  CHECK(game.solved() == true);
}

TEST_CASE("Sudoku version") {
  static_assert(std::string_view(SUDOKU_VERSION) == std::string_view("1.0"));
  CHECK(std::string(SUDOKU_VERSION) == std::string("1.0"));
}
