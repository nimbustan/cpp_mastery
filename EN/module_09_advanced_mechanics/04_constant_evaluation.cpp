/**
 * @file module_09_advanced_mechanics/04_constant_evaluation.cpp
 * @brief Advanced Mechanics: C++20 Constants (constexpr vs consteval vs constinit)
 *
 * @details
 * =============================================================================
 * [1. constexpr]
 * =============================================================================
 * EN: A `constexpr` function CAN be evaluated at compile-time (if all inputs are known). But if
 * inputs are dynamic (from user), it naturally degrades to a normal run-time function! It is
 * basically saying: "Compiler, please do this beforehand IF you can."
 *
 *
 * =============================================================================
 * [2. consteval]
 * =============================================================================
 * EN: A `consteval` function (Immediate Function) MUST strictly be evaluated at compile-time! If
 * the compiler cannot determine the inputs beforehand, it throws a HARD BUILD ERROR. Use this
 * when you absolutely CANNOT afford calculation overhead in the final binary execution.
 *
 *
 * =============================================================================
 * [3. constinit]
 * =============================================================================
 * EN: The `constinit` keyword guarantees that a static/global variable is fully initialized
 * during compilation (Constant Initialization). It solves the infamous "Static Initialization
 * Order Fiasco" in massive C++ engines. The variable CAN be mutable (changeable) later!
 *
 *
 * [CPPREF DEPTH: constexpr vs consteval vs constinit — Compile-Time Guarantees]
 * =============================================================================
 * EN: `constexpr` (C++11): function/variable CAN be evaluated at compile time if all inputs are
 * constant; otherwise degrades to runtime. `consteval` (C++20): "immediate function" — MUST be
 * evaluated at compile time; runtime call is a hard error. `constinit` (C++20): the variable
 * must be initialized at compile time, but may be modified at runtime — solves the static
 * initialization order fiasco. `if consteval` (C++23) tests whether execution is currently in a
 * constexpr evaluation context. Constexpr dynamic allocations (C++20): `new`/`delete` are
 * allowed inside constexpr if fully deallocated by the end of evaluation.
 *
 *
 * @note Compile: g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion 04_constant_evaluation.cpp -o 04_constant_evaluation
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <iostream>

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// [constexpr] function (Flex / Esnek)
// ═════════════════════════════════════════════════════════════════════════════════════════════════
constexpr int squareConstexpr(int n) { return n * n; }

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// [consteval] function (Strict Compile-Time
// ═════════════════════════════════════════════════════════════════════════════════════════════════
// C++20 Feature! Only available if compiled with -std=c++20
consteval int squareConsteval(int n) {
  return n * n; // Strictly must be replaced by the answer before ECU boots!
}

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// [constinit] Variable (No Runtime startup cost, but Mutable)
// ═════════════════════════════════════════════════════════════════════════════════════════════════
// Calculated at compile time (100).
constinit int GlobalConfigurationScale = squareConstexpr(10);
// Notice it is an `int` not `const int`. It can be mutated later!

int main() {
  std::cout << "=== MODULE 9: CONSTANTS DEEP DIVE (C++20) ===\n" << std::endl;

  // --- 1. constexpr (ESNEK TEST) ---
  constexpr int compileTimeResult = squareConstexpr(5);

  int dynamicVar = 8;
  int runTimeResult = squareConstexpr(dynamicVar);

  // --- 2. consteval ---
  int immediateResult = squareConsteval(6); // 36 is baked directly!

  std::cout << "constexpr(5)=" << compileTimeResult << " | constexpr(" << dynamicVar << ")=" <<
      runTimeResult << " | consteval(6)=" << immediateResult << std::endl;

  // [FATAL ERROR]: This would completely crash the compilation! int
  // failedAttempt = squareConsteval(dynamicVar);
  // EN: The compiler cannot know what `dynamicVar` is at compile time, so `consteval` throws an
  // error!

  // --- 3. constinit (GLOBAL/STATIC TEST) ---
  std::cout << "Original constinit global Config Scale: " << GlobalConfigurationScale << std::endl;
  // EN: Still mutable! It just started without runtime cost.
  GlobalConfigurationScale = 450;
  std::cout << "Mutated Global Config Scale           : " << GlobalConfigurationScale << std::endl;

  std::cout << "\n[ARCHITECTURAL RECAP]:" << std::endl;
  std::cout << "1. Use 'constexpr' by default. (It falls back to runtime "
               "gently)."
            << std::endl;
  std::cout << "2. Use 'consteval' when overhead is forbidden."
            << std::endl;
  std::cout << "3. Use 'constinit' on Globals/Statics to prevent startup "
               "fiascos."
            << std::endl;

  return 0;
}
