/**
 * @file module_08_core_guidelines/07_api_design.cpp
 * @brief API Design in Modern C++ — Modern C++'da API Tasarımı
 *
 * @details
 * =============================================================================
 * [THEORY: Principles of Good C++ API Design / TEORİ: İyi C++ API Tasarımının İlkeleri]
 * =============================================================================
 *
 * EN: A well-designed API should be:
 *     1. EASY TO USE CORRECTLY — intuitive names, strong types
 *     2. HARD TO USE INCORRECTLY — compile-time errors > runtime errors
 *     3. STABLE — ABI/API breaks are documented, versioned
 *     4. MINIMAL — expose only what users need
 *     5. CONSISTENT — follow patterns from std library
 *
 * TR: İyi tasarlanmış bir API şunları sağlamalıdır:
 *     1. DOĞRU KULLANMAK KOLAY — sezgisel isimler, güçlü türler
 *     2. YANLIŞ KULLANMAK ZOR — derleme zamanı hataları > çalışma zamanı hataları
 *     3. KARARLI — ABI/API kırılmaları belgelenir, sürümlenir
 *     4. MİNİMAL — sadece kullanıcıların ihtiyaç duyduğunu aç
 *     5. TUTARLI — std kütüphanesinden kalıpları izle
 *
 * @see C++ Core Guidelines: https://isocpp.github.io/CppCoreGuidelines/
 * @note Compile: g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion 07_api_design.cpp -o 07_api_design
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <iostream>
#include <string>
#include <memory>
#include <stdexcept>
#include <vector>
#include <type_traits>
#include <functional>

// ═════════════════════════════════════════════════════════════════════════════
// PART 1: Strong Types / NamedType Pattern
// ═════════════════════════════════════════════════════════════════════════════

/**
 * EN: PROBLEM: Weak types lead to "primitive obsession" bugs:
 *       void connect(string host, int port, int timeout_ms, int retry_count);
 *       connect("server", 5000, 8080, 3);  // BUG: port and timeout swapped!
 *       Compiles fine but the bug is silent. The types are too weak.
 *
 *     SOLUTION: Strong types (NamedType) wrap primitives with distinct types:
 *       connect(Host{"server"}, Port{8080}, Timeout{5000ms}, RetryCount{3});
 *       connect(Host{"server"}, Timeout{5000ms}, Port{8080}, RetryCount{3});
 *       // ↑ COMPILE ERROR: Port and Timeout are different types!
 *
 * TR: SORUN: Zayıf türler sessiz hatalara yol açar (port ve timeout karıştırılır).
 *     ÇÖZÜM: Güçlü türler (NamedType) ilkel türleri farklı türlerle sarar.
 */
template<typename T, typename Tag>
class StrongType {
public:
    explicit StrongType(T value) : value_(std::move(value)) {}
    T& get() { return value_; }
    const T& get() const { return value_; }
private:
    T value_;
};

// EN: Define distinct types — they're all `int` underneath but incompatible
// TR: Farklı türler tanımla — hepsi altta `int` ama birbirleriyle uyumsuz
using Port       = StrongType<int, struct PortTag>;
using TimeoutMs  = StrongType<int, struct TimeoutTag>;
using RetryCount = StrongType<int, struct RetryTag>;

struct ConnectionConfig {
    std::string host;
    Port port;
    TimeoutMs timeout;
    RetryCount retries;
};

// EN: Now the API is impossible to misuse — types enforce correct ordering
// TR: Artık API'yi yanlış kullanmak imkansız — türler doğru sırayı zorlar
bool connect_to_server(const ConnectionConfig& config) {
    std::cout << "    Connecting to " << config.host
              << ":" << config.port.get()
              << " (timeout=" << config.timeout.get() << "ms"
              << ", retries=" << config.retries.get() << ")\n";
    return true;
}

// ═════════════════════════════════════════════════════════════════════════════
// PART 2: PIMPL (Pointer to Implementation)
// ═════════════════════════════════════════════════════════════════════════════

/**
 * EN: PIMPL (Pointer to IMPLementation) hides implementation details behind
 *     a pointer to a forward-declared class. Benefits:
 *
 *     1. ABI STABILITY: Changing private members doesn't break binary compatibility
 *        → Users don't need to recompile when implementation changes
 *     2. COMPILE FIREWALL: Header doesn't include implementation headers
 *        → Faster builds (fewer transitive includes)
 *     3. CLEAN HEADER: Public header shows only the API, no private details
 *
 *     Cost: One extra heap allocation + pointer indirection (usually negligible)
 *
 *     Pattern:
 *       // sensor.hpp (PUBLIC HEADER — shipped to users)
 *       class Sensor {
 *       public:
 *           Sensor();
 *           ~Sensor();
 *           double read();
 *       private:
 *           struct Impl;                      // Forward declaration only!
 *           std::unique_ptr<Impl> pimpl_;     // Pointer to hidden impl
 *       };
 *
 *       // sensor.cpp (PRIVATE — not shipped to users)
 *       struct Sensor::Impl {
 *           double calibration;
 *           FilterAlgorithm filter;  // Heavy internal dependency
 *           DatabaseConnection db;   // Users don't need to know about this!
 *       };
 *
 * TR: PIMPL, uygulama detaylarını ileri bildirilen bir sınıfa işaretçi arkasında gizler.
 *     1. ABI KARARLILIĞI: Özel üyeleri değiştirmek ikili uyumluluğu bozmaz
 *     2. DERLEME GÜVENLİK DUVARI: Başlık, uygulama başlıklarını içermez
 *     3. TEMİZ BAŞLIK: Genel başlık yalnızca API'yi gösterir
 */
class ECUDiagnostics {
public:
    ECUDiagnostics(const std::string& ecu_name);
    ~ECUDiagnostics();

    // EN: Move-only (because of unique_ptr) / TR: Sadece taşınabilir (unique_ptr yüzünden)
    ECUDiagnostics(ECUDiagnostics&& other) noexcept;
    ECUDiagnostics& operator=(ECUDiagnostics&& other) noexcept;

    // EN: Public API — stable ABI / TR: Genel API — kararlı ABI
    void run_diagnostics();
    int get_dtc_count() const;
    std::string get_summary() const;

private:
    struct Impl;                       // EN: Forward declaration / TR: İleri bildirim
    std::unique_ptr<Impl> pimpl_;      // EN: Hidden implementation / TR: Gizli uygulama
};

// EN: Implementation — normally in .cpp file, NOT in header
// TR: Uygulama — normalde .cpp dosyasında, başlıkta DEĞİL
struct ECUDiagnostics::Impl {
    std::string ecu_name;
    int dtc_count = 0;
    std::vector<std::string> log;

    void internal_scan() {
        log.push_back("Scanning ECU: " + ecu_name);
        dtc_count = 3;  // EN: Simulated / TR: Simüle edilmiş
        log.push_back("Found " + std::to_string(dtc_count) + " DTCs");
    }
};

ECUDiagnostics::ECUDiagnostics(const std::string& ecu_name)
    : pimpl_(std::make_unique<Impl>()) {
    pimpl_->ecu_name = ecu_name;
}

ECUDiagnostics::~ECUDiagnostics() = default;
ECUDiagnostics::ECUDiagnostics(ECUDiagnostics&&) noexcept = default;
ECUDiagnostics& ECUDiagnostics::operator=(ECUDiagnostics&&) noexcept = default;

void ECUDiagnostics::run_diagnostics() { pimpl_->internal_scan(); }
int ECUDiagnostics::get_dtc_count() const { return pimpl_->dtc_count; }

std::string ECUDiagnostics::get_summary() const {
    std::string result;
    for (const auto& entry : pimpl_->log) {
        result += "    " + entry + "\n";
    }
    return result;
}

// ═════════════════════════════════════════════════════════════════════════════
// PART 3: [[deprecated]] Attribute & API Versioning
// ═════════════════════════════════════════════════════════════════════════════

/**
 * EN: Inline namespaces enable API versioning without breaking existing code:
 *
 *     namespace mylib {
 *         inline namespace v2 {   // Current default version
 *             void process(int x);
 *         }
 *         namespace v1 {          // Old version still available
 *             [[deprecated("Use v2::process instead")]]
 *             void process(int x);
 *         }
 *     }
 *
 *     mylib::process(42);        // Calls v2::process (inline namespace = default)
 *     mylib::v1::process(42);    // Still compiles but warns "deprecated"
 *
 * TR: Inline ad alanları, mevcut kodu bozmadan API sürümlemeyi sağlar.
 */
namespace sensor_api {

    // EN: Version 1 — old API (deprecated but still accessible)
    // TR: Sürüm 1 — eski API (kullanımdan kaldırılmış ama erişilebilir)
    namespace v1 {
        [[deprecated("Use sensor_api::read_temperature() instead (v2 with units)")]]
        double read_temp(int sensor_id) {
            return 22.5 + static_cast<double>(sensor_id) * 0.1;
        }
    }

    // EN: Version 2 — current API (inline = default when using sensor_api::)
    // TR: Sürüm 2 — mevcut API (inline = sensor_api:: kullanıldığında varsayılan)
    inline namespace v2 {
        struct Temperature {
            double value;
            std::string unit;
        };

        Temperature read_temperature(int sensor_id) {
            return {22.5 + static_cast<double>(sensor_id) * 0.1, "°C"};
        }
    }
}

// ═════════════════════════════════════════════════════════════════════════════
// PART 4: Tag Dispatch & Overload Sets
// ═════════════════════════════════════════════════════════════════════════════

/**
 * EN: Tag dispatch uses empty struct types to select function overloads
 *     at compile time. This avoids runtime branching and makes intent clear.
 *
 * TR: Tag dispatch, derleme zamanında fonksiyon aşırı yüklemelerini seçmek
 *     için boş struct türleri kullanır.
 */
namespace tags {
    struct Celsius {};
    struct Fahrenheit {};
    struct Kelvin {};
}

// EN: Same function name, different behavior based on tag type
// TR: Aynı fonksiyon adı, tag türüne göre farklı davranış
double convert_to_celsius(double value, tags::Celsius) {
    return value;  // EN: Already Celsius / TR: Zaten Celsius
}

double convert_to_celsius(double value, tags::Fahrenheit) {
    return (value - 32.0) * 5.0 / 9.0;
}

double convert_to_celsius(double value, tags::Kelvin) {
    return value - 273.15;
}

// ═════════════════════════════════════════════════════════════════════════════
// PART 5: Header-Only Library Pattern
// ═════════════════════════════════════════════════════════════════════════════

/**
 * EN: Header-only libraries put EVERYTHING in headers (no .cpp files).
 *     Popular examples: nlohmann/json, Catch2, Eigen.
 *
 *     Rules for header-only libraries:
 *     1. All functions must be `inline` (to avoid ODR violations)
 *     2. Templates are inherently inline (no keyword needed)
 *     3. Use `inline` for non-template functions and variables
 *     4. Consider a single-header approach (#include "lib.hpp")
 *
 * TR: Header-only kütüphaneler HER ŞEYİ başlıklara koyar (.cpp yok).
 *     Tüm fonksiyonlar `inline` olmalıdır (ODR ihlallerini önlemek için).
 */
namespace header_only_lib {

    // EN: `inline` is required for non-template functions in headers
    //     Without it: multiple definition error when included from 2+ .cpp files
    // TR: Başlıklarda şablon olmayan fonksiyonlar için `inline` gereklidir
    inline double clamp(double value, double low, double high) {
        return (value < low) ? low : (value > high) ? high : value;
    }

    // EN: Templates are implicitly inline — no keyword needed
    // TR: Şablonlar örtük olarak inline — anahtar kelime gerekmez
    template<typename Container>
    auto sum(const Container& c) {
        using T = typename Container::value_type;
        T result{};
        for (const auto& elem : c) result += elem;
        return result;
    }

    // EN: C++17 inline variables — header-safe global state
    // TR: C++17 inline değişkenler — başlık dosyası güvenli global durum
    inline constexpr double VERSION = 2.0;
}

// ═════════════════════════════════════════════════════════════════════════════
// MAIN
// ═════════════════════════════════════════════════════════════════════════════
int main() {
    std::cout << "============================================\n";
    std::cout << "   Module 08 - API Design in Modern C++\n";
    std::cout << "============================================\n\n";

    // ─── Demo 1: Strong Types ────────────────────────────────────────────
    {
        std::cout << "--- Demo 1: Strong Types (NamedType Pattern) ---\n";

        // EN: BEFORE (dangerous): connect("host", 5000, 8080, 3)
        //     Port and timeout easily swapped with no compiler error!
        // TR: ÖNCE (tehlikeli): port ve timeout kolayca karıştırılır!
        std::cout << "  Before (weak types): connect(host, port?, timeout?, retries?)\n";

        // EN: AFTER (safe): types ENFORCE correct usage
        // TR: SONRA (güvenli): türler doğru kullanımı ZORLAR
        ConnectionConfig cfg{
            "ecu-gateway.local",
            Port{8080},
            TimeoutMs{5000},
            RetryCount{3}
        };
        std::cout << "  After (strong types):\n";
        connect_to_server(cfg);

        // EN: This would NOT compile:
        // TR: Bu derlenmez:
        //   ConnectionConfig bad{"host", TimeoutMs{5000}, Port{8080}, RetryCount{3}};
        //   Error: cannot convert TimeoutMs to Port
        std::cout << "  (Swapping Port/Timeout gives COMPILE ERROR!)\n\n";
    }

    // ─── Demo 2: PIMPL ──────────────────────────────────────────────────
    {
        std::cout << "--- Demo 2: PIMPL (Pointer to Implementation) ---\n";
        ECUDiagnostics diag("ECU_ENGINE_01");
        diag.run_diagnostics();

        std::cout << "  DTC count: " << diag.get_dtc_count() << "\n";
        std::cout << "  Log:\n" << diag.get_summary();
        std::cout << "  sizeof(ECUDiagnostics) = " << sizeof(ECUDiagnostics)
                  << " bytes (just a unique_ptr!)\n\n";
    }

    // ─── Demo 3: API Versioning ──────────────────────────────────────────
    {
        std::cout << "--- Demo 3: API Versioning (inline namespace) ---\n";

        // EN: Default (v2) — called without version prefix
        // TR: Varsayılan (v2) — sürüm öneki olmadan çağrılır
        auto temp = sensor_api::read_temperature(1);
        std::cout << "  sensor_api::read_temperature(1) = "
                  << temp.value << " " << temp.unit << " (v2 — current)\n";

        // EN: Explicit v1 — compiles with deprecation warning
        // TR: Açık v1 — kullanımdan kaldırma uyarısıyla derlenir
        // double old_temp = sensor_api::v1::read_temp(1);
        std::cout << "  sensor_api::v1::read_temp(1) → [[deprecated]] warning\n\n";
    }

    // ─── Demo 4: Tag Dispatch ────────────────────────────────────────────
    {
        std::cout << "--- Demo 4: Tag Dispatch ---\n";
        std::cout << "  100°F → " << convert_to_celsius(100.0, tags::Fahrenheit{}) << " °C\n";
        std::cout << "  373K  → " << convert_to_celsius(373.15, tags::Kelvin{}) << " °C\n";
        std::cout << "  25°C  → " << convert_to_celsius(25.0, tags::Celsius{}) << " °C\n";
        std::cout << "  (No runtime branching — overload resolved at compile time)\n\n";
    }

    // ─── Demo 5: Header-Only Library ─────────────────────────────────────
    {
        std::cout << "--- Demo 5: Header-Only Library Pattern ---\n";
        std::cout << "  clamp(150, 0, 100) = " << header_only_lib::clamp(150, 0, 100) << "\n";

        std::vector<double> data = {1.5, 2.5, 3.5, 4.5};
        std::cout << "  sum({1.5, 2.5, 3.5, 4.5}) = " << header_only_lib::sum(data) << "\n";
        std::cout << "  Library version: " << header_only_lib::VERSION << "\n\n";

        std::cout << "  Rules for header-only libraries:\n";
        std::cout << "    1. All functions: inline (ODR compliance)\n";
        std::cout << "    2. Templates: inherently inline\n";
        std::cout << "    3. Variables: inline constexpr (C++17)\n";
        std::cout << "    4. Classes: define in namespace, methods inline\n\n";
    }

    // ─── Demo 6: API Design Checklist ────────────────────────────────────
    {
        std::cout << "--- Demo 6: API Design Checklist ---\n";
        std::cout << "  ✅ Use strong types to prevent argument swapping\n";
        std::cout << "  ✅ PIMPL for ABI stability and compilation firewall\n";
        std::cout << "  ✅ [[deprecated]] for graceful API evolution\n";
        std::cout << "  ✅ inline namespace for API versioning\n";
        std::cout << "  ✅ Tag dispatch for zero-cost type-based dispatch\n";
        std::cout << "  ✅ const& for read-only params, value for to-be-moved\n";
        std::cout << "  ✅ [[nodiscard]] for functions where ignoring return is bug\n";
        std::cout << "  ✅ explicit constructors to prevent implicit conversions\n";
        std::cout << "  ✅ noexcept on move operations and destructors\n";
        std::cout << "  ✅ Header-only + inline for simple distribution\n\n";
    }

    std::cout << "============================================\n";
    std::cout << "   End of API Design\n";
    std::cout << "============================================\n";

    return 0;
}
