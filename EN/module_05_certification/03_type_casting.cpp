/**
 * @file module_05_certification/03_type_casting.cpp
 * @brief CPA/CPP Prep: Type Casting
 *
 * @details
 * =============================================================================
 * [THEORY: Why are C-Style Casts (int) banned in Modern C++?]
 * =============================================================================
 * EN: In C, you could force any type into any other type using `(int) x` or `(char*) ptr`. This
 * tells the compiler: "Shut up, I know what I'm doing." But it's violent. It breaks constness,
 * converts pointers blindly, and causes severe run-time crashes. C++ introduced 4 safe,
 * intentional, and searchable cast operators.
 *
 * =============================================================================
 * [THEORY: The 4 Modern C++ Casts]
 * =============================================================================
 *
 * EN: C++ replaced the dangerous C-style `(int)x` cast with 4 explicit, searchable, and
 * intentional cast operators. Each has a specific purpose:
 *
 * ─────────────────────────────────────────────────────────────────────────────
 * 1. static_cast<Type>(val)
 * ─────────────────────────────────────────────────────────────────────────────
 * The "logical/ideal" cast. Used for well-defined conversions like float → int, int → enum,
 * base* → derived* (when you know the type). Checked at COMPILE-TIME. Zero runtime cost. This is
 * the cast you use 95% of the time.
 *
 * ─────────────────────────────────────────────────────────────────────────────
 * 2. dynamic_cast<Type>(val)
 * ─────────────────────────────────────────────────────────────────────────────
 * The "safe polymorphism" cast. Used to safely downcast a base class pointer/reference to a
 * derived class. Checks the V-Table at RUN-TIME (RTTI). If the cast is invalid, returns nullptr
 * (pointer) or throws std::bad_cast (reference). Slower than static_cast but SAFE.
 *
 * ─────────────────────────────────────────────────────────────────────────────
 * 3. const_cast<Type>(val)
 * ─────────────────────────────────────────────────────────────────────────────
 * Strips away `const` or `volatile` qualifiers. Used ONLY when interfacing with old C APIs
 * that forgot to mark parameters as `const`. Modifying a truly const object through const_cast
 * is UNDEFINED BEHAVIOR. Use with extreme caution!
 *
 * ─────────────────────────────────────────────────────────────────────────────
 * 4. reinterpret_cast<Type>(val)
 * ─────────────────────────────────────────────────────────────────────────────
 * The "brute force" cast. Reinterprets the raw bit pattern of one type as another. No
 * checks, no safety net. Used almost exclusively in low-level hardware/driver programming and
 * memory-mapped I/O. Casting unrelated pointer types and dereferencing them triggers Strict
 * Aliasing UB. Only safe targets: `char*`, `unsigned char*`, `std::byte*`. FORBIDDEN in
 * application-level code!
 *
 * =============================================================================
 * [CPPREF DEPTH: `dynamic_cast` Cost & `reinterpret_cast` Strict Aliasing UB]
 * =============================================================================
 * EN: `dynamic_cast` performs a string-search on the Run-Time Type Information (RTTI) tree,
 * making it EXTREMELY SLOW compared to `static_cast`. Use it sparingly! Also, `reinterpret_cast`
 * frequently triggers "Strict Aliasing" Undefined Behavior if you cast two unrelated pointers
 * and dereference them. Only cast to `char*` or `std::byte*`.
 *
 * @note Compile: g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion 03_type_casting.cpp -o 03_type_casting
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <iostream>

class Organism {
public:
  virtual ~Organism() {}
// EN: Virtual Destructor REQUIRED for V-Table (Polymorphism)
};

class Human : public Organism {
public:
  void run() { std::cout << "Human is running!" << std::endl; }
};

class Plant : public Organism {};

void cStyle_VS_staticCast() {
  double pi = 3.14159;

  // EN: Old C-way (Bad Habit) - Unclear intention and unsearchable.
  int oldInt = (int)pi;

  // EN: Modern C++ way (static_cast) - Explicit intention. Compiler verifies it.
  int modernInt = static_cast<int>(pi);

  std::cout << "C-Style cast: " << oldInt << " | static_cast: " << modernInt << std::endl;
}

void unlockDynamicCast() {
  Organism *unknownEntity1 = new Human();
  Organism *unknownEntity2 = new Plant();

  std::cout << "\n--- DYNAMIC CAST (Polymorphic Safety) ---" << std::endl;

  // EN: [INTERVIEW QUESTION]: I have an Organism*. If it's truly a Human, I want to make it run.
  // If it's a Plant, I do nothing. How do I do this without crashing?

  // EN: dynamic_cast queries the V-Table! If it matches `Human*`, it returns the pointer. Else,
  // `nullptr`.

  Human *isReallyHuman1 = dynamic_cast<Human *>(unknownEntity1);
  // EN: C++17 allows "if (auto p = dynamic_cast...)"
  if (isReallyHuman1 != nullptr) {
    std::cout << "[SUCCESS] 1st Entity is Human!" << std::endl;
    isReallyHuman1->run();
  }

  Human *isReallyHuman2 = dynamic_cast<Human *>(unknownEntity2);
  if (isReallyHuman2 == nullptr) {
    std::cout << "[REJECTED] 2nd Entity is NOT Human (Probably Plant). "
           "CRASH PREVENTED!"
        << std::endl;
  }

  delete unknownEntity1;
  delete unknownEntity2;
}

int main() {
  std::cout << "=== MODULE 5: MODERN TYPE CASTING ===\n" << std::endl;

  unlockDynamicCast();

  std::cout << "\n--- reinterpret_cast (Danger Zone) ---" << std::endl;
  // EN: Do not use this unless hacking hardware/drivers. It tells the OS to treat a meaningless
  // number as a RAM address.

  long hardwareAddress = 0x7FFAABBCCDD;
  int *fakePointer = reinterpret_cast<int *>(hardwareAddress);

  std::cout << "Integer masked as a Pointer: " << fakePointer <<
      " (If you dereference this, PROGRAM CRASHES!)" << std::endl;

  return 0;
}
