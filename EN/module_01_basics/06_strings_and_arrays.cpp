/**
 * @file module_01_basics/06_strings_and_arrays.cpp
 * @brief Basics: Strings and Arrays
 *
 * @details
 * =============================================================================
 * [THEORY: C-Strings vs std::string]
 * =============================================================================
 * EN: C++ inherited NULL-terminated character arrays from C (`char str[]`).
 *     These are dangerous: no bounds checking, manual memory management,
 *     easy buffer overflows. Modern C++ uses `std::string` from `<string>`:
 *     1. Automatic memory management (grows/shrinks as needed).
 *     2. Safe concatenation with `+` operator.
 *     3. Rich API: `.length()`, `.substr()`, `.find()`, `.compare()`, etc.
 *
 * =============================================================================
 * [THEORY: C-Style Arrays vs std::array]
 * =============================================================================
 * EN: C-style arrays (`int arr[5]`) decay to pointers when passed to functions,
 *     losing size information. `std::array<int, 5>` (C++11) wraps a fixed-size
 *     array with:
 *     1. Bounds checking via `.at()`.
 *     2. Knows its own size via `.size()`.
 *     3. For dynamic sizes, use `std::vector`.
 *
 * =============================================================================
 * [CPPREF DEPTH: Small String Optimization (SSO)]
 * =============================================================================
 * EN: Most `std::string` implementations apply SSO: strings shorter than ~15-22
 *     characters are stored INSIDE the object (on the stack), avoiding heap
 *     allocation entirely. Longer strings trigger a `new` allocation. This is
 *     crucial for embedded systems where heap usage is expensive.
 *
 * ⚠  Ref: https://en.cppreference.com/w/cpp/string/basic_string
 * ⚠  Ref: https://en.cppreference.com/w/cpp/container/array
 *
 * @note Compile: g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion 06_strings_and_arrays.cpp -o 06_strings_and_arrays
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <array>
#include <iostream>
#include <string>

int main() {
  std::cout << "=== MODULE 1: STRINGS & ARRAYS ===\n" << std::endl;

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // 1. C-STYLE STRING (Legacy — avoid in new code)
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  std::cout << "--- 1. C-Style String (Legacy) ---" << std::endl;

  // EN: NULL-terminated character array. Last char is '\0' automatically.
  char vinCstr[] = "WBAPH5C55BA271234";
  std::cout << "VIN (C-String): " << vinCstr << std::endl;
  std::cout << "First char    : " << vinCstr[0] << std::endl;
  std::cout << "Array size    : " << sizeof(vinCstr)
            << " (17 chars + 1 null)\n" << std::endl;

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // 2. std::string (Modern C++)
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  std::cout << "--- 2. std::string (Modern C++) ---" << std::endl;

  std::string ecuName = "BCM_BodyControl";
  std::string version = "v2.4.1";

  // Concatenation
  std::string fullId = ecuName + " [" + version + "]";
  std::cout << "Full ID       : " << fullId << std::endl;
  std::cout << "Length        : " << fullId.length() << std::endl;
  std::cout << "Char at [4]   : " << fullId[4] << std::endl;
  std::cout << "Char at .at(4): " << fullId.at(4) << " (bounds-checked)"
            << std::endl;

  // Substring
  std::string prefix = fullId.substr(0, 3); // "BCM"
  std::cout << "Prefix (0,3)  : " << prefix << std::endl;

  // Find
  size_t pos = fullId.find("Body");
  if (pos != std::string::npos) {
    std::cout << "Found 'Body' at index: " << pos << std::endl;
  }

  // Compare
  std::string other = "BCM_BodyControl [v2.4.1]";
  std::cout << "Equal to other? : " << (fullId == other ? "Yes" : "No")
            << std::endl;

  // Append
  fullId.append(" — Active");
  std::cout << "After append  : " << fullId << std::endl;

  // Empty check
  std::string emptyStr;
  std::cout << "emptyStr.empty(): " << emptyStr.empty() << "\n" << std::endl;

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // 3. C-STYLE ARRAY
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  std::cout << "--- 3. C-Style Array ---" << std::endl;

  // EN: Fixed-size array of sensor voltage readings (mV).
  int sensorMv[5] = {3300, 2800, 3100, 2950, 3050};

  // Classic for iteration
  for (int i = 0; i < 5; ++i) {
    std::cout << "  Sensor[" << i << "] = " << sensorMv[i] << " mV"
              << std::endl;
  }
  std::cout << "Array sizeof: " << sizeof(sensorMv)
            << " bytes (" << sizeof(sensorMv) / sizeof(sensorMv[0])
            << " elements)\n" << std::endl;

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // 4. std::array<T, N> (C++11) — Modern Fixed-Size Array
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  std::cout << "--- 4. std::array (Modern C++11) ---" << std::endl;

  // EN: Knows its own size, supports bounds-checked access with .at().
  std::array<double, 4> egtTemps = {650.5, 720.3, 695.0, 710.8};

  std::cout << "EGT array size: " << egtTemps.size() << std::endl;
  for (size_t i = 0; i < egtTemps.size(); ++i) {
    std::cout << "  EGT_Cyl" << (i + 1) << ": " << egtTemps[i] << " °C"
              << std::endl;
  }

  // Safe access with .at() — throws std::out_of_range if bad index.
  std::cout << "egtTemps.at(2): " << egtTemps.at(2) << " °C" << std::endl;
  std::cout << "Front: " << egtTemps.front()
            << " | Back: " << egtTemps.back() << "\n" << std::endl;

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // 5. MULTIDIMENSIONAL ARRAY
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  std::cout << "--- 5. Multidimensional Array ---" << std::endl;

  // EN: 3 ECUs × 4 sensor channels each (voltage mV readings).
  int sensorGrid[3][4] = {
      {3300, 2900, 3100, 3050}, // ECU 0: BCM
      {3200, 3000, 2800, 3150}, // ECU 1: Powertrain
      {3100, 3250, 2950, 3000}  // ECU 2: HVAC
  };

  const char *ecuNames[] = {"BCM", "Powertrain", "HVAC"};
  for (int ecu = 0; ecu < 3; ++ecu) {
    std::cout << "  " << ecuNames[ecu] << ": ";
    for (int ch = 0; ch < 4; ++ch) {
      std::cout << sensorGrid[ecu][ch] << " mV";
      if (ch < 3) std::cout << ", ";
    }
    std::cout << std::endl;
  }
  std::cout << std::endl;

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // 6. STRING ITERATION
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  std::cout << "--- 6. String Iteration ---" << std::endl;

  std::string vin = "WBAPH5C55BA271234";
  std::cout << "VIN chars: ";
  for (const char &ch : vin) {
    std::cout << ch << ' ';
  }
  std::cout << "\n" << std::endl;

  // EN: Count digits in VIN.
  int digitCount = 0;
  for (const char &ch : vin) {
    if (ch >= '0' && ch <= '9') {
      ++digitCount;
    }
  }
  std::cout << "Digits in VIN: " << digitCount << std::endl;

  return 0;
}
