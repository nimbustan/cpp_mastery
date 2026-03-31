/**
 * @file module_01_basics/01_variables_and_memory.cpp
 * @brief Variables, Data Types, and Memory Model — Değişkenler, Veri Tipleri ve Bellek Modeli
 *
 * @details
 * =============================================================================
 * [THEORY: Primitive Data Types & Memory / TEORİ: İlkel Veri Tipleri ve Bellek]
 * =============================================================================
 * EN: Every variable in C++ is a named region of memory. When you write `int age = 30;`, the
 * compiler reserves 4 bytes on the Stack, names that region "age", and writes the binary
 * representation of 30 into it. The Stack is a LIFO (Last-In, First-Out) memory region that
 * grows/shrinks automatically as functions are called/returned. Local variables live here — they
 * are born when their scope `{` begins, and die when `}` ends. The Heap is a separate region
 * used for dynamic allocation (`new`/`delete`) where objects survive beyond function scope.
 * Understanding WHERE a variable lives (Stack vs Heap) is the foundation of all C++ memory
 * management.
 *
 * C++ defines several primitive data types, each consuming a specific amount of memory. The
 * `sizeof()` operator reveals the exact byte count at compile-time. Choosing the correct type
 * directly affects memory usage, performance, and correctness — a `float` where a `double` is
 * needed causes silent precision loss; an `int` where a `long long` is needed causes overflow.
 *
 * TR: C++'ta her değişken, bellekte isimlendirilmiş bir bölgedir. `int age = 30;` yazdığınızda,
 * derleyici Stack'te (Yığın) 4 baytlık bir alan ayırır, o bölgeye "age" adını verir ve 30'un
 * ikili (binary) temsilini oraya yazar. Stack, fonksiyonlar çağrıldıkça/döndükçe otomatik olarak
 * büyüyen/küçülen bir LIFO (Son Giren İlk Çıkar) bellek bölgesidir. Yerel değişkenler burada
 * yaşar — kapsamları `{` ile başladığında doğar, `}` ile bittiğinde ölür. Heap (Öbek) ise
 * `new`/`delete` ile dinamik tahsis yapılan ayrı bir bölgedir; buradaki nesneler fonksiyon
 * ötesinde yaşayabilir. Bir değişkenin NEREDE yaşadığını (Stack vs Heap) anlamak, tüm C++ bellek
 * yönetiminin temelidir.
 *
 * C++ birden fazla ilkel veri tipi tanımlar ve her biri bellekte belirli miktarda yer kaplar.
 * `sizeof()` operatörü derleme zamanında tam bayt sayısını gösterir. Doğru tipi seçmek, bellek
 * kullanımını, performansı ve doğruluğu doğrudan etkiler — `double` gereken yerde `float`
 * kullanmak sessiz hassasiyet kaybına, `long long` gereken yerde `int` kullanmak taşmaya (overflow)
 * neden olur.
 *
 * =============================================================================
 * [THEORY: Stack vs Heap — Where Do Variables Live? / TEORİ: Stack vs Heap — Değişkenler
 * Nerede Yaşar?]
 * =============================================================================
 * EN: The Stack is fast (allocation is just moving a pointer), automatically managed, and limited
 * in size (typically 1-8 MB). The Heap is slower (OS must find free blocks), manually managed
 * (you must `delete` what you `new`), but virtually unlimited. Rule of thumb:
 *   1. Use Stack for small, short-lived variables (int, float, small arrays).
 *   2. Use Heap for large data or data that must outlive the current scope.
 *   3. In Modern C++, prefer smart pointers (unique_ptr, shared_ptr) over raw `new`/`delete`.
 *
 * TR: Stack hızlıdır (tahsis sadece bir pointer kaydırmaktır), otomatik yönetilir ve boyutu
 * sınırlıdır (genellikle 1-8 MB). Heap daha yavaştır (işletim sistemi boş blok aramalıdır),
 * manuel yönetilir (`new` ile aldığınızı `delete` ile geri vermelisiniz) ama neredeyse sınırsız
 * büyüklüktedir. Temel kural:
 *   1. Küçük ve kısa ömürlü değişkenler için Stack kullanın (int, float, küçük diziler).
 *   2. Büyük veriler veya mevcut kapsamın ötesinde yaşaması gereken veriler için Heap kullanın.
 *   3. Modern C++'ta ham `new`/`delete` yerine akıllı işaretçileri (unique_ptr, shared_ptr)
 *      tercih edin.
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
 * @note Compile: g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion 01_variables_and_memory.cpp -o 01_variables_and_memory
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
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // EN: The C++ standard does NOT guarantee the size of `int` — it only guarantees minimum
  // ranges. On most 64-bit systems `int` is 4 bytes, but on some embedded platforms it could
  // be 2 bytes. For portable, safety-critical code (MISRA, AUTOSAR), ALWAYS use fixed-width
  // types from <cstdint>: int8_t, int16_t, int32_t, int64_t.
  //
  // TR: C++ standardı, `int`'in boyutunu GARANTİ ETMEZ — sadece minimum aralıkları garanti
  // eder. Çoğu 64-bit sistemde `int` 4 bayt olsa da, bazı gömülü platformlarda 2 bayt
  // olabilir. Taşınabilir ve güvenlik kritik kodlarda (MISRA, AUTOSAR) DAİMA <cstdint>'ten
  // sabit genişlikli tipler kullanın: int8_t, int16_t, int32_t, int64_t.

  int age = 30; // 4 bytes on most systems (Çoğu sistemde 4 bayt, ama garanti değil!)
  std::cout << "int takes: " << sizeof(int) << " bytes (bayt). Example: " << age << std::endl;

  // [CPPREF DEPTH: Uninitialized Variables / CPPREF DERİNLİK: Başlatılmamış Değişkenler]
  // EN: DANGER (UB)! Reading an uninitialized local variable is one of the most common bugs in
  // C++. The variable holds whatever garbage data was left at that RAM address from a previous
  // function call. The program may work in debug mode but crash in release mode because the
  // optimizer assumes no UB and makes aggressive optimizations based on that assumption.
  //
  // TR: TEHLİKE (UB)! Başlatılmamış yerel değişken okumak, C++'taki en yaygın hatalardan
  // biridir. Değişken, o RAM adresinde önceki fonksiyon çağrısından kalan çöp verileri tutar.
  // Program debug modunda çalışabilir ama release modunda çökebilir çünkü optimize edici UB
  // olmadığını varsayarak agresif optimizasyonlar yapar.
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
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // EN: Floating-point numbers follow the IEEE 754 standard. `float` uses 32 bits (23-bit
  // mantissa = ~7 decimal digits of precision). `double` uses 64 bits (52-bit mantissa = ~15
  // decimal digits). NEVER compare floats with `==` because of rounding errors! Use an epsilon:
  // `std::abs(a - b) < 1e-9`. In financial software, avoid floats entirely — use integer cents.
  //
  // TR: Kayan noktalı sayılar IEEE 754 standardını takip eder. `float` 32 bit kullanır (23-bit
  // mantis = ~7 ondalık basamak hassasiyet). `double` 64 bit kullanır (52-bit mantis = ~15
  // ondalık basamak). Yuvarlama hataları nedeniyle float'ları ASLA `==` ile karşılaştırmayın!
  // Epsilon kullanın: `std::abs(a - b) < 1e-9`. Finansal yazılımda float'dan tamamen kaçının
  // — tam sayı olarak kuruş/cent birimiyle çalışın.

  float piFloat = 3.14159f;         // 'f' suffix = float literal (f eki float literal belirtir)
  double piDouble = 3.14159265359;  // Default: double (Varsayılan: double, daha yüksek hassasiyet)

  std::cout << "float takes: " << sizeof(float) << " bytes. Value: " << piFloat << std::endl;
  std::cout << "double takes: " << sizeof(double) << " bytes. Value: " << piDouble << "\n" <<
      std::endl;

  // 3. Characters and Booleans (Karakterler ve Mantıksal Tipler)
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // EN: `char` is actually a 1-byte INTEGER (0-255 or -128 to 127). It stores an ASCII code,
  // not a "letter". 'A' == 65 in memory. This is why you can do math with chars:
  // `char lower = 'A' + 32;` gives 'a'. `bool` is also an integer: `true` == 1, `false` == 0.
  // In conditions, ANY non-zero value is truthy: `if (42)` is true, `if (0)` is false.
  //
  // TR: `char` aslında 1 baytlık bir TAM SAYIDIR (0-255 veya -128 ile 127). Bir "harf" değil,
  // ASCII kodu saklar. Bellekte 'A' == 65'tir. Bu yüzden char ile matematik yapabilirsiniz:
  // `char lower = 'A' + 32;` sonucu 'a' verir. `bool` da bir tam sayıdır: `true` == 1,
  // `false` == 0. Koşullarda sıfır olmayan HER değer doğrudur: `if (42)` true, `if (0)` false.

  char grade = 'A';      // Stores ASCII 65, not the letter (ASCII 65 saklar, harf değil)
  bool isCppFun = true;  // Stored as integer 1 (Bellekte tam sayı 1 olarak saklanır)

  std::cout << "char takes: " << sizeof(char) << " byte. Value: " << grade << std::endl;
  std::cout << "bool takes: " << sizeof(bool) << " byte. Value: " << isCppFun << "\n" << std::endl;

  // 4. Constants (Sabitler)
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // EN: `const` makes a variable read-only AFTER initialization — the value is set at runtime
  // and cannot be changed afterward. `constexpr` is stronger: the value MUST be computable at
  // COMPILE-TIME. The compiler embeds it directly into the binary as a literal constant — no
  // memory is allocated, no runtime cost. Prefer `constexpr` over `#define` macros: it is
  // type-safe, scoped, and debuggable. In Modern C++ (C++17+), `constexpr` can even be used
  // with `if constexpr` for compile-time branching.
  //
  // TR: `const` bir değişkeni başlatıldıktan SONRA salt okunur yapar — değer çalışma zamanında
  // belirlenir ve sonra değiştirilemez. `constexpr` daha güçlüdür: değer DERLEME ZAMANINDA
  // hesaplanabilir OLMALIDIR. Derleyici değeri doğrudan binary'ye sabit olarak gömer — bellek
  // ayrılmaz, çalışma zamanı maliyeti yoktur. `#define` makroları yerine `constexpr` tercih
  // edin: tip güvenlidir, kapsamı vardır ve debug edilebilir. Modern C++'ta (C++17+)
  // `constexpr` ile `if constexpr` kullanarak derleme zamanlı dallanma bile yapılabilir.

  const int MAX_USERS = 100;  // Runtime constant (Çalışma zamanı sabiti)

  // EN: constexpr came with C++11. The compiler evaluates the expression at compile-time and
  // embeds the result directly. No CPU cycles are wasted at runtime calculating this.
  // TR: constexpr C++11 ile geldi. Derleyici ifadeyi derleme zamanında hesaplar ve sonucu
  // doğrudan gömer. Çalışma zamanında bu hesaplama için CPU döngüsü harcanmaz.
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
