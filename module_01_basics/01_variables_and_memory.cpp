/**
 * @file module_01_basics/01_variables_and_memory.cpp
 * @brief Variables, Data Types, and Memory Model / Değişkenler, Veri Tipleri ve Bellek Modeli
 *
 * @details
 * =============================================================================
 * [THEORY: Primitive Data Types & Memory / TEORİ: İlkel Veri Tipleri ve Bellek]
 * =============================================================================
 * EN: This file demonstrates primitive data types in C++, how much memory they consume using the
 * sizeof() operator, and the concept of constants (const, constexpr).
 *
 * TR: Bu dosya C++'daki ilkel veri tiplerini, bellek tüketimlerini (sizeof() operatörü ile) ve
 * sabitlerin (const, constexpr) kavramlarını gösterir.
 *
 * =============================================================================
 * [CPPREF DEPTH: Undefined Behavior (UB) & Strict Aliasing / CPPREF DERİNLİK: Tanımsız Davranış
 * (UB) ve Sıkı Örtüşme]
 * =============================================================================
 * EN: cppreference.com explicitly warns: "Reading an uninitialized block of memory is Undefined
 * Behavior (UB)." Also, casting a `float*` to an `int*` violates the "Strict Aliasing Rule" in
 * C++, allowing the optimizer to silently break your firmware.
 *
 * TR: cppreference uyarıyor: İlk değer (0 veya başka bir sayı) atanmamış bir değişkeni ekrana
 * basmak "Tanımsız Davranış (UB)" yaratır, firmware rastgele çöker. Ayrıca bir `float`
 * pointer'ını zorla `int` pointer'ına çevirmek C++'ın "Strict Aliasing (Sıkı Örtüşme)" kuralını
 * çiğner, derleyici optimizasyon yaparken kodu bozar!
 *
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

// Fixed width integer types (Sabit genişlikli tam sayı tipleri için)
#include <cstdint>
#include <iostream>

int main() {
  std::cout << "--- C++ Primitive Data Types & Memory ---" << std::endl;
  std::cout << "--- C++ İlkel Veri Tipleri ve Bellek ---\n" << std::endl;

  // 1. Integer Types (Tam Sayı Tipleri)
  // ===============================================================================================
  // EN: Integer sizes may vary based on OS and compiler. For robust porting, we use the
  // <cstdint> library.
  //
  // TR: C++'da tam sayı boyutları işletim sistemine ve derleyiciye göre değişebilir. Güvenilir
  // ve taşınabilir kod için <cstdint> kütüphanesini kullanırız.

  int age = 30; // Usually 4 bytes (Genellikle 4 bayt)
  std::cout << "int takes: " << sizeof(int) << " bytes (bayt). Example: " << age << std::endl;

  // [CPPREF DEPTH: Uninitialized Variables / CPPREF DERİNLİK: Başlatılmamış Değişkenler]
  // EN: DANGER (UB)! No value assigned. Whatever garbage value is at that RAM address will be
  // used.
  // TR: TEHLİKE (UB)! Değer verilmedi. RAM'de o an o adreste ne çöp (garbage value) varsa onu
  // alır.
  int uninitializedAge;
  // EN: Suppress warning. Reading it is UB!
  // TR: Uyarıyı bastırır. Okumak tanımsız davranıştır (UB)!
  (void)uninitializedAge;
  // EN: Doing this causes Undefined Behavior.
  // TR: Bunu yapmak Tanımsız Davranış'a (UB) neden olur. std::cout << uninitializedAge;

  // Fixed width (Sabit genişlikli)
  int16_t smallNumber = 32767; // Guaranteed 2 bytes (Garantili 2 bayt, 16 bits)
  // Guaranteed 4 bytes (Garantili 4 bayt, 32 bits)
  int32_t standardNumber = 2147483647;

  std::cout << "int16_t takes: " << sizeof(int16_t) << " bytes. Value: " << smallNumber <<
      std::endl;
  std::cout << "int32_t takes: " << sizeof(int32_t) << " bytes. Value: " << standardNumber << "\n"
      << std::endl;

  // 2. Floating Point Types (Ondalıklı Sayı Tipleri)
  // ===============================================================================================
  // 'f' indicates float (F harfi float olduğunu belirtir)
  float piFloat = 3.14159f;
  double piDouble = 3.14159265359; // Higher precision (Daha yüksek hassasiyet)

  std::cout << "float takes: " << sizeof(float) << " bytes. Value: " << piFloat << std::endl;
  std::cout << "double takes: " << sizeof(double) << " bytes. Value: " << piDouble << "\n" <<
      std::endl;

  // 3. Characters and Booleans (Karakterler ve Mantıksal Tipler)
  // ===============================================================================================
  char grade = 'A'; // Single quotes used (Tek tırnak kullanılır)
  // Only true (1) or false (0) (Sadece true veya false alır)
  bool isCppFun = true;

  std::cout << "char takes: " << sizeof(char) << " byte. Value: " << grade << std::endl;
  std::cout << "bool takes: " << sizeof(bool) << " byte. Value: " << isCppFun << "\n" << std::endl;

  // 4. Constants (Sabitler)
  // ===============================================================================================
  // EN: const (Unchangeable at Run-time) vs constexpr (Calculated at Compile-time)
  // TR: const (Çalışma zamanında değişmez) vs constexpr (Derleme zamanında hesaplanır)

  // Cannot be changed during run-time. (Çalışırken değiştirilemez.)
  const int MAX_USERS = 100;

  // EN: constexpr came with C++11. Evaluated at compile-time for run-time performance.
  // TR: constexpr C++11 ile geldi. Derleme aşamasında hesaplanarak çalışma zamanında performans
  // kazandırır.
  constexpr int SECONDS_IN_MIN = 60;
  constexpr int MINS_IN_HOUR = 60;
  // Compiler hardcodes this as 3600!
  constexpr int SECONDS_IN_HOUR = SECONDS_IN_MIN * MINS_IN_HOUR;
                                     // (Derleyici bunu 3600 olarak yazar!)

  std::cout << "Constants:" << std::endl;
  std::cout << "MAX_USERS: " << MAX_USERS << std::endl;
  std::cout << "SECONDS_IN_HOUR (constexpr): " << SECONDS_IN_HOUR << "\n" << std::endl;

  return 0;
}
