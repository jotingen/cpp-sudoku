#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>
#include <sudoku/sudoku.h>
#include <sudoku/version.h>

#include <cxxopts.hpp>
#include <iostream>
#include <print>
#include <string>
#include <unordered_map>

void init_logging() {
  auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
  console_sink->set_level(spdlog::level::info);

  auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("sudoku.log", true);
  file_sink->set_level(spdlog::level::debug);

  std::vector<spdlog::sink_ptr> sinks{console_sink, file_sink};

  auto logger = std::make_shared<spdlog::logger>("sudoku", sinks.begin(), sinks.end());
  spdlog::register_logger(logger);
  spdlog::set_default_logger(logger);

  spdlog::set_level(spdlog::level::debug);
  spdlog::flush_on(spdlog::level::info);
}

auto main(int argc, char** argv) -> int {
  init_logging();
  spdlog::info("Hello, Sudoku world!");
  cxxopts::Options options(*argv, "A Sudoku Solver");

  std::string filename;
  std::vector<std::string> sudokus;

  // clang-format off
  options.add_options()
    ("h,help", "Show help")
    ("v,version", "Print the current version number")
    ("f,file", "File of sudokus to solve, one per line", cxxopts::value(filename))
    ("sudokus", "Sudokus to solve", cxxopts::value(sudokus))
  ;
  // clang-format on
  options.parse_positional({"sudokus"});
  options.positional_help("<Sudokus to solve>");

  auto result = options.parse(argc, argv);

  if (result["help"].as<bool>()) {
    std::cout << options.help() << std::endl;
    return 0;
  }

  if (result["version"].as<bool>()) {
    std::cout << "Sudoku, version " << SUDOKU_VERSION << std::endl;
    return 0;
  }

  for (uint i = 0; i < sudokus.size(); i++) {
    try {
      sudoku::Sudoku game = sudoku::Sudoku(sudokus[i]);
      // std::cout << game << std::endl;
      std::cout << game.toString() << std::endl;

      // std::cout << game.toTable() << std::flush;

      // std::cout << game.toDebug() << std::flush;

      bool updated = true;
      int step = 0;
      while (updated) {
        // while (updated && step < 1) {
        std::cout << "Step: " << ++step << std::endl << std::flush;
        std::cout << game.toTable() << std::flush;
        // std::cout << game.toDebug() << std::flush;
        updated = game.solveStep();
      }

      std::println("Steps taken: {}", game.stepsTaken());

    } catch (const std::invalid_argument& e) {
      std::cerr << e.what() << std::endl;
      continue;
    }
  }

  return 0;
}
