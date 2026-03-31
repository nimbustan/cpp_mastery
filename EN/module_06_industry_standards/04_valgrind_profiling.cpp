/**
 * @file module_06_industry_standards/04_valgrind_profiling.cpp
 * @brief Industry Standards: Memory Profiling and Valgrind — Profiling ve Valgrind ile Bellek
 * Denetimi
 *
 * @details
 * =============================================================================
 * [THEORY: Why Use Valgrind and Profiling?]
 * =============================================================================
 * EN: Writing code that compiles without errors is NOT enough. If your program consumes 1
 * Megabyte of RAM extra every hour, it will crash your server after a week (OOM - Out of
 * Memory). 'Valgrind' is a magical Linux tool. You run your program "inside" Valgrind, and it
 * tracks EVERY SINGLE BYTE of memory you allocated and checks if you freed it.
 *
 *
 *
 * =============================================================================
 * [HOW TO APPLY?]
 * =============================================================================
 * EN: Add the "-g" flag while compiling to inject line numbers into the binary.
 *
 * 1. Compile: `g++ -g -o leak_test 04_valgrind_profiling.cpp`
 * 2. Execute: `valgrind --leak-check=full ./leak_test`
 *
 * [CPPREF DEPTH: Sanitizer Internals — How ASan, TSan, UBSan Actually Work]
 * =============================================================================
 * EN: ASan (AddressSanitizer) uses shadow memory with a 1:8 mapping — every 8 bytes of
 * application memory have 1 byte of shadow tracking whether that region is fully accessible,
 * partially accessible, or poisoned. Typical slowdown is ~2x with ~3x memory overhead. TSan
 * (ThreadSanitizer) uses happens-before analysis backed by vector clocks to detect data races;
 * it incurs ~5-15x slowdown and 5-10x memory overhead. UBSan (UndefinedBehaviorSanitizer)
 * instruments arithmetic and type operations at compile time (signed overflow, null dereference,
 * misaligned access) with minimal runtime overhead. ASan and TSan CANNOT be combined in the same
 * binary — they use incompatible shadow memory layouts. MSan (MemorySanitizer) detects reads of
 * uninitialized memory; also incompatible with ASan. In CI pipelines, run separate builds for
 * each sanitizer: one ASan+UBSan build and one TSan build.
 *
 *
 * @note Compile: g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion 04_valgrind_profiling.cpp -o 04_valgrind_profiling
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <cstring>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// EN: AddressSanitizer (ASan) — Demonstrates what ASan targets.
// ═════════════════════════════════════════════════════════════════════════════════════════════════
// EN: ASan is a compile-time instrumentation flag: -fsanitize=address. It detects
// heap-buffer-overflow, stack-buffer-overflow, use-after-free, and memory leaks at ~2x runtime
// cost (vs Valgrind's ~20x slowdown). Compile: g++ -g -fsanitize=address -o asan_test file.cpp
// -fsanitize=address -o asan_test dosya.cpp
void demonstrateASanTargets() {
  std::cout << "\n--- ASan Target Patterns (Commented — triggering would crash!) ---\n";

  // ─── Heap-Buffer-Overflow ──────────────────────────────────────────────────────────────────────
  // EN: Writing past allocated heap memory. ASan catches this immediately.
  // NOT UNCOMMENT: int *buf = new int[10]; buf[10] = 0xDEAD;  // heap-buffer-overflow! delete[]
  // buf;
  std::cout << "[ASan] Heap-buffer-overflow: writing buf[10] on a 10-element array\n";

  // ─── Stack-Buffer-Overflow ─────────────────────────────────────────────────────────────────────
  // EN: Overrunning a local stack array. ASan instruments stack frames too.
  // UNCOMMENT: int stack_arr[5]; stack_arr[5] = 42;  // stack-buffer-overflow!
  std::cout << "[ASan] Stack-buffer-overflow: stack_arr[5] on a 5-element array\n";

  // ─── Use-After-Free ────────────────────────────────────────────────────────────────────────────
  // EN: Accessing memory after delete. Classic dangling pointer.
  // *p = new int(7); delete p; *p = 99;  // use-after-free!
  std::cout << "[ASan] Use-after-free: dereferencing deleted pointer\n";
}

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// EN: ThreadSanitizer (TSan) — Data Race Detection
// ═════════════════════════════════════════════════════════════════════════════════════════════════
// EN: TSan is enabled with -fsanitize=thread. It detects data races where two threads access the
// same memory without synchronization and at least one is a write. CANNOT be used simultaneously
// with ASan. Compile: g++ -g -fsanitize=thread -o tsan_test file.cpp
//
// EN: Example data race pattern (DO NOT run without TSan):
// sharedCounter++;  // WRITE — no lock! // Thread 2:  sharedCounter++;  // WRITE — no lock! =>
// DATA RACE
//
// EN: Fix: protect with std::mutex:
// lk(mtx); sharedCounter++; } // Thread 2:  { std::lock_guard<std::mutex> lk(mtx);
// sharedCounter++; }

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// EN: UndefinedBehaviorSanitizer (UBSan) — Catches Silent Killers
// ═════════════════════════════════════════════════════════════════════════════════════════════════
// EN: UBSan is enabled with -fsanitize=undefined. It catches signed integer overflow, null
// pointer dereference, misaligned access, and more. These are "undefined behavior" — compiler
// may do ANYTHING. Compile: g++ -g -fsanitize=undefined -o ubsan_test file.cpp
int potentialSignedOverflow(int a, int b) {
  // EN: If a + b exceeds INT_MAX, this is undefined behavior. UBSan will report: "signed integer
  // overflow: X + Y".
  // integer overflow: X + Y".
  return a + b;
}

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// EN: RAII Approach — Valgrind Reports 0 Leaks
// ═════════════════════════════════════════════════════════════════════════════════════════════════
void properRAII_VectorApproach() {
  // EN: std::vector manages heap memory automatically. When the function returns, the vector
  // destructor frees all memory — zero leaks.
  std::vector<int> sensorBuffer(1000000);
  sensorBuffer[0] = 42;
  std::cout << "[RAII Vector] Data loaded: " << sensorBuffer[0]
            << " — memory auto-freed on scope exit" << std::endl;
}

void properRAII_UniquePtrApproach() {
  // EN: std::unique_ptr owns the resource exclusively. Automatically calls delete[] when it goes
  // out of scope — impossible to leak.
  auto canBuffer = std::make_unique<int[]>(1000000);
  canBuffer[0] = 0xCAFE;
  std::cout << "[RAII unique_ptr] CAN frame loaded: 0x" << std::hex
            << canBuffer[0] << std::dec
            << " — unique_ptr frees on scope exit" << std::endl;
}

void theLeakyFunction() {
  // EN: Allocating 1 million integers (approx 4 MB of RAM).
  int *databaseSimulation = new int[1000000];

  databaseSimulation[0] = 42;
  std::cout << "[Valgrind Test] Data loaded: " << databaseSimulation[0] <<
      std::endl;

  // EN: OH NO! We exit the function WITHOUT calling 'delete[]'! This is a disaster. When the
  // function ends, the pointer 'databaseSimulation' vanishes, but the 4MB memory it grabbed
  // remains occupied forever as a ZOMBIE.
  //
  // (MEMORY LEAK).
}

int main() {
  std::cout << "=== MODULE 6: VALGRIND & MEMORY PROFILING ===\n" << std::endl;

  std::cout << "[SYSTEM] WARNING: Triggering 4MB Memory Leak intentionally...\n";

  theLeakyFunction();

  std::cout << "\n[SYSTEM] Function ended. But 4 Megabytes of garbage remains in RAM." << std::endl;

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // EN: How to fix this in Modern C++? NEVER use RAW POINTERS inside 'theLeakyFunction'. Use
  // `std::vector<int>` or `std::unique_ptr` instead! When they go out of scope, they instantly
  // wipe the 4MB data (RAII rules supreme).
  //
  // ═══════════════════════════════════════════════════════════════════════════════════════════════

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // EN: Demo 2 — ASan Target Patterns
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  std::cout << "\n--- Demo 2: AddressSanitizer (ASan) Targets ---" << std::endl;
  demonstrateASanTargets();

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // EN: Demo 3 — Proper RAII: Vector vs Raw Pointer
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  std::cout << "\n--- Demo 3: RAII Vector Approach (0 Leaks Under Valgrind) ---" << std::endl;
  properRAII_VectorApproach();

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // EN: Demo 4 — unique_ptr Automatic Cleanup
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  std::cout << "\n--- Demo 4: unique_ptr Automatic Cleanup ---" << std::endl;
  properRAII_UniquePtrApproach();

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // EN: Demo 5 — UBSan: Signed Overflow Example
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  std::cout << "\n--- Demo 5: UBSan Signed Overflow ---" << std::endl;
  // EN: Safe call — no overflow here. Compile with -fsanitize=undefined to instrument and catch
  // overflows automatically.
  // ile derleyin.
  int safeResult = potentialSignedOverflow(100, 200);
  std::cout << "100 + 200 = " << safeResult << " (safe, no overflow)" << std::endl;
  std::cout << "Compile with -fsanitize=undefined to catch cases like INT_MAX + 1\n";

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // EN: Demo 6 — Sanitizer & Profiling Compilation Flags Reference
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  std::cout << "\n--- Demo 6: Sanitizer Compilation Flags Reference ---" << std::endl;
  std::cout << R"(
  +----------+----------------------------------+--------------------------------------+
  | Tool     | Compile Flag                     | Detects                              |
  +----------+----------------------------------+--------------------------------------+
  | ASan     | -fsanitize=address               | Buffer overflow, use-after-free, leak|
  | TSan     | -fsanitize=thread                | Data races between threads           |
  | UBSan    | -fsanitize=undefined             | Signed overflow, null deref, align   |
  | MSan     | -fsanitize=memory (clang)        | Uninitialized memory reads           |
  +----------+----------------------------------+--------------------------------------+
  | Valgrind | valgrind --leak-check=full ./bin | All leaks, ~20x slower               |
  | Perf     | perf record ./bin; perf report   | CPU hotspots, cache misses           |
  | Callgrind| valgrind --tool=callgrind ./bin  | Function-level profiling             |
  +----------+----------------------------------+--------------------------------------+
)" << std::endl;

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // EN: Practical Profiling Workflow (Automotive ECU Software)
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // EN: Step 1 — Compile with debug symbols: g++ -g -O0 -o ecu_test main.cpp
  //
  // EN: Step 2 — Run under Valgrind Memcheck for full leak report:
  // --leak-check=full --show-leak-kinds=all ./ecu_test
  //
  // EN: Step 3 — Compile with ASan for faster turnaround (~2x vs 20x):
  // -fsanitize=address -o ecu_test main.cpp && ./ecu_test
  //
  // EN: Step 4 — Profile CPU hotspots with perf:
  // report
  //
  // EN: Step 5 — Function-level profiling with Callgrind:
  // kcachegrind callgrind.out.*   (GUI visualization)

  return 0;
}
