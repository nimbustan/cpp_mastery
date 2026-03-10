/**
 * @file module_01_basics/04_operators_and_expressions.cpp
 * @brief Basics: Operators, Expressions & Precedence / Temeller: Operatörler, İfadeler ve
 * Öncelik Sırası
 *
 * @details
 * =============================================================================
 * [THEORY: Operator Categories in C++ / TEORİ: C++ Operatör Kategorileri]
 * =============================================================================
 * EN: C++ provides a rich set of operators grouped as: 1. Arithmetic   (+, -, *, /, %) 2.
 * Comparison   (==, !=, <, >, <=, >=) 3. Logical      (&&, ||, !) 4. Bitwise      (&, |, ^, ~,
 * <<, >>) 5. Assignment   (=, +=, -=, *=, /=, %=, <<=, >>=, &=, |=, ^=) 6. Ternary     
 * (condition ? trueVal : falseVal) 7. Increment    (++x pre, x++ post, --x, x--)
 *
 * TR: C++ zengin bir operatör setine sahiptir: 1. Aritmetik    (+, -, *, /, %) 2.
 * Karşılaştırma(==, !=, <, >, <=, >=) 3. Mantıksal    (&&, ||, !) 4. Bitsel       (&, |, ^, ~,
 * <<, >>) 5. Atama        (=, +=, -=, *=, /=, %=, <<=, >>=, &=, |=, ^=) 6. Üçlü (Ternary)(koşul
 * ? doğru : yanlış) 7. Artırma/Azaltma(++x ön, x++ son, --x, x--)
 *
 * =============================================================================
 * [CPPREF DEPTH: Operator Precedence & Associativity / CPPREF DERİNLİK: Operatör Önceliği ve
 * İlişkilendirme Yönü]
 * =============================================================================
 * EN: Operators follow a strict "Precedence Table" (highest -> lowest): 1. `()` (Parentheses)  
 * — Highest 2. `* / %`              — Left-to-Right (L→R) 3. `+ -`                — L→R 4. `<<
 * >>`              — L→R 5. `< <= > >=`          — L→R 6. `== !=`              — L→R 7. `&`
 * (Bitwise AND)    — L→R 8. `^` (XOR)            — L→R 9. `|` (Bitwise OR)     — L→R 10. `&&`   
 * — L→R 11. `||`                 — L→R 12. `?:` (Ternary)       — R→L 13. `= += -= ...`        —
 * R→L (Assignment is Right-to-Left!)
 *
 * TR: Operatörler katı bir "Öncelik Tablosu"na uyar (yüksek → düşük): 1. `()` (Parantez)       
 * — En yüksek 2. `* / %`                — Soldan Sağa (L→R) 3. `+ -`                  — L→R 4.
 * `<< >>`                — L→R 5. `< <= > >=`            — L→R 6. `== !=`                — L→R
 * 7. `&` (Bitsel AND)       — L→R 8. `^` (XOR)              — L→R 9. `|` (Bitsel OR)        —
 * L→R 10. `&&`                   — L→R 11. `||`                   — L→R 12. `?:` (Üçlü Ternary) 
 * — R→L 13. `= += -= ...`          — R→L (Atama Sağdan Soladır!)
 *
 * ⚠  Ref: https://en.cppreference.com/w/cpp/language/operator_precedence
 *
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <iostream>

int main() {
  std::cout << "=== MODULE 1: OPERATORS & EXPRESSIONS ===\n" << std::endl;

  // ===============================================================================================
  // 1. ARITHMETIC OPERATORS / ARİTMETİK OPERATÖRLER
  // ===============================================================================================
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
  // TR: Tam sayı bölmesi kesir kısmını atar! 7 / 2 = 3 (3.5 değil)
  std::cout << "Integer division: 7 / 2 = " << (7 / 2) << std::endl;
  std::cout << "Double  division: 7.0 / 2 = " << (7.0 / 2) << "\n" << std::endl;

  // ===============================================================================================
  // 2. COMPARISON OPERATORS / KARŞILAŞTIRMA OPERATÖRLERİ
  // ===============================================================================================
  std::cout << "--- 2. Comparison Operators ---" << std::endl;

  int coolantTemp = 95;          // °C
  int overheatThreshold = 110;   // °C limit

  // EN: These all return bool (true=1 / false=0).
  // TR: Hepsi bool döndürür (true=1 / false=0).
  std::cout << "coolantTemp == 95  : " << (coolantTemp == 95) << std::endl;
  std::cout << "coolantTemp != 100 : " << (coolantTemp != 100) << std::endl;
  std::cout << "coolantTemp <  110 : " << (coolantTemp < overheatThreshold)
            << std::endl;
  std::cout << "coolantTemp >= 90  : " << (coolantTemp >= 90) << "\n"
            << std::endl;

  // ===============================================================================================
  // 3. LOGICAL OPERATORS / MANTIKSAL OPERATÖRLER
  // ===============================================================================================
  std::cout << "--- 3. Logical Operators ---" << std::endl;

  bool engineOn = true;
  bool seatbeltOn = false;

  // EN: && = AND, || = OR, ! = NOT
  // TR: && = VE, || = VEYA, ! = DEĞİL
  std::cout << "Engine ON && Seatbelt ON : " << (engineOn && seatbeltOn)
            << std::endl;
  std::cout << "Engine ON || Seatbelt ON : " << (engineOn || seatbeltOn)
            << std::endl;
  std::cout << "!seatbeltOn (NOT)        : " << (!seatbeltOn) << std::endl;

  // EN: Short-circuit evaluation: if left side of && is false, right side is NEVER evaluated.
  // This saves CPU cycles in embedded systems.
  // TR: Kısa devre değerlendirmesi: && ifadesinin sol tarafı false ise sağ taraf ASLA
  // değerlendirilmez. Gömülü sistemlerde CPU döngüsü kazandırır.
  if (engineOn && coolantTemp > 80) {
    std::cout << "[ECU] Engine running and coolant warm — normal.\n"
              << std::endl;
  }

  // ===============================================================================================
  // 4. INCREMENT / DECREMENT (ARTIRMA / AZALTMA)
  // ===============================================================================================
  std::cout << "--- 4. Increment & Decrement ---" << std::endl;

  int fuelLevel = 50; // % fuel remaining

  // EN: Pre-increment (++x): Increments FIRST, then uses the new value.
  // TR: Ön artırma (++x): ÖNCE artırır, sonra kullanır.
  std::cout << "Pre-increment ++fuelLevel: " << ++fuelLevel << std::endl; // 51

  // EN: Post-increment (x++): Uses current value FIRST, then increments.
  // TR: Son artırma (x++): ÖNCE mevcut değeri kullanır, sonra artırır.
  std::cout << "Post-increment fuelLevel++: " << fuelLevel++ << std::endl; // 51
  std::cout << "After post-increment     : " << fuelLevel << std::endl;   // 52

  // Pre-decrement and Post-decrement
  std::cout << "Pre-decrement --fuelLevel: " << --fuelLevel << std::endl; // 51
  std::cout << "Post-decrement fuelLevel--:" << fuelLevel-- << std::endl; // 51
  std::cout << "After post-decrement     : " << fuelLevel << "\n"
            << std::endl; // 50

  // ===============================================================================================
  // 5. COMPOUND ASSIGNMENT / BİLEŞİK ATAMA OPERATÖRLERİ
  // ===============================================================================================
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

  // ===============================================================================================
  // 6. TERNARY OPERATOR / ÜÇLÜ OPERATÖR
  // ===============================================================================================
  std::cout << "--- 6. Ternary Operator ---" << std::endl;

  int batteryVoltage = 12;
  // EN: condition ? value_if_true : value_if_false
  // TR: koşul ? doğruysa_değer : yanlışsa_değer
  std::string batteryStatus =
      (batteryVoltage >= 12) ? "NORMAL" : "LOW VOLTAGE";
  std::cout << "Battery (" << batteryVoltage
            << "V): " << batteryStatus << std::endl;

  // EN: Nested ternary (use sparingly — hurts readability)
  // TR: İç içe ternary (dikkatli kullanın — okunabilirliği bozar)
  int speedKmh = 135;
  std::string zone = (speedKmh > 130) ? "DANGER"
                     : (speedKmh > 90) ? "HIGHWAY"
                                       : "CITY";
  std::cout << "Speed " << speedKmh << " km/h -> Zone: " << zone << "\n"
            << std::endl;

  // ===============================================================================================
  // 7. BITWISE OPERATORS (Preview) / BİTSEL OPERATÖRLER (Ön İzleme)
  // ===============================================================================================
  std::cout << "--- 7. Bitwise Operators (Preview) ---" << std::endl;

  // EN: Covered in depth in Module 9. Quick demo here:
  // TR: Detaylı anlatım Modül 9'da. Burada kısa önizleme:
  unsigned char statusReg = 0b00001010; // Binary literal (C++14)

  std::cout << "OR  (0b1010 | 0b0101): " << (0b1010 | 0b0101) << std::endl;
  std::cout << "AND (0b1010 & 0b1100): " << (0b1010 & 0b1100) << std::endl;
  std::cout << "XOR (0b1010 ^ 0b1100): " << (0b1010 ^ 0b1100) << std::endl;
  std::cout << "Left Shift (1 << 3)  : " << (1 << 3) << std::endl;
  std::cout << "Right Shift (16 >> 2): " << (16 >> 2) << std::endl;
  std::cout << "NOT (~statusReg)     : " << static_cast<int>(
                   static_cast<unsigned char>(~statusReg))
            << "\n" << std::endl;

  // ===============================================================================================
  // 8. OPERATOR PRECEDENCE DEMO / OPERATÖR ÖNCELİK GÖSTERİMİ
  // ===============================================================================================
  std::cout << "--- 8. Precedence Demo ---" << std::endl;

  // EN: Without parentheses, * binds tighter than +.
  // TR: Parantez olmadan * operatörü + operatöründen önce çalışır.
  int resultA = 2 + 3 * 4;     // 14 (not 20!)
  int resultB = (2 + 3) * 4;   // 20 (forced with parentheses)

  std::cout << "2 + 3 * 4     = " << resultA << " (mul first!)" << std::endl;
  std::cout << "(2 + 3) * 4   = " << resultB << " (forced add first)"
            << std::endl;

  // EN: Assignment is RIGHT-TO-LEFT (R→L).
  // TR: Atama operatörü SAĞDAN SOLA (R→L) çalışır.
  int a = 0;
  int b = 0;
  int c = 0;
  a = b = c = 42; // c=42, b=c(42), a=b(42)
  std::cout << "a = b = c = 42 -> a=" << a << " b=" << b << " c=" << c
            << std::endl;

  return 0;
}
