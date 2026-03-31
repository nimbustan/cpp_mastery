/**
 * @file module_16_autosar/03_functional_safety.cpp
 * @brief Functional Safety — ISO 26262 & ASIL / Fonksiyonel Güvenlik
 *
 * @details
 * =============================================================================
 * [THEORY: ISO 26262 — Functional Safety for Road Vehicles / TEORİ: ISO 26262 — Karayolu Araçları İçin Fonksiyonel Güvenlik]
 * =============================================================================
 *
 * EN: ISO 26262 is the international standard for functional safety of
 *     electrical/electronic systems in road vehicles. It defines:
 *
 *     V-Model (ISO 26262 Parts):
 *     ┌───────────────────────────────────────────────────────────────┐
 *     │ Part 3: Concept Phase     ←─────────→  Part 4: System Level   │
 *     │   Item Definition                        System Design        │
 *     │   HARA (Hazard Analysis)                 Safety Validation    │
 *     │   Safety Goals (ASIL)                                         │
 *     │                                                               │
 *     │ Part 5: HW Level          ←─────────→  Part 6: SW Level       │
 *     │   HW Safety Requirements                SW Safety Req.        │
 *     │   HW Design                             SW Architecture       │
 *     │   HW Verification                       Unit Design/Test      │
 *     │                                         Integration Test      │
 *     │                                                               │
 *     │ Part 7: Production  │  Part 8: Supporting  │  Part 9: ASIL-D  │
 *     └─────────────────────┴──────────────────────┴──────────────────┘
 *
 * TR: ISO 26262, karayolu araçlarındaki elektriksel/elektronik sistemler
 *     için uluslararası fonksiyonel güvenlik standardıdır.
 *
 *     V-Modeli (ISO 26262 Bölümleri):
 *     ┌───────────────────────────────────────────────────────────────┐
 *     │ Bölüm 3: Konsept Fazı   ←─────────→ Bölüm 4: Sistem Seviyesi  │
 *     │   Öğe Tanımı                          Sistem Tasarımı         │
 *     │   HARA (Tehlike Analizi)              Güvenlik Doğrulama      │
 *     │   Güvenlik Hedefleri (ASIL)                                   │
 *     │                                                               │
 *     │ Bölüm 5: HW Seviyesi   ←─────────→ Bölüm 6: SW Seviyesi       │
 *     │   HW Güvenlik Gereks.                SW Güvenlik Gereks.      │
 *     │   HW Tasarımı                         SW Mimarisi             │
 *     │   HW Doğrulama                        Birim Tasarım/Test      │
 *     │                                       Entegrasyon Testi       │
 *     │                                                               │
 *     │ Bölüm 7: Üretim     │  Bölüm 8: Destek   │  Bölüm 9: ASIL-D   │
 *     └─────────────────────┴────────────────────┴────────────────────┘
 *
 * =============================================================================
 * [THEORY: ASIL — Automotive Safety Integrity Level / TEORİ: ASIL — Otomotiv Güvenlik Bütünlük Seviyesi]
 * =============================================================================
 *
 * EN: ASIL is determined by HARA (Hazard Analysis & Risk Assessment):
 *     Three factors: Severity (S), Exposure (E), Controllability (C)
 *
 *     ┌───────┬───────────────────────────────────────────────────────┐
 *     │ ASIL  │ Description                                           │
 *     ├───────┼───────────────────────────────────────────────────────┤
 *     │ QM    │ Quality Management — no safety requirements           │
 *     │ A     │ Lowest safety level (e.g., rear light failure)        │
 *     │ B     │ Medium-low (e.g., headlight failure at night)         │
 *     │ C     │ Medium-high (e.g., airbag deployment timing)          │
 *     │ D     │ Highest safety level (e.g., steering, braking)        │
 *     └───────┴───────────────────────────────────────────────────────┘
 *
 *     ASIL Determination Matrix (simplified):
 *     ┌──────────┬──────────┬─────────────────┬──────┐
 *     │ Severity │ Exposure │ Controllability │ ASIL │
 *     ├──────────┼──────────┼─────────────────┼──────┤
 *     │ S3       │ E4       │ C3              │ D    │
 *     │ S3       │ E4       │ C2              │ C    │
 *     │ S3       │ E3       │ C3              │ C    │
 *     │ S2       │ E4       │ C3              │ C    │
 *     │ S2       │ E3       │ C3              │ B    │
 *     │ S1       │ E4       │ C3              │ B    │
 *     │ S1       │ E3       │ C3              │ A    │
 *     │ S1       │ E2       │ C2              │ QM   │
 *     └──────────┴──────────┴─────────────────┴──────┘
 *
 *     ASIL Decomposition:
 *     ASIL-D = ASIL-B(D) + ASIL-B(D)   (independent dual path)
 *     ASIL-D = ASIL-C(D) + ASIL-A(D)   (asymmetric decomposition)
 *     → Allows simpler implementation per channel if independence proven.
 *
 * TR: ASIL, HARA (Tehlike Analizi ve Risk Değerlendirmesi) ile belirlenir:
 *     Üç faktör: Şiddet (S), Maruziyet (E), Kontrol Edilebilirlik (C)
 *
 *     ┌───────┬───────────────────────────────────────────────────────┐
 *     │ ASIL  │ Açıklama                                              │
 *     ├───────┼───────────────────────────────────────────────────────┤
 *     │ QM    │ Kalite Yönetimi — güvenlik gereksinimi yok            │
 *     │ A     │ En düşük güvenlik (örn. arka lamba arızası)           │
 *     │ B     │ Orta-düşük (örn. gece far arızası)                    │
 *     │ C     │ Orta-yüksek (örn. hava yastığı zamanl.)               │
 *     │ D     │ En yüksek güvenlik (örn. direksiyon, fren)            │
 *     └───────┴───────────────────────────────────────────────────────┘
 *
 *     ASIL Belirleme Matrisi (basitleştirilmiş):
 *     ┌──────────┬──────────┬─────────────────┬──────┐
 *     │ Şiddet   │ Maruziyet│ Kontrol Edileb. │ ASIL │
 *     ├──────────┼──────────┼─────────────────┼──────┤
 *     │ S3       │ E4       │ C3              │ D    │
 *     │ S3       │ E4       │ C2              │ C    │
 *     │ S3       │ E3       │ C3              │ C    │
 *     │ S2       │ E4       │ C3              │ C    │
 *     │ S2       │ E3       │ C3              │ B    │
 *     │ S1       │ E4       │ C3              │ B    │
 *     │ S1       │ E3       │ C3              │ A    │
 *     │ S1       │ E2       │ C2              │ QM   │
 *     └──────────┴──────────┴─────────────────┴──────┘
 *
 *     ASIL Ayrıştırma:
 *     ASIL-D = ASIL-B(D) + ASIL-B(D)   (bağımsız çift yol)
 *     ASIL-D = ASIL-C(D) + ASIL-A(D)   (asimetrik ayrıştırma)
 *     → Bağımsızlık kanıtlanırsa kanal başına daha basit uygulama sağlar.
 *
 * =============================================================================
 * [THEORY: Defensive Programming Patterns / TEORİ: Savunmacı Programlama Kalıpları]
 * =============================================================================
 *
 * EN: Safety-critical software must employ:
 *     1) Range check: validate all inputs against physical limits
 *     2) Plausibility check: cross-validate redundant sensors
 *     3) Alive counter: detect frozen/stuck signal values
 *     4) CRC/checksum: detect data corruption in communication
 *     5) Program flow monitoring: ensure correct execution sequence
 *     6) Safe state: define and transition to safe state on failure
 *
 * TR: Güvenlik kritik yazılımda savunmacı programlama kalıpları:
 *     1) Aralık kontrolü: tüm girdileri fiziksel sınırlara karşı doğrula
 *     2) Makullük kontrolü: yedekli sensörleri çapraz doğrula
 *     3) Canlılık sayacı: donmuş/takılı sinyal değerlerini tespit et
 *     4) CRC/sağlama: iletişimde veri bozulmasını tespit et
 *     5) Program akış izleme: doğru yürütme sırasını sağla
 *     6) Güvenli durum: arızada güvenli duruma geçişi tanımla
 *
 * =============================================================================
 * [THEORY: Code Coverage Requirements per ASIL / TEORİ: ASIL Seviyesine Göre Kod Kapsama Gereksinimleri]
 * =============================================================================
 *
 * EN:
 *     ┌───────┬──────────────┬──────────────┬──────────────┬──────────┐
 *     │ ASIL  │ Statement    │ Branch       │ MC/DC        │ Method   │
 *     ├───────┼──────────────┼──────────────┼──────────────┼──────────┤
 *     │ A     │ Recommended  │ —            │ —            │ Required │
 *     │ B     │ Required     │ Recommended  │ —            │ Required │
 *     │ C     │ Required     │ Required     │ Recommended  │ Required │
 *     │ D     │ Required     │ Required     │ Required     │ Required │
 *     └───────┴──────────────┴──────────────┴──────────────┴──────────┘
 *
 *     MC/DC = Modified Condition/Decision Coverage
 *     → Every condition independently affects the decision outcome.
 *
 * TR: ASIL seviyesine göre kod kapsama gereksinimleri:
 *     ┌───────┬──────────────┬──────────────┬──────────────┬──────────┐
 *     │ ASIL  │ Deyim        │ Dal          │ MC/DC        │ Metot    │
 *     ├───────┼──────────────┼──────────────┼──────────────┼──────────┤
 *     │ A     │ Önerilen     │ —            │ —            │ Zorunlu  │
 *     │ B     │ Zorunlu      │ Önerilen     │ —            │ Zorunlu  │
 *     │ C     │ Zorunlu      │ Zorunlu      │ Önerilen     │ Zorunlu  │
 *     │ D     │ Zorunlu      │ Zorunlu      │ Zorunlu      │ Zorunlu  │
 *     └───────┴──────────────┴──────────────┴──────────────┴──────────┘
 *     MC/DC = Değiştirilmiş Koşul/Karar Kapsaması
 *     → Her koşul karar sonucunu bağımsız olarak etkiler.
 *
 * @note Compile: g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion 03_functional_safety.cpp -o 03_functional_safety
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <iostream>
#include <cstdint>
#include <array>
#include <vector>
#include <string>
#include <string_view>
#include <functional>
#include <cmath>
#include <iomanip>
#include <algorithm>
#include <numeric>
#include <cassert>
#include <optional>
#include <variant>

// ═════════════════════════════════════════════════════════════════════════════
// PART 1: ASIL Determination — HARA Simulation
// ═════════════════════════════════════════════════════════════════════════════

namespace safety {

// EN: Severity classes (S0-S3)
// TR: Şiddet sınıfları (S0-S3)
enum class Severity : uint8_t { S0 = 0, S1 = 1, S2 = 2, S3 = 3 };

// EN: Exposure classes (E0-E4)
// TR: Maruz kalma sınıfları (E0-E4)
enum class Exposure : uint8_t { E0 = 0, E1 = 1, E2 = 2, E3 = 3, E4 = 4 };

// EN: Controllability classes (C0-C3)
// TR: Kontrol edilebilirlik sınıfları (C0-C3)
enum class Controllability : uint8_t { C0 = 0, C1 = 1, C2 = 2, C3 = 3 };

enum class ASIL : uint8_t { QM = 0, A = 1, B = 2, C = 3, D = 4 };

struct HazardEntry {
    std::string            hazard;
    Severity               severity;
    Exposure               exposure;
    Controllability        controllability;
    ASIL                   determined_asil = ASIL::QM;
};

// EN: ASIL determination lookup (simplified)
// TR: ASIL belirleme tablosu (basitleştirilmiş)
inline ASIL DetermineASIL(Severity s, Exposure e, Controllability c) {
    if (static_cast<uint8_t>(s) == 0) return ASIL::QM;
    if (static_cast<uint8_t>(e) == 0) return ASIL::QM;
    if (static_cast<uint8_t>(c) == 0) return ASIL::QM;

    int score = static_cast<int>(s) + static_cast<int>(e) + static_cast<int>(c);
    if (score >= 10) return ASIL::D;
    if (score >= 8)  return ASIL::C;
    if (score >= 6)  return ASIL::B;
    if (score >= 4)  return ASIL::A;
    return ASIL::QM;
}

inline std::string_view ASILStr(ASIL a) {
    switch (a) {
        case ASIL::QM: return "QM";
        case ASIL::A:  return "ASIL-A";
        case ASIL::B:  return "ASIL-B";
        case ASIL::C:  return "ASIL-C";
        case ASIL::D:  return "ASIL-D";
    }
    return "???";
}

// ═════════════════════════════════════════════════════════════════════════════
// PART 2: Defensive Programming — Range & Plausibility Check
// ═════════════════════════════════════════════════════════════════════════════

// EN: Range check: is value within physical limits?
// TR: Aralık kontrolü: değer fiziksel sınırlar içinde mi?
template <typename T>
struct RangeChecker {
    T min_val;
    T max_val;
    std::string signal_name;

    [[nodiscard]] bool Check(T value) const {
        return value >= min_val && value <= max_val;
    }

    void Report(T value) const {
        if (!Check(value)) {
            std::cout << "  [RANGE_FAIL] " << signal_name << "=" << value
                      << " not in [" << min_val << ", " << max_val << "]\n";
        }
    }
};

// EN: Plausibility check: cross-validate two redundant sensors
// TR: Makullük kontrolü: iki yedekli sensörü çapraz doğrula
struct PlausibilityChecker {
    double max_deviation;
    std::string name;

    [[nodiscard]] bool Check(double sensor_a, double sensor_b) const {
        return std::abs(sensor_a - sensor_b) <= max_deviation;
    }

    void Report(double a, double b) const {
        bool ok = Check(a, b);
        std::cout << "  [PLAUSIBILITY] " << name
                  << " A=" << a << " B=" << b
                  << " diff=" << std::abs(a - b)
                  << " max=" << max_deviation
                  << (ok ? " → OK" : " → FAIL") << "\n";
    }
};

// EN: Alive counter: detect frozen signal
// TR: Canlılık sayacı: donmuş sinyal tespiti
class AliveCounter {
    uint8_t  expected_counter_ = 0;
    uint32_t fail_count_ = 0;
    uint32_t pass_count_ = 0;
    std::string name_;

public:
    explicit AliveCounter(std::string name) : name_(std::move(name)) {}

    bool Validate(uint8_t received_counter) {
        // EN: Counter should increment by 1 each cycle (mod 16)
        // TR: Sayaç her döngüde 1 artmalı (mod 16)
        bool ok = (received_counter == expected_counter_);
        expected_counter_ = static_cast<uint8_t>((received_counter + 1u) & 0x0Fu);
        if (ok) ++pass_count_; else ++fail_count_;
        return ok;
    }

    [[nodiscard]] uint32_t FailCount() const { return fail_count_; }
    [[nodiscard]] uint32_t PassCount() const { return pass_count_; }

    void Report() const {
        std::cout << "  [ALIVE] " << name_
                  << " pass=" << pass_count_ << " fail=" << fail_count_
                  << (fail_count_ > 0 ? " → DEGRADED" : " → OK") << "\n";
    }
};

// ═════════════════════════════════════════════════════════════════════════════
// PART 3: Throttle Position Plausibility (ASIL-D Example)
// ═════════════════════════════════════════════════════════════════════════════

// EN: Real ASIL-D scenario: Electronic Throttle Control (ETC)
//     Two independent TPS sensors (TPS1 + TPS2) for redundancy
//     If plausibility fails → limp-home mode (limited torque)
// TR: Gerçek ASIL-D senaryosu: Elektronik Gaz Kontrolü
//     İki bağımsız TPS sensörü (TPS1 + TPS2) yedeklilik için

enum class ThrottleState : uint8_t {
    NORMAL, DEGRADED, LIMP_HOME, SAFE_STOP
};

struct ThrottleReading {
    double tps1_percent;       // 0-100%
    double tps2_percent;       // 0-100% (redundant)
    uint8_t alive_counter;     // 4-bit rolling counter
};

class ThrottlePlausibility {
    RangeChecker<double> range_{0.0, 100.0, "TPS"};
    PlausibilityChecker  plaus_{5.0, "TPS1_vs_TPS2"};
    AliveCounter         alive_{"TPS_Alive"};
    ThrottleState        state_ = ThrottleState::NORMAL;
    uint32_t             consecutive_fails_ = 0;
    static constexpr uint32_t FAIL_THRESHOLD = 3;  // debounce
    static constexpr uint32_t FTTI_MS = 50;  // Fault Tolerant Time Interval

public:
    ThrottleState Evaluate(const ThrottleReading& reading) {
        bool range_ok = range_.Check(reading.tps1_percent) &&
                        range_.Check(reading.tps2_percent);
        bool plaus_ok = plaus_.Check(reading.tps1_percent, reading.tps2_percent);
        bool alive_ok = alive_.Validate(reading.alive_counter);

        if (range_ok && plaus_ok && alive_ok) {
            consecutive_fails_ = 0;
            if (state_ == ThrottleState::DEGRADED)
                state_ = ThrottleState::NORMAL;  // heal
        } else {
            ++consecutive_fails_;
            if (consecutive_fails_ >= FAIL_THRESHOLD) {
                if (!range_ok || !alive_ok)
                    state_ = ThrottleState::SAFE_STOP;
                else
                    state_ = ThrottleState::LIMP_HOME;
            } else {
                state_ = ThrottleState::DEGRADED;
            }
        }
        return state_;
    }

    [[nodiscard]] double GetSafeThrottle(const ThrottleReading& r) const {
        switch (state_) {
            case ThrottleState::NORMAL:
                return (r.tps1_percent + r.tps2_percent) / 2.0;
            case ThrottleState::DEGRADED:
                return (r.tps1_percent + r.tps2_percent) / 2.0;
            case ThrottleState::LIMP_HOME:
                return std::min(r.tps1_percent, 20.0);  // max 20%
            case ThrottleState::SAFE_STOP:
                return 0.0;
        }
        return 0.0;
    }

    static std::string_view StateStr(ThrottleState s) {
        switch (s) {
            case ThrottleState::NORMAL:    return "NORMAL";
            case ThrottleState::DEGRADED:  return "DEGRADED";
            case ThrottleState::LIMP_HOME: return "LIMP_HOME";
            case ThrottleState::SAFE_STOP: return "SAFE_STOP";
        }
        return "???";
    }

    void PrintDiag() const {
        std::cout << "  [Throttle] State=" << StateStr(state_)
                  << " ConsecFails=" << consecutive_fails_
                  << " FTTI=" << FTTI_MS << "ms\n";
        alive_.Report();
    }
};

// ═════════════════════════════════════════════════════════════════════════════
// PART 4: Redundancy Patterns
// ═════════════════════════════════════════════════════════════════════════════

// EN: Dual-execution with majority voting (TMR — Triple Modular Redundancy)
// TR: Çift yürütme ile çoğunluk oylama

template <typename T>
struct RedundantValue {
    T channel_a;
    T channel_b;
    T channel_c;

    // EN: 2-out-of-3 majority voting
    // TR: 3'ten 2'si çoğunluk oylama
    [[nodiscard]] T Vote() const {
        if (channel_a == channel_b) return channel_a;
        if (channel_a == channel_c) return channel_a;
        if (channel_b == channel_c) return channel_b;
        return channel_a;  // no majority — use channel A as default
    }

    [[nodiscard]] bool IsConsistent() const {
        return (channel_a == channel_b) || (channel_a == channel_c) ||
               (channel_b == channel_c);
    }
};

// EN: Diverse Redundancy — different algorithms for same computation
// TR: Çeşitli Yedeklilik — aynı hesaplama için farklı algoritmalar

struct DiverseChecksum {
    // EN: Method A: simple XOR checksum
    // TR: Yöntem A: basit XOR sağlama
    static uint8_t ComputeXOR(const std::vector<uint8_t>& data) {
        uint8_t cs = 0;
        for (auto b : data) cs ^= b;
        return cs;
    }

    // EN: Method B: arithmetic sum checksum (complemented)
    // TR: Yöntem B: aritmetik toplam sağlama (tümleyen)
    static uint8_t ComputeSum(const std::vector<uint8_t>& data) {
        uint8_t sum = 0;
        for (auto b : data) sum = static_cast<uint8_t>(sum + b);
        return static_cast<uint8_t>(~sum + 1u);  // two's complement
    }

    // EN: Verify using both methods — diverse redundancy
    // TR: Her iki yöntemle doğrula — çeşitli yedeklilik
    static bool Verify(const std::vector<uint8_t>& data,
                       uint8_t expected_xor, uint8_t expected_sum) {
        return (ComputeXOR(data) == expected_xor) &&
               (ComputeSum(data) == expected_sum);
    }
};

// ═════════════════════════════════════════════════════════════════════════════
// PART 5: Program Flow Monitoring
// ═════════════════════════════════════════════════════════════════════════════

// EN: Ensures correct execution sequence: Init → Read → Process → Output
// TR: Doğru yürütme sırasını garanti eder: Init → Read → Process → Output

class ProgramFlowMonitor {
    using CheckpointID = uint16_t;
    std::vector<CheckpointID> expected_sequence_;
    std::vector<CheckpointID> actual_sequence_;
    uint32_t violation_count_ = 0;

public:
    void DefineSequence(std::initializer_list<CheckpointID> seq) {
        expected_sequence_.assign(seq);
    }

    void ReportCheckpoint(CheckpointID cp) {
        actual_sequence_.push_back(cp);
    }

    bool Evaluate() {
        bool ok = (actual_sequence_ == expected_sequence_);
        if (!ok) {
            ++violation_count_;
            std::cout << "  [FLOW_MON] VIOLATION! Expected: ";
            for (auto c : expected_sequence_) std::cout << c << " ";
            std::cout << "| Actual: ";
            for (auto c : actual_sequence_) std::cout << c << " ";
            std::cout << "\n";
        }
        actual_sequence_.clear();
        return ok;
    }

    [[nodiscard]] uint32_t ViolationCount() const { return violation_count_; }
};

// ═════════════════════════════════════════════════════════════════════════════
// PART 6: ASIL Decomposition Calculator
// ═════════════════════════════════════════════════════════════════════════════

struct ASILDecomposition {
    ASIL original;
    ASIL channel_a;
    ASIL channel_b;
    bool valid;
};

// EN: Valid ASIL decompositions per ISO 26262-9
// TR: ISO 26262-9'a göre geçerli ASIL ayrıştırmaları
inline std::vector<ASILDecomposition> GetDecompositions(ASIL target) {
    std::vector<ASILDecomposition> results;
    switch (target) {
        case ASIL::D:
            results.push_back({ASIL::D, ASIL::D, ASIL::QM, true});
            results.push_back({ASIL::D, ASIL::C, ASIL::A, true});
            results.push_back({ASIL::D, ASIL::B, ASIL::B, true});
            break;
        case ASIL::C:
            results.push_back({ASIL::C, ASIL::C, ASIL::QM, true});
            results.push_back({ASIL::C, ASIL::B, ASIL::A, true});
            break;
        case ASIL::B:
            results.push_back({ASIL::B, ASIL::B, ASIL::QM, true});
            results.push_back({ASIL::B, ASIL::A, ASIL::A, true});
            break;
        case ASIL::A:
            results.push_back({ASIL::A, ASIL::A, ASIL::QM, true});
            break;
        default:
            break;
    }
    return results;
}

}  // namespace safety

// ═════════════════════════════════════════════════════════════════════════════
// MAIN — Demos
// ═════════════════════════════════════════════════════════════════════════════

int main() {
    std::cout << "╔══════════════════════════════════════════════════════════════╗\n"
              << "║ Module 16: Functional Safety — ISO 26262 & ASIL              ║\n"
              << "╚══════════════════════════════════════════════════════════════╝\n\n";

    // --- Demo 1: HARA — ASIL Determination ---
    std::cout << "--- Demo 1: HARA — ASIL Determination ---\n";
    {
        using namespace safety;
        std::vector<HazardEntry> hazards = {
            {"Unintended steering torque",       Severity::S3, Exposure::E4, Controllability::C3, ASIL::QM},
            {"Headlight sudden off (night)",      Severity::S2, Exposure::E3, Controllability::C3, ASIL::QM},
            {"Rear wiper stuck on",               Severity::S0, Exposure::E4, Controllability::C1, ASIL::QM},
            {"Brake force reduction >50%",        Severity::S3, Exposure::E4, Controllability::C2, ASIL::QM},
            {"Airbag early deployment (no crash)", Severity::S3, Exposure::E3, Controllability::C3, ASIL::QM},
            {"Seat heater overheat",              Severity::S1, Exposure::E3, Controllability::C2, ASIL::QM},
        };

        for (auto& h : hazards)
            h.determined_asil = DetermineASIL(h.severity, h.exposure, h.controllability);

        std::cout << "  ┌────────────────────────────────────┬────┬────┬────┬────────┐\n";
        std::cout << "  │ Hazard                             │ S  │ E  │ C  │ ASIL   │\n";
        std::cout << "  ├────────────────────────────────────┼────┼────┼────┼────────┤\n";
        for (const auto& h : hazards) {
            std::cout << "  │ " << std::setw(34) << std::left << h.hazard << " │ S"
                      << static_cast<int>(h.severity) << " │ E"
                      << static_cast<int>(h.exposure) << " │ C"
                      << static_cast<int>(h.controllability) << " │ "
                      << std::setw(6) << ASILStr(h.determined_asil) << " │\n";
        }
        std::cout << std::right;
        std::cout << "  └────────────────────────────────────┴────┴────┴────┴────────┘\n";
    }

    // --- Demo 2: Throttle Plausibility (ASIL-D) ---
    std::cout << "\n--- Demo 2: Throttle Plausibility Check (ASIL-D) ---\n";
    {
        safety::ThrottlePlausibility checker;

        struct TestCase { safety::ThrottleReading r; std::string desc; };
        std::vector<TestCase> tests = {
            {{45.0,  44.5, 0}, "Normal — sensors agree"},
            {{45.5,  45.2, 1}, "Normal — sensors agree"},
            {{50.0,  42.0, 2}, "Plausibility fail — >5% diff"},
            {{50.0,  42.0, 3}, "Plausibility fail — >5% diff"},
            {{50.0,  42.0, 4}, "Plausibility fail x3 → LIMP_HOME"},
            {{30.0,  29.8, 5}, "Healed — sensors agree again"},
            {{110.0, 30.0, 6}, "Range fail — TPS1 >100%"},
            {{110.0, 30.0, 7}, "Range fail again "},
            {{110.0, 30.0, 8}, "Range fail x3 → SAFE_STOP"},
        };

        for (auto& tc : tests) {
            auto state = checker.Evaluate(tc.r);
            double safe = checker.GetSafeThrottle(tc.r);
            std::cout << "  [" << std::setw(40) << std::left << tc.desc << "] "
                      << "TPS1=" << std::setw(5) << std::right << tc.r.tps1_percent
                      << " TPS2=" << std::setw(5) << tc.r.tps2_percent
                      << " → " << std::setw(10) << std::left
                      << safety::ThrottlePlausibility::StateStr(state)
                      << " safe_throttle=" << std::right << safe << "%\n";
        }
        std::cout << std::right;
        checker.PrintDiag();
    }

    // --- Demo 3: Range Check ---
    std::cout << "\n--- Demo 3: Range Check (Defensive Programming) ---\n";
    {
        safety::RangeChecker<double> rpm_check{0.0, 8000.0, "RPM"};
        safety::RangeChecker<double> temp_check{-40.0, 150.0, "Coolant_Temp"};
        safety::RangeChecker<int>    volt_check{6, 18, "Battery_Voltage"};

        double test_rpm[] = {3500.0, 8500.0, -100.0, 0.0, 7999.0};
        for (auto v : test_rpm) {
            std::cout << "  RPM=" << std::setw(8) << v << " → "
                      << (rpm_check.Check(v) ? "OK" : "FAIL") << "\n";
        }
        rpm_check.Report(8500.0);
        temp_check.Report(200.0);
        volt_check.Report(5);
    }

    // --- Demo 4: Alive Counter ---
    std::cout << "\n--- Demo 4: Alive Counter (Frozen Signal Detection) ---\n";
    {
        safety::AliveCounter alive("CAN_0x123_Alive");

        // EN: Correct sequence: 0, 1, 2, ..., 15, 0, 1, ...
        // TR: Doğru sıra: 0, 1, 2, ..., 15, 0, 1, ...
        for (uint8_t i = 0; i < 20; ++i) {
            uint8_t counter = static_cast<uint8_t>(i & 0x0Fu);
            alive.Validate(counter);
        }
        std::cout << "  After 20 correct: ";
        alive.Report();

        // EN: Simulate frozen counter (stays at 5)
        // TR: Donmuş sayaç simülasyonu (5'te kalır)
        for (int i = 0; i < 5; ++i)
            alive.Validate(5);
        std::cout << "  After 5 frozen (value=5): ";
        alive.Report();
    }

    // --- Demo 5: Redundancy — TMR Voting ---
    std::cout << "\n--- Demo 5: Triple Modular Redundancy (TMR Voting) ---\n";
    {
        safety::RedundantValue<int> sensor1{100, 100, 100};
        std::cout << "  Channels [100, 100, 100] → Vote=" << sensor1.Vote()
                  << " consistent=" << sensor1.IsConsistent() << "\n";

        safety::RedundantValue<int> sensor2{100, 100, 105};
        std::cout << "  Channels [100, 100, 105] → Vote=" << sensor2.Vote()
                  << " consistent=" << sensor2.IsConsistent() << "\n";

        safety::RedundantValue<int> sensor3{100, 105, 110};
        std::cout << "  Channels [100, 105, 110] → Vote=" << sensor3.Vote()
                  << " consistent=" << sensor3.IsConsistent() << "\n";
    }

    // --- Demo 6: Diverse Checksum ---
    std::cout << "\n--- Demo 6: Diverse Redundancy (Dual Checksum) ---\n";
    {
        std::vector<uint8_t> data = {0x10, 0x22, 0x33, 0x44, 0x55};
        auto cs_xor = safety::DiverseChecksum::ComputeXOR(data);
        auto cs_sum = safety::DiverseChecksum::ComputeSum(data);
        std::cout << "  Data: [10 22 33 44 55]\n";
        std::cout << "  XOR checksum = 0x" << std::hex << static_cast<int>(cs_xor) << "\n";
        std::cout << "  SUM checksum = 0x" << static_cast<int>(cs_sum) << std::dec << "\n";
        bool ok = safety::DiverseChecksum::Verify(data, cs_xor, cs_sum);
        std::cout << "  Diverse verify: " << (ok ? "PASS" : "FAIL") << "\n";

        // EN: Corrupt one byte and re-verify
        // TR: Bir byte boz ve tekrar doğrula
        data[2] = 0xFF;
        ok = safety::DiverseChecksum::Verify(data, cs_xor, cs_sum);
        std::cout << "  After corruption: " << (ok ? "PASS" : "FAIL") << "\n";
    }

    // --- Demo 7: Program Flow Monitoring ---
    std::cout << "\n--- Demo 7: Program Flow Monitoring ---\n";
    {
        safety::ProgramFlowMonitor pfm;
        pfm.DefineSequence({10, 20, 30, 40});  // Init=10, Read=20, Process=30, Output=40

        // EN: Correct sequence
        // TR: Doğru sıra
        pfm.ReportCheckpoint(10);
        pfm.ReportCheckpoint(20);
        pfm.ReportCheckpoint(30);
        pfm.ReportCheckpoint(40);
        std::cout << "  Correct sequence: " << (pfm.Evaluate() ? "PASS" : "FAIL") << "\n";

        // EN: Wrong sequence (skipped 30)
        // TR: Yanlış sıra (30 atlanmış)
        pfm.ReportCheckpoint(10);
        pfm.ReportCheckpoint(20);
        pfm.ReportCheckpoint(40);  // skipped 30!
        std::cout << "  Wrong sequence:   " << (pfm.Evaluate() ? "PASS" : "FAIL") << "\n";

        std::cout << "  Total violations: " << pfm.ViolationCount() << "\n";
    }

    // --- Demo 8: ASIL Decomposition ---
    std::cout << "\n--- Demo 8: ASIL Decomposition Options ---\n";
    {
        for (auto target : {safety::ASIL::D, safety::ASIL::C, safety::ASIL::B}) {
            auto decomps = safety::GetDecompositions(target);
            std::cout << "  " << safety::ASILStr(target) << " decompositions:\n";
            for (const auto& d : decomps) {
                std::cout << "    " << safety::ASILStr(d.original) << " = "
                          << safety::ASILStr(d.channel_a) << " + "
                          << safety::ASILStr(d.channel_b) << "\n";
            }
        }
    }

    std::cout << "\n[All demos completed successfully.]\n";
    return 0;
}
