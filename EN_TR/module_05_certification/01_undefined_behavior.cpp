/**
 * @file module_05_certification/01_undefined_behavior.cpp
 * @brief CPA/CPP Prep: Undefined Behavior & Tricky Questions — Sertifikasyon Hazırlığı: Tanımsız
 * Davranışlar ve Tuzak Sorular
 *
 * @details
 * =============================================================================
 * [THEORY: Why do C++ Institute and CppQuiz ask such hard questions? / TEORİ: Sertifika
 * Tuzakları]
 * =============================================================================
 * EN: In C++, unlike managed languages like Java or C#, the compiler assumes YOU know exactly
 * what you are doing. If you access memory out of bounds or use a pointer after it's deleted,
 * C++ DOES NOT stop you. It results in "Undefined Behavior" (UB). Certification exams test your
 * ability to spot these hidden traps.
 *
 * TR: C++, Java veya C# gibi dillerin aksine kontrolü tamamen size bırakır. Derleyici sizin NE
 * YAPTIĞINIZI KESİN OLARAK BİLDİĞİNİZİ varsayar. Dizi boyutunu aşarsanız veya silinmiş bir
 * belleği okumaya çalışırsanız, C++ sizi durdurmaz (hata vermez). Buna "Undefined Behavior"
 * (Tanımsız Davranış - UB) denir. CPA/CPP ve CppQuiz sınavları tam olarak bu gizli tuzakları
 * görme yeteneğinizi test eder.
 *
 * =============================================================================
 * [THEORY: Examples of Undefined Behavior / TEORİ: Tanımsız Davranış Örnekleri]
 * =============================================================================
 * EN:
 *   1. Dangling Pointers: Pointing to an address that has already been freed.
 *   2. Out of Bounds: Reading the 10th element of a 5-element array.
 *   3. Uninitialized Variables: Reading a local variable containing garbage memory.
 *   4. Signed Integer Overflow: Adding 1 to the absolute MAX value of an `int`.
 *
 * TR:
 *   1. Dangling (Sarkan) Pointers: Silinmiş bir adresi işaret eden pointer (Use after free).
 *   2. Sınır Aşımı (Out of Bounds): 5 elemanlı dizinin 10. elemanını okumak.
 *   3. İlklenmemiş Değişken (Uninitialized): İçine değer atanmamış çöp belleği okumak.
 *   4. İşaretli Taşma (Signed Overflow): Maksimum sınırındaki int değerine 1 eklemek
 *      (C++'ta UB'dir).
 *
 * =============================================================================
 * [CPPREF DEPTH: Unsequenced Modification HACK! / CPPREF DERİNLİK: Sırasız Değişiklik (Sırasız
 * Değişim)!]
 * =============================================================================
 * EN: CppReference explicitly forbids modifying a variable and reading it twice in the same
 * sequence point (e.g., `cout << i++ << ++i;`). The C++ standard DOES NOT GUARANTEE which one
 * executes first. Depending on the compiler (GCC vs Clang), it might output completely different
 * numbers, or crash!
 *
 * TR: CppReference, bir değişkeni aynı satırda (sekans noktasında) hem okuyup hem de
 * değiştirmeyi (Örn: `cout << i++ << ++i;`) kesinlikle YASAKLAR (UB'dir). C++ standardı
 * derleyicilere sağdan mı yoksa soldan mı hesaplayacaklarını söylemez! Mac'te farklı, Windows'ta
 * farklı sonuç çıkar. Oyun state'leriniz darmadağın olur!
 *
 * @note Compile: g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion 01_undefined_behavior.cpp -o 01_undefined_behavior
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <iostream>

void testDanglingPointer() {
  std::cout << "\n--- Danger Zone: Dangling Pointer (Sarkan İşaretçi) ---" << std::endl;
  int *ptr = new int(100);
  std::cout << "Original Value (Orijinal Değer): " << *ptr << std::endl;

  // EN: Memory is freed.
  // TR: Belleği iade ettik.
  delete ptr;

  // DANGER (TEHLİKE)!
  // EN: 'ptr' still points to that address, but it belongs to the OS now.
  // TR: 'ptr' hala o adresi gösteriyor ama adres artık bize ait değil.
  //
  // *ptr = 500; // UNDEFINED BEHAVIOR! (Do not uncomment! / Yorumdan çıkarmayın!)

  // Correct Way (Doğru Yönem):
  // EN: Safer!
  // TR: Artık hiçbir yeri göstermiyor. Güvenli!
  ptr = nullptr;
  std::cout << "Pointer safely nullified. (İşaretçi sıfırlandı.)" << std::endl;
}

void testUninitializedVariable() {
  std::cout << "\n--- Danger Zone: Uninitialized Variable (İlklenmemiş Değişken) ---" << std::endl;
  // EN: Memory allocated, but holds generic GARBAGE.
  // TR: Sadece hafızada yer ayrıldı, içi çöp.
  int mysteriousNumber;

  // std::cout << "Value: " << mysteriousNumber << std::endl; // UNDEFINED BEHAVIOR: Just reading
  // it is UB!
  // EN: Now initialized.
  // TR: Ancak değer atandıktan sonra okunabilir.
  mysteriousNumber = 42;
  std::cout << "Initialized safely (Güvenle ilk değer atandı): " << mysteriousNumber << std::endl;
}

int main() {
  std::cout << "=== MODULE 5: CERTIFICATION & CPPQUIZ TRAPS ===\n" << std::endl;
  std::cout << "EN: Exams ask you to READ code and spot hidden DANGER ZONES, "
               "not write them!"
            << std::endl;
  std::cout << "TR: Sertifika sınavları kodu YAZMANIZI değil, OKUMANIZI ve "
               "gizli tehlikeleri bulmanızı ister!\n"
            << std::endl;

  testDanglingPointer();
  testUninitializedVariable();

  // 3. Trap: Object Slicing (Nesne Dilimlenmesi)
  class Base {
  public:
    virtual void print() { std::cout << "Base"; }
  };
  class Derived : public Base {
  public:
    void print() override { std::cout << "Derived"; }
  };

  Derived myDerived;

  // EN: ERROR! Specific traits were sliced off! (Object Slicing)
  // TR: HATA! Özel olan kısımlar "dilimlenip" kesilip atıldı. (Nesne Dilimlenmesi)
  Base slicedBase = myDerived;

  std::cout << "\nObject Slicing Test: ";
  // EN: Prints "Base"! Polymorphism FAILED.
  // TR: "Base" yazar! Polimorfizm çalışmaz!
  slicedBase.print();

  std::cout << " (Polymorphism FAILED due to Value Copy. Reference was needed!)" << std::endl;

  return 0;
}
