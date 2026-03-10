/**
 * @file module_01_basics/07_enums_type_conversions.cpp
 * @brief Basics: Enumerations, auto, typedef/using & Type Conversions / Temeller:
 * Numaralandırmalar, auto, typedef/using ve Tür Dönüşümleri
 *
 * @details
 * =============================================================================
 * [THEORY: enum vs enum class / TEORİ: enum ve enum class Karşılaştırması]
 * =============================================================================
 * EN: Classic C-style `enum` leaks its members into the enclosing scope and implicitly converts
 * to `int`. This causes name collisions and bugs. Modern `enum class` (C++11) provides: - Scoped
 * names: `Color::Red` (not just `Red`). - No implicit conversion to int — you must use
 * `static_cast`. - Underlying type can be specified: `enum class Gear : uint8_t`.
 *
 * TR: Klasik C-tarzı `enum` üyelerini kapsam dışına sızdırır ve `int`'e dolaylı olarak dönüşür.
 * Bu isim çakışmasına ve hatalara neden olur. Modern `enum class` (C++11): - Kapsamlı isimler:
 * `Color::Red` (sadece `Red` değil). - int'e dolaylı dönüşüm YOK — `static_cast`
 * kullanmalısınız. - Alt tür belirlenebilir: `enum class Gear : uint8_t`.
 *
 * =============================================================================
 * [THEORY: Type Conversions / TEORİ: Tür Dönüşümleri]
 * =============================================================================
 * EN: C++ performs two kinds of conversions: 1. Implicit (automatic): The compiler silently
 * widens/narrows types (e.g., int → double). Narrowing conversions can lose data! 2. Explicit
 * (casts): - `static_cast<T>()`: Compile-time type conversion (SAFEST). - `(T)x` or `T(x)`:
 * C-style cast (DANGEROUS, no checks). - `reinterpret_cast`, `const_cast`, `dynamic_cast` for
 * advanced cases.
 *
 * TR: C++ iki tür dönüşüm yapar: 1. Dolaylı (otomatik): Derleyici sessizce tipleri
 * genişletir/daraltır. Daraltma dönüşümleri veri kaybına yol açabilir! 2. Açık (cast): -
 * `static_cast<T>()`: Derleme zamanı dönüşümü (EN GÜVENLİ). - `(T)x` veya `T(x)`: C-tarzı cast
 * (TEHLİKELİ, kontrol yok). - `reinterpret_cast`, `const_cast`, `dynamic_cast` ileri seviye
 * durumlar için.
 *
 * =============================================================================
 * [CPPREF DEPTH: auto Type Deduction / CPPREF DERİNLİK: auto Tip Çıkarımı]
 * =============================================================================
 * EN: `auto` (C++11) tells the compiler to deduce the type from the initializer. It reduces
 * boilerplate but should be used carefully: - Good: `auto it = vec.begin();` (avoids the long
 * iterator type). - Bad:  `auto x = 42;` (is it int? unsigned? long? — clarity lost). -
 * `decltype(expr)`: Deduces the type of an expression without evaluating it.
 *
 * TR: `auto` (C++11) derleyiciye başlatıcıdan tipi çıkarmasını söyler. Standart kodu azaltır ama
 * dikkatli kullanılmalıdır: - İyi: `auto it = vec.begin();` (uzun iterator tipinden kaçınır). -
 * Kötü: `auto x = 42;` (int mi? unsigned mı? long mu? — netlik kaybolur). - `decltype(expr)`:
 * Bir ifadenin tipini, onu çalıştırmadan çıkarır.
 *
 * ⚠  Ref: https://en.cppreference.com/w/cpp/language/enum
 * ⚠  Ref: https://en.cppreference.com/w/cpp/language/static_cast
 * ⚠  Ref: https://en.cppreference.com/w/cpp/language/auto
 *
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <cstdint> // uint8_t
#include <iostream>
#include <string>

// =================================================================================================
// 1. CLASSIC enum (C-Style — avoid in new code)
// =================================================================================================
// EN: Members leak into surrounding scope. RED, GREEN, BLUE are globally visible
// TR: Üyeler dış kapsama sızar. RED, GREEN, BLUE global olarak görünür
enum LegacyIndicatorColor { RED, GREEN, BLUE };

// =================================================================================================
// 2. MODERN enum class (C++11 — strongly typed)
// =================================================================================================
// EN: Scoped and type-safe. Must use GearPosition::Park syntax.
// TR: Kapsamlı ve tip güvenli. GearPosition::Park sözdizimi zorunlu.
enum class GearPosition : uint8_t {
  Park = 0,
  Reverse = 1,
  Neutral = 2,
  Drive = 3,
  Sport = 4
};

// EN: ECU operating states for a Body Control Module.
// TR: Gövde Kontrol Modülü (BCM) için ECU çalışma durumları.
enum class ECUState { Off, Booting, Running, Sleep, Error };

// =================================================================================================
// 3. typedef & using (Type Aliases)
// =================================================================================================
// EN: Old C-style / TR: Eski C tarzı
typedef unsigned long SensorId;

// EN: Modern C++11 alias (preferred) / TR: Modern C++11 takma adı (tercih edilen)
using MilliVolts = int;
using TemperatureC = double;

int main() {
  std::cout << "=== MODULE 1: ENUMS, AUTO & TYPE CONVERSIONS ===\n"
            << std::endl;

  // ===============================================================================================
  // 1. CLASSIC ENUM DEMO
  // ===============================================================================================
  std::cout << "--- 1. Classic enum (Legacy) ---" << std::endl;

  LegacyIndicatorColor indicator = GREEN;
  // EN: Implicitly converts to int! No type safety.
  // TR: int'e dolaylı olarak dönüşür! Tip güvenliği yok.
  int colorCode = indicator;
  std::cout << "Indicator color code (int): " << colorCode << "\n"
            << std::endl;

  // ===============================================================================================
  // 2. MODERN ENUM CLASS DEMO
  // ===============================================================================================
  std::cout << "--- 2. enum class (Modern C++11) ---" << std::endl;

  GearPosition gear = GearPosition::Drive;
  // int gearNum = gear;  // ERROR! No implicit conversion allowed!
  int gearNum = static_cast<int>(gear); // Must be explicit
  std::cout << "Gear position: " << gearNum << " (Drive)" << std::endl;

  ECUState bcmState = ECUState::Running;
  // EN: Switch on enum class values — type safe.
  // TR: enum class değerleri üzerinde switch — tip güvenli.
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

  // ===============================================================================================
  // 3. typedef / using DEMO
  // ===============================================================================================
  std::cout << "--- 3. typedef & using aliases ---" << std::endl;

  SensorId egtId = 1001UL;
  MilliVolts reading = 3300;
  TemperatureC ambientTemp = 22.5;

  std::cout << "Sensor ID  : " << egtId << std::endl;
  std::cout << "Reading    : " << reading << " mV" << std::endl;
  std::cout << "Ambient    : " << ambientTemp << " °C\n" << std::endl;

  // ===============================================================================================
  // 4. auto TYPE DEDUCTION (C++11)
  // ===============================================================================================
  std::cout << "--- 4. auto keyword (C++11) ---" << std::endl;

  auto voltage = 13.8;        // Deduced as double
  auto ecuCount = 5;           // Deduced as int
  auto vinStr = std::string("WBAPH5C55BA271234"); // Deduced as std::string

  std::cout << "voltage (auto = double): " << voltage << std::endl;
  std::cout << "ecuCount (auto = int)  : " << ecuCount << std::endl;
  std::cout << "vinStr (auto = string) : " << vinStr << "\n" << std::endl;

  // ===============================================================================================
  // 5. IMPLICIT CONVERSIONS / DOLAYLI DÖNÜŞÜMLER
  // ===============================================================================================
  std::cout << "--- 5. Implicit Conversions ---" << std::endl;

  // EN: Widening (safe): int → double. No data loss.
  // TR: Genişletme (güvenli): int → double. Veri kaybı yok.
  int rpm = 3500;
  double rpmDouble = rpm; // Implicit widening
  std::cout << "int 3500 -> double: " << rpmDouble << std::endl;

  // EN: Narrowing (DANGEROUS!): double → int. Fractional part lost!
  // TR: Daraltma (TEHLİKELİ!): double → int. Kesir kısmı kaybolur!
  double precisePressure = 2.75;
  int roughPressure = static_cast<int>(precisePressure); // 2 (not 2.75!)
  std::cout << "double 2.75 -> int: " << roughPressure
            << " (fractional part LOST!)" << std::endl;

  // EN: bool -> int conversion (true=1, false=0).
  // TR: bool -> int dönüşümü (true=1, false=0).
  bool absActive = true;
  int absFlag = absActive;
  std::cout << "bool true -> int: " << absFlag << "\n" << std::endl;

  // ===============================================================================================
  // 6. EXPLICIT CONVERSIONS (static_cast) / AÇIK DÖNÜŞÜMLER
  // ===============================================================================================
  std::cout << "--- 6. static_cast (Explicit) ---" << std::endl;

  // EN: Use static_cast for clear, compile-time checked conversions.
  // TR: Açık, derleme zamanı kontrollü dönüşümler için static_cast kullanın.
  int totalDistance = 15000; // meters
  int numSegments = 7;

  // EN: Without cast: integer division → truncated result (2142).
  // TR: Cast olmadan: tam sayı bölmesi → kırpılmış sonuç.
  std::cout << "Integer div: " << totalDistance / numSegments << std::endl;

  // EN: With cast: one operand becomes double → full precision.
  // TR: Cast ile: bir operand double olur → tam hassasiyet.
  double avgSegment =
      static_cast<double>(totalDistance) / static_cast<double>(numSegments);
  std::cout << "static_cast div: " << avgSegment << " m/segment" << std::endl;

  // EN: Casting enum class to its underlying type.
  // TR: enum class'ı temel türüne dönüştürme.
  auto gearValue = static_cast<uint8_t>(GearPosition::Sport);
  std::cout << "GearPosition::Sport -> uint8_t: "
            << static_cast<int>(gearValue) << "\n" << std::endl;

  // ===============================================================================================
  // 7. C-STYLE CAST (Avoid!) / C-TARZI CAST (Kaçının!)
  // ===============================================================================================
  std::cout << "--- 7. C-Style Cast (Dangerous Legacy) ---" << std::endl;

  // EN: C-style casts bypass ALL safety checks. Prefer static_cast.
  // TR: C-tarzı cast'ler TÜM güvenlik kontrollerini atlar. static_cast tercih edin.
  double sensorVoltage = 4.85;
  int rawADC = (int)sensorVoltage; // C-style: works but unsafe
  std::cout << "C-style (int)4.85 = " << rawADC
            << " (Compiles, but avoid in modern C++!)" << std::endl;

  return 0;
}
