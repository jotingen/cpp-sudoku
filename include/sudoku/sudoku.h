#pragma once

#include <iostream>
#include <string>

namespace sudoku {

  /**
   * @brief A class for saying hello in multiple languages
   */
  class Sudoku {
  private:
    std::string sudoku;

  public:
    /**
     * @brief Creates a new sudoku
     * @param name the name to greet
     */
    Sudoku(std::string sudoku);

    /**
     * @brief Creates a localized string containing the greeting
     * @param lang the language to greet in
     * @return a string containing the greeting
     */
    std::string toString() const;

    // Friend function to overload <<
    friend std::ostream& operator<<(std::ostream& os, const Sudoku& s);
  };

}  // namespace sudoku
