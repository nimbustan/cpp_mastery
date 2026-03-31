/**
 * @file module_05_certification/01_undefined_behavior.cpp
 * @brief CPA/CPP Prep: Undefined Behavior & Tricky Questions
 *
 * @details
 * =============================================================================
 * [THEORY: Why do C++ Institute and CppQuiz ask such hard questions?]
 * =============================================================================
 * EN: In C++, unlike managed languages like Java or C#, the compiler assumes YOU know exactly
 * what you are doing. If you access memory out of bounds or use a pointer after it's deleted,
 * C++ DOES NOT stop you. It results in "Undefined Behavior" (UB). Certification exams test your
 * ability to spot these hidden traps.
 *
 * =============================================================================
 * [THEORY: Examples of Undefined Behavior]
 * =============================================================================
 * EN:
 *   1. Dangling Pointers: Pointing to an address that has already been freed.
 *   2. Out of Bounds: Reading the 10th element of a 5-element array.
 *   3. Uninitialized Variables: Reading a local variable containing garbage memory.
 *   4. Signed Integer Overflow: Adding 1 to the absolute MAX value of an `int`.
 *
 * =============================================================================
 * [CPPREF DEPTH: Unsequenced Modification HACK!]
 * =============================================================================
 * EN: CppReference explicitly forbids modifying a variable and reading it twice in the same
 * sequence point (e.g., `cout << i++ << ++i;`). The C++ standard DOES NOT GUARANTEE which one
 * executes first. Depending on the compiler (GCC vs Clang), it might output completely different
 * numbers, or crash!
 *
 * @note Compile: g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion 01_undefined_behavior.cpp -o 01_undefined_behavior
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <iostream>

void testDanglingPointer() {
  std::cout << "\n--- Danger Zone: Dangling Pointer ---" << std::endl;
  int *ptr = new int(100);
  std::cout << "Original Value: " << *ptr << std::endl;

  // EN: Memory is freed.
  delete ptr;

  // DANGER!
  // EN: 'ptr' still points to that address, but it belongs to the OS now.
  //
  // *ptr = 500; // UNDEFINED BEHAVIOR! (Do not uncomment!)

  // Correct Way:
  // EN: Safer!
  ptr = nullptr;
  std::cout << "Pointer safely nullified." << std::endl;
}

void testUninitializedVariable() {
  std::cout << "\n--- Danger Zone: Uninitialized Variable ---" << std::endl;
  // EN: Memory allocated, but holds generic GARBAGE.
  int mysteriousNumber;

  // std::cout << "Value: " << mysteriousNumber << std::endl; // UNDEFINED BEHAVIOR: Just reading
  // it is UB!
  // EN: Now initialized.
  mysteriousNumber = 42;
  std::cout << "Initialized safely: " << mysteriousNumber << std::endl;
}

int main() {
  std::cout << "=== MODULE 5: CERTIFICATION & CPPQUIZ TRAPS ===\n" << std::endl;
  std::cout << "EN: Exams ask you to READ code and spot hidden DANGER ZONES, "
               "not write them!"
            << std::endl;

  testDanglingPointer();
  testUninitializedVariable();

  // 3. Trap: Object Slicing
  class Base {
  public:
    virtual void print() { std::cout << "Base"; }
  };
  class Derived : public Base {
  public:
    void print() override { std::cout << "Derived"; }
  };

  Derived myDerived;

  // EN: ERROR! Specific traits were sliced off! (Object Slicing)
  Base slicedBase = myDerived;

  std::cout << "\nObject Slicing Test: ";
  // EN: Prints "Base"! Polymorphism FAILED.
  slicedBase.print();

  std::cout << " (Polymorphism FAILED due to Value Copy. Reference was needed!)" << std::endl;

  return 0;
}
