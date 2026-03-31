/**
 * @file module_01_basics/01_variables_and_memory.cpp
 * @brief Variables, Data Types, and Memory Model
 *
 * @details
 * =============================================================================
 * [THEORY: Primitive Data Types & Memory]
 * =============================================================================
 * EN: Every variable in C++ is a named region of memory. When you write `int age = 30;`, the
 * compiler reserves 4 bytes on the Stack, names that region "age", and writes the binary
 * representation of 30 into it. The Stack is a LIFO (Last-In, First-Out) memory region that
 * grows/shrinks automatically as functions are called/returned. Local variables live here — they
 * are born when their scope `{` begins, and die when `}` ends. The Heap is a separate region
 * used for dynamic allocation (`new`/`delete`) where objects survive beyond function scope.
 * Understanding WHERE a variable lives (Stack vs Heap) is the foundation of all C++ memory
 * management.
 *
 * C++ defines several primitive data types, each consuming a specific amount of memory. The
 * `sizeof()` operator reveals the exact byte count at compile-time. Choosing the correct type
 * directly affects memory usage, performance, and correctness — a `float` where a `double` is
 * needed causes silent precision loss; an `int` where a `long long` is needed causes overflow.
 *
 * =============================================================================
 * [THEORY: Stack vs Heap — Where Do Variables Live?]
 * =============================================================================
 * EN: The Stack is fast (allocation is just moving a pointer), automatically managed, and limited
 * in size (typically 1-8 MB). The Heap is slower (OS must find free blocks), manually managed
 * (you must `delete` what you `new`), but virtually unlimited. Rule of thumb:
 *   1. Use Stack for small, short-lived variables (int, float, small arrays).
 *   2. Use Heap for large data or data that must outlive the current scope.
 *   3. In Modern C++, prefer smart pointers (unique_ptr, shared_ptr) over raw `new`/`delete`.
 *
 * =============================================================================
 * [CPPREF DEPTH: Undefined Behavior (UB) & Strict Aliasing]
 * =============================================================================
 * EN: cppreference.com explicitly warns: "Reading an uninitialized block of memory is Undefined
 * Behavior (UB)." Also, casting a `float*` to an `int*` violates the "Strict Aliasing Rule" in
 * C++, allowing the optimizer to silently break your firmware.
 *
 * @note Compile: g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion 01_variables_and_memory.cpp -o 01_variables_and_memory
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

// Fixed width integer types
#include <cstdint>
#include <iostream>

int main() {
  std::cout << "--- C++ Primitive Data Types & Memory ---\n" << std::endl;

  // 1. Integer Types
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // EN: The C++ standard does NOT guarantee the size of `int` — it only guarantees minimum
  // ranges. On most 64-bit systems `int` is 4 bytes, but on some embedded platforms it could
  // be 2 bytes. For portable, safety-critical code (MISRA, AUTOSAR), ALWAYS use fixed-width
  // types from <cstdint>: int8_t, int16_t, int32_t, int64_t.

  int age = 30; // 4 bytes on most systems (not guaranteed!)
  std::cout << "int takes: " << sizeof(int) << " bytes. Example: " << age << std::endl;

  // [CPPREF DEPTH: Uninitialized Variables]
  // EN: DANGER (UB)! Reading an uninitialized local variable is one of the most common bugs in
  // C++. The variable holds whatever garbage data was left at that RAM address from a previous
  // function call. The program may work in debug mode but crash in release mode because the
  // optimizer assumes no UB and makes aggressive optimizations based on that assumption.
  int uninitializedAge;
  // EN: Suppress warning. Reading it is UB!
  (void)uninitializedAge;
  // EN: Doing this causes Undefined Behavior. std::cout << uninitializedAge;

  // Fixed width
  int16_t smallNumber = 32767; // Guaranteed 2 bytes (16 bits)
  // Guaranteed 4 bytes (32 bits)
  int32_t standardNumber = 2147483647;

  std::cout << "int16_t takes: " << sizeof(int16_t) << " bytes. Value: " << smallNumber <<
      std::endl;
  std::cout << "int32_t takes: " << sizeof(int32_t) << " bytes. Value: " << standardNumber << "\n"
      << std::endl;

  // 2. Floating Point Types
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // EN: Floating-point numbers follow the IEEE 754 standard. `float` uses 32 bits (23-bit
  // mantissa = ~7 decimal digits of precision). `double` uses 64 bits (52-bit mantissa = ~15
  // decimal digits). NEVER compare floats with `==` because of rounding errors! Use an epsilon:
  // `std::abs(a - b) < 1e-9`. In financial software, avoid floats entirely — use integer cents.

  float piFloat = 3.14159f;         // 'f' suffix = float literal
  double piDouble = 3.14159265359;  // Default: double (higher precision)

  std::cout << "float takes: " << sizeof(float) << " bytes. Value: " << piFloat << std::endl;
  std::cout << "double takes: " << sizeof(double) << " bytes. Value: " << piDouble << "\n" <<
      std::endl;

  // 3. Characters and Booleans
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // EN: `char` is actually a 1-byte INTEGER (0-255 or -128 to 127). It stores an ASCII code,
  // not a "letter". 'A' == 65 in memory. This is why you can do math with chars:
  // `char lower = 'A' + 32;` gives 'a'. `bool` is also an integer: `true` == 1, `false` == 0.
  // In conditions, ANY non-zero value is truthy: `if (42)` is true, `if (0)` is false.

  char grade = 'A';      // Stores ASCII 65, not the letter
  bool isCppFun = true;  // Stored as integer 1

  std::cout << "char takes: " << sizeof(char) << " byte. Value: " << grade << std::endl;
  std::cout << "bool takes: " << sizeof(bool) << " byte. Value: " << isCppFun << "\n" << std::endl;

  // 4. Constants
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // EN: `const` makes a variable read-only AFTER initialization — the value is set at runtime
  // and cannot be changed afterward. `constexpr` is stronger: the value MUST be computable at
  // COMPILE-TIME. The compiler embeds it directly into the binary as a literal constant — no
  // memory is allocated, no runtime cost. Prefer `constexpr` over `#define` macros: it is
  // type-safe, scoped, and debuggable. In Modern C++ (C++17+), `constexpr` can even be used
  // with `if constexpr` for compile-time branching.

  const int MAX_USERS = 100;  // Runtime constant

  // EN: constexpr came with C++11. The compiler evaluates the expression at compile-time and
  // embeds the result directly. No CPU cycles are wasted at runtime calculating this.
  constexpr int SECONDS_IN_MIN = 60;
  constexpr int MINS_IN_HOUR = 60;
  // Compiler hardcodes this as 3600!
  constexpr int SECONDS_IN_HOUR = SECONDS_IN_MIN * MINS_IN_HOUR;

  std::cout << "Constants:" << std::endl;
  std::cout << "MAX_USERS: " << MAX_USERS << std::endl;
  std::cout << "SECONDS_IN_HOUR (constexpr): " << SECONDS_IN_HOUR << "\n" << std::endl;

  return 0;
}
