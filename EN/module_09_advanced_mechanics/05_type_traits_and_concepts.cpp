/**
 * @file module_09_advanced_mechanics/05_type_traits_and_concepts.cpp
 * @brief Advanced Mechanics: Type Traits (SFINAE) & C++20 Concepts
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
 *
 * =============================================================================
 * [2. Concepts: The Savior of C++20]
 * =============================================================================
 * EN: SFINAE is notoriously unreadable and generates 500-page long compiler error logs. C++20
 * introduced `requires` and `Concepts` to completely replace SFINAE! Concepts allow us to write
 * clean, extremely readable restrictions on Templates.
 *
 *
 * [CPPREF DEPTH: Type Traits Implementation — How is_same and conditional Work Under the Hood]
 * =============================================================================
 * EN: `is_same<T,U>`: primary template inherits `false_type`; partial specialization
 * `is_same<T,T>` inherits `true_type`. `conditional<B,T,F>`: primary template defines `type =
 * T`; specialization for `false` defines `type = F`. `enable_if<true, T>` defines member `type =
 * T`; `enable_if<false>` has no `type` — this triggers SFINAE when used in template parameter
 * deduction. `void_t<Ts...>` equals `void` if all `Ts` are well-formed — this is the detection
 * idiom for checking members. C++20 concepts replace most SFINAE patterns with cleaner syntax:
 * `requires` clauses, concept definitions, and constrained `auto`.
 *
 *
 * @note Compile: g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion 05_type_traits_and_concepts.cpp -o 05_type_traits_and_concepts
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <concepts> // Modern C++20 Concepts!
#include <iostream>
#include <string>
#include <type_traits> // Old C++11 traits

class Helicopter {}; // Dummy object

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// [APPROACH 1: The OLD SFINAE Way (C++14) - Extremely Ugly and Cryptic]
// ═════════════════════════════════════════════════════════════════════════════════════════════════
// EN: "This function only exists if T is an arithmetic (number) type."
template <typename T, typename std::enable_if_t<std::is_arithmetic_v<T>, bool> = true>
void addTaxes_SFINAE(T price) {
  std::cout << "[SFINAE (Old)] New Price: " << (price * 1.18) << std::endl;
}

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// [APPROACH 2: The NEW C++20 Concept Way - Beautiful and Readable!]
// ═════════════════════════════════════════════════════════════════════════════════════════════════
// EN: "This function REQUIRES 'T' to be a floating-point number (double
template <typename T>
  requires std::floating_point<T>
void addTaxes_Concept(T price) {
  std::cout << "[C++20 Concept] Floating Tax Calculation: " << (price * 1.18) << std::endl;
}

// EN: We can even create our Custom Concepts!
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

  std::cout << "\n[ARCHITECTURAL CONCLUSION]:" << std::endl;
  std::cout << "- SFINAE and enable_if are obsolete. Move purely to C++20 "
               "`requires` & `Concepts`."
            << std::endl;

  return 0;
}
