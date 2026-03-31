/**
 * @file module_01_basics/02_pointers_and_references.cpp
 * @brief Basics: Pointers, References, and Memory Addresses — Temeller: İşaretçiler, Referanslar
 * ve Bellek Adresleri
 *
 * @details
 * =============================================================================
 * [THEORY: Why Do We Need Pointers (*) and References (&)? / TEORİ: Neden İşaretçilere ve
 * Referanslara İhtiyacımız Var?]
 * =============================================================================
 * EN: When you pass a variable to a function by default in C++, it makes a FULL COPY of that
 * variable (Pass by Value). If you pass a 1GB Image file to a function, the computer freezes
 * while duplicating 1GB in RAM! Pointers (*) and References (&) solve this by passing the
 * "Memory Address". You just give the function the GPS coordinates; it goes there and modifies
 * the original.
 *
 * TR: C++'ta bir değişkene parametre olarak müdahale ettiğinizde varsayılan olarak o değişkenin
 * "Birebir Kopyası" çıkarılır (Değerle Geçiş - Pass by Value). Eğer 1 GB'lık yüksek çözünürlüklü
 * bir resmi bir "Filtre" fonksiyonuna yollarsanız, sistem o 1 GB'ı RAM'de tamamen kopyalar ve
 * bilgisayar donar! Çözüm: İşaretçiler (*) ve Referanslar (&). Bu ikili, nesneyi kopyalamak
 * yerine RAM'deki "GPS Koordinatını (Adresini)" fonksiyona yollar.
 *
 * =============================================================================
 * [THEORY: Pointer (*) vs Reference (&) / TEORİ: İşaretçi vs Referans]
 * =============================================================================
 * EN:
 *   1. POINTER (*): Inherited from C. Can be NULL/nullptr. Flexible, can be REASSIGNED to
 *      point to another object during runtime. Can be dangerous.
 *   2. REFERENCE (&): Introduced in C++. CAN NEVER be NULL (Must be initialized when declared).
 *      Safer and cleaner syntax. Once initialized, it gets LOCKED and cannot point to anything
 *      else.
 *
 * TR:
 *   1. POINTER (*): C dilinden kalmadır. NULL/nullptr olabilir. Esnektir, çalışırken başka bir
 *      nesneyi gösterecek şekilde DEĞİŞTİRİLEBİLİR. Tehlikeli olabilir.
 *   2. REFERENCE (&): C++ ile gelmiştir. ASLA NULL olamaz (Tanımlandığı an bir hedefi olmalıdır).
 *      Daha güvenli ve temiz bir sözdizimi vardır. Oluşturulduktan sonra BAŞKA bir şeyi
 *      gösteremez, tamamen kilitlenir.
 *
 * =============================================================================
 * [CPPREF DEPTH: Dangling References & Pointer Decay / CPPREF DERİNLİK: Bağlantısız Referanslar
 * ve İşaretçi Azalması]
 * =============================================================================
 * EN: Returning a reference or pointer to a LOCAL variable from a function is a catastrophic
 * Undefined Behavior (Dangling Pointer). Because local variables die on the Stack when the
 * function ends. Also, arrays naturally "decay" into a pointer to their first element.
 *
 * TR: Bir fonksiyondan "Yerel (Local)" bir değişkenin Referansını veya Pointer'ını `return` ile
 * geri döndürmek felakettir (Dangling Pointer - Boşlukta Sallanan). Çünkü fonksiyon bitince (})
 * yerel obje Stack'ten (Yığın) silinir. Siz silinmiş bir RAM arazisini göstermiş olursunuz.
 * (SIGSEGV - Hack!)
 *
 * @note Compile: g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion 02_pointers_and_references.cpp -o 02_pointers_and_references
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <iostream>

// 1. PASS BY VALUE (KOPYA YÖNTEMİ) - Bad practice for large data / Büyük veriler için kötü
// pratik
void swapByValue(int a, int b) {
  int temp = a;
  a = b;
  b = temp;
  // EN: The original values in main() WILL NOT CHANGE because 'a' and 'b' are just copies.
  // TR: Main fonksiyonundaki asli değerler DEĞİŞMEYECEKTİR çünkü a ve b sadece 'kopya'lardı.
}

// 2. PASS BY POINTER (İŞARETÇİ YÖNTEMİ) - C style (Tehlikeli ve kalabalık sözdizimi)
void swapByPointer(int *ptrA, int *ptrB) {
  if (ptrA == nullptr || ptrB == nullptr)
    return; // Safety check is mandatory! / Güvenlik kontrolü şarttır!

  // * operator (Dereference): Access the VALUE at the address! / Adresin İÇİNDEKİ değere ulaş!
  int temp = *ptrA;
  *ptrA = *ptrB;
  *ptrB = temp;
}

// 3. PASS BY REFERENCE (REFERANS YÖNTEMİ) - The heart of Modern C++! / Modern C++'ın kalbi!
void swapByReference(int &refA, int &refB) {
  // EN: No null check needed. Cleaner syntax.
  // TR: Null kontrolüne gerek yoktur. Daha temizdir.
  int temp = refA;
  refA = refB;
  refB = temp;
}

// [CPPREF DEPTH: UB (Undefined Behavior) Example / CPPREF DERİNLİK: UB (Tanımsız Davranış) Örneği]
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreturn-local-addr"
int *generateDanglingPointer_DO_NOT_DO_THIS() {
  int localBomb = 100;
  // EN: WARNING! Returning address of stack memory!
  // TR: Asla yapma! Stack'teki gecici degiskenin adresi dondurulmez.
  return &localBomb; // NOLINT — intentional UB demo
}
#pragma GCC diagnostic pop

int main() {
  std::cout << "=== MODULE 1: POINTERS & REFERENCES ===\n" << std::endl;

  int num1 = 10;
  int num2 = 99;

  std::cout << "[RAM COORDINATES / ADRESLER]" << std::endl;
  std::cout << "Address of num1 (&): " << &num1 << " | Value (Değer): " << num1 << std::endl;

  // 1. Test: COPY (Çalışmayacak / Won't work!)
  swapByValue(num1, num2);
  std::cout << "\n[TEST] After swapByValue (Copy): num1 = " << num1 << ", num2 = " << num2 <<
      std::endl;

  // 2. Test: POINTER (Çalışacak ama Çirkin / Will work but ugly!) Passing addresses instead of
  // values / "Değerleri değil, adresleri gönderiyorum"
  swapByPointer(&num1, &num2);
  std::cout << "[TEST] After swapByPointer (Pointer): num1 = " << num1 << ", num2 = " << num2 <<
      std::endl;

  // 3. Test: REFERENCE (Geri Çeviriyoruz, Zarif C++ Yolu / Reverting using elegant C++ way)
  // Syntactically passed like values, but actually passed by
  swapByReference( num1, num2);
                   // reference! / Tıpkı değer yollar gibi gönderiyoruz!
  std::cout << "[TEST] After swapByReference (Reference): num1 = " << num1 << ", num2 = " << num2
      << std::endl;

  // [Interview Question / Mülakat Sorusu]: Const Reference
  // EN: "How do I pass a massive object without COPYING it, but also ensuring the function
  // CANNOT MODIFY it?"
  // TR: "Eğer fonksiyona devasa bir nesneyi yollarken KOPYALANMASINI İSTEMİYORSAM ama aynı
  // zamanda değiştirmesini de İSTEMİYORSAM ne yapmalıyım?" Cevap: `const std::string& hugeData`
  // ! No copy (RAM saved) & Read-Only (Değiştirilemez/Salt Okunur).

  return 0;
}
