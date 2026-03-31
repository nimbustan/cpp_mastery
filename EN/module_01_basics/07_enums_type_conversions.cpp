/**
 * @file module_01_basics/07_enums_type_conversions.cpp
 * @brief Basics: Enumerations, auto, typedef/using & Type Conversions
 *
 * @details
 * =============================================================================
 * [THEORY: enum vs enum class]
 * =============================================================================
 * EN: Classic C-style `enum` leaks its members into the enclosing scope and implicitly
 *     converts to `int`. This causes name collisions and bugs.
 *     Modern `enum class` (C++11) provides:
 *     1. Scoped names: `Color::Red` (not just `Red`).
 *     2. No implicit conversion to int — you must use `static_cast`.
 *     3. Underlying type can be specified: `enum class Gear : uint8_t`.
 *
 * =============================================================================
 * [THEORY: Type Conversions]
 * =============================================================================
 * EN: C++ performs two kinds of conversions:
 *     1. Implicit (automatic): The compiler silently widens/narrows types
 *        (e.g., int → double). Narrowing conversions can lose data!
 *     2. Explicit (casts):
 *        a. `static_cast<T>()`: Compile-time type conversion (SAFEST).
 *        b. `(T)x` or `T(x)`: C-style cast (DANGEROUS, no checks).
 *        c. `reinterpret_cast`, `const_cast`, `dynamic_cast` for advanced cases.
 *
 * =============================================================================
 * [CPPREF DEPTH: auto Type Deduction]
 * =============================================================================
 * EN: `auto` (C++11) tells the compiler to deduce the type from the initializer.
 *     It reduces boilerplate but should be used carefully:
 *     1. Good: `auto it = vec.begin();` (avoids the long iterator type).
 *     2. Bad:  `auto x = 42;` (is it int? unsigned? long? — clarity lost).
 *     3. `decltype(expr)`: Deduces the type of an expression without evaluating it.
 *
 * ⚠  Ref: https://en.cppreference.com/w/cpp/language/enum
 * ⚠  Ref: https://en.cppreference.com/w/cpp/language/static_cast
 * ⚠  Ref: https://en.cppreference.com/w/cpp/language/auto
 *
 * @note Compile: g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion 07_enums_type_conversions.cpp -o 07_enums_type_conversions
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <cstdint> // uint8_t
#include <iostream>
#include <string>

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// 1. CLASSIC enum (C-Style — avoid in new code)
// ═════════════════════════════════════════════════════════════════════════════════════════════════
// EN: Classic enum members LEAK into the surrounding scope. After this declaration,
//     RED, GREEN, BLUE become global names — as if you wrote `const int RED = 0;`.
//     Problem: If another enum also defines RED, you get a COMPILE ERROR (name collision).
//     Also, classic enum implicitly converts to int, so `int x = RED;` compiles silently.
//     For these reasons, prefer `enum class` in modern C++.
enum LegacyIndicatorColor { RED, GREEN, BLUE };

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// 2. MODERN enum class (C++11 — strongly typed)
// ═════════════════════════════════════════════════════════════════════════════════════════════════
// EN: Scoped and type-safe. Must use GearPosition::Park syntax.
//     `: uint8_t` specifies the UNDERLYING TYPE — each enumerator is stored as a
//     single byte (0-255). Without this, the compiler picks the default (usually int).
//     In embedded/automotive code, specifying the underlying type is essential for
//     memory layout control and protocol serialization (e.g., CAN frame bytes).
enum class GearPosition : uint8_t {
  Park = 0,
  Reverse = 1,
  Neutral = 2,
  Drive = 3,
  Sport = 4
};

// EN: ECU operating states for a Body Control Module.
enum class ECUState { Off, Booting, Running, Sleep, Error };

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// 3. typedef & using (Type Aliases)
// ═════════════════════════════════════════════════════════════════════════════════════════════════
// EN: Type aliases give meaningful names to primitive types. Instead of writing
//     `unsigned long` everywhere, you write `SensorId` — the code becomes self-documenting.
//     `typedef` is the old C-style syntax. It works but reads "backwards".
typedef unsigned long SensorId;

// EN: `using` (C++11) is the modern replacement — reads left-to-right like assignment.
//     `using MilliVolts = int;` means "MilliVolts IS an int".
//     It's also the ONLY syntax that supports template aliases:
//         template<typename T> using Vec = std::vector<T>;  // typedef cannot do this!
//     Prefer `using` in all new code.
using MilliVolts = int;
using TemperatureC = double;

int main() {
  std::cout << "=== MODULE 1: ENUMS, AUTO & TYPE CONVERSIONS ===\n"
            << std::endl;

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // 1. CLASSIC ENUM DEMO
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  std::cout << "--- 1. Classic enum (Legacy) ---" << std::endl;

  // EN: We can write just `GREEN` — no prefix needed. This is the "leaking" behavior:
  //     GREEN is visible in the ENTIRE scope as if it were a plain integer constant.
  //     If another enum had Green, you'd get a name collision error.
  LegacyIndicatorColor indicator = GREEN;

  // EN: Classic enum SILENTLY converts to int — no cast needed!
  //     This compiles without warning: the compiler treats GREEN as just `1`.
  //     This is dangerous because you could accidentally compare colors with gear positions
  //     and the compiler would not catch the logical error.
  int colorCode = indicator;
  std::cout << "Indicator color code (int): " << colorCode << "\n"
            << std::endl;

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // 2. MODERN ENUM CLASS DEMO
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  std::cout << "--- 2. enum class (Modern C++11) ---" << std::endl;

  // EN: enum class members are SCOPED — you must write GearPosition::Drive,
  //     not just "Drive". This prevents name collisions across different enums.
  GearPosition gear = GearPosition::Drive;

  // EN: Unlike classic enum, enum class does NOT implicitly convert to int.
  //     The line below would cause a COMPILE ERROR:
  //         int gearNum = gear;   // ERROR: cannot convert 'GearPosition' to 'int'
  //     You MUST use static_cast<> to explicitly convert. This is by design —
  //     it prevents accidental mixing of unrelated enum types with integers.
  int gearNum = static_cast<int>(gear);
  std::cout << "Gear position: " << gearNum << " (Drive)" << std::endl;

  // EN: Underlying type is uint8_t (we specified it in the declaration).
  //     static_cast to uint8_t gives the raw byte value.
  auto sportRaw = static_cast<uint8_t>(GearPosition::Sport);
  std::cout << "Sport raw (uint8_t): " << static_cast<int>(sportRaw) << std::endl;

  // EN: enum class is ideal for switch statements — the compiler can warn
  //     if you forget to handle a case (with -Wswitch).
  ECUState bcmState = ECUState::Running;
  switch (bcmState) {
  case ECUState::Off:
    std::cout << "BCM: OFF" << std::endl;
    break;
  case ECUState::Booting:
    std::cout << "BCM: Booting..." << std::endl;
    break;
  case ECUState::Running:
    std::cout << "BCM: Running normally" << std::endl;
    break;
  case ECUState::Sleep:
    std::cout << "BCM: Sleep mode" << std::endl;
    break;
  case ECUState::Error:
    std::cout << "BCM: ERROR!" << std::endl;
    break;
  }
  std::cout << std::endl;

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // 3. typedef
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  std::cout << "--- 3. typedef & using aliases ---" << std::endl;

  // EN: Instead of `unsigned long egtId = 1001UL;`, we write `SensorId egtId = 1001UL;`.
  //     The compiled code is IDENTICAL — type aliases don't create new types, just new names.
  //     But the readability improvement is significant: you immediately know this
  //     variable holds a sensor identifier, not just "some unsigned long".
  SensorId egtId = 1001UL;
  MilliVolts reading = 3300;
  TemperatureC ambientTemp = 22.5;

  std::cout << "Sensor ID  : " << egtId << std::endl;
  std::cout << "Reading    : " << reading << " mV" << std::endl;
  std::cout << "Ambient    : " << ambientTemp << " °C\n" << std::endl;

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // 4. auto TYPE DEDUCTION (C++11)
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  std::cout << "--- 4. auto keyword (C++11) ---" << std::endl;

  // EN: `auto` tells the compiler: "look at the right-hand side and figure out the type".
  //     13.8 is a double literal       → voltage becomes `double`.
  //     5 is an int literal             → ecuCount becomes `int`.
  //     std::string("...") is a string  → vinStr becomes `std::string`.
  //     auto does NOT mean "untyped" — the type is fixed at compile time, just inferred.
  auto voltage = 13.8;
  auto ecuCount = 5;
  auto vinStr = std::string("WBAPH5C55BA271234");

  // EN: CAUTION: `auto x = "hello";` deduces `const char*`, NOT `std::string`!
  //     If you want std::string, write: `auto s = std::string("hello");`
  //     or use the string literal suffix: `auto s = "hello"s;` (C++14, needs `using namespace std::literals;`)

  std::cout << "voltage (auto = double): " << voltage << std::endl;
  std::cout << "ecuCount (auto = int)  : " << ecuCount << std::endl;
  std::cout << "vinStr (auto = string) : " << vinStr << "\n" << std::endl;

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // 5. IMPLICIT CONVERSIONS
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  std::cout << "--- 5. Implicit Conversions ---" << std::endl;

  // EN: WIDENING (safe): int → double. The compiler automatically promotes the
  //     smaller type to the larger one. 3500 becomes 3500.0 with no data loss.
  //     This happens because double can represent all int values (up to 2^53).
  //     Widening conversions: bool→int, int→long, int→double, float→double.
  int rpm = 3500;
  double rpmDouble = rpm;
  std::cout << "int 3500 -> double: " << rpmDouble << std::endl;

  // EN: NARROWING (DANGEROUS!): double → int. The fractional part is TRUNCATED
  //     (chopped off, NOT rounded). 2.75 becomes 2, not 3.
  //     The compiler may warn with -Wconversion. In C++11 brace initialization,
  //     narrowing is a COMPILE ERROR: `int x{2.75};` → error!
  //     Always use static_cast to make narrowing explicit and intentional.
  double precisePressure = 2.75;
  int roughPressure = static_cast<int>(precisePressure); // 2 (not 2.75!)
  std::cout << "double 2.75 -> int: " << roughPressure
            << " (fractional part LOST!)" << std::endl;

  // EN: bool → int: `true` becomes 1, `false` becomes 0. This is guaranteed by
  //     the C++ standard. The reverse also works: any non-zero int becomes `true`,
  //     zero becomes `false`. This is used heavily in conditions:
  //         if (absFlag) { ... }  // works because non-zero = true
  bool absActive = true;
  int absFlag = absActive;
  std::cout << "bool true -> int: " << absFlag << "\n" << std::endl;

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // 6. EXPLICIT CONVERSIONS (static_cast)
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  std::cout << "--- 6. static_cast (Explicit) ---" << std::endl;

  // EN: `static_cast<T>(expr)` is the SAFEST C++ cast. The compiler checks at
  //     compile time whether the conversion is valid. If it's not (e.g., casting
  //     a pointer to an unrelated class), you get a COMPILE ERROR — not silent UB.
  //     Use static_cast whenever you intentionally convert between numeric types.
  int totalDistance = 15000; // meters
  int numSegments = 7;

  // EN: CLASSIC TRAP — Integer division truncates! When BOTH operands are int,
  //     C++ performs INTEGER division: 15000/7 = 2142 (not 2142.857...).
  //     The fractional part is silently discarded. This is one of the most
  //     common bugs in C/C++ code.
  std::cout << "Integer div: " << totalDistance / numSegments << std::endl;

  // EN: SOLUTION — Cast at least ONE operand to double before division.
  //     This forces floating-point division and preserves the fractional part.
  //     `static_cast<double>(15000) / 7` → `15000.0 / 7` → `2142.857...`
  double avgSegment =
      static_cast<double>(totalDistance) / static_cast<double>(numSegments);
  std::cout << "static_cast div: " << avgSegment << " m/segment" << std::endl;

  // EN: Casting enum class to its underlying type. GearPosition::Sport has value 4.
  //     We cast to uint8_t (the underlying type we declared), then to int for printing.
  //     Why double cast? Because std::cout treats uint8_t as a character (ASCII 4),
  //     not as the number 4. The outer static_cast<int> converts it to a printable number.
  auto gearValue = static_cast<uint8_t>(GearPosition::Sport);
  std::cout << "GearPosition::Sport -> uint8_t: "
            << static_cast<int>(gearValue) << "\n" << std::endl;

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // 7. C-STYLE CAST (Avoid!)
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  std::cout << "--- 7. C-Style Cast (Dangerous Legacy) ---" << std::endl;

  // EN: C-style cast `(int)x` is inherited from C. It looks simple, but it's the
  //     MOST DANGEROUS cast because the compiler tries MULTIPLE cast types in order:
  //         1. const_cast       (removes const — can cause UB if you write to it)
  //         2. static_cast      (normal type conversion)
  //         3. reinterpret_cast (raw bit reinterpretation — almost always UB)
  //     You have NO CONTROL over which one the compiler picks. It just picks the
  //     first one that compiles — even if the result is undefined behavior.
  //     In modern C++, ALWAYS use the named casts (static_cast, etc.) instead.
  //     Named casts are searchable with grep/IDE and make your INTENT explicit.
  double sensorVoltage = 4.85;
  int rawADC = (int)sensorVoltage; // C-style: works but unsafe
  std::cout << "C-style (int)4.85 = " << rawADC
            << " (Compiles, but avoid in modern C++!)" << std::endl;

  return 0;
}
