/**
 * @file module_08_core_guidelines/04_concurrency_and_performance.cpp
 * @brief C++ Core Guidelines: Concurrency & Performance Rules
 *
 * @details
 * =============================================================================
 * [CP.20: Use RAII Locks, Never touch raw mutexes]
 * =============================================================================
 * EN: "Never use plain lock() and unlock(). Use RAII." If you manually call `mtx.lock()` and
 * subsequently throw an exception before reaching `mtx.unlock()`, your underlying mutex NEVER
 * unlocks. Your server will FREEZE FOREVER (Deadlock). Always use `std::lock_guard` or
 * `std::scoped_lock` (C++17).
 *
 * =============================================================================
 * [Per.1 & Per.2: Premature Optimization]
 * =============================================================================
 * EN: "Don't optimize without reason." & "Don't optimize prematurely." C++ Programmers love
 * doing bitwise magic or raw pointer arithmetic to save 2 nanoseconds. 99% of the time, the
 * compiler's optimizer flag (`-O3`) does it much better and safer. First, write CLEAN,
 * INTENTIONAL, READABLE CODE. Second, use a PROFILER (like Valgrind/Callgrind). Third, ONLY
 * optimize the bottleneck!
 *
 *
 * =============================================================================
 * [CPPREF DEPTH: Hardware Concurrency limit & Cache Coherence!]
 * =============================================================================
 * EN: CppReference warns against creating 1000 threads for performance. Use
 * `std::thread::hardware_concurrency()` to find the exact number of physical/ logical cores you
 * have. Spawning more threads than CPU cores causes extreme lag. Also, multiple threads
 * modifying variables physically close together in RAM causes "False Sharing" (L1/L2 Cache
 * Coherence ping-pong), drastically hurting performance!
 *
 *
 * @note Compile: g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion 04_concurrency_and_performance.cpp -o 04_concurrency_and_performance
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

std::mutex databaseLock;
int globalOnlineECUs = 0;

void registerECU_Bad() {
  // EN: CP.20 VIOLATION: Manual Lock/Unlock. VERY DANGEROUS!
  databaseLock.lock();

  globalOnlineECUs++;
  // If a 'throw' happens exactly here, unlock() is silently skipped -> FATAL DEADLOCK!

  databaseLock.unlock();
}

void registerECU_Good() {
  // EN: C++ CORE GUIDELINES (CP.20): Use RAII (std::lock_guard explicitly)
  std::lock_guard<std::mutex> lock(databaseLock); // Automatic Safe Locker!

  globalOnlineECUs++;
  // EN: Even if an Exception throws here, RAII strictly un-winds the lock back to OS. No
  // deadlocks.
}

int main() {
  std::cout << "=== MODULE 8: C++ CORE GUIDELINES - CONCURRENCY & PERFORMANCE ===\n" << std::endl;

  std::vector<std::thread> workers;

  // EN: Launch 10 Threads simultaneously to register ECU modules.
  for (int i = 0; i < 10; ++i) {
    workers.push_back(std::thread(registerECU_Good));
  }

  // EN: Join all threads (Wait for them to finish properly).
  for (auto &w : workers) {
    w.join();
  }

  std::cout << "All ECUs Registered Safely! Total Online: " << globalOnlineECUs << std::endl;

  std::cout << "\n[ARCHITECTURAL CONCLUSION]:" << std::endl;
  std::cout << "1. Threads + Shared Global Data = CATASTROPHE. Always use "
               "std::lock_guard / std::scoped_lock."
            << std::endl;
  std::cout << "2. PERFORMANCE MYTH: Let the '-O3' generic compiler flags do "
               "the 'clever' micro-optimizations!"
            << std::endl;

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // EN: Per.7 — Design to enable optimization (data layout matters)
  // ═══════════════════════════════════════════════════════════════════════════════════════════════

  // EN: C++ has no standard `restrict` keyword (C99 feature). Some compilers offer
  // `__restrict__` as an extension, but it's non-portable.

  // ─── 1. Avoid unnecessary copies in range-for loops ────────────────────────────────────────────
  // EN: Using `auto` (value) copies every element. Use `const auto&` instead.

  std::vector<std::string> sensorNames = {
      "Temperature", "Pressure", "Humidity", "Altitude", "Speed"};

  // EN: BAD — copies every string (expensive for large objects).
  // for (auto name : sensorNames) { /* copy per iteration */ }

  // EN: GOOD — zero-copy read, compiler can auto-vectorize.
  std::cout << "\n[Per.7] Sensor names (const auto& — zero copy):" << std::endl;
  for (const auto& name : sensorNames) {
    std::cout << "  -> " << name << std::endl;
  }

  std::cout << "[Per.7] Always use 'const auto&' in range-for when you only read!"
            << std::endl;

  return 0;
}

