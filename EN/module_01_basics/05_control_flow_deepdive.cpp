/**
 * @file module_01_basics/05_control_flow_deepdive.cpp
 * @brief Basics: Control Flow Deep Dive
 *
 * @details
 * =============================================================================
 * [THEORY: Decision Structures]
 * =============================================================================
 * EN: Every program needs decision-making. C++ provides:
 *     1. if / else if / else: General-purpose branching.
 *     2. switch / case: Optimized jump-table for integral types (int, char, enum).
 *     3. Ternary (?:): Inline conditional (covered in 04_operators).
 *
 *     In automotive ECU firmware, decision structures control everything:
 *     engine management, climate, ABS activation, lighting logic.
 *
 * =============================================================================
 * [THEORY: Loop Structures]
 * =============================================================================
 * EN: Loops repeat a block of code:
 *     1. for: When the iteration count is KNOWN.
 *     2. while: When it depends on a CONDITION (unknown iterations).
 *     3. do-while: Same as while, but executes AT LEAST ONCE.
 *     4. Range-based for (C++11): Safe iteration over containers.
 *     5. break: Exit the loop immediately.
 *     6. continue: Skip this iteration, go to next.
 *
 * =============================================================================
 * [CPPREF DEPTH: Structured Bindings & Init-Statements (C++17)]
 * =============================================================================
 * EN: C++17 allows `if (auto x = compute(); x > threshold)` — the variable is scoped ONLY inside
 * the if-block. This reduces variable leakage. Also, `switch` supports init-statements: `switch
 * (auto code = getDTC(); code)`.
 *
 * ⚠  Ref: https://en.cppreference.com/w/cpp/language/if
 * ⚠  Ref: https://en.cppreference.com/w/cpp/language/switch
 *
 * @note Compile: g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion 05_control_flow_deepdive.cpp -o 05_control_flow_deepdive
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <iostream>
#include <string>

int main() {
  std::cout << "=== MODULE 1: CONTROL FLOW DEEP DIVE ===\n" << std::endl;

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // 1. IF / ELSE IF / ELSE
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  std::cout << "--- 1. if / else if / else ---" << std::endl;

  int coolantTempC = 105;

  // EN: Multi-branch decision for coolant temperature monitoring.
  if (coolantTempC > 120) {
    std::cout << "[ALARM] CRITICAL OVERHEAT: " << coolantTempC << " °C!"
              << std::endl;
  } else if (coolantTempC > 100) {
    std::cout << "[WARNING] Coolant high: " << coolantTempC << " °C"
              << std::endl;
  } else if (coolantTempC > 80) {
    std::cout << "[NORMAL] Coolant normal: " << coolantTempC << " °C"
              << std::endl;
  } else {
    std::cout << "[INFO] Engine cold: " << coolantTempC << " °C" << std::endl;
  }

  // C++17 if with init-statement
  // EN: C++17 allows declaring a variable in the if-statement itself.
  //     The variable lives in if + all else if + else blocks, but NOT outside.
  //     You can also reassign it inside any branch (unless declared const).
  //     Equivalent to: { double v = 13.8; if (v > 14.0) {...} else if (...) {...} else {...} }
  if (double voltage = 13.8; voltage > 14.0) {
    std::cout << "[ECU] Overcharge detected: " << voltage << " V" << std::endl;
  } else if (voltage > 12.0) {
    // EN: 'voltage' is still accessible here — same scope as if-block.
    std::cout << "[ECU] Battery voltage OK: " << voltage << " V" << std::endl;
  } else {
    // EN: Even in else, the variable is alive. We can reassign it.
    voltage = 0.0;
    std::cout << "[ECU] LOW voltage! Reset to: " << voltage << " V"
              << std::endl;
  }
  // EN: 'voltage' is now OUT OF SCOPE — cannot be used here.
  std::cout << std::endl;

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // 2. SWITCH / CASE
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  std::cout << "--- 2. switch / case ---" << std::endl;

  // EN: DTC (Diagnostic Trouble Code) severity levels: 1=Info, 2=Warning, 3=Critical. switch
  // creates an O(1) jump table.
  int dtcSeverity = 2;

  switch (dtcSeverity) {
  case 1:
    std::cout << "[DTC] Severity 1: Informational" << std::endl;
    break; // EN: Without break, execution "falls through" to next case!
  case 2:
    std::cout << "[DTC] Severity 2: Warning — schedule maintenance"
              << std::endl;
    break;
  case 3:
    std::cout << "[DTC] Severity 3: CRITICAL — stop vehicle!" << std::endl;
    break;
  default:
    std::cout << "[DTC] Unknown severity level" << std::endl;
    break;
  }

  // EN: Intentional fall-through demo (grouping cases).
  char gearPosition = 'D';
  std::cout << "Gear: " << gearPosition << " -> ";
  switch (gearPosition) {
  case 'D':
  case 'd':
    std::cout << "Drive mode" << std::endl;
    break;
  case 'R':
  case 'r':
    std::cout << "Reverse mode" << std::endl;
    break;
  case 'N':
  case 'n':
    std::cout << "Neutral" << std::endl;
    break;
  case 'P':
  case 'p':
    std::cout << "Park" << std::endl;
    break;
  default:
    std::cout << "Invalid gear!" << std::endl;
    break;
  }
  std::cout << std::endl;

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // 3. CLASSIC FOR LOOP
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  std::cout << "--- 3. Classic for loop ---" << std::endl;

  // EN: Simulate scanning 6 CAN bus message slots.
  for (int slot = 0; slot < 6; ++slot) {
    std::cout << "Scanning CAN slot #" << slot << std::endl;
  }
  std::cout << std::endl;

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // 4. WHILE LOOP
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  std::cout << "--- 4. while loop ---" << std::endl;

  // EN: Simulate coolant warm-up until threshold.
  int warmUpTemp = 20;
  while (warmUpTemp < 80) {
    warmUpTemp += 15; // Simulating heating
    std::cout << "Warming up... " << warmUpTemp << " °C" << std::endl;
  }
  std::cout << "[ECU] Operating temperature reached!\n" << std::endl;

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // 5. DO-WHILE LOOP
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  std::cout << "--- 5. do-while loop ---" << std::endl;

  // EN: Executes AT LEAST ONCE, even if condition is already false. Use case: Sensor self-test
  // that must run at least once on boot.
  int sensorRetry = 0;
  do {
    std::cout << "Sensor self-test attempt #" << (sensorRetry + 1)
              << std::endl;
    sensorRetry++;
  } while (sensorRetry < 3);
  std::cout << "[ECU] Self-test complete.\n" << std::endl;

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // 6. BREAK & CONTINUE
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  std::cout << "--- 6. break & continue ---" << std::endl;

  // EN: Scan 10 ECU addresses. Skip address 5 (reserved), stop at 8 (limit).
  for (int addr = 1; addr <= 10; ++addr) {
    if (addr == 5) {
      std::cout << "  Address " << addr << " -> RESERVED (skip)" << std::endl;
      continue; // Skip this iteration
    }
    if (addr == 8) {
      std::cout << "  Address " << addr << " -> LIMIT reached (break)"
                << std::endl;
      break; // Exit the for loop entirely
    }
    std::cout << "  Address " << addr << " -> OK" << std::endl;
  }
  std::cout << std::endl;

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // 7. NESTED LOOPS
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  std::cout << "--- 7. Nested Loops ---" << std::endl;

  // EN: 4 cylinders × 3 sensors each = 12 readings.
  for (int cyl = 1; cyl <= 4; ++cyl) {
    for (int sensor = 1; sensor <= 3; ++sensor) {
      std::cout << "  Cyl" << cyl << "-Sensor" << sensor << " ";
    }
    std::cout << std::endl;
  }
  std::cout << std::endl;

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // 8. RANGE-BASED FOR (C++11) — revisited
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  std::cout << "--- 8. Range-based for (C++11) ---" << std::endl;

  // EN: Iterating over DTC codes stored in an array.
  std::string dtcCodes[] = {"P0300", "P0171", "P0420", "P0442"};

  for (const auto &dtc : dtcCodes) {
    std::cout << "  Active DTC: " << dtc << std::endl;
  }
  std::cout << std::endl;

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // 9. INFINITE LOOP PATTERN (common in embedded)
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  std::cout << "--- 9. Infinite Loop Pattern ---" << std::endl;
  // EN: In real embedded firmware, `while(true)` or `for(;;)` is the main super-loop. Here we
  // simulate with a counter.
  int tick = 0;
  while (true) {
    std::cout << "  ECU tick #" << tick << std::endl;
    ++tick;
    if (tick >= 5) {
      std::cout << "  [DEMO] Exiting super-loop after 5 ticks.\n" << std::endl;
      break;
    }
  }

  return 0;
}
