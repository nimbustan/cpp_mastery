/**
 * @file module_09_advanced_mechanics/04_constant_evaluation.cpp
 * @brief Advanced Mechanics: C++20 Constants (constexpr vs consteval vs constinit) / Sabit
 * Değerleme Mekaniği
 *
 * @details
 * =============================================================================
 * [1. constexpr / Derleme Zamanı VEYA Çalışma Zamanı Mimarisi (C++11/14)]
 * =============================================================================
 * EN: A `constexpr` function CAN be evaluated at compile-time (if all inputs are known). But if
 * inputs are dynamic (from user), it naturally degrades to a normal run-time function! It is
 * basically saying: "Compiler, please do this beforehand IF you can."
 *
 * TR: `constexpr` fonksiyon, derleme (Build) anında işlemcinin hesaplayabileceği bir emirdir.
 * ANCAK değişkenler kullanıcı elinden dinamik olarak gelirse, derleyici mırın kırın etmez,
 * "Tamam bunu oyun içerisinde Runtime'da normal fonksiyon gibi yapacağım" der. Yani hem
 * derlemede hem oyun anında esnek çalışır.
 *
 * =============================================================================
 * [2. consteval / YALNIZCA Derleme Zamanı Şartı (C++20)]
 * =============================================================================
 * EN: A `consteval` function (Immediate Function) MUST strictly be evaluated at compile-time! If
 * the compiler cannot determine the inputs beforehand, it throws a HARD BUILD ERROR. Use this
 * when you absolutely CANNOT afford calculation overhead in the final binary execution.
 *
 * TR: `consteval` "Hemen Hesapla!" (C++20) anlamına gelir. Eğer bu fonksiyon derleme aşamasında
 * bitmezse Oyun asla derlenemez. Askeri/Cerrahi robot yazılımlarında, hızın sıfır kayıp (0
 * overhead) yaşaması istenen kriptolu hash / konfigürasyon işlemlerinde mecburidir.
 *
 * =============================================================================
 * [3. constinit / Statik Değerlerin Temiz Başlangıcı (C++20)]
 * =============================================================================
 * EN: The `constinit` keyword guarantees that a static/global variable is fully initialized
 * during compilation (Constant Initialization). It solves the infamous "Static Initialization
 * Order Fiasco" in massive C++ engines. The variable CAN be mutable (changeable) later!
 *
 * TR: `constinit` kelimesi Global veya Static değişkenleri hedefler. Sadece "Oyun açılmadan önce
 * ilk değerini" derleme aşamasında sabitler. Ama `const` DEĞİLDİR! Yani oyun akarken sonradan
 * değiştirilebilir! C++ motorlarındaki meşhur Global çökme bug'larını kökünden engeller.
 *
 * [CPPREF DEPTH: constexpr vs consteval vs constinit — Compile-Time Guarantees / CPPREF
 * DERİNLİK: constexpr vs consteval vs constinit — Derleme Zamanı Garantileri]
 * =============================================================================
 * EN: `constexpr` (C++11): function/variable CAN be evaluated at compile time if all inputs are
 * constant; otherwise degrades to runtime. `consteval` (C++20): "immediate function" — MUST be
 * evaluated at compile time; runtime call is a hard error. `constinit` (C++20): the variable
 * must be initialized at compile time, but may be modified at runtime — solves the static
 * initialization order fiasco. `if consteval` (C++23) tests whether execution is currently in a
 * constexpr evaluation context. Constexpr dynamic allocations (C++20): `new`/`delete` are
 * allowed inside constexpr if fully deallocated by the end of evaluation.
 *
 * TR: `constexpr` (C++11): tüm girdiler sabit ise derleme zamanında değerlendirilebilir; aksi
 * halde çalışma zamanına düşer. `consteval` (C++20): "anlık fonksiyon" — derleme zamanında
 * hesaplanmak ZORUNDADIR; çalışma zamanı çağrısı derleme hatasıdır. `constinit` (C++20):
 * değişken derleme zamanında başlatılmalıdır ama çalışma zamanında değiştirilebilir — statik
 * başlatma sırası fiyaskosunu çözer. `if consteval` (C++23), yürütmenin şu anda constexpr
 * bağlamında olup olmadığını test eder. Constexpr dinamik tahsisler (C++20): `new`/`delete`
 * değerlendirme sonunda tam serbest bırakılırsa constexpr içinde kullanılabilir.
 *
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <iostream>

// =================================================================================================
// [constexpr] function (Flex / Esnek)
// =================================================================================================
constexpr int squareConstexpr(int n) { return n * n; }

// =================================================================================================
// [consteval] function (Strict Compile-Time / ASLA Çalışma Zamanına Kalmaz)
// =================================================================================================
// C++20 Feature! Only available if compiled with -std=c++20
consteval int squareConsteval(int n) {
  return n * n; // Strictly must be replaced by the answer before ECU boots!
}

// =================================================================================================
// [constinit] Variable (No Runtime startup cost, but Mutable)
// =================================================================================================
// Calculated at compile time (100).
constinit int GlobalConfigurationScale = squareConstexpr(10);
// Notice it is an `int` not `const int`. It can be mutated later!
// TR: Oyun başlamadan önce bu (100) oldu. Oyun oynanırken 500 yapılabilir!

int main() {
  std::cout << "=== MODULE 9: CONSTANTS DEEP DIVE (C++20) ===\n" << std::endl;

  // --- 1. constexpr (ESNEK TEST) ---
  // TR: Parametre sabit (5), o halde 25 olarak Derleme Zamaninda bitir ve Binaya gom!
  constexpr int compileTimeResult = squareConstexpr(5);

  int dynamicVar = 8;
  // TR: Parametre dışarıdan/dosyadan geldi, derleyici pes etti, bunu ÇALIŞMA (Run) zamanında
  // hesapladı!
  int runTimeResult = squareConstexpr(dynamicVar);

  // --- 2. consteval (KUSURSUZ ASKER TESTİ) ---
  // TR: Bu %100 Build aşamasında Assembly'e gömüldü. Maliyet 0 CPU.
  int immediateResult = squareConsteval(6); // 36 is baked directly!

  std::cout << "constexpr(5)=" << compileTimeResult << " | constexpr(" << dynamicVar << ")=" <<
      runTimeResult << " | consteval(6)=" << immediateResult << std::endl;

  // [FATAL ERROR]: This would completely crash the compilation! int
  // failedAttempt = squareConsteval(dynamicVar);
  // EN: The compiler cannot know what `dynamicVar` is at compile time, so `consteval` throws an
  // error! TR: Derleyici dynamicVar'ın kaç olduğunu bilemez, C++20'ye göre cinayet!

  // --- 3. constinit (GLOBAL/STATIC TEST) ---
  std::cout << "Original constinit global Config Scale: " << GlobalConfigurationScale << std::endl;
  // EN: Still mutable! It just started without runtime cost.
  GlobalConfigurationScale = 450;
           // TR: Başlangıç değeri 100 olarak derlemeden geldi ama sonra değiştirebildik!
  std::cout << "Mutated Global Config Scale           : " << GlobalConfigurationScale << std::endl;

  std::cout << "\n[ARCHITECTURAL RECAP]:" << std::endl;
  std::cout << "1. Use 'constexpr' by default. (It falls back to runtime "
               "gently). // Genelde bunu kullan."
            << std::endl;
  std::cout << "2. Use 'consteval' when overhead is forbidden. // Hız kaybı "
               "ölümcül ise bunu kullan."
            << std::endl;
  std::cout << "3. Use 'constinit' on Globals/Statics to prevent startup "
               "fiascos. // Globalleri güvene al."
            << std::endl;

  return 0;
}
