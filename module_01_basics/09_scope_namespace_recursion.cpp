/**
 * @file module_01_basics/09_scope_namespace_recursion.cpp
 * @brief Basics: Scope, Lifetime, Namespace & Recursion / Temeller: Kapsam, Ömür, İsim Alanı ve
 * Özyineleme
 *
 * @details
 * =============================================================================
 * [THEORY: Scope & Lifetime / TEORİ: Kapsam ve Ömür]
 * =============================================================================
 * EN: Variables live in "scopes" — regions delimited by `{ }`. A variable declared inside a
 * block is INVISIBLE outside. When the closing `}` is reached, the variable is DESTROYED (stack
 * unwinding). Key scopes: - Global scope: Declared outside all functions. Lives for the whole
 * program. - Function/Local scope: Inside a function or block `{ }`. - `static` local:
 * Initialized once, persists across calls (like a global but scoped to the function). Useful for
 * caching sensor calibration data.
 *
 * TR: Değişkenler "kapsam" (scope) içinde yaşar — `{ }` ile sınırlanır. Bir blok içinde
 * tanımlanan değişken dışarıda GÖRÜNMEZdir. Kapanış `}` e ulaşıldığında yok edilir. Anahtar
 * kapsamlar: - Global kapsam: Tüm fonksiyonların dışında. Program boyunca yaşar. -
 * Fonksiyon/Yerel kapsam: Fonksiyon veya blok `{ }` içinde. - `static` yerel: Bir kez
 * başlatılır, çağrılar arasında kalıcıdır.
 *
 * =============================================================================
 * [THEORY: Namespaces / TEORİ: İsim Alanları]
 * =============================================================================
 * EN: Namespaces prevent name collisions in large codebases. Without them, two libraries
 * defining `init()` would cause a linker error. Use `::` to access members. `using namespace
 * std;` pollutes the global scope — avoid it.
 *
 * TR: İsim alanları büyük kod tabanlarında isim çakışmalarını önler. `using namespace std;`
 * global kapsamı kirletir — kaçının.
 *
 * =============================================================================
 * [CPPREF DEPTH: Recursion & Stack Overflow / CPPREF DERİNLİK: Özyineleme ve Yığın Taşması]
 * =============================================================================
 * EN: Recursion means a function calls ITSELF. Every call pushes a new frame onto the call
 * stack. Without a proper "base case", the stack overflows (SIGSEGV). Stack size is typically
 * 1-8 MB. For deeply recursive algorithms, prefer iterative solutions or tail-call optimization
 * hints.
 *
 * TR: Özyineleme bir fonksiyonun KENDİSİNİ çağırması demektir. Her çağrı çağrı yığınına yeni bir
 * çerçeve ekler. Uygun bir "temel durum" (base case) olmadan yığın taşar (SIGSEGV). Yığın boyutu
 * genellikle 1-8 MB'tır.
 *
 * ⚠  Ref: https://en.cppreference.com/w/cpp/language/scope
 * ⚠  Ref: https://en.cppreference.com/w/cpp/language/namespace
 *
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <iostream>
#include <string>

// =================================================================================================
// 1. GLOBAL SCOPE VARIABLE / GLOBAL DEĞİŞKEN
// =================================================================================================
// EN: Accessible everywhere. Lives for the entire program duration.
// TR: Her yerden erişilebilir. Programın tüm süresi boyunca yaşar.
const std::string FIRMWARE_VERSION = "v3.2.1";
int globalDtcCount = 0;

// =================================================================================================
// 2. NAMESPACES / İSİM ALANLARI
// =================================================================================================
// EN: Simulating two different ECU subsystem libraries.
// TR: İki farklı ECU alt sistem kütüphanesini simüle ediyoruz.
namespace Powertrain {
void init() {
  std::cout << "  [Powertrain] Engine management initialized." << std::endl;
}
int getStatus() { return 1; } // 1 = OK
} // namespace Powertrain

namespace BodyControl {
void init() {
  std::cout << "  [BodyControl] Door/window/lighting initialized." << std::endl;
}
int getStatus() { return 1; } // 1 = OK
} // namespace BodyControl

// EN: Nested namespace (C++17 shorthand).
// TR: İç içe isim alanı (C++17 kısaltması).
namespace Vehicle::Diagnostics {
void scanAll() {
  std::cout << "  [Vehicle::Diagnostics] Full OBD-II scan complete."
            << std::endl;
}
} // namespace Vehicle::Diagnostics

// =================================================================================================
// 3. STATIC LOCAL VARIABLE / STATİK YEREL DEĞİŞKEN
// =================================================================================================
// EN: Simulates a sensor reading counter that persists across calls.
// TR: Çağrılar arasında kalıcı bir sensör okuma sayacını simüle eder.
void readSensor() {
  // EN: `static` variables are initialized ONCE, then persist.
  // TR: `static` değişkenler BİR KEZ başlatılır, sonra kalıcıdır.
  static int readCount = 0;
  ++readCount;
  std::cout << "  Sensor read #" << readCount << " (static counter)"
            << std::endl;
}

// =================================================================================================
// 4. DEFAULT PARAMETERS / VARSAYILAN PARAMETRELER
// =================================================================================================
// EN: Parameters with default values can be omitted at call site.
// TR: Varsayılan değerli parametreler çağrı yerinde atlanabilir.
void logDTC(const std::string &code, int severity = 1,
            const std::string &module = "UNKNOWN") {
  ++globalDtcCount;
  std::cout << "  DTC[" << globalDtcCount << "]: " << code
            << " | Severity: " << severity << " | Module: " << module
            << std::endl;
}

// =================================================================================================
// 5. RECURSION / ÖZYİNELEME
// =================================================================================================
// EN: Factorial: Classic recursion demo. factorial(5) = 5×4×3×2×1 = 120.
// TR: Faktöriyel: Klasik özyineleme. factorial(5) = 5×4×3×2×1 = 120.
int factorial(int n) {
  // Base case — stops recursion / Temel durum — özyinelemeyi durdurur
  if (n <= 1) return 1;
  // Recursive case — calls itself / Özyinelemeli durum — kendini çağırır
  return n * factorial(n - 1);
}

// EN: Fibonacci: Another classic. fib(6) = 8 (0,1,1,2,3,5,8...). WARNING: Exponential time
// complexity O(2^n) — see Module 4 for memoization.
// TR: Fibonacci: Bir diğer klasik. UYARI: Üssel zaman karmaşıklığı O(2^n).
int fibonacci(int n) {
  if (n <= 0) return 0;
  if (n == 1) return 1;
  return fibonacci(n - 1) + fibonacci(n - 2);
}

// EN: Count-down recursion — simulates ECU shutdown sequence.
// TR: Geri sayım özyinelemesi — ECU kapatma sırasını simüle eder.
void shutdownCountdown(int n) {
  if (n < 0) return; // Base case
  std::cout << "  Shutdown in: " << n << "..." << std::endl;
  shutdownCountdown(n - 1); // Recursive call
}

int main() {
  std::cout << "=== MODULE 1: SCOPE, NAMESPACE & RECURSION ===\n" << std::endl;

  // ===============================================================================================
  // 1. SCOPE DEMONSTRATION / KAPSAM GÖSTERİMİ
  // ===============================================================================================
  std::cout << "--- 1. Scope & Lifetime ---" << std::endl;
  std::cout << "Global firmware version: " << FIRMWARE_VERSION << std::endl;

  {
    // EN: This variable lives ONLY inside this block.
    // TR: Bu değişken SADECE bu bloğun içinde yaşar.
    int localRpm = 3500;
    std::cout << "Inside block: localRpm = " << localRpm << std::endl;
  }
  // localRpm is DEAD here! / localRpm artık YOK! std::cout << localRpm; // ERROR: 'localRpm'
  // undeclared

  // EN: Variable shadowing — inner scope hides outer variable.
  // TR: Değişken gölgeleme — iç kapsam dış değişkeni gizler.
  int outerTemp = 25;
  std::cout << "Outer temp: " << outerTemp << std::endl;
  {
    int innerTemp = 90; // Different name to avoid -Wshadow
    std::cout << "Inner temp (different scope): " << innerTemp << std::endl;
  }
  std::cout << "Outer temp unchanged: " << outerTemp << "\n" << std::endl;

  // ===============================================================================================
  // 2. NAMESPACE DEMO / İSİM ALANI GÖSTERİMİ
  // ===============================================================================================
  std::cout << "--- 2. Namespaces ---" << std::endl;

  // EN: Both have init() but no collision thanks to namespaces.
  // TR: İkisi de init() fonksiyonuna sahip ama isim alanları sayesinde çakışma yok.
  Powertrain::init();
  BodyControl::init();
  Vehicle::Diagnostics::scanAll();

  std::cout << "Powertrain status: " << Powertrain::getStatus() << std::endl;
  std::cout << "BodyControl status: " << BodyControl::getStatus() << "\n"
            << std::endl;

  // ===============================================================================================
  // 3. STATIC LOCAL VARIABLE / STATİK YEREL DEĞİŞKEN
  // ===============================================================================================
  std::cout << "--- 3. Static Local Variable ---" << std::endl;

  // EN: readCount inside readSensor() persists across calls.
  // TR: readSensor() içindeki readCount çağrılar arasında kalıcıdır.
  readSensor(); // #1
  readSensor(); // #2
  readSensor(); // #3
  std::cout << std::endl;

  // ===============================================================================================
  // 4. DEFAULT PARAMETERS / VARSAYILAN PARAMETRELER
  // ===============================================================================================
  std::cout << "--- 4. Default Parameters ---" << std::endl;

  logDTC("P0300");                            // Uses all defaults
  logDTC("P0171", 2);                         // Custom severity
  logDTC("P0420", 3, "Powertrain");           // All specified
  std::cout << "Total DTCs logged: " << globalDtcCount << "\n" << std::endl;

  // ===============================================================================================
  // 5. RECURSION DEMOS / ÖZYİNELEME GÖSTERİMLERİ
  // ===============================================================================================
  std::cout << "--- 5. Recursion ---" << std::endl;

  // Factorial
  std::cout << "factorial(5) = " << factorial(5) << std::endl;  // 120
  std::cout << "factorial(10) = " << factorial(10) << std::endl; // 3628800

  // Fibonacci
  std::cout << "fibonacci(6) = " << fibonacci(6) << std::endl;  // 8
  std::cout << "fibonacci(10) = " << fibonacci(10) << std::endl; // 55

  // ECU Shutdown Countdown
  std::cout << "\nECU shutdown sequence:" << std::endl;
  shutdownCountdown(3);

  return 0;
}
