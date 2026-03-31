/**
 * @file module_03_modern_cpp/04_modern_types.cpp
 * @brief Modern C++: New Data Types (C++17)
 *
 * @details
 * =============================================================================
 * [THEORY: Why Do We Lose Points in Code Reviews?]
 * =============================================================================
 * EN: In old C++ code, if a function failed, it usually returned "-1", "NULL" or "nullptr"
 * (Magic Numbers). This is dangerous because what if "-1" is the ACTUAL valid answer? With
 * C++17, things became extremely safe, similar to Rust.
 * 1. std::optional<T> : Contains a value of type "T" OR it is empty (nullopt).
 * 2. std::variant<T, Y> : A type-safe version of C's "union". Holds only ONE type at
 *    a time but allocates memory equal to the BIGGEST mapped type (RAM saver).
 * 3. std::any : Holds "anything" (like variables in Python or JS). Heavy overhead.
 * 4. std::string_view : A lightweight purely observational "window" to look at a string
 *    WITHOUT copying it!
 *
 * [CPPREF DEPTH: auto Type Deduction Rules — Template Argument Deduction in Disguise]
 * =============================================================================
 * EN: `auto` uses the same rules as template argument deduction (§7.1.7.4 / §13.10.2). When you
 * write `auto x = expr;`, the compiler deduces the type as if `x` were a function template
 * parameter: references and top-level const/volatile are stripped. This means:
 * • `const int& r = n; auto a = r;`  → a is `int`, NOT `const int&`.
 * • `auto x = {1,2,3};` deduces `std::initializer_list<int>` (special rule).
 * • `auto x{42};` in C++17 deduces `int` (changed from init-list in C++11).
 * `decltype(auto)` preserves the EXACT type including references:
 * • `decltype(auto) y = r;` → y is `const int&`.
 * Parenthesized id-expressions change decltype semantics:
 * • `decltype(x)` → declared type of x.
 * • `decltype((x))` → always an lvalue reference (T&), because (x) is an lvalue expression,
 *   not an id-expression!
 * This subtle difference is a common source of bugs in return type deduction with
 * `decltype(auto)` — returning `(localVar)` gives a dangling reference.
 *
 * @note Compile: g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion 04_modern_types.cpp -o 04_modern_types
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <any>
#include <iostream>
#include <optional>
#include <string>
#include <string_view>
#include <variant>

// 1. std::optional (C++17) Example: How to securely handle "Data Not Found" without MAGIC
// NUMBERS?
std::optional<int> findAgeByName(const std::string &name) {
  if (name == "Alice")
    return 25;
  if (name == "Bob")
    return 30;

  // EN: Instead of -1, we return 'nullopt' (no value)!
  return std::nullopt;
}

// 2. std::string_view (C++17) Example: Preventing massive RAM copies line by line!
void printFirst5Chars(std::string_view text) {
  if (text.length() >= 5)
    std::cout << "[StringView] " << text.substr(0, 5) << "..." << std::endl;
}

int main() {
  std::cout << "=== MODULE 3: MODERN TYPES (C++17) ===\n" << std::endl;

  // --- TEST 1: std::optional ---
  std::cout << "--- 1. std::optional ---" << std::endl;
  // Not in database
  auto ageResult = findAgeByName("Charlie");

  if (ageResult.has_value()) { // or just: if (ageResult)
    std::cout << "Age Found: " << ageResult.value() << std::endl;
  } else {
    std::cout << "Person DOES NOT EXIST" << std::endl;
  }

  // --- TEST 2: std::variant ---
  std::cout << "\n--- 2. std::variant ---" << std::endl;
  // EN: An automotive diagnostic result. Can hold EITHER an int error code OR a string
  // description.
  std::variant<int, std::string> diagnosticResult;

  diagnosticResult = 300; // Int Mode (DTC numeric code)
  std::cout << "DTC Code: " << std::get<int>(diagnosticResult) << std::endl;

  // Switch to String immediately (Old data vanishes)
  diagnosticResult = "P0300 - Random Misfire";
  std::cout << "DTC Description: " << std::get<std::string>(diagnosticResult) <<
      std::endl;

  // --- TEST 3: std::any ---
  std::cout << "\n--- 3. std::any ---" << std::endl;
  // EN: Like 'var' in JS. Heavy performance hit. DO NOT USE unless desperate.
  std::any anything = 3.14;
  anything = std::string("Dynamic Word");
  std::cout << "Content: " << std::any_cast<std::string>(anything) <<
      std::endl;

  // --- TEST 4: std::string_view ---
  std::cout << "\n--- 4. std::string_view ---" << std::endl;
  const char *massiveText = "This is a very large text block. Copying it is suicide.";
  printFirst5Chars(massiveText);

  return 0;
}
