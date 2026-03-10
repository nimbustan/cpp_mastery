/**
 * @file module_08_core_guidelines/01_philosophy_and_interfaces.cpp
 * @brief C++ Core Guidelines: Philosophy & Interfaces / C++ Çekirdek Kuralları: Felsefe ve
 * Arayüzler
 *
 * @details
 * =============================================================================
 * [INTRODUCTION: What are C++ Core Guidelines? / GİRİŞ: C++ Core Guidelines Nedir?]
 * =============================================================================
 * EN: The "C++ Core Guidelines" are a set of tried-and-true rules about coding in C++. Written
 * by Bjarne Stroustrup (creator of C++) and Herb Sutter, they aim to help developers write MUCH
 * Safer, Cleaner, and Faster modern C++.
 *
 * TR: "C++ Core Guidelines", C++'ın yaratıcısı Bjarne Stroustrup ve ISO C++ Komitesi tarafından
 * yazılan, Modern C++'ın nasıl DOĞRU, GÜVENLİ ve PERFORMANSLI yazılacağını anlatan resmi bir
 * anayasadır. Sektördeki en iyi Mimarlar bu kurallara uyar.
 *
 * =============================================================================
 * [P.1: Express ideas directly in code / P.1: Fikirlerinizi Koda Doğrudan Dökün]
 * =============================================================================
 * EN: Compilers don't read comments. If you have to write a comment to explain WHAT a loop does,
 * name your variables better or use Standard Library (STL) algorithms.
 *
 * TR: Derleyiciler yorum satırlarını (comments) okumazlar. Bir döngünün NE yaptığını anlatmak
 * için satırlarca yoruma ihtiyaç duyuyorsanız, kodun iskeleti kötüdür. Standart Kütüphane (STL)
 * metodlarını (Örn: std::find_if) kullanın. İşin NİYETİNİ belli edin!
 *
 * =============================================================================
 * [P.4 & P.5: Compile-Time Checking / Derleme Zamanı Kontrolleri]
 * =============================================================================
 * EN: "Prefer compile-time checking to run-time checking." If you can calculate or check a rule
 * during COMPILATION, do it! Use `constexpr` and `static_assert()`. A bug caught at compile-time
 * costs $0. At runtime, it causes a crash.
 *
 * TR: "Derleme zamanını Çalışma zamanına (Runtime) tercih edin." Eğer bir değeri oyun çalışmadan
 * (Build alırken) absolut hesaplayabiliyorsan, `constexpr` ve `static_assert()` kullan!
 * Derlemede yakalanan bir hatanın maliyeti SIFIRDIR, çalışırken yakalanan bir hata programı
 * (veya sunucuyu) çökertir.
 *
 * [CPPREF DEPTH: Type Safety as Zero-Cost Abstraction — Strong Typedefs / CPPREF DERİNLİK: Sıfır
 * Maliyetli Soyutlama Olarak Tip Güvenliği — Güçlü Typedef'ler]
 * =============================================================================
 * EN: C++ Core Guidelines I.4: "Make interfaces precisely and strongly typed". Primitive
 * obsession — using bare `int` for meters, seconds, and IDs — lets the compiler silently accept
 * nonsensical expressions like `meters + seconds`. Strong typedefs (a thin wrapper class with
 * explicit constructors) enforce correctness at ZERO runtime cost because the optimizer elides
 * the wrapper entirely. `enum class` prevents implicit conversions between unrelated
 * enumerations. `[[nodiscard]]` forces callers to use (not ignore) a return value — essential
 * for error codes. In C++20, Concepts formalize interface contracts: `template<Sortable T>`
 * replaces unnamed SFINAE with readable, diagnosable constraints.
 *
 * TR: C++ Çekirdek Kılavuzu I.4: "Arayüzleri kesin ve güçlü tipli yapın". İlkel tip takıntısı —
 * metre, saniye ve kimlik için çıplak `int` kullanmak — derleyicinin `metre + saniye` gibi
 * anlamsız ifadeleri sessizce kabul etmesine yol açar. Güçlü typedef'ler (açık kuruculu ince
 * sarmalayıcı sınıf) SIFIR çalışma zamanı maliyetiyle doğruluğu zorlar çünkü optimizer
 * sarmalayıcıyı tamamen eler. `enum class` ilgisiz numaralandırmalar arası örtük dönüşümü
 * engeller. `[[nodiscard]]` çağıranı dönüş değerini kullanmaya zorlar. C++20'de Kavramlar arayüz
 * sözleşmelerini resmileştirir.
 *
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <algorithm> // for std::find_if
#include <iostream>
#include <memory>
#include <string>
#include <vector>

// -------------------------------------------------------------------------------------------------
// [P.1] Express Intent (Kötü vs İyi Örnek)
// -------------------------------------------------------------------------------------------------
class ECU {
public:
  std::string name;
  bool isOnline = true;
};

void checkECUStatus_Bad(const std::vector<ECU> &ecus, const std::string &targetName) {
  // BAD (Kötü): C-Style manual loop. Hard to catch the intent at a glance.
  // TR: KÖTÜ: Geleneksel döngü. Okuması ve arkasındaki "Niyeti" anlamak vakit kaybettirir.
  bool found = false;
  for (size_t i = 0; i < ecus.size(); ++i) {
    if (ecus[i].name == targetName) {
      found = true;
      break;
    }
  }
  (void)found; // EN: Suppress — intentional bad example
}

void checkECUStatus_Good(const std::vector<ECU> &ecus, const std::string &targetName) {
  // GOOD (İyi - C++ Core Guidelines): Use STL algorithm! Intent is 100% clear immediately: "Find
  // if any ECU matches".
  // TR: İYİ: STL Algoritması kullanımı. Niyet çok açıktır: "Bir Koşula Göre Ara ve Bul
  // (find_if)".
  auto it = std::find_if(ecus.begin(), ecus.end(),
      [&](const ECU &e) { return e.name == targetName; });

  if (it != ecus.end()) {
    std::cout << "[Intent/Niyet: STL] ECU '" << targetName << "' found!" << std::endl;
  }
}

// -------------------------------------------------------------------------------------------------
// [P.4 & P.5] Compile-Time Checking (`constexpr` and `static_assert`)
// -------------------------------------------------------------------------------------------------
// EN: Because of `constexpr`, compiler calculates this during BUILD time (0 CPU cost at
// runtime). TR: `constexpr` sayesinde CPU bu fonksiyonun hesabını BUILD (Derleme) anında
// tamamlar.
constexpr int calculateMaxSensorChannels(int baseChannels, int expansionModules) {
  return baseChannels + (expansionModules * 2);
}

int main() {
  std::cout << "=== MODULE 8: C++ CORE GUIDELINES - PHILOSOPHY & INTERFACES ===\n" << std::endl;

  // 1. Express Intent demonstration
  std::vector<ECU> vehicleECUs = {
      {"BCM", true}, {"Powertrain", false}, {"HVAC", true}};
  checkECUStatus_Good(vehicleECUs, "Powertrain");

  // 2. Compile-Time Magic
  // EN: This is NOT calculated at runtime! Replaced directly with `20` in the ASM binary. TR: Bu
  // işlem çalışırken (Runtime) YAPILMAZ! Derleyici satırı Assembly kodunda doğrudan `20` yapar.
  constexpr int maxChannels = calculateMaxSensorChannels(10, 5);

  // EN: static_assert checks rules ALWAYS DURING COMPILE. If false -> Red Compiler Error! TR:
  // static_assert kuralları DERLEME AŞAMASINDA kontrol eder. Yanlışsa IDE direkt kırmızıyı
  // basar, Build alınmaz.
  static_assert(maxChannels >= 10, "[FATAL ERROR] Sensor channels cannot be less than 10!");

  std::cout << "\nMax Sensor Channels (Calculated purely at Compile Time!): " << maxChannels <<
      std::endl;

  std::cout << "\n[ARCHITECTURAL CONCLUSION / MİMARİ SONUÇ]:" << std::endl;
  std::cout << "- EN: Write code for the COMPILER to optimize and HUMANS to read!" << std::endl;
  std::cout << "- TR: İnsanların rahat okuyacağı (STL kullanarak niyet belli eden), "
         "Derleyicinin uçuracağı (constexpr) mimariler kurun!"
      << std::endl;

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // EN: I.4 — Make interfaces precisely and strongly typed
  // TR: I.4 — Arayüzleri kesin ve güçlü-tipli yapın
  // ═══════════════════════════════════════════════════════════════════════════════════════════════

  // ─── 1. BAD: Weakly typed interface ────────────────────────────────────────────────────────────
  // EN: What does 'unit' mean? 0 = KPH? 1 = MPH? Magic numbers everywhere.
  // TR: 'unit' ne demek? 0 = KPH mi? 1 = MPH mi? Anlaşılmaz büyü sayıları.
  //
  // void setSpeed(int speed, int unit);   // BAD — caller can pass anything setSpeed(120, 3);   
  // // What is 3? Compiles fine!

  // ─── 2. GOOD: Strongly typed interface ─────────────────────────────────────────────────────────
  // EN: Type-safe enum makes invalid calls impossible at compile time.
  // TR: Tip-güvenli enum, geçersiz çağrıları derleme zamanında engeller.
  enum class SpeedUnit { KPH, MPH };

  auto setSpeed = [](double speed, SpeedUnit unit) {
    const char* label = (unit == SpeedUnit::KPH) ? "km/h" : "mph";
    std::cout << "[I.4 GOOD] Speed set to " << speed << " " << label << std::endl;
  };

  setSpeed(120.0, SpeedUnit::KPH);
  // setSpeed(120.0, 2);  // EN: Won't compile! Type safety enforced.
                          // TR: Derlenmez! Tip güvenliği sağlandı.

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // EN: I.11 — Never transfer ownership by a raw pointer
  // TR: I.11 — Ham işaretçi ile sahiplik aktarımı ASLA yapılmaz
  // ═══════════════════════════════════════════════════════════════════════════════════════════════

  // ─── 3. BAD: Raw pointer ownership transfer ────────────────────────────────────────────────────
  // EN: Who owns this pointer? Caller? Callee? Nobody knows → leak.
  // TR: Bu işaretçinin sahibi kim? Çağıran mı? Çağrılan mı? → sızıntı.
  //
  // ECU* createECU() { return new ECU{"Raw", true}; }  // BAD

  // ─── 4. GOOD: unique_ptr ownership transfer ────────────────────────────────────────────────────
  // EN: Ownership is explicit. unique_ptr is the ONLY owner, auto-deletes.
  // TR: Sahiplik açıkça bellidir. unique_ptr TEK sahiptir, otomatik siler.
  auto createECU_Safe = []() -> std::unique_ptr<ECU> {
    return std::make_unique<ECU>(ECU{"SmartECU", true});
  };

  auto safeEcu = createECU_Safe();
  std::cout << "[I.11 GOOD] Owned ECU: " << safeEcu->name
            << " (unique_ptr, no leak possible)" << std::endl;

  return 0;
}
