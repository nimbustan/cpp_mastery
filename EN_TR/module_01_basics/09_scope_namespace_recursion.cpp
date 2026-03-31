/**
 * @file module_01_basics/09_scope_namespace_recursion.cpp
 * @brief Basics: Scope, Lifetime, Namespace & Recursion — Temeller: Kapsam, Ömür, İsim Alanı ve
 * Özyineleme
 *
 * @details
 * =============================================================================
 * [THEORY: Scope & Lifetime / TEORİ: Kapsam ve Ömür]
 * =============================================================================
 * EN: Variables live in "scopes" — regions delimited by `{ }`. A variable declared inside a
 * block is INVISIBLE outside. When the closing `}` is reached, the variable is DESTROYED (stack
 * unwinding). Key scopes:
 *     1. Global scope: Declared outside all functions. Lives for the whole program.
 *     2. Function/Local scope: Inside a function or block `{ }`.
 *     3. `static` local: Initialized once, persists across calls (like a global
 *        but scoped to the function). Useful for caching sensor calibration data.
 *
 * TR: Değişkenler "kapsam" (scope) içinde yaşar — `{ }` ile sınırlanır. Bir blok içinde
 * tanımlanan değişken dışarıda GÖRÜNMEZdir. Kapanış `}` e ulaşıldığında yok edilir.
 *     Anahtar kapsamlar:
 *     1. Global kapsam: Tüm fonksiyonların dışında. Program boyunca yaşar.
 *     2. Fonksiyon/Yerel kapsam: Fonksiyon veya blok `{ }` içinde.
 *     3. `static` yerel: Bir kez başlatılır, çağrılar arasında kalıcıdır.
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
 * @note Compile: g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion 09_scope_namespace_recursion.cpp -o 09_scope_namespace_recursion
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <iostream>
#include <string>

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// 1. GLOBAL SCOPE VARIABLE / GLOBAL DEĞİŞKEN
// ═════════════════════════════════════════════════════════════════════════════════════════════════
// EN: Global variables are declared OUTSIDE all functions. They live in the DATA segment
//     of the process memory (not on the stack). Their lifetime = entire program duration.
//     They are ZERO-INITIALIZED by default (unlike local variables which are GARBAGE).
//
//     DANGER: In multi-file projects, the initialization ORDER of globals across
//     different .cpp files is UNDEFINED ("Static Initialization Order Fiasco").
//     File A's global might use File B's global before B is initialized → UB.
//     Solution: Use function-local statics ("Meyers Singleton" idiom) instead.
//
//     `const` globals are fine because they are immutable and typically optimized
//     into read-only memory by the compiler.
//
// TR: Global değişkenler tüm fonksiyonların DIŞINDA tanımlanır. Proses belleğinin DATA
//     segmentinde yaşarlar (yığında değil). Ömürleri = tüm program süresi.
//     Varsayılan olarak SIFIR BAŞLATILIRLAR (yerel değişkenler ÇÖP olan değil).
//
//     TEHLİKE: Çok dosyalı projelerde, farklı .cpp dosyalarındaki globallerin
//     başlatma SIRASI TANIMSIZDIR ("Statik Başlatma Sırası Felaketi").
//     A dosyasının global'i, B başlatılmadan B'nin global'ini kullanabilir → UB.
//     Çözüm: Bunun yerine fonksiyon-yerel static kullanın ("Meyers Singleton" deyimi).
//
//     `const` globaller güvenlidir çünkü değiştirilemezler ve genellikle derleyici
//     tarafından salt okunur belleğe optimize edilirler.
const std::string FIRMWARE_VERSION = "v3.2.1";


// ═════════════════════════════════════════════════════════════════════════════════════════════════
// 2. NAMESPACES / İSİM ALANLARI
// ═════════════════════════════════════════════════════════════════════════════════════════════════
// EN: Namespaces group related code under a unique name to prevent collisions.
//     Without namespaces, if Powertrain and BodyControl both define `init()`,
//     the linker would see TWO definitions of the same function → linker error.
//     With namespaces, they become `Powertrain::init()` and `BodyControl::init()`
//     — two completely separate functions.
//
//     The `::` operator is called the SCOPE RESOLUTION OPERATOR. It tells the
//     compiler exactly which namespace's function you mean.
//
//     WHY `using namespace std;` IS DANGEROUS:
//     `std` contains thousands of names (min, max, count, distance, move, swap...).
//     `using namespace std;` dumps ALL of them into your scope. If you define
//     your own `count` variable, it silently collides with `std::count`.
//     In header files, this is ESPECIALLY catastrophic — every file that #includes
//     your header inherits the pollution. ALWAYS use `std::` prefix instead.
//
// TR: İsim alanları ilişkili kodu çakışmayı önlemek için benzersiz bir isim altında gruplar.
//     İsim alanları olmadan, Powertrain ve BodyControl ikisi de `init()` tanımlarsa,
//     bağlayıcı aynı fonksiyonun İKİ tanımını görür → bağlayıcı hatası.
//     İsim alanlarıyla `Powertrain::init()` ve `BodyControl::init()` olurlar
//     — iki tamamen ayrı fonksiyon.
//
//     `::` operatörüne KAPSAM ÇÖZÜMLEME OPERATÖRÜ denir. Derleyiciye hangi
//     isim alanının fonksiyonunu kastettiğinizi söyler.
//
//     NEDEN `using namespace std;` TEHLİKELİDİR:
//     `std` binlerce isim içerir (min, max, count, distance, move, swap...).
//     `using namespace std;` HEPSİNİ kapsamınıza döker. Kendi `count`
//     değişkeninizi tanımlarsanız `std::count` ile sessizce çakışır.
//     Başlık dosyalarında bu ÖZELLİKLE FELAKETTİR — başlığınızı #include
//     eden her dosya kirliliği miras alır. HER ZAMAN `std::` öneki kullanın.
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

// EN: Nested namespace — C++17 shorthand syntax. Before C++17 you had to write:
//         namespace Vehicle { namespace Diagnostics { ... } }
//     C++17 allows the compact `Vehicle::Diagnostics` form.
//     Nested namespaces are common in large projects: `company::product::module`.
// TR: İç içe isim alanı — C++17 kısa sözdizimi. C++17 öncesi şöyle yazmanız gerekirdi:
//         namespace Vehicle { namespace Diagnostics { ... } }
//     C++17 kompakt `Vehicle::Diagnostics` biçimine izin verir.
//     İç içe isim alanları büyük projelerde yaygındır: `sirket::urun::modul`.
namespace Vehicle::Diagnostics {
void scanAll() {
  std::cout << "  [Vehicle::Diagnostics] Full OBD-II scan complete."
            << std::endl;
}
} // namespace Vehicle::Diagnostics

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// 3. STATIC LOCAL VARIABLE / STATİK YEREL DEĞİŞKEN
// ═════════════════════════════════════════════════════════════════════════════════════════════════
// EN: A function-local `static` variable combines two properties:
//     1. LOCAL SCOPE: Only visible inside this function (encapsulated).
//     2. STATIC LIFETIME: Initialized the FIRST time the function is called,
//        then PERSISTS in memory until the program ends (not destroyed on return).
//     It lives in the DATA segment (like a global), but its NAME is only
//     visible inside the function (unlike a global).
//
//     C++11 GUARANTEE: Initialization of function-local statics is THREAD-SAFE.
//     If two threads call readSensor() simultaneously for the first time, only
//     ONE thread initializes readCount — the other waits. ("Magic statics").
//
//     Use cases: call counters, lazy-initialized caches, Meyers Singleton.
//
// TR: Fonksiyon-yerel `static` değişken iki özelliği birleştirir:
//     1. YEREL KAPSAM: Sadece bu fonksiyonun içinde görünür (kapsüllenmiş).
//     2. STATİK ÖMÜR: Fonksiyon İLK çağrıldığında başlatılır, sonra program
//        bitene kadar bellekte KALIR (dönüşte yok edilmez).
//     DATA segmentinde yaşar (global gibi), ama İSMİ sadece fonksiyon
//     içinde görünür (globalden farklı olarak).
//
//     C++11 GARANTİSİ: Fonksiyon-yerel static'lerin başlatılması İŞ PARÇACIĞI
//     GÜVENLİDİR. İki iş parçacığı aynı anda ilk kez readSensor() çağırırsa,
//     sadece BİRİ readCount'u başlatır — diğeri bekler. ("Sihirli static'ler").
//
//     Kullanım alanları: çağrı sayaçları, tembel başlatılan önbellekler, Meyers Singleton.
void readSensor() {
  // EN: `static int readCount = 0;` — this line executes ONLY ONCE (first call).
  //     On subsequent calls, it is SKIPPED. readCount retains its value from
  //     the previous call. Without `static`, readCount would be 0 every time.
  // TR: `static int readCount = 0;` — bu satır SADECE BİR KEZ çalışır (ilk çağrı).
  //     Sonraki çağrılarda ATLANIR. readCount önceki çağrıdaki değerini korur.
  //     `static` olmadan readCount her seferinde 0 olurdu.
  static int readCount = 0;
  ++readCount;
  std::cout << "  Sensor read #" << readCount << " (static counter)"
            << std::endl;
}

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// 4. RECURSION / ÖZYİNELEME
// ═════════════════════════════════════════════════════════════════════════════════════════════════
// EN: Factorial — the "hello world" of recursion.
//     factorial(5) → 5 × factorial(4) → 5 × 4 × factorial(3) → ... → 5×4×3×2×1 = 120
//
//     Every recursive function MUST have:
//     1. BASE CASE: The condition that STOPS recursion (here: n <= 1 → return 1).
//        Without it, the function calls itself forever → stack overflow (SIGSEGV).
//     2. RECURSIVE CASE: The call that makes the problem SMALLER (here: n-1).
//        Each call MUST move toward the base case, otherwise → infinite recursion.
//
//     HOW IT WORKS IN MEMORY:
//     Each call pushes a new STACK FRAME containing: parameters, local variables,
//     and the return address. factorial(5) creates 5 frames on the stack.
//     When the base case returns, frames are POPPED one by one (unwinding),
//     multiplying the results back up the chain.
//
// TR: Faktöriyel — özyinelemenin "merhaba dünya"sı.
//     factorial(5) → 5 × factorial(4) → 5 × 4 × factorial(3) → ... → 5×4×3×2×1 = 120
//
//     Her özyinelemeli fonksiyonun OLMASI GEREKENLER:
//     1. TEMEL DURUM: Özyinelemeyi DURDURAN koşul (burada: n <= 1 → return 1).
//        Olmadan fonksiyon sonsuza dek kendini çağırır → yığın taşması (SIGSEGV).
//     2. ÖZYİNELEMELİ DURUM: Problemi KÜÇÜLTEN çağrı (burada: n-1).
//        Her çağrı temel duruma YAKLAŞMALIDIR, yoksa → sonsuz özyineleme.
//
//     BELLEKTE NASIL ÇALIŞIR:
//     Her çağrı yeni bir YIĞIN ÇERÇEVESİ ekler: parametreler, yerel değişkenler
//     ve dönüş adresi. factorial(5) yığında 5 çerçeve oluşturur.
//     Temel durum döndüğünde, çerçeveler birer birer ÇIKARILIR (geri sarma),
//     sonuçları zincirde yukarı çarparak birleştirir.
int factorial(int n) {
  if (n <= 1) return 1;       // Base case / Temel durum
  return n * factorial(n - 1); // Recursive case / Özyinelemeli durum
}

// EN: Fibonacci — demonstrates the DANGER of naive recursion.
//     Sequence: 0, 1, 1, 2, 3, 5, 8, 13, 21, 34, 55...
//     Each number = sum of the two before it: fib(n) = fib(n-1) + fib(n-2).
//
//     PROBLEM: This implementation has O(2^n) time complexity!
//     fibonacci(40) makes over 200 MILLION calls. fibonacci(6) alone creates
//     this call tree:  fib(6) → fib(5) + fib(4)
//                      fib(5) → fib(4) + fib(3)    ← fib(4) computed AGAIN!
//     The same sub-problems are recalculated exponentially many times.
//
//     SOLUTIONS (covered in later modules):
//     1. Memoization: Cache results in a map/array → O(n) time, O(n) space.
//     2. Iterative: Use a simple loop with two variables → O(n) time, O(1) space.
//     3. Dynamic Programming: Bottom-up table filling.
//
// TR: Fibonacci — naif özyinelemenin TEHLİKESİNİ gösterir.
//     Dizi: 0, 1, 1, 2, 3, 5, 8, 13, 21, 34, 55...
//     Her sayı = önceki ikisinin toplamı: fib(n) = fib(n-1) + fib(n-2).
//
//     PROBLEM: Bu uygulama O(2^n) zaman karmaşıklığına sahiptir!
//     fibonacci(40) 200 MİLYONDAN fazla çağrı yapar. fibonacci(6) bile
//     şu çağrı ağacını oluşturur: fib(6) → fib(5) + fib(4)
//                                  fib(5) → fib(4) + fib(3) ← fib(4) TEKRAR hesaplanıyor!
//     Aynı alt problemler üssel olarak defalarca yeniden hesaplanır.
//
//     ÇÖZÜMLER (sonraki modüllerde):
//     1. Memoization: Sonuçları map/dizide önbelleğe al → O(n) zaman, O(n) alan.
//     2. İteratif: İki değişkenli basit döngü → O(n) zaman, O(1) alan.
//     3. Dinamik Programlama: Aşağıdan yukarıya tablo doldurma.
int fibonacci(int n) {
  if (n <= 0) return 0;  // Base case 1
  if (n == 1) return 1;  // Base case 2
  return fibonacci(n - 1) + fibonacci(n - 2);  // Two recursive calls!
}

// EN: TAIL RECURSION example — the recursive call is the LAST operation.
//     Some compilers can optimize this into a loop (no extra stack frames).
//     This eliminates stack overflow risk for deep recursion.
//     GCC with -O2 can perform Tail Call Optimization (TCO) on this pattern.
// TR: KUYRUK ÖZYİNELEME örneği — özyinelemeli çağrı SON işlemdir.
//     Bazı derleyiciler bunu döngüye optimize edebilir (ekstra yığın çerçevesi yok).
//     Bu, derin özyinelemede yığın taşması riskini ortadan kaldırır.
//     GCC -O2 ile bu kalıpta Kuyruk Çağrı Optimizasyonu (TCO) yapabilir.
void shutdownCountdown(int n) {
  if (n < 0) return;  // Base case: countdown finished
  std::cout << "  Shutdown in: " << n << "..." << std::endl;
  shutdownCountdown(n - 1);  // Tail call: nothing happens after this returns
}

int main() {
  std::cout << "=== MODULE 1: SCOPE, NAMESPACE & RECURSION ===\n" << std::endl;

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // 1. SCOPE DEMONSTRATION / KAPSAM GÖSTERİMİ
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  std::cout << "--- 1. Scope & Lifetime ---" << std::endl;

  // EN: Global variables are accessible from ANY function in this file.
  //     FIRMWARE_VERSION was declared at file scope (line ~65) and lives
  //     until the program terminates. It's `const`, so it cannot be modified.
  // TR: Global değişkenler bu dosyadaki HERHANGİ bir fonksiyondan erişilebilir.
  //     FIRMWARE_VERSION dosya kapsamında (~65. satır) tanımlandı ve program
  //     sonlanana kadar yaşar. `const` olduğu için değiştirilemez.
  std::cout << "Global firmware version: " << FIRMWARE_VERSION << std::endl;

  {
    // EN: The `{ }` braces create a NEW SCOPE (block scope). Any variable
    //     declared here is BORN at this point and DIES at the closing `}`.
    //     The compiler DEALLOCATES the stack space automatically.
    //     This is useful for limiting variable lifetime and freeing resources early.
    // TR: `{ }` süslü parantezler YENİ BİR KAPSAM (blok kapsamı) oluşturur.
    //     Burada tanımlanan her değişken bu noktada DOĞAR ve kapanış `}` da ÖLÜR.
    //     Derleyici yığın alanını otomatik olarak serbest bırakır.
    //     Değişken ömrünü sınırlamak ve kaynakları erken serbest bırakmak için kullanışlıdır.
    int localRpm = 3500;
    std::cout << "Inside block: localRpm = " << localRpm << std::endl;
  }
  // EN: localRpm is DESTROYED here. Trying to use it would cause:
  //         error: 'localRpm' was not declared in this scope
  //     This is NOT a runtime error — it's a COMPILE-TIME error. The compiler
  //     enforces scope rules and prevents you from accessing dead variables.
  // TR: localRpm burada YOK EDİLDİ. Kullanmaya çalışmak şu hatayı verir:
  //         error: 'localRpm' was not declared in this scope
  //     Bu çalışma zamanı hatası DEĞİLDİR — DERLEME ZAMANI hatasıdır. Derleyici
  //     kapsam kurallarını uygular ve ölü değişkenlere erişmenizi engeller.

  // EN: VARIABLE SHADOWING — when an inner scope declares a variable with the
  //     same name as an outer scope variable, the inner one "shadows" (hides) it.
  //     The outer variable still exists but is UNREACHABLE by that name.
  //     Our -Wshadow flag warns about this, so we use different names here.
  //     In real code, shadowing is a common source of subtle bugs.
  // TR: DEĞİŞKEN GÖLGELEME — iç kapsam, dış kapsamdaki bir değişkenle aynı
  //     isimde değişken tanımladığında, içteki dıştakini "gölgeler" (gizler).
  //     Dış değişken hâlâ var ama o isimle ERİŞİLEMEZ.
  //     -Wshadow bayrağımız bunu uyarır, bu yüzden burada farklı isimler kullanıyoruz.
  //     Gerçek kodda, gölgeleme ince hataların yaygın bir kaynağıdır.
  int outerTemp = 25;
  std::cout << "Outer temp: " << outerTemp << std::endl;
  {
    int innerTemp = 90;
    std::cout << "Inner temp (different scope): " << innerTemp << std::endl;
  }
  std::cout << "Outer temp unchanged: " << outerTemp << "\n" << std::endl;

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // 2. NAMESPACE DEMO / İSİM ALANI GÖSTERİMİ
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  std::cout << "--- 2. Namespaces ---" << std::endl;

  // EN: Both Powertrain and BodyControl define init(), but there's NO collision
  //     because the full names are Powertrain::init() and BodyControl::init().
  //     Think of namespaces like folders on a filesystem: you can have
  //     /Powertrain/init and /BodyControl/init without conflict.
  // TR: Hem Powertrain hem BodyControl init() tanımlıyor, ama ÇAKIŞMA YOK
  //     çünkü tam isimleri Powertrain::init() ve BodyControl::init().
  //     İsim alanlarını dosya sistemindeki klasörler gibi düşünün:
  //     /Powertrain/init ve /BodyControl/init çakışmadan var olabilir.
  Powertrain::init();
  BodyControl::init();

  // EN: Nested namespace accessed with double :: — Vehicle::Diagnostics::scanAll().
  //     In real automotive software, you might see: oem::vehicle::diag::readDTC().
  // TR: İç içe isim alanına çift :: ile erişilir — Vehicle::Diagnostics::scanAll().
  //     Gerçek otomotiv yazılımında görebilirsiniz: oem::vehicle::diag::readDTC().
  Vehicle::Diagnostics::scanAll();

  std::cout << "Powertrain status: " << Powertrain::getStatus() << std::endl;
  std::cout << "BodyControl status: " << BodyControl::getStatus() << "\n"
            << std::endl;

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // 3. STATIC LOCAL VARIABLE / STATİK YEREL DEĞİŞKEN
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  std::cout << "--- 3. Static Local Variable ---" << std::endl;

  // EN: Calling readSensor() 3 times. Watch the counter INCREMENT each time:
  //     Call 1 → readCount becomes 1 (initialized from 0)
  //     Call 2 → readCount becomes 2 (NOT re-initialized, just incremented)
  //     Call 3 → readCount becomes 3
  //     If readCount were a normal (non-static) local, it would be 1 every call.
  // TR: readSensor()'u 3 kez çağırıyoruz. Sayacın her seferinde ARTTIĞINI izleyin:
  //     Çağrı 1 → readCount 1 olur (0'dan başlatıldı)
  //     Çağrı 2 → readCount 2 olur (yeniden başlatılMAZ, sadece artırılır)
  //     Çağrı 3 → readCount 3 olur
  //     readCount normal (statik olmayan) yerel olsaydı, her çağrıda 1 olurdu.
  readSensor(); // #1
  readSensor(); // #2
  readSensor(); // #3
  std::cout << std::endl;

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // 4. RECURSION DEMOS / ÖZYİNELEME GÖSTERİMLERİ
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  std::cout << "--- 4. Recursion ---" << std::endl;

  // EN: FACTORIAL — each call multiplies n by the result of factorial(n-1).
  //     Call stack for factorial(5):
  //         factorial(5) waits for factorial(4)
  //         factorial(4) waits for factorial(3)
  //         factorial(3) waits for factorial(2)
  //         factorial(2) waits for factorial(1)
  //         factorial(1) → returns 1 (BASE CASE HIT!)
  //     Then unwinding: 1 → 2×1=2 → 3×2=6 → 4×6=24 → 5×24=120
  // TR: FAKTÖRİYEL — her çağrı n'i factorial(n-1) sonucuyla çarpar.
  //     factorial(5) için çağrı yığını:
  //         factorial(5) factorial(4)'ü bekler
  //         factorial(4) factorial(3)'ü bekler
  //         ... → factorial(1) → 1 döner (TEMEL DURUM!)
  //     Sonra geri sarma: 1 → 2×1=2 → 3×2=6 → 4×6=24 → 5×24=120
  std::cout << "factorial(5) = " << factorial(5) << std::endl;   // 120
  std::cout << "factorial(10) = " << factorial(10) << std::endl; // 3628800

  // EN: FIBONACCI — demonstrates exponential blowup of naive recursion.
  //     fibonacci(10) = 55, but it takes 177 function calls to compute.
  //     fibonacci(30) would take over 2 MILLION calls. In production code,
  //     NEVER use this naive approach — use iteration or memoization.
  // TR: FIBONACCI — naif özyinelemenin üssel patlamasını gösterir.
  //     fibonacci(10) = 55, ama hesaplamak 177 fonksiyon çağrısı sürer.
  //     fibonacci(30) 2 MİLYONDAN fazla çağrı yapar. Üretim kodunda
  //     bu naif yaklaşımı ASLA kullanmayın — iterasyon veya memoization kullanın.
  std::cout << "fibonacci(6) = " << fibonacci(6) << std::endl;   // 8
  std::cout << "fibonacci(10) = " << fibonacci(10) << std::endl; // 55

  // EN: COUNTDOWN — practical recursion: each call prints n, then calls (n-1).
  //     This is a TAIL RECURSIVE pattern because the recursive call is the
  //     absolute last thing the function does (no computation after it returns).
  // TR: GERİ SAYIM — pratik özyineleme: her çağrı n'i yazdırır, sonra (n-1) çağırır.
  //     Bu KUYRUK ÖZYİNELEME kalıbıdır çünkü özyinelemeli çağrı fonksiyonun
  //     yaptığı mutlak SON şeydir (döndükten sonra hesaplama yok).
  std::cout << "\nECU shutdown sequence:" << std::endl;
  shutdownCountdown(3);

  return 0;
}
