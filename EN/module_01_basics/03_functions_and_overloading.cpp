/**
 * @file module_01_basics/03_functions_and_overloading.cpp
 * @brief Basics: Functions, Overloading & Default Parameters
 *
 * @details
 * =============================================================================
 * [THEORY: Function Anatomy]
 * =============================================================================
 * EN: A function in C++ has 4 parts:
 * 1. Return type — what the function gives back (int, double, void, bool, ...)
 * 2. Name        — identifier used to call it
 * 3. Parameters  — input data (can be empty)
 * 4. Body        — the code block `{ }` that executes
 *
 * Declaration (prototype) tells the compiler "this function exists":
 *   int add(int a, int b);
 * Definition provides the actual body:
 *   int add(int a, int b) { return a + b; }
 *
 * =============================================================================
 * [THEORY: Why Function Overloading is Possible]
 * =============================================================================
 * EN: In C, you CANNOT have two functions with the same name — the compiler only checks
 * the name. In C++, the compiler appends parameter types to the function name behind the
 * scenes (called "Name Mangling"):
 *   print(int)   -> _Z5printi
 *   print(float) -> _Z5printf
 * Thus you can have many functions with the same name as long as parameters differ!
 *
 * =============================================================================
 * [CPPREF DEPTH: Overload Resolution Rules]
 * =============================================================================
 * EN: How does the compiler decide which overloaded function to call?
 * 1. Exact Match     — passing exactly an `int` to `print(int)`.
 * 2. Promotion       — passing `char` promotes to `int` safely.
 * 3. Std Conversion  — passing `float` converts to `double`.
 * If ambiguous, the compiler throws a Hard Error (build fails).
 *
 * ⚠  Ref: https://en.cppreference.com/w/cpp/language/overload_resolution
 * ⚠  Ref: https://en.cppreference.com/w/cpp/language/default_arguments
 * ⚠  Ref: https://en.cppreference.com/w/cpp/language/inline
 *
 * @note Compile: g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion 03_functions_and_overloading.cpp -o 03_functions_and_overloading
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <cmath>
#include <iostream>
#include <string>

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// 1. FUNCTION DECLARATIONS (PROTOTYPES)
// ═════════════════════════════════════════════════════════════════════════════════════════════════
// EN: Prototypes tell the compiler about the function signature BEFORE main(). The actual body
// (definition) can come after main(). This is how large projects are organized with header files.
double celsiusToFahrenheit(double celsius);
double fahrenheitToCelsius(double fahrenheit);

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// 2. FUNCTION OVERLOADING
// ═════════════════════════════════════════════════════════════════════════════════════════════════
// EN: Same name, different parameter types — compiler picks the correct one via Name Mangling.
void displaySensor(int rawValue) {
  std::cout << "  [INT]    Raw sensor: " << rawValue << " counts" << std::endl;
}

void displaySensor(double voltage) {
  std::cout << "  [DOUBLE] Sensor voltage: " << voltage << " V" << std::endl;
}

void displaySensor(const std::string &name) {
  std::cout << "  [STRING] Sensor name: " << name << std::endl;
}

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// 3. DEFAULT PARAMETERS
// ═════════════════════════════════════════════════════════════════════════════════════════════════
// EN: Parameters with default values can be omitted at call site. Defaults must be specified
// from RIGHT to LEFT — you cannot skip a middle parameter.
void logDTC(const std::string &code, int severity = 1,
            const std::string &module = "UNKNOWN") {
  std::cout << "  DTC: " << code << " | Severity: " << severity
            << " | Module: " << module << std::endl;
}

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// 4. INLINE FUNCTIONS
// ═════════════════════════════════════════════════════════════════════════════════════════════════
// EN: `inline` suggests the compiler to replace the function call with the function body
// directly at the call site, avoiding the overhead of a function call (push/pop stack frame).
// The compiler may ignore the hint if the function is too complex. In modern C++, the compiler
// often inlines small functions automatically (even without the keyword).
inline double clampRPM(double rpm, double minRpm = 800.0, double maxRpm = 7000.0) {
  if (rpm < minRpm) return minRpm;
  if (rpm > maxRpm) return maxRpm;
  return rpm;
}

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// 5. CONSTEXPR FUNCTIONS
// ═════════════════════════════════════════════════════════════════════════════════════════════════
// EN: `constexpr` functions CAN be evaluated at compile time if called with compile-time
// constants. If called with runtime values, they behave like normal functions. This gives
// ZERO runtime cost for constant expressions.
constexpr int calculateBaudDivisor(int clockHz, int baudRate) {
  return clockHz / (16 * baudRate);
}

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// 6. RETURN TYPE EXAMPLES
// ═════════════════════════════════════════════════════════════════════════════════════════════════
// EN: Functions can return different types: int, double, bool, void (nothing), string, etc.

// Returns bool — used for condition checks
bool isOverheating(double tempC) { return tempC > 110.0; }

// Returns void — performs action, returns nothing
void printDivider() {
  std::cout << "  ----------------------------------------" << std::endl;
}

// Returns int — calculation result
int absoluteDifference(int a, int b) { return std::abs(a - b); }

// ════════════════════════════════════════════════════════════════════════════
//                                 MAIN
// ════════════════════════════════════════════════════════════════════════════
int main() {
  std::cout << "=== MODULE 1: FUNCTIONS & OVERLOADING ===\n" << std::endl;

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // 1. FUNCTION DECLARATIONS — calling prototyped functions
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  std::cout << "--- 1. Function Declaration & Definition ---" << std::endl;
  double boilingF = celsiusToFahrenheit(100.0);
  double bodyC = fahrenheitToCelsius(98.6);
  std::cout << "  100 °C = " << boilingF << " °F" << std::endl;
  std::cout << "  98.6 °F = " << bodyC << " °C\n" << std::endl;

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // 2. FUNCTION OVERLOADING
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  std::cout << "--- 2. Function Overloading ---" << std::endl;

  // EN: Compiler selects the correct overload based on argument type.
  displaySensor(4095);              // int version
  displaySensor(3.28);              // double version
  displaySensor("Knock Sensor B2"); // string version
  std::cout << std::endl;

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // 3. RETURN TYPE VARIETY
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  std::cout << "--- 3. Return Types ---" << std::endl;

  double engineTemp = 115.0;
  std::cout << "  Engine temp: " << engineTemp << " °C" << std::endl;
  std::cout << "  Overheating? " << (isOverheating(engineTemp) ? "YES" : "NO") << std::endl;

  int rpm1 = 3500, rpm2 = 4200;
  std::cout << "  RPM difference: |" << rpm1 << " - " << rpm2 << "| = "
            << absoluteDifference(rpm1, rpm2) << std::endl;

  printDivider();
  std::cout << std::endl;

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // 4. DEFAULT PARAMETERS
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  std::cout << "--- 4. Default Parameters ---" << std::endl;

  // EN: Call with varying number of arguments — defaults fill the rest.
  logDTC("P0300");                  // severity=1, module="UNKNOWN"
  logDTC("P0171", 2);              // module="UNKNOWN"
  logDTC("P0420", 3, "Powertrain"); // All specified
  std::cout << std::endl;

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // 5. INLINE FUNCTION
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  std::cout << "--- 5. Inline Function ---" << std::endl;

  // EN: clampRPM is likely inlined by the compiler — no function call overhead.
  std::cout << "  clampRPM(500)  = " << clampRPM(500) << " (min clamped)" << std::endl;
  std::cout << "  clampRPM(3500) = " << clampRPM(3500) << " (unchanged)" << std::endl;
  std::cout << "  clampRPM(9000) = " << clampRPM(9000) << " (max clamped)" << std::endl;
  std::cout << std::endl;

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // 6. CONSTEXPR FUNCTION
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  std::cout << "--- 6. constexpr Function ---" << std::endl;

  // EN: This is evaluated at COMPILE TIME — zero runtime cost. The result is baked into the
  // binary. No function call happens at runtime.
  constexpr int divisor115200 = calculateBaudDivisor(16000000, 115200);
  constexpr int divisor9600 = calculateBaudDivisor(16000000, 9600);

  std::cout << "  UART 16MHz / 115200 baud -> divisor = " << divisor115200 << std::endl;
  std::cout << "  UART 16MHz / 9600 baud   -> divisor = " << divisor9600 << std::endl;

  // EN: Can also be called with runtime values — behaves like a normal function then.
  int runtimeClock = 8000000;
  int runtimeBaud = 19200;
  int runtimeDiv = calculateBaudDivisor(runtimeClock, runtimeBaud);
  std::cout << "  UART 8MHz / 19200 baud   -> divisor = " << runtimeDiv << " (runtime)"
            << std::endl;
  std::cout << std::endl;

  return 0;
}

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// FUNCTION DEFINITIONS (after main)
// ═════════════════════════════════════════════════════════════════════════════════════════════════
// EN: These were declared (prototyped) BEFORE main, defined here AFTER main.
// This is the standard pattern: declaration in .h (header), definition in .cpp (source).

double celsiusToFahrenheit(double celsius) {
  return (celsius * 9.0 / 5.0) + 32.0;
}

double fahrenheitToCelsius(double fahrenheit) {
  return (fahrenheit - 32.0) * 5.0 / 9.0;
}
