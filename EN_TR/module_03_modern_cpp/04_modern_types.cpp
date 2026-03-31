/**
 * @file module_03_modern_cpp/04_modern_types.cpp
 * @brief Modern C++: New Data Types (C++17) — Modern Veri Tipleri: optional, variant, any,
 * string_view
 *
 * @details
 * =============================================================================
 * [THEORY: Why Do We Lose Points in Code Reviews? / TEORİ: Neden Puan Kaybederiz?]
 * =============================================================================
 * EN: In old C++ code, if a function failed, it usually returned "-1", "NULL" or "nullptr"
 * (Magic Numbers). This is dangerous because what if "-1" is the ACTUAL valid answer? With
 * C++17, things became extremely safe, similar to Rust.
 * 1. std::optional<T> : Contains a value of type "T" OR it is empty (nullopt).
 * 2. std::variant<T, Y> : A type-safe version of C's "union". Holds only ONE type at
 *    a time but allocates memory equal to the BIGGEST mapped type (RAM saver).
 * 3. std::any : Holds "anything" (like variables in Python or JS). Heavy overhead.
 * 4. std::string_view : A lightweight purely observational "window" to look at a string
 *    WITHOUT copying it!
 *
 * TR: Eski C++ kodlarında bir fonksiyon başarılı olamazsa genellikle "-1", "NULL" veya "nullptr"
 * döndürülürdü (Magic Numbers/Sihirli Sayılar). Bu tehlikelidir çünkü kişi gerçekten -1 değerini
 * bulmuş olabilir! C++17 ile beraber işler "Rust" dillerine benzer şekilde muhteşem güvenli hale
 * geldi.
 * 1. std::optional<T> : İçinde "T" tipi bir değer YA VARDIR YA DA YOKTUR (nullopt).
 * 2. std::variant<T, Y> : C'deki "union"ın güvenli halidir. Aynı anda tek tip tutar,
 *    RAM cimrisidir.
 * 3. std::any : "İçine ne atarsan at" tipidir. Maliyetlidir, zorda kalınmadıkça kullanılmaz.
 * 4. std::string_view : Bir metni kopyalamadan (yeni RAM tüketmeden) SADECE BAKMA penceresidir.
 *
 * [CPPREF DEPTH: auto Type Deduction Rules — Template Argument Deduction in Disguise / CPPREF
 * DERİNLİK: auto Tür Çıkarım Kuralları — Şablon Argüman Çıkarımının Gizli Yüzü]
 * =============================================================================
 * EN: `auto` uses the same rules as template argument deduction (§7.1.7.4 / §13.10.2). When you
 * write `auto x = expr;`, the compiler deduces the type as if `x` were a function template
 * parameter: references and top-level const/volatile are stripped. This means:
 * • `const int& r = n; auto a = r;`  → a is `int`, NOT `const int&`.
 * • `auto x = {1,2,3};` deduces `std::initializer_list<int>` (special rule).
 * • `auto x{42};` in C++17 deduces `int` (changed from init-list in C++11).
 * `decltype(auto)` preserves the EXACT type including references:
 * • `decltype(auto) y = r;` → y is `const int&`.
 * Parenthesized id-expressions change decltype semantics:
 * • `decltype(x)` → declared type of x.
 * • `decltype((x))` → always an lvalue reference (T&), because (x) is an lvalue expression,
 *   not an id-expression!
 * This subtle difference is a common source of bugs in return type deduction with
 * `decltype(auto)` — returning `(localVar)` gives a dangling reference.
 *
 * TR: `auto` şablon argüman çıkarımıyla (template argument deduction) aynı kuralları kullanır.
 * `auto x = expr;` yazıldığında derleyici, x'i bir şablon parametresiymiş gibi çıkarır:
 * referanslar ve üst-düzey const/volatile atılır.
 * • `const int& r = n; auto a = r;`  → a tipi `int`'tir, `const int&` DEĞİL.
 * • `auto x = {1,2,3};` → `std::initializer_list<int>` çıkarılır.
 * • `auto x{42};` → C++17'de `int` (C++11'de init-list'ti, değişti).
 * `decltype(auto)` referanslar dahil TAM türü korur:
 * • `decltype(auto) y = r;` → y tipi `const int&`.
 * Parantezli ifadeler decltype semantiğini değiştirir:
 * • `decltype(x)`   → x'in bildirim türü.
 * • `decltype((x))` → her zaman lvalue referans (T&)! Çünkü (x) bir lvalue ifadesidir,
 *   id-expression değildir.
 * Bu ince fark, `decltype(auto)` dönüş türüyle `(localVar)` döndürürken sarkan referans
 * (dangling reference) hatasına yol açar.
 *
 * @note Compile: g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion 04_modern_types.cpp -o 04_modern_types
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <any>
#include <iostream>
#include <optional>
#include <string>
#include <string_view>
#include <variant>

// 1. std::optional (C++17) Example: How to securely handle "Data Not Found" without MAGIC
// NUMBERS? (Veri bulamama durumu "-1" kullanılmadan nasıl çözülür?)
std::optional<int> findAgeByName(const std::string &name) {
  if (name == "Alice")
    return 25;
  if (name == "Bob")
    return 30;

  // EN: Instead of -1, we return 'nullopt' (no value)!
  // TR: Bulamadıysak -1 döndürmek yerine C++17'deki harika "nullopt" döneriz!
  return std::nullopt;
}

// 2. std::string_view (C++17) Example: Preventing massive RAM copies line by line!
// TR: Satır satır metin kopyalamayı önlemek! Eğer std::string alsaydık, o devasa metni RAM'e
// kopyalardık.
void printFirst5Chars(std::string_view text) {
  if (text.length() >= 5)
    std::cout << "[StringView] " << text.substr(0, 5) << "..." << std::endl;
}

int main() {
  std::cout << "=== MODULE 3: MODERN TYPES (C++17) ===\n" << std::endl;

  // --- TEST 1: std::optional ---
  std::cout << "--- 1. std::optional ---" << std::endl;
  // Not in database / Veritabanında yok.
  auto ageResult = findAgeByName("Charlie");

  if (ageResult.has_value()) { // or just: if (ageResult)
    std::cout << "Age Found (Bulunan Yaş): " << ageResult.value() << std::endl;
  } else {
    std::cout << "Person DOES NOT EXIST (Bu isimde biri YOK. '-1' hatası "
                 "yapmadan güvenle çözüldü.)"
              << std::endl;
  }

  // --- TEST 2: std::variant ---
  std::cout << "\n--- 2. std::variant ---" << std::endl;
  // EN: An automotive diagnostic result. Can hold EITHER an int error code OR a string
  // description.
  // TR: Bir otomotiv teşhis sonucu. SADECE Ya hata kodu (int) ya açıklama (string) taşır!
  std::variant<int, std::string> diagnosticResult;

  diagnosticResult = 300; // Int Mode (DTC numeric code)
  std::cout << "DTC Code (Hata Kodu): " << std::get<int>(diagnosticResult) << std::endl;

  // Switch to String immediately (Old data vanishes/Yok olur)
  diagnosticResult = "P0300 - Random Misfire";
  std::cout << "DTC Description (Açıklama): " << std::get<std::string>(diagnosticResult) <<
      std::endl;

  // --- TEST 3: std::any ---
  std::cout << "\n--- 3. std::any ---" << std::endl;
  // EN: Like 'var' in JS. Heavy performance hit. DO NOT USE unless desperate.
  // TR: Dinamik tür! Performansı çok kötüdür, ZORUNDA KALMADIKÇA KULLANILMAZ.
  std::any anything = 3.14;
  anything = std::string("Dynamic Word / Dinamik Kelime");
  std::cout << "Content (Dinamik Yapı İçeriği): " << std::any_cast<std::string>(anything) <<
      std::endl;

  // --- TEST 4: std::string_view ---
  std::cout << "\n--- 4. std::string_view ---" << std::endl;
  const char *massiveText = "This is a very large text block. Copying it is suicide.";
  // TR: C Tipi metin veya C++ string fark etmez, RAM'de KOPYA ÜRETMEDEN okur!
  printFirst5Chars(massiveText);

  return 0;
}
