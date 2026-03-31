/**
 * @file module_08_core_guidelines/01_philosophy_and_interfaces.cpp
 * @brief C++ Core Guidelines: Philosophy & Interfaces
 *
 * @details
 * =============================================================================
 * [INTRODUCTION: What are C++ Core Guidelines?]
 * =============================================================================
 * EN: The "C++ Core Guidelines" are a set of tried-and-true rules about coding in C++. Written
 * by Bjarne Stroustrup (creator of C++) and Herb Sutter, they aim to help developers write MUCH
 * Safer, Cleaner, and Faster modern C++.
 *
 * =============================================================================
 * [P.1: Express ideas directly in code]
 * =============================================================================
 * EN: Compilers don't read comments. If you have to write a comment to explain WHAT a loop does,
 * name your variables better or use Standard Library (STL) algorithms.
 *
 * =============================================================================
 * [P.4 & P.5: Compile-Time Checking]
 * =============================================================================
 * EN: "Prefer compile-time checking to run-time checking." If you can calculate or check a rule
 * during COMPILATION, do it! Use `constexpr` and `static_assert()`. A bug caught at compile-time
 * costs $0. At runtime, it causes a crash.
 *
 * [CPPREF DEPTH: Type Safety as Zero-Cost Abstraction — Strong Typedefs]
 * =============================================================================
 * EN: C++ Core Guidelines I.4: "Make interfaces precisely and strongly typed". Primitive
 * obsession — using bare `int` for meters, seconds, and IDs — lets the compiler silently accept
 * nonsensical expressions like `meters + seconds`. Strong typedefs (a thin wrapper class with
 * explicit constructors) enforce correctness at ZERO runtime cost because the optimizer elides
 * the wrapper entirely. `enum class` prevents implicit conversions between unrelated
 * enumerations. `[[nodiscard]]` forces callers to use (not ignore) a return value — essential
 * for error codes. In C++20, Concepts formalize interface contracts: `template<Sortable T>`
 * replaces unnamed SFINAE with readable, diagnosable constraints.
 *
 *
 * @note Compile: g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion 01_philosophy_and_interfaces.cpp -o 01_philosophy_and_interfaces
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <algorithm> // for std::find_if
#include <iostream>
#include <memory>
#include <string>
#include <vector>

// -------------------------------------------------------------------------------------------------
// [P.1] Express Intent
// -------------------------------------------------------------------------------------------------
class ECU {
public:
  std::string name;
  bool isOnline = true;
};

void checkECUStatus_Bad(const std::vector<ECU> &ecus, const std::string &targetName) {
  // BAD: C-Style manual loop. Hard to catch the intent at a glance.
  bool found = false;
  for (size_t i = 0; i < ecus.size(); ++i) {
    if (ecus[i].name == targetName) {
      found = true;
      break;
    }
  }
  (void)found; // EN: Suppress — intentional bad example
}

void checkECUStatus_Good(const std::vector<ECU> &ecus, const std::string &targetName) {
  // GOOD: Use STL algorithm! Intent is 100% clear immediately: "Find
  // if any ECU matches".
  auto it = std::find_if(ecus.begin(), ecus.end(),
      [&](const ECU &e) { return e.name == targetName; });

  if (it != ecus.end()) {
    std::cout << "[Intent: STL] ECU '" << targetName << "' found!" << std::endl;
  }
}

// -------------------------------------------------------------------------------------------------
// [P.4 & P.5] Compile-Time Checking (`constexpr` and `static_assert`)
// -------------------------------------------------------------------------------------------------
// EN: Because of `constexpr`, compiler calculates this during BUILD time (0 CPU cost at
// runtime).
constexpr int calculateMaxSensorChannels(int baseChannels, int expansionModules) {
  return baseChannels + (expansionModules * 2);
}

int main() {
  std::cout << "=== MODULE 8: C++ CORE GUIDELINES - PHILOSOPHY & INTERFACES ===\n" << std::endl;

  // 1. Express Intent demonstration
  std::vector<ECU> vehicleECUs = {
      {"BCM", true}, {"Powertrain", false}, {"HVAC", true}};
  checkECUStatus_Good(vehicleECUs, "Powertrain");

  // 2. Compile-Time Magic
  // EN: This is NOT calculated at runtime! Replaced directly with `20` in the ASM binary.
  constexpr int maxChannels = calculateMaxSensorChannels(10, 5);

  // EN: static_assert checks rules ALWAYS DURING COMPILE. If false -> Red Compiler Error!
  static_assert(maxChannels >= 10, "[FATAL ERROR] Sensor channels cannot be less than 10!");

  std::cout << "\nMax Sensor Channels (Calculated purely at Compile Time!): " << maxChannels <<
      std::endl;

  std::cout << "\n[ARCHITECTURAL CONCLUSION]:" << std::endl;
  std::cout << "- EN: Write code for the COMPILER to optimize and HUMANS to read!" << std::endl;

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // EN: I.4 — Make interfaces precisely and strongly typed
  // ═══════════════════════════════════════════════════════════════════════════════════════════════

  // ─── 1. BAD: Weakly typed interface ────────────────────────────────────────────────────────────
  // EN: What does 'unit' mean? 0 = KPH? 1 = MPH? Magic numbers everywhere.
  //
  // void setSpeed(int speed, int unit);   // BAD — caller can pass anything setSpeed(120, 3);   
  // // What is 3? Compiles fine!

  // ─── 2. GOOD: Strongly typed interface ─────────────────────────────────────────────────────────
  // EN: Type-safe enum makes invalid calls impossible at compile time.
  enum class SpeedUnit { KPH, MPH };

  auto setSpeed = [](double speed, SpeedUnit unit) {
    const char* label = (unit == SpeedUnit::KPH) ? "km/h" : "mph";
    std::cout << "[I.4 GOOD] Speed set to " << speed << " " << label << std::endl;
  };

  setSpeed(120.0, SpeedUnit::KPH);
  // setSpeed(120.0, 2);  // EN: Won't compile! Type safety enforced.

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // EN: I.11 — Never transfer ownership by a raw pointer
  // ═══════════════════════════════════════════════════════════════════════════════════════════════

  // ─── 3. BAD: Raw pointer ownership transfer ────────────────────────────────────────────────────
  // EN: Who owns this pointer? Caller? Callee? Nobody knows → leak.
  //
  // ECU* createECU() { return new ECU{"Raw", true}; }  // BAD

  // ─── 4. GOOD: unique_ptr ownership transfer ────────────────────────────────────────────────────
  // EN: Ownership is explicit. unique_ptr is the ONLY owner, auto-deletes.
  auto createECU_Safe = []() -> std::unique_ptr<ECU> {
    return std::make_unique<ECU>(ECU{"SmartECU", true});
  };

  auto safeEcu = createECU_Safe();
  std::cout << "[I.11 GOOD] Owned ECU: " << safeEcu->name
            << " (unique_ptr, no leak possible)" << std::endl;

  return 0;
}

