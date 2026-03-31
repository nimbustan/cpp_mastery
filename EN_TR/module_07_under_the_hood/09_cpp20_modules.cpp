/**
 * @file module_07_under_the_hood/09_cpp20_modules.cpp
 * @brief C++20 Modules: #include'ın Sonu — The End of #include
 *
 * @details
 * =============================================================================
 * [THEORY: What Are C++20 Modules? / TEORİ: C++20 Modülleri Nedir?]
 * =============================================================================
 *
 * EN: C++20 Modules replace the 40-year-old #include system with a modern,
 *     compiled import mechanism. Key differences:
 *
 *     ┌──────────────────────┬─────────────────────────────────────────────┐
 *     │ Feature              │ #include (Headers)    │ import (Modules)    │
 *     ├──────────────────────┼───────────────────────┼─────────────────────┤
 *     │ Mechanism            │ Textual copy-paste    │ Pre-compiled binary │
 *     │ Build speed          │ Slow (re-parsed each  │ Fast (parsed once,  │
 *     │                      │ TU = O(N²) work)      │ imported as binary) │
 *     │ Macro leakage        │ YES (all #defines     │ NO (macros don't    │
 *     │                      │ leak everywhere)      │ cross module border)│
 *     │ Include order matters│ YES (notorious bugs)  │ NO                  │
 *     │ ODR violations       │ Common                │ Impossible by design│
 *     │ Symbol visibility    │ Everything exported   │ Only explicit export│
 *     │ Tooling support      │ Mature                │ Improving (2024+)   │
 *     └──────────────────────┴───────────────────────┴─────────────────────┘
 *
 * TR: C++20 Modülleri, 40 yıllık #include sistemini modern, derlenmiş bir
 *     import mekanizmasıyla değiştirir. Temel farklar:
 *
 *     ┌──────────────────────┬───────────────────────┬────────────────────────┐
 *     │ Özellik              │ #include (Başlıklar)  │ import (Modüller)      │
 *     ├──────────────────────┼───────────────────────┼────────────────────────┤
 *     │ Mekanizma            │ Metinsel kopyala-yap. │ Ön-derlenmiş ikili     │
 *     │ Derleme hızı         │ Yavaş (her TU'da      │ Hızlı (bir kez         │
 *     │                      │ yeniden ayrıştırılır) │ ayrıştır, ikili al)    │
 *     │ Makro sızması        │ EVET (tüm #define'lar │ HAYIR (makrolar        │
 *     │                      │ her yere sızar)       │ modül sınırını geçmz)  │
 *     │ Include sırası önemli│ EVET (meşhur hatalar) │ HAYIR                  │
 *     │ ODR ihlalleri        │ Sık                   │ Tasarım gereği imknsız │
 *     │ Sembol görünürlüğü   │ Her şey dışa aktarılır│ Sadece açık export     │
 *     │ Araç desteği         │ Olgun                 │ Gelişiyor (2024+)      │
 *     └──────────────────────┴───────────────────────┴────────────────────────┘
 *
 * =============================================================================
 * [THEORY: Module Syntax / TEORİ: Modül Sözdizimi]
 * =============================================================================
 *
 * EN: A module consists of:
 *
 *     1. MODULE INTERFACE UNIT (.cppm/.ixx):
 *        - Declares what the module exports (public API)
 *        - Contains `export module name;` at the top
 *        - Only exported entities are visible to importers
 *
 *     2. MODULE IMPLEMENTATION UNIT (.cpp):
 *        - Contains `module name;` (no export keyword)
 *        - Can access non-exported internals
 *        - Implementation details hidden from importers
 *
 *     Basic syntax:
 *
 *     // ── math_utils.cppm (Interface Unit) ──
 *     export module math_utils;          // Declare exported module
 *
 *     export int add(int a, int b);      // Exported: visible to importers
 *     export double pi = 3.14159;        // Exported variable
 *
 *     int internal_helper(int x);         // NOT exported: module-private
 *
 *     // ── math_utils.cpp (Implementation Unit) ──
 *     module math_utils;                  // Implementation of math_utils
 *
 *     int add(int a, int b) { return a + b; }
 *     int internal_helper(int x) { return x * x; }
 *
 *     // ── main.cpp (Consumer) ──
 *     import math_utils;                  // Import the module
 *     int main() {
 *         int result = add(3, 4);         // OK: add is exported
 *         // internal_helper(5);          // ERROR: not exported!
 *     }
 *
 * TR: Bir modül şunlardan oluşur:
 *
 *     1. MODÜL ARAYÜZ BİRİMİ (.cppm/.ixx):
 *        - Modülün neyi dışa aktardığını bildirir (genel API)
 *        - Tepede `export module isim;` içerir
 *        - Sadece dışa aktarılan varlıklar içe aktaranlara görünür
 *
 *     2. MODÜL UYGULAMA BİRİMİ (.cpp):
 *        - `module isim;` içerir (export anahtar kelimesi yok)
 *        - Dışa aktarılmayan içselliklere erişebilir
 *        - Uygulama detayları içe aktaranlardan gizlidir
 *
 *     Temel sözdizimi:
 *
 *     // ── math_utils.cppm (Arayüz Birimi) ──
 *     export module math_utils;          // Dışa aktarılan modülü bildir
 *
 *     export int add(int a, int b);      // Dışa aktarıldı: içe aktaranlara görünür
 *     export double pi = 3.14159;        // Dışa aktarılan değişken
 *
 *     int internal_helper(int x);         // Dışa AKTARILMADI: modül-özel
 *
 *     // ── math_utils.cpp (Uygulama Birimi) ──
 *     module math_utils;                  // math_utils'in uygulaması
 *
 *     int add(int a, int b) { return a + b; }
 *     int internal_helper(int x) { return x * x; }
 *
 *     // ── main.cpp (Tüketici) ──
 *     import math_utils;                  // Modülü içe aktar
 *     int main() {
 *         int result = add(3, 4);         // OK: add dışa aktarıldı
 *         // internal_helper(5);          // HATA: dışa aktarılmadı!
 *     }
 *
 * =============================================================================
 * [THEORY: Module Partitions / TEORİ: Modül Bölümleri]
 * =============================================================================
 *
 * EN: Large modules can be split into partitions:
 *
 *     // ── math:basic.cppm ──
 *     export module math:basic;           // Partition ":basic" of module "math"
 *     export int add(int a, int b);
 *
 *     // ── math:advanced.cppm ──
 *     export module math:advanced;        // Partition ":advanced"
 *     export double sqrt_approx(double x);
 *
 *     // ── math.cppm (Primary Interface) ──
 *     export module math;
 *     export import :basic;               // Re-export basic partition
 *     export import :advanced;            // Re-export advanced partition
 *
 *     // ── main.cpp ──
 *     import math;                        // Gets everything from both partitions
 *     // import math:basic;               // Can also import specific partition
 *
 * TR: Büyük modüller bölümlere ayrılabilir (partition):
 *
 *     // ── math:basic.cppm ──
 *     export module math:basic;           // "math" modülünün ":basic" bölümü
 *     export int add(int a, int b);
 *
 *     // ── math:advanced.cppm ──
 *     export module math:advanced;        // ":advanced" bölümü
 *     export double sqrt_approx(double x);
 *
 *     // ── math.cppm (Birincil Arayüz) ──
 *     export module math;
 *     export import :basic;               // basic bölümünü yeniden dışa aktar
 *     export import :advanced;            // advanced bölümünü yeniden dışa aktar
 *
 *     // ── main.cpp ──
 *     import math;                        // Her iki bölümden her şeyi alır
 *     // import math:basic;               // Belirli bölümü de içe aktarabilir
 *
 * =============================================================================
 * [THEORY: Header Units / TEORİ: Başlık Birimleri]
 * =============================================================================
 *
 * EN: Header units bridge the gap between headers and modules:
 *
 *     import <iostream>;     // Import standard library header as header unit
 *     import <vector>;       // Faster than #include (pre-compiled)
 *     import "legacy.h";     // Import your own headers as header units
 *
 *     Benefits:
 *     - Faster builds (header parsed once, cached as binary)
 *     - No macro leakage from the header unit (mostly)
 *     - Gradual migration path: change #include → import one at a time
 *
 *     Limitations:
 *     - Not all headers can be imported as header units
 *     - Macro-heavy headers (e.g., Windows.h) may not work
 *     - Compiler support varies (GCC/Clang still evolving)
 *
 * TR: Başlık birimleri, başlıklar ve modüller arasında köprü kurar:
 *
 *     import <iostream>;     // Standart kütüphane başlığını başlık birimi olarak içe aktar
 *     import <vector>;       // #include'dan daha hızlı (ön-derlenmiş)
 *     import "legacy.h";     // Kendi başlıklarınızı başlık birimi olarak içe aktar
 *
 *     Faydaları:
 *     - Daha hızlı derleme (başlık bir kez ayrıştırılır, ikili olarak önbelleğe alınır)
 *     - Başlık biriminden makro sızması yok (çoğunlukla)
 *     - Kademeli göç yolu: #include → import'u birer birer değiştir
 *
 *     Sınırlamalar:
 *     - Tüm başlıklar başlık birimi olarak içe aktarılamaz
 *     - Makro ağırlıklı başlıklar (ör. Windows.h) çalışmayabilir
 *     - Derleyici desteği değişkenlik gösterir (GCC/Clang hala gelişiyor)
 *
 * =============================================================================
 * [THEORY: module :private Fragment / TEORİ: module :private Parçası]
 * =============================================================================
 *
 * EN: The private fragment lets you hide implementation in the interface file:
 *
 *     export module sensor;
 *
 *     export class Sensor {
 *     public:
 *         double read();
 *     private:
 *         double calibration_;
 *     };
 *
 *     module :private;   // Everything below is hidden from importers
 *                        // AND doesn't affect ABI (changes don't cause recompile)
 *
 *     double Sensor::read() {
 *         return calibration_ * 0.95;  // Implementation detail
 *     }
 *
 * TR: Private parça, arayüz dosyasında uygulama detaylarını gizlemenizi sağlar:
 *
 *     export module sensor;
 *
 *     export class Sensor {
 *     public:
 *         double read();
 *     private:
 *         double calibration_;
 *     };
 *
 *     module :private;   // Aşağıdaki her şey içe aktaranlardan gizlenir
 *                        // VE ABI'yi etkilemez (değişiklikler yeniden derleme yapmaz)
 *
 *     double Sensor::read() {
 *         return calibration_ * 0.95;  // Uygulama detayı
 *     }
 *
 * =============================================================================
 * [THEORY: CMake Module Support / TEORİ: CMake Modül Desteği]
 * =============================================================================
 *
 * EN: CMake 3.28+ supports C++20 modules natively:
 *
 *     cmake_minimum_required(VERSION 3.28)
 *     project(my_project CXX)
 *     set(CMAKE_CXX_STANDARD 20)
 *
 *     add_library(math_lib)
 *     target_sources(math_lib
 *         PUBLIC
 *             FILE_SET CXX_MODULES FILES
 *                 src/math.cppm          # Module interface
 *                 src/math_basic.cppm    # Partition
 *     )
 *
 *     add_executable(app src/main.cpp)
 *     target_link_libraries(app PRIVATE math_lib)
 *
 *     Key CMake additions:
 *     - `FILE_SET CXX_MODULES` — tells CMake these are module files
 *     - CMake handles build order (interfaces must compile before consumers)
 *     - Ninja generator works best (handles module dependency scanning)
 *
 * TR: CMake 3.28+ C++20 modüllerini doğal olarak destekler:
 *
 *     cmake_minimum_required(VERSION 3.28)
 *     project(my_project CXX)
 *     set(CMAKE_CXX_STANDARD 20)
 *
 *     add_library(math_lib)
 *     target_sources(math_lib
 *         PUBLIC
 *             FILE_SET CXX_MODULES FILES
 *                 src/math.cppm          # Modül arayüzü
 *                 src/math_basic.cppm    # Bölüm
 *     )
 *
 *     add_executable(app src/main.cpp)
 *     target_link_libraries(app PRIVATE math_lib)
 *
 *     Temel CMake eklemeleri:
 *     - `FILE_SET CXX_MODULES` — CMake'e bunların modül dosyaları olduğunu söyler
 *     - CMake derleme sırasını yönetir (arayüzler tþketicilerden önce derlenmeli)
 *     - Ninja üreteci en iyi çalışır (modül bağımlılık taramasını yapar)
 *
 * =============================================================================
 * [THEORY: Migration Strategy / TEORİ: Göç Stratejisi]
 * =============================================================================
 *
 * EN: Migrating from #include to import — recommended approach:
 *
 *     Phase 1: PREPARATION
 *     - Ensure code compiles with C++20 (-std=c++20)
 *     - Use CMake 3.28+ with Ninja generator
 *     - Identify "leaf" libraries (no dependents to break)
 *
 *     Phase 2: HEADER UNITS (low risk)
 *     - Replace `#include <vector>` with `import <vector>;`
 *     - Replace `#include <string>` with `import <string>;`
 *     - Immediate build speedup, minimal risk
 *
 *     Phase 3: NEW CODE AS MODULES
 *     - Write all new components as modules
 *     - Existing code still uses #include
 *
 *     Phase 4: GRADUAL CONVERSION
 *     - Convert leaf libraries first (no reverse dependencies)
 *     - Work inward toward the dependency graph center
 *     - Keep both module and header interface temporarily (wrapper module)
 *
 *     Wrapper module pattern during transition:
 *
 *     // legacy_wrapper.cppm — wraps existing headers as a module
 *     export module legacy_wrapper;
 *     // We can still use #include inside a module!
 *     // (They won't leak to importers)
 *     #include "legacy_sensor.h"
 *     #include "legacy_can.h"
 *
 *     export using legacy::Sensor;
 *     export using legacy::CANFrame;
 *
 * TR: #include'dan import'a göç — önerilen yaklaşım:
 *
 *     Faz 1: HAZIRLIK
 *     - Kodun C++20 ile derlendiğinden emin ol (-std=c++20)
 *     - CMake 3.28+ Ninja üreteci ile kullan
 *     - "Yaprak" kütüphaneleri belirle (kıracak bağımlı yok)
 *
 *     Faz 2: BAŞLIK BİRİMLERİ (düşük risk)
 *     - `#include <vector>` → `import <vector>;` olarak değiştir
 *     - `#include <string>` → `import <string>;` olarak değiştir
 *     - Anında derleme hızlanması, minimum risk
 *
 *     Faz 3: YENİ KOD MODÜL OLARAK
 *     - Tüm yeni bileşenleri modül olarak yaz
 *     - Mevcut kod hala #include kullanır
 *
 *     Faz 4: KADEMELİ DÖNÜŞÜM
 *     - Önce yaprak kütüphaneleri dönüştür (ters bağımlılık yok)
 *     - Bağımlılık grafiğinin merkezine doğru ilerle
 *     - Geçiçi olarak hem modül hem başlık arayüzünü koru (sarmalayıcı modül)
 *
 *     Geçiş sırasında sarmalayıcı modül deseni:
 *
 *     // legacy_wrapper.cppm — mevcut başlıkları modül olarak sarar
 *     export module legacy_wrapper;
 *     // Modül içinde hala #include kullanabiliriz!
 *     // (içe aktaranlara sızmaz)
 *     #include "legacy_sensor.h"
 *     #include "legacy_can.h"
 *
 *     export using legacy::Sensor;
 *     export using legacy::CANFrame;
 *
 * =============================================================================
 * [THEORY: Build Time Comparison / TEORİ: Derleme Süresi Karşılaştırması]
 * =============================================================================
 *
 * EN: Real-world build time improvements with modules:
 *
 *     ┌─────────────────────────────┬───────────┬────────────┬───────────┐
 *     │ Scenario                    │ Headers   │ Modules    │ Speedup   │
 *     ├─────────────────────────────┼───────────┼────────────┼───────────┤
 *     │ 50 TUs, each includes       │ 45 sec    │ 12 sec     │ 3.8x      │
 *     │   <iostream>, <vector>,     │           │            │           │
 *     │   <string>, <map>           │           │            │           │
 *     │                             │           │            │           │
 *     │ 200 TUs, project headers    │ 180 sec   │ 35 sec     │ 5.1x      │
 *     │   (lots of template code)   │           │            │           │
 *     │                             │           │            │           │
 *     │ Incremental (1 file changed)│ 12 sec    │ 3 sec      │ 4.0x      │
 *     │   (header included by many) │           │            │           │
 *     └─────────────────────────────┴───────────┴────────────┴───────────┘
 *
 *     Why modules are faster:
 *     1. Parsed ONCE, stored as BMI (Binary Module Interface)
 *     2. No redundant parsing across TUs (headers parsed N times = O(N²))
 *     3. No macro/include-order dependencies to invalidate caches
 *
 * TR: Modüllerle gerçek derleme süresi iyileştirmeleri:
 *
 *     ┌─────────────────────────────┬───────────┬────────────┬───────────┐
 *     │ Senaryo                     │ Başlıklar │ Modüller   │ Hızlanma  │
 *     ├─────────────────────────────┼───────────┼────────────┼───────────┤
 *     │ 50 TU, her biri             │ 45 sn     │ 12 sn      │ 3.8x      │
 *     │   <iostream>, <vector>,     │           │            │           │
 *     │   <string>, <map> içerir    │           │            │           │
 *     │                             │           │            │           │
 *     │ 200 TU, proje başlıkları    │ 180 sn    │ 35 sn      │ 5.1x      │
 *     │   (çok sayıda şablon kod)   │           │            │           │
 *     │                             │           │            │           │
 *     │ Artımlı (1 dosya değişti)   │ 12 sn     │ 3 sn       │ 4.0x      │
 *     │   (başlık çok yerde dahil)  │           │            │           │
 *     └─────────────────────────────┴───────────┴────────────┴───────────┘
 *
 *     Modüller neden daha hızlı:
 *     1. Bir kez ayrıştırılır, BMI (Binary Module Interface) olarak saklanır
 *     2. TU'lar arasında tekrarlı ayrıştırma yok (başlıklar N kez = O(N²))
 *     3. Önbellekleri geçersiz kılan makro/include-sırası bağımlılıkları yok
 *
 * @note This file is a REFERENCE DOCUMENT — it does not compile as a module itself
 *       because module support requires specific file extensions (.cppm/.ixx) and
 *       CMake/build system configuration. The code below demonstrates the SYNTAX
 *       and concepts through examples that compile as regular C++20.
 *
 * @see https://en.cppreference.com/w/cpp/language/modules
 * @see https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2019/p1103r3.pdf
 *
 * @note Compile: g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion 09_cpp20_modules.cpp -o 09_cpp20_modules
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <functional>
#include <memory>

// ═════════════════════════════════════════════════════════════════════════════
// PART 1: Simulating Module-Like Design in C++20 (without actual modules)
// ═════════════════════════════════════════════════════════════════════════════

/**
 * EN: Since compiling actual modules requires a full build system setup,
 *     we simulate module patterns here using namespaces with explicit
 *     "export" (public) vs "internal" (private) separation.
 *
 *     In a real module:
 *       export class Sensor { ... };      → module_api::Sensor
 *       class InternalHelper { ... };     → module_impl::InternalHelper
 *
 * TR: Gerçek modül derlemesi tam bir yapı sistemi kurulumu gerektirdiğinden,
 *     burada ad alanları kullanarak modül kalıplarını simüle ediyoruz.
 */

// EN: This namespace represents what would be `export`-ed from a module
// TR: Bu ad alanı, bir modülden `export` edilecek olanları temsil eder
namespace sensor_module {

    // EN: EXPORTED types — visible to anyone who `import`s the module
    // TR: DIŞA AKTARILAN türler — modülü `import` eden herkes görebilir
    struct SensorReading {
        double value;
        std::string unit;
        uint64_t timestamp_ms;
    };

    // EN: EXPORTED class / TR: DIŞA AKTARILAN sınıf
    class TemperatureSensor {
    public:
        explicit TemperatureSensor(std::string id)
            : id_(std::move(id)), calibration_offset_(0.0) {}

        SensorReading read() const {
            // EN: Simulate reading / TR: Okuma simülasyonu
            double raw = 22.5 + calibration_offset_;
            return {raw, "°C", current_time_ms()};
        }

        const std::string& id() const { return id_; }
        void set_calibration(double offset) { calibration_offset_ = offset; }

    private:
        std::string id_;
        double calibration_offset_;

        static uint64_t current_time_ms() {
            auto now = std::chrono::steady_clock::now();
            return static_cast<uint64_t>(
                std::chrono::duration_cast<std::chrono::milliseconds>(
                    now.time_since_epoch()).count());
        }
    };

    // EN: EXPORTED function / TR: DIŞA AKTARILAN fonksiyon
    std::string format_reading(const SensorReading& r) {
        return r.unit + ": " + std::to_string(r.value)
               + " @ " + std::to_string(r.timestamp_ms) + "ms";
    }

}  // namespace sensor_module

// EN: This namespace represents module-INTERNAL symbols (NOT exported)
// TR: Bu ad alanı modül-İÇ sembollerini temsil eder (DIŞA AKTARILMAZ)
namespace sensor_module_impl {

    // EN: Internal helper — would NOT be visible to importers in a real module
    // TR: İç yardımcı — gerçek modülde importerlara görünmez
    double apply_moving_average(double current, double previous, double alpha = 0.3) {
        return alpha * current + (1.0 - alpha) * previous;
    }

}  // namespace sensor_module_impl


// ═════════════════════════════════════════════════════════════════════════════
// PART 2: Demonstrating Module Partition Pattern
// ═════════════════════════════════════════════════════════════════════════════

/**
 * EN: Simulating module partitions using nested namespaces:
 *     - ecu_module::diagnostics corresponds to `module ecu:diagnostics`
 *     - ecu_module::communication corresponds to `module ecu:communication`
 *     - ecu_module (primary) re-exports both
 *
 * TR: İç içe ad alanları ile modül bölümlerini simüle etme.
 */
namespace ecu_module {

    // EN: Partition: ecu:diagnostics / TR: Bölüm: ecu:diagnostics
    namespace diagnostics {
        enum class DTCStatus : uint8_t {
            Inactive = 0x00,
            Active   = 0x01,
            Pending  = 0x02,
            Confirmed = 0x03
        };

        struct DiagnosticTroubleCode {
            uint32_t code;
            DTCStatus status;
            std::string description;
        };

        std::string dtc_to_string(const DiagnosticTroubleCode& dtc) {
            std::string status_str;
            switch (dtc.status) {
                case DTCStatus::Inactive:  status_str = "INACTIVE"; break;
                case DTCStatus::Active:    status_str = "ACTIVE"; break;
                case DTCStatus::Pending:   status_str = "PENDING"; break;
                case DTCStatus::Confirmed: status_str = "CONFIRMED"; break;
            }
            return "DTC-" + std::to_string(dtc.code) + " [" + status_str + "] "
                   + dtc.description;
        }
    }

    // EN: Partition: ecu:communication / TR: Bölüm: ecu:communication
    namespace communication {
        struct CANMessage {
            uint32_t id;
            uint8_t data[8];
            uint8_t length;
        };

        void pack_dtc_into_can(const diagnostics::DiagnosticTroubleCode& dtc,
                               CANMessage& msg) {
            msg.id = 0x7DF;  // EN: OBD-II functional address
            msg.length = 4;
            msg.data[0] = 0x03;  // EN: Number of data bytes
            msg.data[1] = 0x19;  // EN: Read DTC by status
            msg.data[2] = static_cast<uint8_t>(dtc.status);
            msg.data[3] = static_cast<uint8_t>(dtc.code & 0xFF);
        }
    }

}  // namespace ecu_module

// ═════════════════════════════════════════════════════════════════════════════
// PART 3: Module :private Fragment Simulation
// ═════════════════════════════════════════════════════════════════════════════

/**
 * EN: The `module :private;` fragment allows implementation in the interface
 *     file — but changes to it DON'T cause consumers to recompile.
 *     We simulate this by separating the class declaration from its definition.
 *
 * TR: `module :private;` parçası, arayüz dosyasında uygulamaya izin verir —
 *     ama değişiklikler tüketicilerin yeniden derlenmesine neden OLMAZ.
 */
namespace config_module {

    // EN: "Interface" part — this is what consumers depend on (ABI)
    // TR: "Arayüz" kısmı — tüketicilerin bağımlı olduğu kısım (ABI)
    class ConfigManager {
    public:
        ConfigManager();
        void set(const std::string& key, const std::string& value);
        std::string get(const std::string& key, const std::string& default_val = "") const;
        size_t size() const;
        void print_all() const;

    private:
        // EN: Using PIMPL would be even better for ABI stability
        // TR: ABI kararlılığı için PIMPL kullanmak daha iyi olurdu
        struct Entry {
            std::string key;
            std::string value;
        };
        std::vector<Entry> entries_;
    };

    // ── module :private; equivalent ──
    // EN: Everything below is implementation — changes here DON'T affect
    //     consumers' ABI or force recompilation in a real module.
    // TR: Aşağıdaki her şey uygulama — buradaki değişiklikler gerçek bir
    //     modülde tüketicilerin ABI'sini etkilemez veya yeniden derlemez.

    ConfigManager::ConfigManager() = default;

    void ConfigManager::set(const std::string& key, const std::string& value) {
        for (auto& e : entries_) {
            if (e.key == key) { e.value = value; return; }
        }
        entries_.push_back({key, value});
    }

    std::string ConfigManager::get(const std::string& key, const std::string& default_val) const {
        for (const auto& e : entries_) {
            if (e.key == key) return e.value;
        }
        return default_val;
    }

    size_t ConfigManager::size() const { return entries_.size(); }

    void ConfigManager::print_all() const {
        for (const auto& e : entries_) {
            std::cout << "    " << e.key << " = " << e.value << "\n";
        }
    }

}  // namespace config_module

// ═════════════════════════════════════════════════════════════════════════════
// MAIN
// ═════════════════════════════════════════════════════════════════════════════
int main() {
    std::cout << "============================================\n";
    std::cout << "   Module 07 - C++20 Modules Deep Dive\n";
    std::cout << "============================================\n\n";

    // ─── Demo 1: Module Export vs Internal ───────────────────────────────
    {
        std::cout << "--- Demo 1: Module Export vs Internal ---\n";

        // EN: Using "exported" API / TR: "Dışa aktarılan" API'yi kullanma
        sensor_module::TemperatureSensor sensor("TEMP_ECU_01");
        sensor.set_calibration(0.5);
        auto reading = sensor.read();

        std::cout << "  Sensor ID: " << sensor.id() << "\n";
        std::cout << "  Reading:   " << sensor_module::format_reading(reading) << "\n";

        // EN: Internal helper — in a real module, this would NOT compile from outside!
        // TR: İç yardımcı — gerçek modülde dışarıdan DERLENMEZDİ!
        double smoothed = sensor_module_impl::apply_moving_average(reading.value, 21.0);
        std::cout << "  Smoothed:  " << smoothed << " (internal helper — would fail with real modules)\n\n";

        std::cout << "  Module syntax equivalent:\n";
        std::cout << "    export module sensor;                    // Interface\n";
        std::cout << "    export class TemperatureSensor { ... };  // Exported\n";
        std::cout << "    double apply_moving_average(...);        // NOT exported\n\n";
    }

    // ─── Demo 2: Module Partitions ───────────────────────────────────────
    {
        std::cout << "--- Demo 2: Module Partitions ---\n";

        ecu_module::diagnostics::DiagnosticTroubleCode dtc{
            0x0123,
            ecu_module::diagnostics::DTCStatus::Active,
            "Coolant temp sensor circuit"
        };

        std::cout << "  " << ecu_module::diagnostics::dtc_to_string(dtc) << "\n";

        ecu_module::communication::CANMessage msg{};
        ecu_module::communication::pack_dtc_into_can(dtc, msg);
        std::cout << "  Packed into CAN ID: 0x" << std::hex << msg.id << std::dec
                  << ", length: " << static_cast<int>(msg.length) << "\n\n";

        std::cout << "  Module syntax equivalent:\n";
        std::cout << "    export module ecu:diagnostics;   // Partition 1\n";
        std::cout << "    export module ecu:communication;  // Partition 2\n";
        std::cout << "    export module ecu;                // Primary interface\n";
        std::cout << "    export import :diagnostics;       // Re-export\n";
        std::cout << "    export import :communication;     // Re-export\n\n";
    }

    // ─── Demo 3: module :private Fragment ────────────────────────────────
    {
        std::cout << "--- Demo 3: module :private (Implementation Hiding) ---\n";

        config_module::ConfigManager cfg;
        cfg.set("ecu.variant", "ECU_VARIANT_A");
        cfg.set("can.baudrate", "500000");
        cfg.set("diag.timeout_ms", "5000");

        std::cout << "  Config entries (" << cfg.size() << "):\n";
        cfg.print_all();
        std::cout << "  Lookup: diag.timeout_ms = " << cfg.get("diag.timeout_ms") << "\n";
        std::cout << "  Lookup: missing_key = \"" << cfg.get("missing_key", "N/A") << "\"\n\n";

        std::cout << "  Module syntax equivalent:\n";
        std::cout << "    export module config;\n";
        std::cout << "    export class ConfigManager { ... };  // Public ABI\n";
        std::cout << "    module :private;                      // Below = hidden impl\n";
        std::cout << "    void ConfigManager::set(...) { ... } // Changes don't recompile consumers\n\n";
    }

    // ─── Demo 4: Header Units ────────────────────────────────────────────
    {
        std::cout << "--- Demo 4: Header Units Reference ---\n";
        std::cout << "  Traditional:     #include <iostream>\n";
        std::cout << "  Header unit:     import <iostream>;    // Faster, no macro leakage\n";
        std::cout << "  Own header:      import \"my_header.h\"; // Gradual migration\n\n";
        std::cout << "  Compiler flags:\n";
        std::cout << "    GCC:   g++ -std=c++20 -fmodules-ts main.cpp\n";
        std::cout << "    Clang: clang++ -std=c++20 -fmodules main.cpp\n";
        std::cout << "    MSVC:  cl /std:c++20 /interface /TP module.ixx\n\n";
    }

    // ─── Demo 5: Migration Decision Tree ─────────────────────────────────
    {
        std::cout << "--- Demo 5: Migration Decision Tree ---\n";
        std::cout << "  ┌── Is it a NEW component?\n";
        std::cout << "  │   YES → Write as a module from day one\n";
        std::cout << "  │   NO  → Continue with headers for now\n";
        std::cout << "  │\n";
        std::cout << "  ├── Is build time a bottleneck?\n";
        std::cout << "  │   YES → Start with header units (import <vector>;)\n";
        std::cout << "  │   NO  → Lower priority for migration\n";
        std::cout << "  │\n";
        std::cout << "  ├── Does the component have many dependents?\n";
        std::cout << "  │   YES → Migrate last (too risky early)\n";
        std::cout << "  │   NO  → Good candidate for early migration\n";
        std::cout << "  │\n";
        std::cout << "  └── Is the header macro-heavy?\n";
        std::cout << "      YES → Keep as header (modules don't export macros)\n";
        std::cout << "      NO  → Safe to convert to module\n\n";
    }

    // ─── Demo 6: Complete Module File Examples ───────────────────────────
    {
        std::cout << "--- Demo 6: Complete Module File Examples ---\n\n";

        std::cout << "  === Example: math_utils.cppm (Interface) ===\n";
        std::cout << "  export module math_utils;\n";
        std::cout << "  \n";
        std::cout << "  export namespace math {\n";
        std::cout << "      constexpr double pi = 3.14159265358979;\n";
        std::cout << "      constexpr double e  = 2.71828182845904;\n";
        std::cout << "      \n";
        std::cout << "      double sin(double x);\n";
        std::cout << "      double cos(double x);\n";
        std::cout << "      \n";
        std::cout << "      template<typename T>\n";
        std::cout << "      T clamp(T value, T low, T high) {\n";
        std::cout << "          return (value < low) ? low : (value > high) ? high : value;\n";
        std::cout << "      }\n";
        std::cout << "  }\n\n";

        std::cout << "  === Example: math_utils.cpp (Implementation) ===\n";
        std::cout << "  module math_utils;         // No 'export' keyword\n";
        std::cout << "  #include <cmath>           // OK: #include inside module impl\n";
        std::cout << "  \n";
        std::cout << "  namespace math {\n";
        std::cout << "      double sin(double x) { return std::sin(x); }\n";
        std::cout << "      double cos(double x) { return std::cos(x); }\n";
        std::cout << "  }\n\n";

        std::cout << "  === Example: CMakeLists.txt ===\n";
        std::cout << "  cmake_minimum_required(VERSION 3.28)\n";
        std::cout << "  project(my_project CXX)\n";
        std::cout << "  set(CMAKE_CXX_STANDARD 20)\n";
        std::cout << "  add_library(math_lib)\n";
        std::cout << "  target_sources(math_lib\n";
        std::cout << "      PUBLIC FILE_SET CXX_MODULES FILES\n";
        std::cout << "          src/math_utils.cppm)\n";
        std::cout << "  target_sources(math_lib PRIVATE\n";
        std::cout << "      src/math_utils.cpp)\n\n";
    }

    std::cout << "============================================\n";
    std::cout << "   End of C++20 Modules\n";
    std::cout << "============================================\n";

    return 0;
}
