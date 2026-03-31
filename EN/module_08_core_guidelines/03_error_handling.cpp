/**
 * @file module_08_core_guidelines/03_error_handling.cpp
 * @brief C++ Core Guidelines: Error Handling
 *
 * @details
 * =============================================================================
 * [E.2 & E.3: Use Exceptions for Errors]
 * =============================================================================
 * EN: "Throw an exception to signal that a function can't perform its assigned task." C++ Core
 * Guidelines heavily prefers Exceptions (`throw std::runtime_error`) over returning Error Codes
 * (like `-1` or `false`). Error codes get ignored, leading to silent bugs. Exceptions CANNOT be
 * ignored unless explicitly caught (`catch`).
 *
 * =============================================================================
 * [E.15: Throw by value, catch by reference]
 * =============================================================================
 * EN: ALWAYS throw standard exceptions by value (`throw MyError();`) and catch them by const
 * reference (`catch (const MyError& e)`). This prevents "Object Slicing".
 *
 *
 * @note Compile: g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion 03_error_handling.cpp -o 03_error_handling
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

// EN: BAD PRACTICE: Using Error Codes (C-Style)
int openDatabaseOldWay(const std::string &address) {
  if (address.empty())
    return -1; // -1 means FATAL ERROR
  return 0;    // 0 means Success
}
// EN: GOOD PRACTICE: Using Exceptions (C++ Core Guidelines E.2)
void openDatabaseModern(const std::string &address) {
  if (address.empty()) {
    // EN: E.15: Throw by value!
    throw std::invalid_argument(
        "FATAL: Database address cannot be empty!");
  }
  std::cout << "[Database] Successfully connected to: " << address << std::endl;
}

// EN: [[nodiscard]] makes the compiler WARN if the return value is ignored.
[[nodiscard]] int getErrorCode() {
  return -1; // EN: Simulated error code
}

int main() {
  std::cout << "=== MODULE 8: C++ CORE GUIDELINES - ERROR HANDLING ===\n" << std::endl;

  // 1. DANGER OF ERROR CODES
  // EN: I forgot to check the return value (-1). My program continues
  openDatabaseOldWay( "");

  // 2. SAFETY OF EXCEPTIONS
  try {
    // EN: This will immediately blast an error and JUMP to catch!
    openDatabaseModern("");
    std::cout << "This line will NEVER be printed." << std::endl;
  }
  // EN: CABLE-RULE (E.15): CATCH BY CONST REFERENCE! (Referans ile Yakala)
  catch (const std::invalid_argument &e) {
    std::cerr << "[Core Guidelines Exception Catcher] -> " << e.what() << std::endl;
  }

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // [E.12 & E.16: Destructors and noexcept]
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // EN: Destructors (`~MyClass()`) and memory deallocation functions MUST NEVER THROW
  // EXCEPTIONS! If an exception happens inside a destructor during another exception unwinding,
  // the C++ runtime immediately terminates (`std::terminate`).
  //
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  //
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // [CPPREF DEPTH: The Brutality of `std::terminate()`]
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // EN: `std::terminate()` is the absolute end of the line. It doesn't call destructors, it
  // doesn't free memory, it doesn't close files. It violently kills the process from the OS
  // level (`abort()`). This happens if: 1. You throw an exception and no `catch` block exists
  // anywhere to catch it. 2. An exception throws INSIDE a destructor while another exception is
  // currently flying (Stack Unwinding collision). 3. A `noexcept` function explicitly throws.
  //
  // ═══════════════════════════════════════════════════════════════════════════════════════════════

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // EN: E.6 — Use RAII to prevent leaks in error paths
  // ═══════════════════════════════════════════════════════════════════════════════════════════════

  // ─── 1. BAD: Raw pointer leaks on exception ────────────────────────────────────────────────────
  // EN: If process() throws, the delete never runs → memory leak.
  //
  // int* data = new int[1024]; process(data);      // If this throws... delete[] data;      //
  // ...this line is NEVER reached! LEAK!

  // ─── 2. GOOD: unique_ptr + exception = no leak ─────────────────────────────────────────────────
  // EN: unique_ptr guarantees cleanup even when exceptions fly.
  try {
    auto safeData = std::make_unique<int[]>(1024);
    safeData[0] = 42;
    std::cout << "[E.6 GOOD] safeData[0] = " << safeData[0] << std::endl;

    // EN: Simulate an error after allocation
    throw std::runtime_error("Simulated processing error!");

    // EN: This line is skipped, but unique_ptr still frees memory.
  } catch (const std::runtime_error& e) {
    std::cerr << "[E.6 Caught] " << e.what()
              << " (unique_ptr freed memory automatically!)" << std::endl;
  }

  // ─── 3. [[nodiscard]]: Force callers to check return values ────────────────────────────────────
  // EN: [[nodiscard]] makes the compiler WARN if the return value is ignored.
  // (getErrorCode() defined above main as a [[nodiscard]] free function)

  // EN: Good — caller checks the result. Compiler is happy.
  int errCode = getErrorCode();
  if (errCode != 0) {
    std::cout << "[nodiscard Demo] Error code received: " << errCode << std::endl;
  }

  // EN: Bad — ignoring the return would trigger a compiler warning:
  // getErrorCode();  // WARNING: ignoring return value with [[nodiscard]]

  return 0;
}

