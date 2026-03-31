/**
 * @file module_01_basics/09_scope_namespace_recursion.cpp
 * @brief Basics: Scope, Lifetime, Namespace & Recursion
 *
 * @details
 * =============================================================================
 * [THEORY: Scope & Lifetime]
 * =============================================================================
 * EN: Variables live in "scopes" — regions delimited by `{ }`. A variable declared inside a
 * block is INVISIBLE outside. When the closing `}` is reached, the variable is DESTROYED (stack
 * unwinding). Key scopes:
 *     1. Global scope: Declared outside all functions. Lives for the whole program.
 *     2. Function/Local scope: Inside a function or block `{ }`.
 *     3. `static` local: Initialized once, persists across calls (like a global
 *        but scoped to the function). Useful for caching sensor calibration data.
 *
 * =============================================================================
 * [THEORY: Namespaces]
 * =============================================================================
 * EN: Namespaces prevent name collisions in large codebases. Without them, two libraries
 * defining `init()` would cause a linker error. Use `::` to access members. `using namespace
 * std;` pollutes the global scope — avoid it.
 *
 * =============================================================================
 * [CPPREF DEPTH: Recursion & Stack Overflow]
 * =============================================================================
 * EN: Recursion means a function calls ITSELF. Every call pushes a new frame onto the call
 * stack. Without a proper "base case", the stack overflows (SIGSEGV). Stack size is typically
 * 1-8 MB. For deeply recursive algorithms, prefer iterative solutions or tail-call optimization
 * hints.
 *
 * ⚠  Ref: https://en.cppreference.com/w/cpp/language/scope
 * ⚠  Ref: https://en.cppreference.com/w/cpp/language/namespace
 *
 * @note Compile: g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion 09_scope_namespace_recursion.cpp -o 09_scope_namespace_recursion
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <iostream>
#include <string>

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// 1. GLOBAL SCOPE VARIABLE
// ═════════════════════════════════════════════════════════════════════════════════════════════════
// EN: Global variables are declared OUTSIDE all functions. They live in the DATA segment
//     of the process memory (not on the stack). Their lifetime = entire program duration.
//     They are ZERO-INITIALIZED by default (unlike local variables which are GARBAGE).
//
//     DANGER: In multi-file projects, the initialization ORDER of globals across
//     different .cpp files is UNDEFINED ("Static Initialization Order Fiasco").
//     File A's global might use File B's global before B is initialized → UB.
//     Solution: Use function-local statics ("Meyers Singleton" idiom) instead.
//
//     `const` globals are fine because they are immutable and typically optimized
//     into read-only memory by the compiler.
//
const std::string FIRMWARE_VERSION = "v3.2.1";

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// 2. NAMESPACES
// ═════════════════════════════════════════════════════════════════════════════════════════════════
// EN: Namespaces group related code under a unique name to prevent collisions.
//     Without namespaces, if Powertrain and BodyControl both define `init()`,
//     the linker would see TWO definitions of the same function → linker error.
//     With namespaces, they become `Powertrain::init()` and `BodyControl::init()`
//     — two completely separate functions.
//
//     The `::` operator is called the SCOPE RESOLUTION OPERATOR. It tells the
//     compiler exactly which namespace's function you mean.
//
//     WHY `using namespace std;` IS DANGEROUS:
//     `std` contains thousands of names (min, max, count, distance, move, swap...).
//     `using namespace std;` dumps ALL of them into your scope. If you define
//     your own `count` variable, it silently collides with `std::count`.
//     In header files, this is ESPECIALLY catastrophic — every file that #includes
//     your header inherits the pollution. ALWAYS use `std::` prefix instead.
//
namespace Powertrain {
void init() {
  std::cout << "  [Powertrain] Engine management initialized." << std::endl;
}
int getStatus() { return 1; } // 1 = OK
} // namespace Powertrain

namespace BodyControl {
void init() {
  std::cout << "  [BodyControl] Door/window/lighting initialized." << std::endl;
}
int getStatus() { return 1; } // 1 = OK
} // namespace BodyControl

// EN: Nested namespace — C++17 shorthand syntax. Before C++17 you had to write:
//         namespace Vehicle { namespace Diagnostics { ... } }
//     C++17 allows the compact `Vehicle::Diagnostics` form.
//     Nested namespaces are common in large projects: `company::product::module`.
namespace Vehicle::Diagnostics {
void scanAll() {
  std::cout << "  [Vehicle::Diagnostics] Full OBD-II scan complete."
            << std::endl;
}
} // namespace Vehicle::Diagnostics

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// 3. STATIC LOCAL VARIABLE
// ═════════════════════════════════════════════════════════════════════════════════════════════════
// EN: A function-local `static` variable combines two properties:
//     1. LOCAL SCOPE: Only visible inside this function (encapsulated).
//     2. STATIC LIFETIME: Initialized the FIRST time the function is called,
//        then PERSISTS in memory until the program ends (not destroyed on return).
//     It lives in the DATA segment (like a global), but its NAME is only
//     visible inside the function (unlike a global).
//
//     C++11 GUARANTEE: Initialization of function-local statics is THREAD-SAFE.
//     If two threads call readSensor() simultaneously for the first time, only
//     ONE thread initializes readCount — the other waits. ("Magic statics").
//
//     Use cases: call counters, lazy-initialized caches, Meyers Singleton.
//
void readSensor() {
  // EN: `static int readCount = 0;` — this line executes ONLY ONCE (first call).
  //     On subsequent calls, it is SKIPPED. readCount retains its value from
  //     the previous call. Without `static`, readCount would be 0 every time.
  static int readCount = 0;
  ++readCount;
  std::cout << "  Sensor read #" << readCount << " (static counter)"
            << std::endl;
}

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// 4. RECURSION
// ═════════════════════════════════════════════════════════════════════════════════════════════════
// EN: Factorial — the "hello world" of recursion.
//     factorial(5) → 5 × factorial(4) → 5 × 4 × factorial(3) → ... → 5×4×3×2×1 = 120
//
//     Every recursive function MUST have:
//     1. BASE CASE: The condition that STOPS recursion (here: n <= 1 → return 1).
//        Without it, the function calls itself forever → stack overflow (SIGSEGV).
//     2. RECURSIVE CASE: The call that makes the problem SMALLER (here: n-1).
//        Each call MUST move toward the base case, otherwise → infinite recursion.
//
//     HOW IT WORKS IN MEMORY:
//     Each call pushes a new STACK FRAME containing: parameters, local variables,
//     and the return address. factorial(5) creates 5 frames on the stack.
//     When the base case returns, frames are POPPED one by one (unwinding),
//     multiplying the results back up the chain.
//
int factorial(int n) {
  if (n <= 1) return 1;       // Base case
  return n * factorial(n - 1); // Recursive case
}

// EN: Fibonacci — demonstrates the DANGER of naive recursion.
//     Sequence: 0, 1, 1, 2, 3, 5, 8, 13, 21, 34, 55...
//     Each number = sum of the two before it: fib(n) = fib(n-1) + fib(n-2).
//
//     PROBLEM: This implementation has O(2^n) time complexity!
//     fibonacci(40) makes over 200 MILLION calls. fibonacci(6) alone creates
//     this call tree:  fib(6) → fib(5) + fib(4)
//                      fib(5) → fib(4) + fib(3)    ← fib(4) computed AGAIN!
//     The same sub-problems are recalculated exponentially many times.
//
//     SOLUTIONS (covered in later modules):
//     1. Memoization: Cache results in a map/array → O(n) time, O(n) space.
//     2. Iterative: Use a simple loop with two variables → O(n) time, O(1) space.
//     3. Dynamic Programming: Bottom-up table filling.
//
int fibonacci(int n) {
  if (n <= 0) return 0;  // Base case 1
  if (n == 1) return 1;  // Base case 2
  return fibonacci(n - 1) + fibonacci(n - 2);  // Two recursive calls!
}

// EN: TAIL RECURSION example — the recursive call is the LAST operation.
//     Some compilers can optimize this into a loop (no extra stack frames).
//     This eliminates stack overflow risk for deep recursion.
//     GCC with -O2 can perform Tail Call Optimization (TCO) on this pattern.
void shutdownCountdown(int n) {
  if (n < 0) return;  // Base case: countdown finished
  std::cout << "  Shutdown in: " << n << "..." << std::endl;
  shutdownCountdown(n - 1);  // Tail call: nothing happens after this returns
}

int main() {
  std::cout << "=== MODULE 1: SCOPE, NAMESPACE & RECURSION ===\n" << std::endl;

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // 1. SCOPE DEMONSTRATION
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  std::cout << "--- 1. Scope & Lifetime ---" << std::endl;

  // EN: Global variables are accessible from ANY function in this file.
  //     FIRMWARE_VERSION was declared at file scope (line ~65) and lives
  //     until the program terminates. It's `const`, so it cannot be modified.
  std::cout << "Global firmware version: " << FIRMWARE_VERSION << std::endl;

  {
    // EN: The `{ }` braces create a NEW SCOPE (block scope). Any variable
    //     declared here is BORN at this point and DIES at the closing `}`.
    //     The compiler DEALLOCATES the stack space automatically.
    //     This is useful for limiting variable lifetime and freeing resources early.
    int localRpm = 3500;
    std::cout << "Inside block: localRpm = " << localRpm << std::endl;
  }
  // EN: localRpm is DESTROYED here. Trying to use it would cause:
  //         error: 'localRpm' was not declared in this scope
  //     This is NOT a runtime error — it's a COMPILE-TIME error. The compiler
  //     enforces scope rules and prevents you from accessing dead variables.

  // EN: VARIABLE SHADOWING — when an inner scope declares a variable with the
  //     same name as an outer scope variable, the inner one "shadows" (hides) it.
  //     The outer variable still exists but is UNREACHABLE by that name.
  //     Our -Wshadow flag warns about this, so we use different names here.
  //     In real code, shadowing is a common source of subtle bugs.
  int outerTemp = 25;
  std::cout << "Outer temp: " << outerTemp << std::endl;
  {
    int innerTemp = 90;
    std::cout << "Inner temp (different scope): " << innerTemp << std::endl;
  }
  std::cout << "Outer temp unchanged: " << outerTemp << "\n" << std::endl;

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // 2. NAMESPACE DEMO
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  std::cout << "--- 2. Namespaces ---" << std::endl;

  // EN: Both Powertrain and BodyControl define init(), but there's NO collision
  //     because the full names are Powertrain::init() and BodyControl::init().
  //     Think of namespaces like folders on a filesystem: you can have
  //     /Powertrain/init and /BodyControl/init without conflict.
  Powertrain::init();
  BodyControl::init();

  // EN: Nested namespace accessed with double :: — Vehicle::Diagnostics::scanAll().
  //     In real automotive software, you might see: oem::vehicle::diag::readDTC().
  Vehicle::Diagnostics::scanAll();

  std::cout << "Powertrain status: " << Powertrain::getStatus() << std::endl;
  std::cout << "BodyControl status: " << BodyControl::getStatus() << "\n"
            << std::endl;

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // 3. STATIC LOCAL VARIABLE
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  std::cout << "--- 3. Static Local Variable ---" << std::endl;

  // EN: Calling readSensor() 3 times. Watch the counter INCREMENT each time:
  //     Call 1 → readCount becomes 1 (initialized from 0)
  //     Call 2 → readCount becomes 2 (NOT re-initialized, just incremented)
  //     Call 3 → readCount becomes 3
  //     If readCount were a normal (non-static) local, it would be 1 every call.
  readSensor(); // #1
  readSensor(); // #2
  readSensor(); // #3
  std::cout << std::endl;

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // 4. RECURSION DEMOS
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  std::cout << "--- 4. Recursion ---" << std::endl;

  // EN: FACTORIAL — each call multiplies n by the result of factorial(n-1).
  //     Call stack for factorial(5):
  //         factorial(5) waits for factorial(4)
  //         factorial(4) waits for factorial(3)
  //         factorial(3) waits for factorial(2)
  //         factorial(2) waits for factorial(1)
  //         factorial(1) → returns 1 (BASE CASE HIT!)
  //     Then unwinding: 1 → 2×1=2 → 3×2=6 → 4×6=24 → 5×24=120
  std::cout << "factorial(5) = " << factorial(5) << std::endl;   // 120
  std::cout << "factorial(10) = " << factorial(10) << std::endl; // 3628800

  // EN: FIBONACCI — demonstrates exponential blowup of naive recursion.
  //     fibonacci(10) = 55, but it takes 177 function calls to compute.
  //     fibonacci(30) would take over 2 MILLION calls. In production code,
  //     NEVER use this naive approach — use iteration or memoization.
  std::cout << "fibonacci(6) = " << fibonacci(6) << std::endl;   // 8
  std::cout << "fibonacci(10) = " << fibonacci(10) << std::endl; // 55

  // EN: COUNTDOWN — practical recursion: each call prints n, then calls (n-1).
  //     This is a TAIL RECURSIVE pattern because the recursive call is the
  //     absolute last thing the function does (no computation after it returns).
  std::cout << "\nECU shutdown sequence:" << std::endl;
  shutdownCountdown(3);

  return 0;
}
