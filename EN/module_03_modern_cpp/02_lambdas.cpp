/**
 * @file module_03_modern_cpp/02_lambdas.cpp
 * @brief Modern C++: Lambda Expressions
 *
 * @details
 * =============================================================================
 * [THEORY: What is a Lambda? (Lambdas for Dummies)]
 * =============================================================================
 * EN: Imagine you need to tell the ECU diagnostic system: "Filter only the fault codes above
 * severity 3." Before C++11 you had to write a separate named function somewhere far away and
 * pass a pointer to it. With a Lambda you write the rule RIGHT WHERE you need it — inline,
 * compact, readable.
 *
 * A Lambda is an ANONYMOUS (unnamed) function object that the compiler generates for you behind
 * the scenes. Think of it as a disposable mini-function you define on the spot. Lambdas are the
 * cornerstone of modern STL algorithms (std::sort, std::for_each, std::find_if, std::transform,
 * ...).
 *
 * =============================================================================
 * [THEORY: Lambda Anatomy — The Full Syntax]
 * =============================================================================
 * EN: The complete Lambda syntax is:
 *   [capture_list] (parameters) mutable -> return_type { body }
 *
 * - [capture_list] : HOW to grab outside variables (see next theory).
 * - (parameters)   : Input arguments, just like a normal function.
 * - mutable        : (Optional) Lets you modify captured-by-value copies.
 * - -> return_type : (Optional) Explicit return type. Usually auto-deduced.
 * - { body }       : The actual code.
 *
 * Minimal Lambda: [](){} — captures nothing, takes nothing, does nothing. When there are no
 * parameters you may even omit (): []{}.
 *
 * =============================================================================
 * [THEORY: Capture List Deep Dive]
 * =============================================================================
 * EN: The capture list is what makes Lambdas special. It controls how outside variables enter
 * the Lambda's body:
 *
 *   []          — Capture nothing. Pure function, no outside variables.
 *   [=]         — Capture ALL outside variables by VALUE (read-only copies).
 *   [&]         — Capture ALL outside variables by REFERENCE (modify originals).
 *   [x]         — Capture only 'x' by value.
 *   [&x]        — Capture only 'x' by reference.
 *   [=, &x]     — Capture everything by value EXCEPT 'x' by reference.
 *   [&, x]      — Capture everything by reference EXCEPT 'x' by value.
 *   [this]      — Capture the enclosing object's 'this' pointer (C++11).
 *   [*this]     — Capture a COPY of the enclosing object (C++17).
 *   [x = expr]  — Init-capture: create a new variable 'x' initialized to
 * 'expr' (C++14). Great for move-capturing!
 *
 * Golden Rule: Capture only what you need. Prefer explicit captures [x, &y] over blanket [=] or
 * [&] for clarity and safety.
 *
 * =============================================================================
 * [CPPREF DEPTH: Dangling Captures & Lifetime Traps]
 * =============================================================================
 * EN: cppreference.com/w/cpp/language/lambda warns: If a Lambda captures local variables by
 * REFERENCE [&] and then OUTLIVES the scope where those variables were defined (e.g., returned
 * from a function, stored in a container, passed to std::thread), you get a DANGLING REFERENCE —
 * Undefined Behavior! The Lambda holds an address to memory that no longer exists.
 *
 * Safe rule: If the Lambda escapes the current scope, capture by VALUE [=] or use init-capture
 * [x = std::move(obj)] to own the data.
 *
 * =============================================================================
 * [CPPREF DEPTH: std::function — Type-Erased Lambda Storage]
 * =============================================================================
 * EN: cppreference.com/w/cpp/utility/functional/function documents:
 * std::function<ReturnType(Args...)> is a general-purpose polymorphic function wrapper. It can
 * store ANY callable: a Lambda, a function pointer, a functor, or std::bind result. Use it when
 * you need to store a Lambda in a member variable, a container, or pass it across API
 * boundaries. Be aware: it has overhead (heap allocation, virtual dispatch). For
 * performance-critical automotive loops, prefer templates or auto.
 *
 * =============================================================================
 * [THEORY: Generic Lambdas (C++14) & Template Lambdas (C++20)]
 * =============================================================================
 * EN: C++14 introduced generic Lambdas: use 'auto' in the parameter list and the compiler
 * generates a templated operator() internally. This is perfect for writing ECU-agnostic utility
 * functions.
 *
 * C++20 goes further: you can write explicit template parameter lists on Lambdas: []<typename
 * T>(T value){ ... }. This gives you SFINAE/concepts power directly inside a Lambda — useful for
 * compile-time filtering of sensor types in embedded systems.
 *
 * =============================================================================
 * [CPPREF DEPTH: Lambda as Constexpr (C++17) & Consteval (C++20)]
 * =============================================================================
 * EN: cppreference.com/w/cpp/language/lambda documents: Since C++17, Lambdas are implicitly
 * constexpr if their body satisfies constexpr requirements. You can compute values at COMPILE
 * TIME using Lambdas — zero runtime cost. In automotive ECUs with limited flash, this means
 * look-up tables, CRC checksums, and protocol constants can be Lambda-computed at compile time.
 *
 * C++20 allows `consteval` Lambdas that MUST execute at compile time.
 *
 * @note Compile: g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion 02_lambdas.cpp -o 02_lambdas
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <algorithm>  // std::sort, std::for_each, std::find_if, std::transform
#include <cstdint>    // uint8_t, uint16_t
#include <functional> // std::function
#include <iostream>
#include <numeric>    // std::accumulate
#include <string>
#include <vector>

// ─── Automotive Helper Structures ────────────────────────────────────────────────────────────────

// EN: Diagnostic Trouble Code (DTC) — core data in any automotive ECU.
struct DTC {
  uint16_t code;        // e.g. 0x0171
  std::string name;     // e.g. "EGT Sensor Open Circuit"
  uint8_t severity;     // 1 = info, 2 = warning, 3 = critical, 4 = fatal
  bool active;          // currently present?
};

// EN: Sensor reading from a vehicle bus (CAN/LIN).
struct SensorReading {
  std::string sensor;
  double value;
  std::string unit;
};

int main() {
  std::cout << "=== MODULE 3/02: LAMBDA EXPRESSIONS — FULL GUIDE ===\n"
            << std::endl;

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // 1. THE SIMPLEST LAMBDA
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // EN: A Lambda with no capture, no parameters. Just prints a message.

  auto greet = []() {
    std::cout << "1. ECU Boot: Lambda initialized successfully!" << std::endl;
  };
  // EN: Call the Lambda
  greet();

  // EN: Even shorter — omit () when there are no parameters:
  auto greetShort = [] {
    std::cout << "   (Same Lambda without parentheses)\n" << std::endl;
  };
  greetShort();

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // 2. LAMBDA WITH PARAMETERS
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // EN: Lambdas accept parameters just like regular functions. Here we convert a raw ADC value
  // to a temperature (automotive formula).

  auto adcToTemperature = [](int rawAdc) -> double {
    // EN: Simplified EGT sensor formula: T = (raw * 0.1) - 40.0
    return (rawAdc * 0.1) - 40.0;
  };

  int rawValue = 850;
  std::cout << "2. ADC Raw=" << rawValue
            << " -> Temperature=" << adcToTemperature(rawValue)
            << " C\n" << std::endl;

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // 3. CAPTURE BY VALUE [=]
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // EN: The Lambda copies the outside variable's VALUE at the moment of creation. Changes to the
  // original do NOT affect the Lambda's copy.

  // EN: Critical and above
  int severityThreshold = 3;
  auto isCritical = [=](const DTC& dtc) {
    return dtc.severity >= severityThreshold && dtc.active;
  };

  // EN: Even if we change threshold AFTER creating the Lambda, the Lambda still sees the OLD
  // value (3), because it captured a COPY.
  severityThreshold = 999;

  DTC testDtc{0x0171, "EGT Sensor Open", 4, true};
  std::cout << "3. Capture by value: DTC 0x0171 critical? "
            << (isCritical(testDtc) ? "YES" : "NO")
            << " (threshold was 3 at capture time)\n" << std::endl;

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // 4. CAPTURE BY REFERENCE [&]
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // EN: The Lambda gets a REFERENCE to the outside variable. Any change the Lambda makes
  // modifies the ORIGINAL. Also, if the outside changes, the Lambda sees the new value.

  int activeFaultCount = 0;
  std::vector<DTC> faultLog = {
      {0x0100, "Coolant Temp High",   3, true},
      {0x0200, "Oil Pressure Low",    4, true},
      {0x0300, "Battery Voltage",     2, false},
      {0x0400, "Throttle Pos Sensor", 3, true},
      {0x0500, "O2 Sensor Circuit",   2, true},
      {0x0600, "Knock Sensor Range",  4, false},
      {0x0700, "Misfire Cylinder 2",  3, true},
  };

  // EN: Count active faults by capturing counter BY REFERENCE.
  std::for_each(faultLog.begin(), faultLog.end(),
                [&activeFaultCount](const DTC& dtc) {
    if (dtc.active) {
      activeFaultCount++;
    }
  });
  std::cout << "4. Capture by reference: Active faults = "
            << activeFaultCount << "\n" << std::endl;

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // 5. MIXED CAPTURE [=, &x]
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // EN: Best practice — capture everything by value for safety, but explicitly capture specific
  // variables by reference when you need to modify them.

  int criticalCount = 0;
  int warningCount = 0;
  int critThreshold = 3;

  std::for_each(faultLog.begin(), faultLog.end(),
                [=, &criticalCount, &warningCount](const DTC& dtc) {
    if (!dtc.active) return;
    if (dtc.severity >= critThreshold) {
      // EN: Modified via reference
      criticalCount++;
    } else {
      warningCount++;
    }
  });
  std::cout << "5. Mixed capture: Critical=" << criticalCount
            << ", Warning=" << warningCount << "\n" << std::endl;

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // 6. MUTABLE LAMBDA
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // EN: By default, a Lambda captured by VALUE cannot modify its copies (they are const). The
  // 'mutable' keyword lifts this restriction. Note: It modifies the COPY, not the original!

  int sequenceId = 1000;
  auto generateId = [sequenceId]() mutable -> int {
    // EN: Each call increments the Lambda's internal copy.
    return sequenceId++;
  };

  std::cout << "6. Mutable Lambda (sequence generator):" << std::endl;
  std::cout << "   ID: " << generateId() << std::endl; // 1000
  std::cout << "   ID: " << generateId() << std::endl; // 1001
  std::cout << "   ID: " << generateId() << std::endl; // 1002
  std::cout << "   Original sequenceId still = " << sequenceId
            << "\n" << std::endl;

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // 7. INIT CAPTURE / MOVE CAPTURE (C++14)
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // EN: C++14 allows creating new variables inside the capture list itself. This is essential
  // for MOVE SEMANTICS — you can move a unique_ptr or a large buffer into the Lambda without
  // copying.

  std::string rawFrame = "CAN_ID:0x7E8 DATA:41 0C 1A F0";
  auto processFrame = [frame = std::move(rawFrame)]() {
    std::cout << "7. Init-capture (moved): Processing frame: "
              << frame << std::endl;
  };
  processFrame();
  std::cout << "   Original rawFrame is now empty: \""
            << rawFrame << "\"\n" << std::endl;

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // 8. LAMBDA WITH STL ALGORITHMS
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // EN: This is where Lambdas truly shine in automotive software.

  // --- 8a. std::sort — Sort DTCs by severity (descending) ---
  // EN: Custom sorting: highest severity first for prioritized display.
  std::vector<DTC> sortedFaults = faultLog;
  std::sort(sortedFaults.begin(), sortedFaults.end(),
            [](const DTC& a, const DTC& b) {
    return a.severity > b.severity;
  });
  std::cout << "8a. std::sort by severity (descending):" << std::endl;
  for (const auto& dtc : sortedFaults) {
    std::cout << "    [" << static_cast<int>(dtc.severity) << "] 0x"
              << std::hex << dtc.code << std::dec
              << " " << dtc.name << std::endl;
  }

  // --- 8b. std::find_if — Find first fatal DTC ---
  // EN: Search for the first DTC with severity == 4 (fatal).
  auto it = std::find_if(faultLog.begin(), faultLog.end(),
                         [](const DTC& dtc) {
    return dtc.severity == 4;
  });
  if (it != faultLog.end()) {
    std::cout << "\n8b. std::find_if: First fatal DTC -> 0x"
              << std::hex << it->code << std::dec
              << " " << it->name << std::endl;
  }

  // --- 8c. std::transform — Extract fault names into a string vector ---
  // EN: Map DTCs to their names (functional-style projection).
  std::vector<std::string> faultNames(faultLog.size());
  std::transform(faultLog.begin(), faultLog.end(), faultNames.begin(),
                 [](const DTC& dtc) { return dtc.name; });
  std::cout << "\n8c. std::transform: DTC names extracted ("
            << faultNames.size() << " items)" << std::endl;

  // --- 8d. std::accumulate — Sum all severity scores ---
  // EN: Total severity score across all DTCs (risk assessment).
  int totalSeverity = std::accumulate(
      faultLog.begin(), faultLog.end(), 0,
      [](int sum, const DTC& dtc) {
    return sum + static_cast<int>(dtc.severity);
  });
  std::cout << "8d. std::accumulate: Total severity score = "
            << totalSeverity << "\n" << std::endl;

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // 9. GENERIC LAMBDA (C++14 auto)
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // EN: Use 'auto' parameters to create Lambdas that work with ANY type. The compiler generates
  // a templated operator() behind the scenes.

  auto printReading = [](const auto& r) {
    std::cout << "   Sensor: " << r.sensor
              << " = " << r.value << " " << r.unit << std::endl;
  };

  std::vector<SensorReading> readings = {
      {"Coolant Temp",  92.5, "C"},
      {"Engine RPM",  3500.0, "rpm"},
      {"Battery",      13.8, "V"},
      {"Fuel Pressure", 3.2, "bar"},
  };

  std::cout << "9. Generic Lambda (auto) — Sensor Readings:" << std::endl;
  std::for_each(readings.begin(), readings.end(), printReading);
  std::cout << std::endl;

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // 10. std::function — STORING LAMBDAS
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // EN: auto is great for local use, but if you need to store a Lambda in a class member,
  // container, or pass it through an API, use std::function. Trade-off: small overhead (type
  // erasure + possible heap allocation).

  // EN: ECU callback registry pattern — store different validation rules.
  std::vector<std::function<bool(const DTC&)>> validators;

  validators.push_back([](const DTC& d) { return d.active; });
  validators.push_back([](const DTC& d) { return d.severity >= 3; });
  validators.push_back([](const DTC& d) { return d.code < 0x0500; });

  DTC sample{0x0400, "Throttle Pos Sensor", 3, true};
  std::cout << "10. std::function validators for DTC 0x0400:" << std::endl;
  int passCount = 0;
  for (size_t i = 0; i < validators.size(); ++i) {
    bool result = validators[i](sample);
    std::cout << "    Validator " << (i + 1) << ": "
              << (result ? "PASS" : "FAIL") << std::endl;
    if (result) passCount++;
  }
  std::cout << "    Passed " << passCount << "/"
            << validators.size() << "\n" << std::endl;

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // 11. LAMBDA AS FUNCTION PARAMETER (HIGHER-ORDER FUNCTIONS)
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // EN: You can write your own functions that ACCEPT Lambdas as parameters. This is the
  // "Higher-Order Function" pattern — a function that takes another function as an argument.
  // Using templates avoids std::function overhead.

  // EN: Using a Lambda with std::count_if for filtered counting.
  auto activeCount = std::count_if(faultLog.begin(), faultLog.end(),
                                   [](const DTC& d) { return d.active; });
  auto severeActive = std::count_if(faultLog.begin(), faultLog.end(),
                                    [](const DTC& d) {
    return d.active && d.severity >= 3;
  });
  std::cout << "11. Higher-order: Active DTCs = " << activeCount
            << ", Severe+Active = " << severeActive << "\n" << std::endl;

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // 12. IMMEDIATELY INVOKED LAMBDA EXPRESSION (IILE)
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // EN: Call a Lambda immediately at the point of definition. Useful for complex initialization
  // of const variables.

  const std::string ecuStatus = [&]() -> std::string {
    int fatal = 0;
    for (const auto& d : faultLog) {
      if (d.active && d.severity == 4) fatal++;
    }
    if (fatal > 0) return "LIMP MODE";
    if (activeFaultCount > 3) return "DEGRADED";
    return "NORMAL";
  }(); // EN: Note the () at the end — immediate invocation!

  std::cout << "12. IILE: ECU Status = " << ecuStatus << "\n" << std::endl;

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // 13. CONSTEXPR LAMBDA (C++17)
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // EN: Since C++17, Lambdas can be constexpr — computed at compile time. Perfect for automotive
  // look-up tables and protocol constants.

  constexpr auto crcByte = [](uint8_t data) constexpr -> uint8_t {
    // EN: Simplified CRC-8 step for demonstration.
    uint8_t crc = data;
    for (int i = 0; i < 8; ++i) {
      if (crc & 0x80U) {
        crc = static_cast<uint8_t>((crc << 1U) ^ 0x07U);
      } else {
        crc = static_cast<uint8_t>(crc << 1U);
      }
    }
    return crc;
  };

  constexpr uint8_t checksum = crcByte(0xA5);
  std::cout << "13. constexpr Lambda: CRC-8(0xA5) = 0x"
            << std::hex << static_cast<int>(checksum) << std::dec
            << " (computed at compile time!)\n" << std::endl;

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // 14. RECURSIVE LAMBDA (via std::function)
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // EN: Lambdas cannot directly call themselves (they have no name). To make a recursive Lambda,
  // wrap it in std::function so it can reference itself through the captured variable.

  // EN: Compute checksum of a DTC code tree (recursive severity sum).
  std::function<int(const std::vector<DTC>&, size_t)> recursiveSum =
      [&recursiveSum](const std::vector<DTC>& dtcs, size_t idx) -> int {
    if (idx >= dtcs.size()) return 0;
    return static_cast<int>(dtcs[idx].severity) +
           recursiveSum(dtcs, idx + 1);
  };

  std::cout << "14. Recursive Lambda: Total severity (recursive) = "
            << recursiveSum(faultLog, 0) << "\n" << std::endl;

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // 15. LAMBDA RETURNING A LAMBDA (FACTORY PATTERN)
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // EN: A Lambda can return ANOTHER Lambda. This is a powerful functional programming pattern —
  // creating specialized filters at runtime.

  auto makeSeverityFilter = [](uint8_t minSev) {
    return [minSev](const DTC& dtc) {
      return dtc.active && dtc.severity >= minSev;
    };
  };

  auto criticalFilter = makeSeverityFilter(4); // fatal only
  auto warningFilter = makeSeverityFilter(2);  // warning and above

  std::cout << "15. Lambda factory:" << std::endl;
  std::cout << "    Oil Pressure Low fatal? "
            << (criticalFilter(faultLog[1]) ? "YES" : "NO") << std::endl;
  std::cout << "    Battery Voltage warning+? "
            << (warningFilter(faultLog[2]) ? "YES" : "NO")
            << " (inactive, so NO)\n" << std::endl;

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // SUMMARY
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  std::cout << "=== LAMBDA CHEAT SHEET ===" << std::endl;
  std::cout << "  [](){}        — Simplest Lambda" << std::endl;
  std::cout << "  [=]           — Capture all by value" << std::endl;
  std::cout << "  [&]           — Capture all by reference" << std::endl;
  std::cout << "  [x, &y]       — Explicit mixed capture" << std::endl;
  std::cout << "  [x=move(v)]   — Init/move capture (C++14)" << std::endl;
  std::cout << "  mutable       — Modify value-captured copies" << std::endl;
  std::cout << "  auto params   — Generic Lambda (C++14)" << std::endl;
  std::cout << "  constexpr     — Compile-time Lambda (C++17)" << std::endl;
  std::cout << "  std::function — Type-erased storage" << std::endl;
  std::cout << "  IILE [](){}() — Immediate invocation" << std::endl;
  std::cout << "\nAll examples use Automotive/Embedded domain data." << std::endl;

  return 0;
}
