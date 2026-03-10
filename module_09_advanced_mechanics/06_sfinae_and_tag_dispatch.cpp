/**
 * @file module_09_advanced_mechanics/06_sfinae_and_tag_dispatch.cpp
 * @brief SFINAE, Tag Dispatch & if constexpr — Compile-Time Overload Selection
 * EN: Deep dive into SFINAE, tag dispatch, and modern if constexpr alternatives for automotive
 * ECU firmware template metaprogramming.
 * TR: Otomotiv ECU yazılımlarında şablon meta-programlama için SFINAE, etiket dağıtımı (tag
 * dispatch) ve modern if constexpr alternatiflerinin derinlemesine incelenmesi.
 *
 * @details
 * [THEORY / TEORİ]
 *
 * ===============================================================================
 * 1. SFINAE — "Substitution Failure Is Not An Error"
 * ===============================================================================
 * EN: When the compiler tries to instantiate a function template, it substitutes the provided
 * type arguments into the template signature. If that substitution produces an invalid type or
 * expression, the compiler does NOT emit a hard error — it simply removes that overload from the
 * candidate set. This is the SFINAE principle. Combined with std::enable_if, we can
 * conditionally enable/disable overloads based on type properties at compile time.
 * TR: Derleyici bir fonksiyon şablonunu somutlaştırmaya çalışırken, sağlanan tür argümanlarını
 * şablon imzasına yerleştirir. Bu yerleştirme geçersiz bir tür veya ifade üretirse, derleyici
 * hata VERMEZ — o aşırı yüklemeyi (overload) aday kümesinden sessizce çıkarır. Bu SFINAE
 * prensibidir.
 *
 * ===============================================================================
 * 2. std::enable_if<condition, T>
 * ===============================================================================
 * EN: enable_if conditionally defines a nested type `type` only when `condition` is true. When
 * false, the substitution fails (SFINAE) and the overload is discarded. This lets us create
 * separate function overloads for integral vs floating-point vs custom types — all resolved at
 * compile time.
 * TR: enable_if, yalnızca koşul doğru olduğunda iç içe `type` türünü tanımlar. Yanlış olduğunda
 * yerleştirme başarısız olur (SFINAE) ve aşırı yükleme atılır. Böylece tamsayı, ondalıklı ve
 * özel türler için ayrı fonksiyon aşırı yüklemeleri — hepsi derleme zamanında çözülür.
 *
 * ===============================================================================
 * 3. Tag Dispatch
 * ===============================================================================
 * EN: Tag dispatch uses empty struct "tags" as function parameters to guide overload resolution
 * at compile time. The caller passes a tag object, and the compiler selects the matching
 * overload. This gives clean, readable compile-time branching without any enable_if clutter.
 * TR: Etiket dağıtımı, derleme zamanında aşırı yükleme çözümlemesini yönlendirmek için boş yapı
 * "etiketlerini" fonksiyon parametresi olarak kullanır. Çağıran bir etiket nesnesi geçirir,
 * derleyici eşleşen aşırı yüklemeyi seçer.
 *
 * ===============================================================================
 * 4. if constexpr (C++17)
 * ===============================================================================
 * EN: C++17 introduced `if constexpr` — a compile-time if-statement. The branch not taken is
 * completely discarded by the compiler (not even type-checked). This is a much cleaner
 * alternative to SFINAE for simple compile-time branching inside a single function template.
 * TR: C++17 ile gelen `if constexpr`, derleme zamanı if ifadesidir. Alınmayan dal derleyici
 * tarafından tamamen atılır (tür kontrolü bile yapılmaz). Basit derleme zamanı dallanma için
 * SFINAE'den çok daha temiz bir yoldur.
 *
 * ===============================================================================
 * 5. Type Traits Used: std::is_integral, std::is_floating_point, std::is_same
 * ===============================================================================
 * EN: The <type_traits> header provides compile-time boolean queries about types.
 * std::is_integral_v<T> is true for int, long, char, etc. std::is_floating_point_v<T> is true
 * for float, double, long double. std::is_same_v<T, U> checks if T and U are the exact same
 * type. These traits are the building blocks of all SFINAE-based dispatch.
 * TR: <type_traits> başlığı, türler hakkında derleme zamanı sorgulamaları sağlar. Bu özellikler,
 * tüm SFINAE tabanlı dağıtımın yapı taşlarıdır.
 *
 * [CPPREF DEPTH: SFINAE Contexts and the Expression SFINAE Revolution / CPPREF DERİNLİK: SFINAE
 * Bağlamları ve İfade SFINAE Devrimi]
 * =============================================================================
 * EN: SFINAE only operates in the "immediate context" of template argument substitution — errors
 * inside a function body are hard compilation errors, NOT SFINAE. Expression SFINAE (C++11):
 * placing `decltype(expr)` in the return type triggers SFINAE if `expr` is ill-formed.
 * `std::void_t<>` simplifies member detection by mapping any well-formed type list to `void`.
 * `if constexpr` (C++17) eliminates most SFINAE need for branching within a single function
 * body. `requires` expressions (C++20) are the final evolution — readable, composable
 * constraints with clear compiler error messages that replace the arcane enable_if chains.
 *
 * TR: SFINAE yalnızca şablon argüman yerleştirmesinin "yakın bağlamında" çalışır — fonksiyon
 * gövdesindeki hatalar SFINAE değil, kesin derleme hatalarıdır. İfade SFINAE (C++11): dönüş
 * türüne `decltype(expr)` yerleştirildiğinde, `expr` geçersizse SFINAE tetiklenir.
 * `std::void_t<>`, herhangi bir geçerli tür listesini `void`'e eşleyerek üye tespitini
 * kolaylaştırır. `if constexpr` (C++17), tek bir fonksiyon gövdesindeki dallanma için SFINAE
 * ihtiyacını büyük ölçüde ortadan kaldırır. `requires` ifadeleri (C++20) son evrimdir — gizemli
 * enable_if zincirlerinin yerini alan okunabilir, birleştirilebilir kısıtlamalar ve açık hata
 * mesajları.
 *
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <cstdint>      // EN: Fixed-width integer types (uint8_t)
#include <iostream>     // EN: Standard I/O for console output
#include <string>       // EN: std::string for text data
#include <type_traits>  // EN: Type traits (is_integral, enable_if, etc.)
#include <sstream>      // EN: String stream for serialization

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// SECTION 1: SFINAE with std::enable_if — ECU Data Processing
// ═════════════════════════════════════════════════════════════════════════════════════════════════

// EN: Process integral ECU data (raw ADC counts, register values, CAN IDs)
// TR: Tamsayı ECU verilerini işle (ham ADC sayıları, register değerleri, CAN ID'leri)
template <typename T, std::enable_if_t<std::is_integral_v<T>, int> = 0>
void processECUData(T value) {
    std::cout << "  [INTEGRAL path] Raw ECU register value: 0x"
              << std::hex << value << std::dec
              << " (" << value << " decimal)" << std::endl;
}

// EN: Process floating-point ECU data (calibrated sensor readings, voltages)
// TR: Ondalıklı ECU verilerini işle (kalibre sensör okumaları, voltajlar)
template <typename T, std::enable_if_t<std::is_floating_point_v<T>, int> = 0>
void processECUData(T value) {
    std::cout << "  [FLOAT path] Calibrated sensor reading: "
              << value << " V" << std::endl;
}

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// SECTION 2: Tag Dispatch — Sensor Data Pipeline
// ═════════════════════════════════════════════════════════════════════════════════════════════════

// EN: Tag types — empty structs used solely for overload selection
// TR: Etiket türleri — yalnızca aşırı yükleme seçimi için kullanılan boş yapılar
struct RawSensorTag {};
struct CalibratedSensorTag {};

// ─── 2a. Implementation overloads selected by tag ────────────────────────────────────────────────

// EN: Process raw sensor data (needs filtering, offset correction)
// TR: Ham sensör verisini işle (filtreleme, ofset düzeltmesi gerekir)
template <typename T>
void processSensorData(T value, RawSensorTag) {
    constexpr double offset = 0.33;
    constexpr double gain   = 1.05;
    double corrected = static_cast<double>(value) * gain - offset;
    std::cout << "  [RAW] Input: " << value
              << " -> After offset/gain correction: " << corrected << std::endl;
}

// EN: Process calibrated sensor data (already engineering units)
// TR: Kalibre sensör verisini işle (zaten mühendislik birimlerinde)
template <typename T>
void processSensorData(T value, CalibratedSensorTag) {
    std::cout << "  [CALIBRATED] Direct engineering value: " << value
              << " (no correction needed)" << std::endl;
}

// ─── 2b. Public dispatch interface ───────────────────────────────────────────────────────────────

// EN: Caller passes a tag to choose the processing path at compile time
// TR: Çağıran, derleme zamanında işleme yolunu seçmek için bir etiket geçirir
template <typename T, typename Tag>
void dispatchSensor(T value, Tag tag) {
    processSensorData(value, tag);
}

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// SECTION 3: if constexpr (C++17) — Modern Replacement
// ═════════════════════════════════════════════════════════════════════════════════════════════════

// EN: Single function with compile-time branching — no SFINAE boilerplate
// TR: Derleme zamanı dallanmalı tek fonksiyon — SFINAE şablonu gerekmez
template <typename T>
void processModern(T value) {
    if constexpr (std::is_integral_v<T>) {
        std::cout << "  [if constexpr → INTEGRAL] Register: 0x"
                  << std::hex << value << std::dec << std::endl;
    } else if constexpr (std::is_floating_point_v<T>) {
        std::cout << "  [if constexpr → FLOAT] Sensor voltage: "
                  << value << " V" << std::endl;
    } else {
        std::cout << "  [if constexpr → OTHER] Unknown data type, forwarding raw bytes"
                  << std::endl;
    }
}

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// SECTION 4: Has-Member Detection via SFINAE — .toString() check
// ═════════════════════════════════════════════════════════════════════════════════════════════════

// ─── 4a. Detector trait ──────────────────────────────────────────────────────────────────────────

// EN: Primary template — fallback when T does NOT have .toString()
// TR: Birincil şablon — T'de .toString() YOKSA geri dönüş
template <typename T, typename = void>
struct HasToString : std::false_type {};

// EN: Specialization — enabled only when T has a .toString() method
// TR: Uzmanlık — yalnızca T'de .toString() metodu olduğunda etkinleşir
template <typename T>
struct HasToString<T, std::void_t<decltype(std::declval<T>().toString())>>
    : std::true_type {};

// ─── 4b. Serialize function using the detector ───────────────────────────────────────────────────

// EN: If T has .toString(), use it; otherwise fall back to stream insertion
// TR: T'de .toString() varsa onu kullan; yoksa akış ekleme operatörüne dön
template <typename T>
auto serialize(const T& val) -> std::string {
    if constexpr (HasToString<T>::value) {
        return val.toString();
    } else {
        std::ostringstream oss;
        oss << val;
        return oss.str();
    }
}

// ─── 4c. Test types ──────────────────────────────────────────────────────────────────────────────

// EN: Automotive diagnostic trouble code — has .toString()
// TR: Otomotiv arıza kodu — .toString() metodu var
struct DiagnosticCode {
    int code;
    std::string module;

    std::string toString() const {
        return "DTC-" + module + "-" + std::to_string(code);
    }
};

// EN: Plain voltage reading — no .toString(), relies on operator<<
// TR: Düz voltaj okuması — .toString() yok, operator<< kullanır
struct VoltageReading {
    double volts;

    friend std::ostream& operator<<(std::ostream& os, const VoltageReading& vr) {
        return os << vr.volts << "V";
    }
};

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// SECTION 5: Compile-Time Error Prevention Demo Types
// ═════════════════════════════════════════════════════════════════════════════════════════════════

// EN: A constrained function that only accepts pointer types (e.g., memory-mapped I/O)
// TR: Yalnızca işaretçi türlerini kabul eden kısıtlı fonksiyon (örn. bellek eşlemeli G/Ç)
template <typename T, std::enable_if_t<std::is_pointer_v<T>, int> = 0>
void writeMemoryMappedIO(T addr, uint8_t value) {
    // EN: In production, this would write to a hardware register
    // TR: Üretimde bu, bir donanım kaydına yazardı
    std::cout << "  [MMIO] Writing 0x" << std::hex << static_cast<int>(value)
              << std::dec << " to address " << static_cast<const void*>(addr)
              << std::endl;
}

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// MAIN — Demonstrations
// ═════════════════════════════════════════════════════════════════════════════════════════════════

int main() {
    std::cout << "╔══════════════════════════════════════════════════════════════╗\n";
    std::cout << "║  MODULE 09: SFINAE, Tag Dispatch & if constexpr            ║\n";
    std::cout << "╚══════════════════════════════════════════════════════════════╝\n\n";

    // ─── Demo 1: SFINAE enable_if — integral vs floating-point paths ─────────────────────────────
    std::cout << "── Demo 1: SFINAE enable_if (ECU Data Processing) ──\n";
    // EN: Integral type → hex register path
    // TR: Tamsayı türü → hex register yolu
    processECUData(0x1A3F);
    // EN: Floating-point type → calibrated sensor path
    // TR: Ondalıklı tür → kalibre sensör yolu
    processECUData(3.28);
    processECUData(255);
    processECUData(1.5f);
    std::cout << std::endl;

    // ─── Demo 2: Tag dispatch — raw vs calibrated sensor data ────────────────────────────────────
    std::cout << "── Demo 2: Tag Dispatch (Sensor Pipeline) ──\n";
    // EN: Raw ADC reading needs offset/gain correction
    // TR: Ham ADC okuması ofset/kazanç düzeltmesi gerektirir
    dispatchSensor(1023, RawSensorTag{});
    dispatchSensor(2.48, RawSensorTag{});
    // EN: Calibrated value already in engineering units
    // TR: Kalibre değer zaten mühendislik birimlerinde
    dispatchSensor(98.6, CalibratedSensorTag{});
    dispatchSensor(14.7, CalibratedSensorTag{});
    std::cout << std::endl;

    // ─── Demo 3: if constexpr — modern compile-time branching ────────────────────────────────────
    std::cout << "── Demo 3: if constexpr (Modern Replacement) ──\n";
    // EN: Same function, different compile-time branches based on type
    // TR: Aynı fonksiyon, türe göre farklı derleme zamanı dalları
    processModern(0xFF);
    processModern(4.75);
    processModern("CAN_BUS_MSG");
    std::cout << std::endl;

    // ─── Demo 4: Has-member detection — .toString() check ────────────────────────────────────────
    std::cout << "── Demo 4: Has-Member Detection (.toString()) ──\n";
    DiagnosticCode dtc{4201, "ECM"};
    VoltageReading vr{12.6};

    // EN: DiagnosticCode has .toString() → uses it
    // TR: DiagnosticCode .toString() metoduna sahip → onu kullanır
    std::cout << "  DiagnosticCode serialized: " << serialize(dtc) << std::endl;
    // EN: VoltageReading has no .toString() → uses operator<<
    // TR: VoltageReading .toString() yok → operator<< kullanır
    std::cout << "  VoltageReading serialized: " << serialize(vr) << std::endl;
    // EN: Plain int has no .toString() → uses operator<<
    // TR: Düz int .toString() yok → operator<< kullanır
    std::cout << "  int serialized:            " << serialize(42) << std::endl;

    // EN: Compile-time trait verification
    // TR: Derleme zamanı özellik doğrulaması
    static_assert(HasToString<DiagnosticCode>::value,
                  "DiagnosticCode must have .toString()");
    static_assert(!HasToString<VoltageReading>::value,
                  "VoltageReading must NOT have .toString()");
    static_assert(!HasToString<int>::value,
                  "int must NOT have .toString()");
    std::cout << std::endl;

    // ─── Demo 5: Compile-time error prevention ───────────────────────────────────────────────────
    std::cout << "── Demo 5: Compile-Time Error Prevention (MMIO) ──\n";
    uint8_t fake_register = 0;
    // EN: Pointer type → SFINAE allows the call
    // TR: İşaretçi türü → SFINAE çağrıya izin verir
    writeMemoryMappedIO(&fake_register, 0xAB);

    // EN: Uncommenting the line below would cause a compile error — SFINAE removes the overload
    // because int is not a pointer type.
    // TR: Aşağıdaki satırın yorumunu kaldırmak derleme hatasına neden olur — SFINAE, int bir
    // işaretçi türü olmadığı için aşırı yüklemeyi kaldırır. writeMemoryMappedIO(0x4000'0000,
    // 0xAB);  // ERROR: int is not a pointer!

    std::cout << "  (Passing a non-pointer to writeMemoryMappedIO is a compile error)\n";
    std::cout << std::endl;

    // ─── Summary ─────────────────────────────────────────────────────────────────────────────────
    std::cout << "╔══════════════════════════════════════════════════════════════╗\n";
    std::cout << "║  SUMMARY / ÖZET                                            ║\n";
    std::cout << "╠══════════════════════════════════════════════════════════════╣\n";
    std::cout << "║  1. SFINAE + enable_if: powerful but verbose               ║\n";
    std::cout << "║  2. Tag dispatch: clean compile-time overload selection     ║\n";
    std::cout << "║  3. if constexpr (C++17): simplest for inline branching    ║\n";
    std::cout << "║  4. Has-member detection: SFINAE metadata introspection    ║\n";
    std::cout << "║  5. For C++20 projects, prefer Concepts over SFINAE        ║\n";
    std::cout << "╚══════════════════════════════════════════════════════════════╝\n";

    return 0;
}
