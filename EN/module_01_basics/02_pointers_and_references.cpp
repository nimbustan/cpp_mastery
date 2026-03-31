/**
 * @file module_01_basics/02_pointers_and_references.cpp
 * @brief Basics: Pointers, References, and Memory Addresses
 *
 * @details
 * =============================================================================
 * [THEORY: Why Do We Need Pointers (*) and References (&)?]
 * =============================================================================
 * EN: When you pass a variable to a function by default in C++, it makes a FULL COPY of that
 * variable (Pass by Value). If you pass a 1GB Image file to a function, the computer freezes
 * while duplicating 1GB in RAM! Pointers (*) and References (&) solve this by passing the
 * "Memory Address". You just give the function the GPS coordinates; it goes there and modifies
 * the original.
 *
 * =============================================================================
 * [THEORY: Pointer (*) vs Reference (&)]
 * =============================================================================
 * EN:
 *   1. POINTER (*): Inherited from C. Can be NULL/nullptr. Flexible, can be REASSIGNED to
 *      point to another object during runtime. Can be dangerous.
 *   2. REFERENCE (&): Introduced in C++. CAN NEVER be NULL (Must be initialized when declared).
 *      Safer and cleaner syntax. Once initialized, it gets LOCKED and cannot point to anything
 *      else.
 *
 * =============================================================================
 * [CPPREF DEPTH: Dangling References & Pointer Decay]
 * =============================================================================
 * EN: Returning a reference or pointer to a LOCAL variable from a function is a catastrophic
 * Undefined Behavior (Dangling Pointer). Because local variables die on the Stack when the
 * function ends. Also, arrays naturally "decay" into a pointer to their first element.
 *
 * @note Compile: g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion 02_pointers_and_references.cpp -o 02_pointers_and_references
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <iostream>

// 1. PASS BY VALUE - Bad practice for large data
void swapByValue(int a, int b) {
  int temp = a;
  a = b;
  b = temp;
  // EN: The original values in main() WILL NOT CHANGE because 'a' and 'b' are just copies.
}

// 2. PASS BY POINTER - C style (dangerous and verbose syntax)
void swapByPointer(int *ptrA, int *ptrB) {
  if (ptrA == nullptr || ptrB == nullptr)
    return; // Safety check is mandatory!

  // * operator (Dereference): Access the VALUE at the address!
  int temp = *ptrA;
  *ptrA = *ptrB;
  *ptrB = temp;
}

// 3. PASS BY REFERENCE - The heart of Modern C++!
void swapByReference(int &refA, int &refB) {
  // EN: No null check needed. Cleaner syntax.
  int temp = refA;
  refA = refB;
  refB = temp;
}

// [CPPREF DEPTH: UB (Undefined Behavior) Example]
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreturn-local-addr"
int *generateDanglingPointer_DO_NOT_DO_THIS() {
  int localBomb = 100;
  // EN: WARNING! Returning address of stack memory!
  return &localBomb; // NOLINT — intentional UB demo
}
#pragma GCC diagnostic pop

int main() {
  std::cout << "=== MODULE 1: POINTERS & REFERENCES ===\n" << std::endl;

  int num1 = 10;
  int num2 = 99;

  std::cout << "[RAM COORDINATES / ADDRESSES]" << std::endl;
  std::cout << "Address of num1 (&): " << &num1 << " | Value: " << num1 << std::endl;

  // 1. Test: COPY (Won't work!)
  swapByValue(num1, num2);
  std::cout << "\n[TEST] After swapByValue (Copy): num1 = " << num1 << ", num2 = " << num2 <<
      std::endl;

  // 2. Test: POINTER (Will work but ugly!) Passing addresses instead of values
  swapByPointer(&num1, &num2);
  std::cout << "[TEST] After swapByPointer (Pointer): num1 = " << num1 << ", num2 = " << num2 <<
      std::endl;

  // 3. Test: REFERENCE (Reverting using elegant C++ way)
  // Syntactically passed like values, but actually passed by reference!
  swapByReference(num1, num2);
  std::cout << "[TEST] After swapByReference (Reference): num1 = " << num1 << ", num2 = " << num2
      << std::endl;

  // [Interview Question]: Const Reference
  // EN: "How do I pass a massive object without COPYING it, but also ensuring the function
  // CANNOT MODIFY it?"
  // Answer: `const std::string& hugeData` — No copy (RAM saved) & Read-Only.

  return 0;
}
