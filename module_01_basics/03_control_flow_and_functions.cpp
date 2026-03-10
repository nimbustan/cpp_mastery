/**
 * @file module_01_basics/03_control_flow_and_functions.cpp
 * @brief Basics: Control Flow and Function Overloading / Temeller: Akış Kontrolü ve Fonksiyon
 * Aşırı Yüklemesi
 *
 * @details
 * =============================================================================
 * [THEORY: Switch-case vs If-else / TEORİ: Switch-case ve If-else Karşılaştırması]
 * =============================================================================
 * EN: In C++, 'switch' is generally FASTER [O(1) complexity] than many 'if-else' blocks because
 * it creates a "Jump Table" under the hood. However, 'switch' only works with integral types
 * (int, char, enum), strings are strictly incompatible.
 *
 * TR: C++'ta 'switch', arka planda bir "Sıçrama Tablosu" (Jump Table) oluşturduğu için çok
 * sayıda (10+) 'if-else' bloğundan DAHA HIZLIDIR (O(1) karmaşıklık sunar). Ancak 'switch' sadece
 * tam sayılarla (int, char, enum) çalışır, stringler uyumsuzdur.
 *
 * =============================================================================
 * [THEORY: Why is Function Overloading Possible? / TEORİ: Aşırı Yükleme Neden Mümkündür?]
 * =============================================================================
 * EN: In C, you CANNOT write two functions with the same name because the compiler only checks
 * the exact name. In C++, the compiler implicitly modifies the function names behind the scenes
 * by appending their parameter types (This is called "Name Mangling"). Example: print(int)   ->
 * _Z5printi print(float) -> _Z5printf Thus, you can have thousands of functions with the same
 * name, as long as their parameters are different!
 *
 * TR: C dilinde aynı isimde iki fonksiyon YAZAMAZSINIZ. Çünkü derleyici sadece "isimlere" bakar.
 * C++'ta ise derleyici fonksiyon isimlerine arka planda parametreleri de ekleyerek gizlice
 * isimlerini değiştirir (Buna "Name Mangling" denir). Örnek: yazdir(int)   -> _Z6yazdiri
 * yazdir(float) -> _Z6yazdirf Bu sayede aynı isimli binlerce fonksiyonunuz olabilir, yeter ki
 * parametreleri farklı olsun!
 *
 * =============================================================================
 * [CPPREF DEPTH: Overload Resolution Rules / CPPREF DERİNLİK: Aşırı Yükleme Çözüm Kuralları]
 * =============================================================================
 * EN: How does the compiler decide which overloaded function to call? 1. Exact Match (e.g.,
 * passing exactly an `int` to `print(int)`). 2. Promotion (e.g., passing `char` -> promotes to
 * `int` safely). 3. Standard Conversion (e.g., passing `float` -> converts to `double`). If it's
 * ambiguous, the compiler throws a Hard Error.
 *
 * TR: Derleyici hangi fonksiyonu çağıracağına cppreference rank (rütbe) sistemiyle karar verir:
 * 1. Birebir Eşleşme (örn., `int` parametreye tam `int` göndermek). 2. Yükseltme (örn., `char`
 * gönderilirse güvenle `int`'e yükseltilir). 3. Standart Çevirim (örn., `float` gönderilirse
 * `double`'a dönüştürülür). İki fonksiyon da eşit uyuyorsa derleyici "Belirsizlik (Ambiguity)"
 * hatası fırlatır ve build kapanır.
 *
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <iostream>
#include <string>

// 1. Function Overloading Examples / Aşırı Yükleme Örnekleri
void print(int value) { std::cout << "[INT] Printed: " << value << std::endl; }

void print(double value) {
  std::cout << "[DOUBLE] Printed: " << value << std::endl;
}

void print(std::string value) {
  std::cout << "[STRING] Printed: " << value << std::endl;
}

int main() {
  std::cout << "=== MODULE 1: CONTROL FLOW & OVERLOADING ===\n" << std::endl;

  // A. FUNCTION OVERLOADING / FONKSİYON AŞIRI YÜKLEMESİ
  std::cout << "--- Function Overloading ---" << std::endl;
  print(42);       // int version called / int sürümü çağrılır
  print(3.14159);  // double version called / double sürümü çağrılır
  print("Hello!"); // string version called / string sürümü çağrılır

  // B. MODERN C++11 RANGE-BASED FOR LOOP / MODERN FOR DÖNGÜSÜ
  std::cout << "\n--- Modern For Loop (Range-based) ---" << std::endl;
  int scores[] = {90, 85, 75, 100};

  // EN: Classic: for(int i=0; i<4; i++) { ... } -> Ugly and prone to out-of-bounds errors.
  // Modern C++11: Automatically finds the end of the array or std::vector. 'const auto&' means:
  // I will only read (const) and I will reference the address to save RAM (&).
  //
  // TR: Klasik: for(int i=0; i<4; i++) { ... } -> Çirkin ve index aşımı riski var. Modern C++11:
  // Dizinin veya Vector'ün sonunu kendi bulur. 'const auto&' demek: Sadece okuyacağım (const) ve
  // kopyalayıp RAM'i yormadan adresten bakacağım (&).

  int rank = 1;
  for (const auto &score : scores) {
    std::cout << rank++ << ". Score: " << score << std::endl;
  }

  return 0;
}
