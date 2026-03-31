/**
 * @file module_01_basics/03_functions_and_overloading.cpp
 * @brief Basics: Functions, Overloading & Default Parameters — Temeller: Fonksiyonlar, Aşırı
 * Yükleme ve Varsayılan Parametreler
 *
 * @details
 * =============================================================================
 * [THEORY: Function Anatomy / TEORİ: Fonksiyon Anatomisi]
 * =============================================================================
 * EN: A function in C++ has 4 parts:
 * 1. Return type — what the function gives back (int, double, void, bool, ...)
 * 2. Name        — identifier used to call it
 * 3. Parameters  — input data (can be empty)
 * 4. Body        — the code block `{ }` that executes
 *
 * Declaration (prototype) tells the compiler "this function exists":
 *   int add(int a, int b);
 * Definition provides the actual body:
 *   int add(int a, int b) { return a + b; }
 *
 * TR: C++'ta bir fonksiyonun 4 parçası vardır:
 * 1. Dönüş tipi  — fonksiyonun geri verdiği şey (int, double, void, bool, ...)
 * 2. İsim        — çağırmak için kullanılan tanımlayıcı
 * 3. Parametreler — giriş verileri (boş olabilir)
 * 4. Gövde       — çalışan kod bloğu `{ }`
 *
 * Bildirim (prototip) derleyiciye "bu fonksiyon var" der:
 *   int topla(int a, int b);
 * Tanımlama asıl gövdeyi sağlar:
 *   int topla(int a, int b) { return a + b; }
 *
 * =============================================================================
 * [THEORY: Why Function Overloading is Possible / TEORİ: Aşırı Yükleme Neden Mümkündür]
 * =============================================================================
 * EN: In C, you CANNOT have two functions with the same name — the compiler only checks
 * the name. In C++, the compiler appends parameter types to the function name behind the
 * scenes (called "Name Mangling"):
 *   print(int)   -> _Z5printi
 *   print(float) -> _Z5printf
 * Thus you can have many functions with the same name as long as parameters differ!
 *
 * TR: C dilinde aynı isimde iki fonksiyon YAZAMAZSINIZ — derleyici sadece isme bakar.
 * C++'ta derleyici fonksiyon isimlerine parametre tiplerini ekler ("Name Mangling"):
 *   yazdir(int)   -> _Z6yazdiri
 *   yazdir(float) -> _Z6yazdirf
 * Parametreler farklı olduğu sürece aynı isimde istediğiniz kadar fonksiyon olabilir!
 *
 * =============================================================================
 * [CPPREF DEPTH: Overload Resolution Rules / CPPREF DERİNLİK: Aşırı Yükleme Çözüm Kuralları]
 * =============================================================================
 * EN: How does the compiler decide which overloaded function to call?
 * 1. Exact Match     — passing exactly an `int` to `print(int)`.
 * 2. Promotion       — passing `char` promotes to `int` safely.
 * 3. Std Conversion  — passing `float` converts to `double`.
 * If ambiguous, the compiler throws a Hard Error (build fails).
 *
 * TR: Derleyici hangi fonksiyonu çağıracağına şu rütbe sistemiyle karar verir:
 * 1. Birebir Eşleşme — `int` parametreye tam `int` göndermek.
 * 2. Yükseltme       — `char` gönderilirse güvenle `int`'e yükseltilir.
 * 3. Standart Çevirim — `float` gönderilirse `double`'a dönüştürülür.
 * İki fonksiyon eşit uyuyorsa derleyici "Belirsizlik (Ambiguity)" hatası fırlatır.
 *
 * ⚠  Ref: https://en.cppreference.com/w/cpp/language/overload_resolution
 * ⚠  Ref: https://en.cppreference.com/w/cpp/language/default_arguments
 * ⚠  Ref: https://en.cppreference.com/w/cpp/language/inline
 *
 * @note Compile: g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion 03_functions_and_overloading.cpp -o 03_functions_and_overloading
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <cmath>
#include <iostream>
#include <string>

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// 1. FUNCTION DECLARATIONS (PROTOTYPES) / FONKSİYON BİLDİRİMLERİ (PROTOTİPLER)
// ═════════════════════════════════════════════════════════════════════════════════════════════════
// EN: Prototypes tell the compiler about the function signature BEFORE main(). The actual body
// (definition) can come after main(). This is how large projects are organized with header files.
// TR: Prototipler fonksiyon imzasını main()'den ÖNCE derleyiciye bildirir. Asıl gövde (tanım)
// main()'den sonra gelebilir. Büyük projelerde header dosyaları böyle çalışır.
double celsiusToFahrenheit(double celsius);
double fahrenheitToCelsius(double fahrenheit);

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// 2. FUNCTION OVERLOADING / FONKSİYON AŞIRI YÜKLEMESİ
// ═════════════════════════════════════════════════════════════════════════════════════════════════
// EN: Same name, different parameter types — compiler picks the correct one via Name Mangling.
// TR: Aynı isim, farklı parametre tipleri — derleyici Name Mangling ile doğru olanı seçer.
void displaySensor(int rawValue) {
  std::cout << "  [INT]    Raw sensor: " << rawValue << " counts" << std::endl;
}

void displaySensor(double voltage) {
  std::cout << "  [DOUBLE] Sensor voltage: " << voltage << " V" << std::endl;
}

void displaySensor(const std::string &name) {
  std::cout << "  [STRING] Sensor name: " << name << std::endl;
}

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// 3. DEFAULT PARAMETERS / VARSAYILAN PARAMETRELER
// ═════════════════════════════════════════════════════════════════════════════════════════════════
// EN: Parameters with default values can be omitted at call site. Defaults must be specified
// from RIGHT to LEFT — you cannot skip a middle parameter.
// TR: Varsayılan değerli parametreler çağrı yerinde atlanabilir. Varsayılanlar SAĞDAN SOLA
// belirtilmelidir — ortadaki bir parametreyi atlayamazsınız.
void logDTC(const std::string &code, int severity = 1,
            const std::string &module = "UNKNOWN") {
  std::cout << "  DTC: " << code << " | Severity: " << severity
            << " | Module: " << module << std::endl;
}

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// 4. INLINE FUNCTIONS / SATIR İÇİ FONKSİYONLAR
// ═════════════════════════════════════════════════════════════════════════════════════════════════
// EN: `inline` suggests the compiler to replace the function call with the function body
// directly at the call site, avoiding the overhead of a function call (push/pop stack frame).
// The compiler may ignore the hint if the function is too complex. In modern C++, the compiler
// often inlines small functions automatically (even without the keyword).
// TR: `inline` derleyiciye fonksiyon çağrısını, çağrı noktasında doğrudan fonksiyon gövdesiyle
// değiştirmesini önerir (stack frame push/pop maliyetinden kaçınır). Fonksiyon çok karmaşıksa
// derleyici bu öneriyi görmezden gelebilir. Modern C++'ta derleyici küçük fonksiyonları zaten
// otomatik inline eder.
inline double clampRPM(double rpm, double minRpm = 800.0, double maxRpm = 7000.0) {
  if (rpm < minRpm) return minRpm;
  if (rpm > maxRpm) return maxRpm;
  return rpm;
}

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// 5. CONSTEXPR FUNCTIONS / DERLEME ZAMANI FONKSİYONLARI
// ═════════════════════════════════════════════════════════════════════════════════════════════════
// EN: `constexpr` functions CAN be evaluated at compile time if called with compile-time
// constants. If called with runtime values, they behave like normal functions. This gives
// ZERO runtime cost for constant expressions.
// TR: `constexpr` fonksiyonlar derleme zamanı sabitleriyle çağrılırsa derleme zamanında
// hesaplanabilir. Çalışma zamanı değerleriyle çağrılırsa normal fonksiyon gibi davranır.
// Sabit ifadeler için SIFIR çalışma zamanı maliyeti sağlar.
constexpr int calculateBaudDivisor(int clockHz, int baudRate) {
  return clockHz / (16 * baudRate);
}

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// 6. RETURN TYPE EXAMPLES / DÖNÜŞ TİPİ ÖRNEKLERİ
// ═════════════════════════════════════════════════════════════════════════════════════════════════
// EN: Functions can return different types: int, double, bool, void (nothing), string, etc.
// TR: Fonksiyonlar farklı tipler döndürebilir: int, double, bool, void (hiçbir şey), string vb.

// Returns bool — used for condition checks / bool döndürür — koşul kontrolü için
bool isOverheating(double tempC) { return tempC > 110.0; }

// Returns void — performs action, returns nothing / void — eylem yapar, hiçbir şey döndürmez
void printDivider() {
  std::cout << "  ----------------------------------------" << std::endl;
}

// Returns int — calculation result / int döndürür — hesaplama sonucu
int absoluteDifference(int a, int b) { return std::abs(a - b); }

// ════════════════════════════════════════════════════════════════════════════
//                                 MAIN
// ════════════════════════════════════════════════════════════════════════════
int main() {
  std::cout << "=== MODULE 1: FUNCTIONS & OVERLOADING ===\n" << std::endl;

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // 1. FUNCTION DECLARATIONS — calling prototyped functions / Prototipli fonksiyonları çağırma
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  std::cout << "--- 1. Function Declaration & Definition ---" << std::endl;
  double boilingF = celsiusToFahrenheit(100.0);
  double bodyC = fahrenheitToCelsius(98.6);
  std::cout << "  100 °C = " << boilingF << " °F" << std::endl;
  std::cout << "  98.6 °F = " << bodyC << " °C\n" << std::endl;

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // 2. FUNCTION OVERLOADING / FONKSİYON AŞIRI YÜKLEMESİ
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  std::cout << "--- 2. Function Overloading ---" << std::endl;

  // EN: Compiler selects the correct overload based on argument type.
  // TR: Derleyici argüman tipine göre doğru aşırı yüklemeyi seçer.
  displaySensor(4095);              // int version / int sürümü
  displaySensor(3.28);              // double version / double sürümü
  displaySensor("Knock Sensor B2"); // string version / string sürümü
  std::cout << std::endl;

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // 3. RETURN TYPE VARIETY / DÖNÜŞ TİPİ ÇEŞİTLİLİĞİ
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  std::cout << "--- 3. Return Types ---" << std::endl;

  double engineTemp = 115.0;
  std::cout << "  Engine temp: " << engineTemp << " °C" << std::endl;
  std::cout << "  Overheating? " << (isOverheating(engineTemp) ? "YES" : "NO") << std::endl;

  int rpm1 = 3500, rpm2 = 4200;
  std::cout << "  RPM difference: |" << rpm1 << " - " << rpm2 << "| = "
            << absoluteDifference(rpm1, rpm2) << std::endl;

  printDivider();
  std::cout << std::endl;

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // 4. DEFAULT PARAMETERS / VARSAYILAN PARAMETRELER
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  std::cout << "--- 4. Default Parameters ---" << std::endl;

  // EN: Call with varying number of arguments — defaults fill the rest.
  // TR: Farklı sayıda argümanla çağır — geri kalanı varsayılanlar doldurur.
  logDTC("P0300");                  // severity=1, module="UNKNOWN"
  logDTC("P0171", 2);              // module="UNKNOWN"
  logDTC("P0420", 3, "Powertrain"); // All specified / Hepsi belirtilmiş
  std::cout << std::endl;

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // 5. INLINE FUNCTION / SATIR İÇİ FONKSİYON
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  std::cout << "--- 5. Inline Function ---" << std::endl;

  // EN: clampRPM is likely inlined by the compiler — no function call overhead.
  // TR: clampRPM derleyici tarafından büyük olasılıkla inline edilir — fonksiyon çağrı maliyeti yok.
  std::cout << "  clampRPM(500)  = " << clampRPM(500) << " (min clamped)" << std::endl;
  std::cout << "  clampRPM(3500) = " << clampRPM(3500) << " (unchanged)" << std::endl;
  std::cout << "  clampRPM(9000) = " << clampRPM(9000) << " (max clamped)" << std::endl;
  std::cout << std::endl;

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // 6. CONSTEXPR FUNCTION / DERLEME ZAMANI FONKSİYONU
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  std::cout << "--- 6. constexpr Function ---" << std::endl;

  // EN: This is evaluated at COMPILE TIME — zero runtime cost. The result is baked into the
  // binary. No function call happens at runtime.
  // TR: Bu DERLEME ZAMANINDA hesaplanır — sıfır çalışma zamanı maliyeti. Sonuç binary'ye gömülür.
  constexpr int divisor115200 = calculateBaudDivisor(16000000, 115200);
  constexpr int divisor9600 = calculateBaudDivisor(16000000, 9600);

  std::cout << "  UART 16MHz / 115200 baud -> divisor = " << divisor115200 << std::endl;
  std::cout << "  UART 16MHz / 9600 baud   -> divisor = " << divisor9600 << std::endl;

  // EN: Can also be called with runtime values — behaves like a normal function then.
  // TR: Çalışma zamanı değerleriyle de çağrılabilir — o zaman normal fonksiyon gibi davranır.
  int runtimeClock = 8000000;
  int runtimeBaud = 19200;
  int runtimeDiv = calculateBaudDivisor(runtimeClock, runtimeBaud);
  std::cout << "  UART 8MHz / 19200 baud   -> divisor = " << runtimeDiv << " (runtime)"
            << std::endl;
  std::cout << std::endl;

  return 0;
}

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// FUNCTION DEFINITIONS (after main) / FONKSİYON TANIMLARI (main'den sonra)
// ═════════════════════════════════════════════════════════════════════════════════════════════════
// EN: These were declared (prototyped) BEFORE main, defined here AFTER main.
// This is the standard pattern: declaration in .h (header), definition in .cpp (source).
// TR: Bunlar main'den ÖNCE bildirildi (prototip), burada main'den SONRA tanımlandı.
// Standart kalıp: bildirim .h (başlık) dosyasında, tanım .cpp (kaynak) dosyasında.

double celsiusToFahrenheit(double celsius) {
  return (celsius * 9.0 / 5.0) + 32.0;
}

double fahrenheitToCelsius(double fahrenheit) {
  return (fahrenheit - 32.0) * 5.0 / 9.0;
}
