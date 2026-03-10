/**
 * @file module_09_advanced_mechanics/05_type_traits_and_concepts.cpp
 * @brief Advanced Mechanics: Type Traits (SFINAE) & C++20 Concepts / Kısıtlandırılmış Şablonlar
 * ve Meta-Programlama
 *
 * @details
 * =============================================================================
 * [1. Type Traits & SFINAE (Substitution Failure Is Not An Error) - Eski C++ (C++11/14)]
 * =============================================================================
 * EN: When using C++ Templates (`template <typename T>`), `T` can be ANYTHING (an `int`, a
 * `std::string`, or an `ECUModule`). To restrict `T` (e.g., "Only accept numbers!"), modern C++
 * uses `<type_traits>` like `std::is_arithmetic_v<T>` combined with `std::enable_if`. If a
 * substitution fails, the compiler simply ignores the overload rather than throwing an error
 * (SFINAE!).
 *
 * TR: C++ Şablonlarını (Templates) kullanırken, Giren 'T' (Type) kelimesi HER ŞEY olabilir. Bir
 * adam oraya 'Helikopter' sınıfını bile atabilir. Derleyiciyi kısıtlamak ("Buraya sadece rakam
 * girebilir!") için Type Traits ve SFINAE denilen ürkütücü derecede karmaşık, ve efsanevi
 * `std::enable_if` mekanizması kullanılırdı.
 *
 * =============================================================================
 * [2. Concepts: The Savior of C++20 / Kurtarıcı Kavramlar (Modern C++20)]
 * =============================================================================
 * EN: SFINAE is notoriously unreadable and generates 500-page long compiler error logs. C++20
 * introduced `requires` and `Concepts` to completely replace SFINAE! Concepts allow us to write
 * clean, extremely readable restrictions on Templates.
 *
 * TR: SFINAE mekanizması korkunç uzun hatalar fırlatmasıyla meşhurdur. C++ Komitesi, C++20 ile
 * birlikte "Oluşumları/Kavramları (Concepts)" piyasaya sürdü! Artık `requires` yazarak kodun
 * tamı tamına ne kabul ettiğini %100 okunabilirlikle derleyiciye izah edebiliyoruz. 500 sayılık
 * hatalar yerine "Bu tipe izin yok!" alırız.
 *
 * [CPPREF DEPTH: Type Traits Implementation — How is_same and conditional Work Under the Hood /
 * CPPREF DERİNLİK: Tür Özelliklerinin İç Yüzü — is_same ve conditional Nasıl Çalışır]
 * =============================================================================
 * EN: `is_same<T,U>`: primary template inherits `false_type`; partial specialization
 * `is_same<T,T>` inherits `true_type`. `conditional<B,T,F>`: primary template defines `type =
 * T`; specialization for `false` defines `type = F`. `enable_if<true, T>` defines member `type =
 * T`; `enable_if<false>` has no `type` — this triggers SFINAE when used in template parameter
 * deduction. `void_t<Ts...>` equals `void` if all `Ts` are well-formed — this is the detection
 * idiom for checking members. C++20 concepts replace most SFINAE patterns with cleaner syntax:
 * `requires` clauses, concept definitions, and constrained `auto`.
 *
 * TR: `is_same<T,U>`: birincil şablon `false_type`'dan türer; kısmi özelleştirme `is_same<T,T>`
 * ise `true_type`'dan türer. `conditional<B,T,F>`: birincil şablon `type = T` tanımlar; `false`
 * özelleştirmesi `type = F` tanımlar. `enable_if<true, T>` `type = T` üyesini tanımlar;
 * `enable_if<false>` hiçbir `type` içermez — şablon parametre çıkarımında SFINAE'yi tetikler.
 * `void_t<Ts...>`, tüm `Ts` geçerliyse `void`'e eşittir — üye tespit deyimi (detection idiom)
 * budur. C++20 kavramları (concepts), SFINAE kalıplarının çoğunu daha temiz bir sözdizimi ile
 * değiştirir: `requires` cümleleri, kavram tanımları ve kısıtlı `auto`.
 *
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <concepts> // Modern C++20 Concepts!
#include <iostream>
#include <string>
#include <type_traits> // Old C++11 traits

class Helicopter {}; // Dummy object / Eğlence için ucube obje

// =================================================================================================
// [APPROACH 1: The OLD SFINAE Way (C++14) - Extremely Ugly and Cryptic]
// =================================================================================================
// EN: "This function only exists if T is an arithmetic (number) type."
// TR: "Bu fonksiyon SADECE 'T' bir sayı türüyse kabul edilsin, yoksa yok sayılsın."
template <typename T, typename std::enable_if_t<std::is_arithmetic_v<T>, bool> = true>
void addTaxes_SFINAE(T price) {
  std::cout << "[SFINAE (Old)] New Price: " << (price * 1.18) << std::endl;
}

// =================================================================================================
// [APPROACH 2: The NEW C++20 Concept Way - Beautiful and Readable!]
// =================================================================================================
// EN: "This function REQUIRES 'T' to be a floating-point number (double/float)." TR: "Bu şablon
// FONKSİYON, T'nin ONDALIKLI SAYI (float, double) olmasını EMREDER! (requires)"
template <typename T>
  requires std::floating_point<T>
void addTaxes_Concept(T price) {
  std::cout << "[C++20 Concept] Floating Tax Calculation: " << (price * 1.18) << std::endl;
}

// EN: We can even create our Custom Concepts!
// TR: Kendi emrimizi / kısıtlamamızı özel Concept kelimesiyle (Mimarisi) yaratabiliriz!
template <typename T>
concept MustBePrintable = requires(T x) {
  // If this code compiles, the concept is met! (Bu derlenirse kabul!)
  std::cout << x;
};

// Target structure that enforces OUR custom concept. (Bizim konseptimiz!)
template <MustBePrintable T> void customPrinter(T data) {
  std::cout << "[Custom Concept Printer]: > " << data << std::endl;
}

int main() {
  std::cout << "=== MODULE 9: TYPE TRAITS & CONCEPTS (C++20) ===\n" << std::endl;

  Helicopter apache;
  (void)apache; // EN: Used only for SFINAE rejection demo

  // --- 1. SFINAE LOGIC (C++11/14) ---
  addTaxes_SFINAE(100);  // int is arithmetic. OK! (int izinlidir)
  addTaxes_SFINAE(45.5); // double is arithmetic. OK! (double izinlidir)
  // addTaxes_SFINAE("10");     // SFINAE REJECTS (String is not arithmetic)
  // addTaxes_SFINAE(apache);   // SFINAE REJECTS (Helicopter cannot pay taxes)

  // --- 2. C++20 CONCEPTS LOGIC --- addTaxes_Concept(100);    // CONCEPT REJECTS / BUILD ERROR!
  // (100 is int, NOT a float!)
  addTaxes_Concept(45.5f); // OK! (float is floating_point)
  addTaxes_Concept(20.90); // OK! (double is floating_point)

  // --- 3. CUSTOM CONCEPT IMPLEMENTATION ---
  customPrinter(999);           // Integers can be printed via std::cout
  customPrinter("Hello World"); // Strings can be printed via std::cout
  // customPrinter(apache);      // WILL CAUSE AN ERROR! (No overload for << on Helicopter)

  std::cout << "\n[ARCHITECTURAL CONCLUSION / MİMARİ SONUÇ]:" << std::endl;
  std::cout << "- SFINAE and enable_if are obsolete. Move purely to C++20 "
               "`requires` & `Concepts`."
            << std::endl;
  std::cout << "- C++20 Kavramları, karmaşık Template kodlarının "
               "okunabilirliğini C#'taki Generics seviyesine taşır!"
            << std::endl;

  return 0;
}
