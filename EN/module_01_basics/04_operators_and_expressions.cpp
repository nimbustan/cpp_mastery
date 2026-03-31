/**
 * @file module_01_basics/04_operators_and_expressions.cpp
 * @brief Basics: Operators, Expressions & Precedence
 *
 * @details
 * =============================================================================
 * [THEORY: Operator Categories in C++]
 * =============================================================================
 * EN: C++ provides a rich set of operators grouped as:
 * 1. Arithmetic      (+, -, *, /, %)
 * 2. Comparison      (==, !=, <, >, <=, >=)
 * 3. Logical         (&&, ||, !)
 * 4. Bitwise         (&, |, ^, ~, <<, >>)
 * 5. Assignment      (=, +=, -=, *=, /=, %=, <<=, >>=, &=, |=, ^=)
 * 6. Ternary         (condition ? trueVal : falseVal)
 * 7. Increment       (++x pre, x++ post, --x, x--)
 *
 * =============================================================================
 * [CPPREF DEPTH: Operator Precedence & Associativity]
 * =============================================================================
 * EN: Operators follow a strict "Precedence Table" (highest -> lowest):
 *  1. `()`              (Parentheses)    — Highest
 *  2. `* / %`                            — Left-to-Right (L→R)
 *  3. `+ -`                              — L→R
 *  4. `<< >>`                            — L→R
 *  5. `< <= > >=`                        — L→R
 *  6. `== !=`                            — L→R
 *  7. `&`               (Bitwise AND)    — L→R
 *  8. `^`               (XOR)            — L→R
 *  9. `|`               (Bitwise OR)     — L→R
 * 10. `&&`                               — L→R
 * 11. `||`                               — L→R
 * 12. `?:`              (Ternary)        — R→L
 * 13. `= += -= ...`                      — R→L (Assignment is Right-to-Left!)
 *
 * ⚠  Ref: https://en.cppreference.com/w/cpp/language/operator_precedence
 *
 * @note Compile: g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion 04_operators_and_expressions.cpp -o 04_operators_and_expressions
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <bitset>
#include <iostream>

int main() {
  std::cout << "=== MODULE 1: OPERATORS & EXPRESSIONS ===\n" << std::endl;

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // 1. ARITHMETIC OPERATORS
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  std::cout << "--- 1. Arithmetic Operators ---" << std::endl;

  int rpm = 3500;                       // Engine RPM
  int idleRpm = 800;                    // Idle RPM threshold
  int rpmAboveIdle = rpm - idleRpm;     // 2700
  int doubleRpm = rpm * 2;             // 7000
  int avgRpm = (rpm + idleRpm) / 2;    // 2150
  int remainder = rpm % 1000;           // 500 (modulo)

  std::cout << "RPM: " << rpm << std::endl;
  std::cout << "RPM above idle: " << rpmAboveIdle << std::endl;
  std::cout << "Double RPM: " << doubleRpm << std::endl;
  std::cout << "Average with idle: " << avgRpm << std::endl;
  std::cout << "RPM % 1000 (modulo): " << remainder << "\n" << std::endl;

  // EN: Integer division truncates! 7 / 2 = 3 (not 3.5)
  std::cout << "Integer division: 7 / 2 = " << (7 / 2) << std::endl;
  std::cout << "Double  division: 7.0 / 2 = " << (7.0 / 2) << "\n" << std::endl;

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // 2. COMPARISON OPERATORS
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  std::cout << "--- 2. Comparison Operators ---" << std::endl;

  int coolantTemp = 95;          // °C
  int overheatThreshold = 110;   // °C limit

  // EN: These all return bool (true=1 / false=0).
  std::cout << "coolantTemp == 95  : " << (coolantTemp == 95) << std::endl;
  std::cout << "coolantTemp != 100 : " << (coolantTemp != 100) << std::endl;
  std::cout << "coolantTemp <  110 : " << (coolantTemp < overheatThreshold)
            << std::endl;
  std::cout << "coolantTemp >= 90  : " << (coolantTemp >= 90) << "\n"
            << std::endl;

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // 3. LOGICAL OPERATORS
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  std::cout << "--- 3. Logical Operators ---" << std::endl;

  bool engineOn = true;
  bool seatbeltOn = false;

  // EN: && = AND, || = OR, ! = NOT
  std::cout << "Engine ON && Seatbelt ON : " << (engineOn && seatbeltOn)
            << std::endl;
  std::cout << "Engine ON || Seatbelt ON : " << (engineOn || seatbeltOn)
            << std::endl;
  std::cout << "!seatbeltOn (NOT)        : " << (!seatbeltOn) << std::endl;

  // EN: Short-circuit evaluation: if left side of && is false, right side is NEVER evaluated.
  // This saves CPU cycles in embedded systems.
  if (engineOn && coolantTemp > 80) {
    std::cout << "[ECU] Engine running and coolant warm — normal.\n"
              << std::endl;
  }

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // 4. INCREMENT / DECREMENT
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  std::cout << "--- 4. Increment & Decrement ---" << std::endl;

  int fuelLevel = 50; // % fuel remaining

  // EN: Pre-increment (++x): Increments FIRST, then uses the new value.
  std::cout << "Pre-increment ++fuelLevel: " << ++fuelLevel << std::endl; // 51

  // EN: Post-increment (x++): Uses current value FIRST, then increments.
  std::cout << "Post-increment fuelLevel++: " << fuelLevel++ << std::endl; // 51
  std::cout << "After post-increment     : " << fuelLevel << std::endl;   // 52

  // Pre-decrement and Post-decrement
  std::cout << "Pre-decrement --fuelLevel: " << --fuelLevel << std::endl; // 51
  std::cout << "Post-decrement fuelLevel--:" << fuelLevel-- << std::endl; // 51
  std::cout << "After post-decrement     : " << fuelLevel << "\n"
            << std::endl; // 50

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // 5. COMPOUND ASSIGNMENT
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  std::cout << "--- 5. Compound Assignment ---" << std::endl;

  int throttle = 30; // Throttle position %
  std::cout << "Initial throttle: " << throttle << " %" << std::endl;

  throttle += 20; // throttle = throttle + 20
  std::cout << "After += 20     : " << throttle << " %" << std::endl;

  throttle -= 10; // throttle = throttle - 10
  std::cout << "After -= 10     : " << throttle << " %" << std::endl;

  throttle *= 2; // throttle = throttle * 2
  std::cout << "After *= 2      : " << throttle << " %" << std::endl;

  throttle /= 4; // throttle = throttle / 4
  std::cout << "After /= 4      : " << throttle << " %" << std::endl;

  throttle %= 7; // throttle = throttle % 7
  std::cout << "After %= 7      : " << throttle << " %\n" << std::endl;

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // 6. TERNARY OPERATOR
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  std::cout << "--- 6. Ternary Operator ---" << std::endl;

  int batteryVoltage = 12;
  // EN: condition ? value_if_true : value_if_false
  std::string batteryStatus =
      (batteryVoltage >= 12) ? "NORMAL" : "LOW VOLTAGE";
  std::cout << "Battery (" << batteryVoltage
            << "V): " << batteryStatus << std::endl;

  // EN: Nested ternary (use sparingly — hurts readability)
  int speedKmh = 135;
  std::string zone = (speedKmh > 130) ? "DANGER"
                     : (speedKmh > 90) ? "HIGHWAY"
                                       : "CITY";
  std::cout << "Speed " << speedKmh << " km/h -> Zone: " << zone << "\n"
            << std::endl;

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // 7. BITWISE OPERATORS (Preview)
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  std::cout << "--- 7. Bitwise Operators (Preview) ---" << std::endl;

  // EN: Covered in depth in Module 9. Quick demo here.
  // std::bitset<8> prints values in binary representation.
  unsigned char statusReg = 0b00001010; // Binary literal (C++14)

  std::cout << "OR  (0b1010 | 0b0101): " << std::bitset<8>(0b1010 | 0b0101) << std::endl;
  std::cout << "AND (0b1010 & 0b1100): " << std::bitset<8>(0b1010 & 0b1100) << std::endl;
  std::cout << "XOR (0b1010 ^ 0b1100): " << std::bitset<8>(0b1010 ^ 0b1100) << std::endl;
  std::cout << "Left Shift (1 << 3)  : " << std::bitset<8>(1 << 3) << std::endl;
  std::cout << "Right Shift (16 >> 2): " << std::bitset<8>(16 >> 2) << std::endl;
  std::cout << "NOT (~statusReg(~0b00001010)): "
            << std::bitset<8>(static_cast<unsigned char>(~statusReg))
            << "\n" << std::endl;

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // 8. OPERATOR PRECEDENCE DEMO
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  std::cout << "--- 8. Precedence Demo ---" << std::endl;

  // EN: Without parentheses, * binds tighter than +.
  int resultA = 2 + 3 * 4;     // 14 (not 20!)
  int resultB = (2 + 3) * 4;   // 20 (forced with parentheses)

  std::cout << "2 + 3 * 4     = " << resultA << " (mul first!)" << std::endl;
  std::cout << "(2 + 3) * 4   = " << resultB << " (forced add first)"
            << std::endl;

  // EN: Assignment is RIGHT-TO-LEFT (R→L).
  int a = 0;
  int b = 0;
  int c = 0;
  a = b = c = 42; // c=42, b=c(42), a=b(42)
  std::cout << "a = b = c = 42 -> a=" << a << " b=" << b << " c=" << c
            << std::endl;

  return 0;
}
