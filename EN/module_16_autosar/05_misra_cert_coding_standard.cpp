/**
 * @file module_16_autosar/05_misra_cert_coding_standard.cpp
 * @brief MISRA C++ 2023 & CERT C++
 *
 * @details
 * =============================================================================
 * [THEORY: MISRA C++ 2023 — Overview]
 * =============================================================================
 *
 * EN: MISRA C++ 2023 is the automotive industry coding standard for C++17.
 *     It merges and replaces:
 *     - MISRA C++ 2008 (C++03-based)
 *     - AUTOSAR C++ Coding Guidelines 2019 (C++14-based)
 *
 *     Rule Categories:
 *     ┌─────────────┬──────────────────────────────────────────────────┐
 *     │ Category    │ Description                                      │
 *     ├─────────────┼──────────────────────────────────────────────────┤
 *     │ Mandatory   │ Must be followed — no deviations allowed         │
 *     │ Required    │ Must be followed — deviations need justification │
 *     │ Advisory    │ Should be followed — best practice               │
 *     └─────────────┴──────────────────────────────────────────────────┘
 *
 *     Key MISRA C++ 2023 Restrictions:
 *     ┌─────────────────────────────────┬──────────────────────────────┐
 *     │ Feature                         │ MISRA C++ 2023 Stance        │
 *     ├─────────────────────────────────┼──────────────────────────────┤
 *     │ Exceptions (throw/catch)        │ FORBIDDEN (no dynamic path)  │
 *     │ RTTI (dynamic_cast, typeid)     │ FORBIDDEN (runtime overhead) │
 *     │ Dynamic allocation (new/delete) │ RESTRICTED (init phase only) │
 *     │ goto                            │ FORBIDDEN                    │
 *     │ C-style casts                   │ FORBIDDEN                    │
 *     │ Implicit narrowing conversions  │ FORBIDDEN                    │
 *     │ Unions with non-trivial members │ FORBIDDEN                    │
 *     │ setjmp/longjmp                  │ FORBIDDEN                    │
 *     ├─────────────────────────────────┼──────────────────────────────┤
 *     │ constexpr / consteval           │ ENCOURAGED                   │
 *     │ auto type deduction             │ ALLOWED (with care)          │
 *     │ Range-based for                 │ ENCOURAGED                   │
 *     │ Smart pointers (unique_ptr)     │ ALLOWED                      │
 *     │ std::optional, std::variant     │ ALLOWED                      │
 *     │ Templates / CRTP                │ ALLOWED                      │
 *     │ Lambda expressions              │ ALLOWED (stateless preferred)│
 *     │ Structured bindings             │ ALLOWED                      │
 *     │ if constexpr                    │ ENCOURAGED                   │
 *     └─────────────────────────────────┴──────────────────────────────┘
 *
 * =============================================================================
 * [THEORY: CERT C++ Secure Coding Standard]
 * =============================================================================
 *
 * EN: CERT C++ focuses on security vulnerabilities:
 *     - Buffer overflow prevention (bounds checking)
 *     - Integer overflow/underflow detection
 *     - Resource management (RAII, no leaks)
 *     - Null pointer dereference prevention
 *     - Thread safety (data race prevention)
 *
 * =============================================================================
 * [THEORY: Static Analysis Tools]
 * =============================================================================
 *
 * EN:
 *     ┌──────────────────────┬──────────────────────────────────────────┐
 *     │ Tool                 │ Key Features                             │
 *     ├──────────────────────┼──────────────────────────────────────────┤
 *     │ cppcheck             │ Open-source, MISRA addon available       │
 *     │ clang-tidy           │ LLVM-based, CERT/MISRA checks            │
 *     │ Parasoft C++test     │ Commercial, MISRA/CERT/AUTOSAR           │
 *     │ Polyspace (MathWorks)│ Proves absence of runtime errors         │
 *     │ QA·C++ (Perforce)    │ MISRA C++ reference checker              │
 *     │ Coverity (Synopsys)  │ Large-scale defect detection             │
 *     │ PVS-Studio           │ Static analysis, MISRA support           │
 *     └──────────────────────┴──────────────────────────────────────────┘
 *
 * @note Compile: g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion 05_misra_cert_coding_standard.cpp -o 05_misra_cert
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
#include <iomanip>
#include <algorithm>
#include <cassert>
#include <optional>
#include <variant>
#include <memory>
#include <limits>
#include <type_traits>
#include <numeric>

// ═════════════════════════════════════════════════════════════════════════════
// PART 1: MISRA C++ 2023 — Compliant vs Non-Compliant Examples
// ═════════════════════════════════════════════════════════════════════════════

namespace misra_examples {

// -------------------------------------------------------------------------
// Rule: "No implicit narrowing conversions" (Required)
// -------------------------------------------------------------------------

// EN: NON-COMPLIANT — implicit narrowing from int to uint8_t
/*
    void bad_narrowing() {
        int value = 300;
        uint8_t byte = value;  // MISRA VIOLATION: narrowing conversion
    }
*/

// EN: COMPLIANT — explicit cast with range check
inline std::optional<uint8_t> safe_narrow(int value) {
    if (value < 0 || value > 255) return std::nullopt;
    return static_cast<uint8_t>(value);
}

// -------------------------------------------------------------------------
// Rule: "No C-style casts" (Required)
// -------------------------------------------------------------------------

// EN: NON-COMPLIANT: int x = (int)ptr;
// EN: COMPLIANT: use static_cast, const_cast, reinterpret_cast explicitly

// -------------------------------------------------------------------------
// Rule: "No exceptions (throw/catch)" (Required in safety code)
// -------------------------------------------------------------------------

// EN: Error handling WITHOUT exceptions — return codes + std::optional

enum class ErrorCode : uint8_t {
    OK = 0, INVALID_PARAM, OUT_OF_RANGE, TIMEOUT, HW_FAULT
};

template <typename T>
struct Result {
    T          value;
    ErrorCode  error;

    [[nodiscard]] bool IsOk() const { return error == ErrorCode::OK; }
    static Result Ok(T val) { return {std::move(val), ErrorCode::OK}; }
    static Result Fail(ErrorCode e) { return {T{}, e}; }
};

// EN: MISRA-compliant sensor read (no exceptions)
inline Result<uint16_t> ReadSensorValue(uint8_t channel) {
    if (channel > 7)
        return Result<uint16_t>::Fail(ErrorCode::INVALID_PARAM);
    // EN: Simulate ADC read
    uint16_t raw = static_cast<uint16_t>(1024u + channel * 100u);
    if (raw > 4095)
        return Result<uint16_t>::Fail(ErrorCode::OUT_OF_RANGE);
    return Result<uint16_t>::Ok(raw);
}

// -------------------------------------------------------------------------
// Rule: "All switch cases shall have break or return" (Required)
// -------------------------------------------------------------------------

enum class GearPosition : uint8_t { PARK, REVERSE, NEUTRAL, DRIVE };

inline std::string_view GearToString(GearPosition gear) {
    switch (gear) {
        case GearPosition::PARK:    return "P";
        case GearPosition::REVERSE: return "R";
        case GearPosition::NEUTRAL: return "N";
        case GearPosition::DRIVE:   return "D";
        // EN: MISRA: ALL enum values must be handled; default optional for enums
    }
    return "?";  // unreachable if all enums covered
}

// -------------------------------------------------------------------------
// Rule: "Use constexpr for compile-time constants" (Advisory)
// -------------------------------------------------------------------------

// EN: NON-COMPLIANT: #define MAX_RPM 8000
// EN: COMPLIANT: constexpr
constexpr uint16_t MAX_RPM = 8000;
constexpr double   THROTTLE_MIN = 0.0;
constexpr double   THROTTLE_MAX = 100.0;

// EN: constexpr function — computed at compile time
constexpr uint32_t CRC32_POLYNOMIAL = 0xEDB88320u;

constexpr std::array<uint32_t, 256> GenerateCRC32Table() {
    std::array<uint32_t, 256> table{};
    for (uint32_t i = 0; i < 256; ++i) {
        uint32_t crc = i;
        for (int j = 0; j < 8; ++j) {
            if ((crc & 1u) != 0)
                crc = (crc >> 1) ^ CRC32_POLYNOMIAL;
            else
                crc >>= 1;
        }
        table[i] = crc;
    }
    return table;
}

constexpr auto CRC32_TABLE = GenerateCRC32Table();

inline uint32_t ComputeCRC32(const uint8_t* data, size_t length) {
    uint32_t crc = 0xFFFFFFFFu;
    for (size_t i = 0; i < length; ++i) {
        uint8_t index = static_cast<uint8_t>((crc ^ data[i]) & 0xFFu);
        crc = (crc >> 8) ^ CRC32_TABLE[index];
    }
    return crc ^ 0xFFFFFFFFu;
}

// -------------------------------------------------------------------------
// Rule: "No dynamic_cast or typeid" (Required for safety)
// -------------------------------------------------------------------------

// EN: Use CRTP instead of virtual + dynamic_cast

template <typename Derived>
class SensorBase {
public:
    uint16_t Read() {
        return static_cast<Derived*>(this)->ReadImpl();
    }
    std::string_view Name() {
        return static_cast<Derived*>(this)->NameImpl();
    }
};

class TemperatureSensor : public SensorBase<TemperatureSensor> {
public:
    uint16_t ReadImpl() { return 2048; }  // ~25°C
    static std::string_view NameImpl() { return "TempSensor"; }
};

class PressureSensor : public SensorBase<PressureSensor> {
public:
    uint16_t ReadImpl() { return 3500; }  // ~101.3 kPa
    static std::string_view NameImpl() { return "PressSensor"; }
};

}  // namespace misra_examples

// ═════════════════════════════════════════════════════════════════════════════
// PART 2: CERT C++ — Secure Coding Patterns
// ═════════════════════════════════════════════════════════════════════════════

namespace cert_examples {

// -------------------------------------------------------------------------
// INT32-C: Ensure integer operations don't overflow
// -------------------------------------------------------------------------

// EN: Safe integer arithmetic with overflow detection
inline std::optional<int32_t> SafeAdd(int32_t a, int32_t b) {
    if (b > 0 && a > std::numeric_limits<int32_t>::max() - b) return std::nullopt;
    if (b < 0 && a < std::numeric_limits<int32_t>::min() - b) return std::nullopt;
    return a + b;
}

inline std::optional<int32_t> SafeMultiply(int32_t a, int32_t b) {
    if (a == 0 || b == 0) return 0;
    if (a > 0 && b > 0 && a > std::numeric_limits<int32_t>::max() / b) return std::nullopt;
    if (a > 0 && b < 0 && b < std::numeric_limits<int32_t>::min() / a) return std::nullopt;
    if (a < 0 && b > 0 && a < std::numeric_limits<int32_t>::min() / b) return std::nullopt;
    if (a < 0 && b < 0 && a < std::numeric_limits<int32_t>::max() / b) return std::nullopt;
    return a * b;
}

// -------------------------------------------------------------------------
// ARR30-C: Do not form or use out-of-bounds references
// -------------------------------------------------------------------------

// EN: Bounds-checked array access
template <typename T, size_t N>
class SafeArray {
    std::array<T, N> data_{};

public:
    std::optional<T> At(size_t index) const {
        if (index >= N) return std::nullopt;
        return data_[index];
    }

    bool Set(size_t index, T value) {
        if (index >= N) return false;
        data_[index] = std::move(value);
        return true;
    }

    [[nodiscard]] constexpr size_t Size() const { return N; }
};

// -------------------------------------------------------------------------
// MEM50-CPP: Use RAII for resource management
// -------------------------------------------------------------------------

// EN: RAII-based peripheral lock (no manual unlock needed)
class PeripheralLock {
    uint32_t peripheral_id_;
    bool     locked_ = false;

    static void HW_Lock(uint32_t id) {
        std::cout << "    [HW] Peripheral 0x" << std::hex << id
                  << std::dec << " LOCKED\n";
    }
    static void HW_Unlock(uint32_t id) {
        std::cout << "    [HW] Peripheral 0x" << std::hex << id
                  << std::dec << " UNLOCKED\n";
    }

public:
    explicit PeripheralLock(uint32_t id) : peripheral_id_(id), locked_(true) {
        HW_Lock(peripheral_id_);
    }
    ~PeripheralLock() {
        if (locked_) HW_Unlock(peripheral_id_);
    }
    PeripheralLock(const PeripheralLock&) = delete;
    PeripheralLock& operator=(const PeripheralLock&) = delete;
    PeripheralLock(PeripheralLock&& other) noexcept
        : peripheral_id_(other.peripheral_id_), locked_(other.locked_) {
        other.locked_ = false;
    }
    PeripheralLock& operator=(PeripheralLock&&) = delete;
};

// -------------------------------------------------------------------------
// EXP34-C: Do not dereference null pointers
// -------------------------------------------------------------------------

// EN: Null-safe pointer wrapper
template <typename T>
class NonNull {
    T* ptr_;
public:
    explicit NonNull(T* p) : ptr_(p) { assert(p != nullptr); }
    T& operator*() const { return *ptr_; }
    T* operator->() const { return ptr_; }
    [[nodiscard]] T* Get() const { return ptr_; }
};

}  // namespace cert_examples

// ═════════════════════════════════════════════════════════════════════════════
// PART 3: Compliant vs Non-Compliant Code Comparison
// ═════════════════════════════════════════════════════════════════════════════

namespace comparison {

// EN: Example: Motor speed controller — showing compliant vs non-compliant

// --- NON-COMPLIANT version (shown as comments for reference) ---
/*
class BadMotorController {
    int speed;              // MISRA: missing initializer
    int* target;            // MISRA: raw pointer, no RAII
public:
    void setSpeed(int s) {  // MISRA: no range check
        speed = s;          // MISRA: possible overflow
        *target = s;        // CERT: possible null dereference
    }
    int getSpeed() {        // MISRA: should be const, [[nodiscard]]
        return speed;
    }
};
*/

// --- COMPLIANT version ---
class MotorController {
    uint16_t speed_rpm_ = 0;
    static constexpr uint16_t MAX_SPEED = 6000;

public:
    [[nodiscard]] misra_examples::ErrorCode SetSpeed(uint16_t target_rpm) {
        if (target_rpm > MAX_SPEED)
            return misra_examples::ErrorCode::OUT_OF_RANGE;
        speed_rpm_ = target_rpm;
        return misra_examples::ErrorCode::OK;
    }

    [[nodiscard]] uint16_t GetSpeed() const { return speed_rpm_; }

    [[nodiscard]] static constexpr uint16_t GetMaxSpeed() { return MAX_SPEED; }
};

// EN: Type-safe register access (MISRA compliant)
template <uint32_t Address, uint8_t Offset, uint8_t Width>
struct RegisterField {
    static constexpr uint32_t MASK = ((1u << Width) - 1u) << Offset;

    static uint32_t Read(const uint32_t reg_value) {
        return (reg_value & MASK) >> Offset;
    }

    static uint32_t Write(uint32_t reg_value, uint32_t field_value) {
        reg_value &= ~MASK;
        reg_value |= (field_value << Offset) & MASK;
        return reg_value;
    }
};

}  // namespace comparison

// ═════════════════════════════════════════════════════════════════════════════
// PART 4: MISRA Critical Rules Reference
// ═════════════════════════════════════════════════════════════════════════════

namespace misra_rules {

struct MISRARule {
    std::string rule_id;
    std::string category;   // Mandatory/Required/Advisory
    std::string description;
    std::string rationale;
};

inline std::vector<MISRARule> GetCriticalRules() {
    return {
        {"Rule 0.1.1", "Required",  "All code shall be reachable",
         "Dead code indicates logic error"},
        {"Rule 0.1.2", "Required",  "All values shall be used",
         "Unused values indicate logic error"},
        {"Rule 4.1.3", "Required",  "No undefined/unspecified behavior",
         "Deterministic behavior required"},
        {"Rule 6.0.1", "Mandatory", "No implicit narrowing conversions",
         "Data loss prevention"},
        {"Rule 6.2.1", "Required",  "Signed/unsigned mixed arithmetic forbidden",
         "Unexpected results prevention"},
        {"Rule 6.4.1", "Required",  "No C-style casts",
         "Type safety enforcement"},
        {"Rule 6.8.1", "Required",  "No goto statements",
         "Structured control flow"},
        {"Rule 7.0.1", "Required",  "No dynamic memory after init",
         "Deterministic memory usage"},
        {"Rule 8.2.5", "Required",  "Functions shall have [[nodiscard]] if return matters",
         "Prevent ignoring error codes"},
        {"Rule 11.6.1","Required",  "No #define for constants (use constexpr)",
         "Type safety enforcement"},
        {"Rule 13.3.1","Required",  "No throw/catch in safety code",
         "Deterministic control flow"},
        {"Rule 14.1.1","Required",  "No RTTI (dynamic_cast/typeid)",
         "No runtime overhead"},
        {"Rule 15.0.1","Required",  "All switch cases shall break/return",
         "Prevent fallthrough bugs"},
        {"Rule 18.3.1","Required",  "No setjmp/longjmp",
         "No non-local jumps"},
        {"Rule 21.6.1","Required",  "No C standard I/O (printf/scanf)",
         "Type-unsafe functions"},
    };
}

}  // namespace misra_rules

// ═════════════════════════════════════════════════════════════════════════════
// MAIN — Demos
// ═════════════════════════════════════════════════════════════════════════════

int main() {
    std::cout << "╔══════════════════════════════════════════════════════════════╗\n"
              << "║ Module 16: MISRA C++ 2023 & CERT C++ Coding Standards        ║\n"
              << "╚══════════════════════════════════════════════════════════════╝\n\n";

    // --- Demo 1: Safe Narrowing Conversion ---
    std::cout << "--- Demo 1: Safe Narrowing Conversion (MISRA Rule 6.0.1) ---\n";
    {
        int test_values[] = {100, 255, 256, -1, 0};
        for (int v : test_values) {
            auto result = misra_examples::safe_narrow(v);
            std::cout << "  safe_narrow(" << std::setw(4) << v << ") → "
                      << (result ? std::to_string(*result) : "NONE (overflow)") << "\n";
        }
    }

    // --- Demo 2: Error Handling Without Exceptions ---
    std::cout << "\n--- Demo 2: Error Handling Without Exceptions (MISRA Rule 13.3.1) ---\n";
    {
        for (uint8_t ch = 0; ch < 10; ++ch) {
            auto result = misra_examples::ReadSensorValue(ch);
            std::cout << "  Channel " << static_cast<int>(ch) << ": "
                      << (result.IsOk() ? "value=" + std::to_string(result.value)
                                        : "ERROR(" + std::to_string(static_cast<int>(result.error)) + ")")
                      << "\n";
        }
    }

    // --- Demo 3: constexpr CRC32 Table ---
    std::cout << "\n--- Demo 3: constexpr CRC32 (MISRA Rule 11.6.1) ---\n";
    {
        std::cout << "  CRC32 table (first 8 entries, compile-time generated):\n  ";
        for (int i = 0; i < 8; ++i)
            std::cout << "0x" << std::hex << misra_examples::CRC32_TABLE[static_cast<size_t>(i)] << " ";
        std::cout << std::dec << "\n";

        std::array<uint8_t, 5> data = {0x48, 0x65, 0x6C, 0x6C, 0x6F};  // "Hello"
        auto crc = misra_examples::ComputeCRC32(data.data(), data.size());
        std::cout << "  CRC32(\"Hello\") = 0x" << std::hex << crc << std::dec << "\n";
    }

    // --- Demo 4: CRTP Instead of dynamic_cast ---
    std::cout << "\n--- Demo 4: CRTP Sensors — No RTTI (MISRA Rule 14.1.1) ---\n";
    {
        misra_examples::TemperatureSensor temp;
        misra_examples::PressureSensor    press;
        std::cout << "  " << temp.Name() << " = " << temp.Read() << "\n";
        std::cout << "  " << press.Name() << " = " << press.Read() << "\n";
    }

    // --- Demo 5: CERT — Safe Integer Arithmetic ---
    std::cout << "\n--- Demo 5: CERT INT32-C — Safe Integer Arithmetic ---\n";
    {
        auto r1 = cert_examples::SafeAdd(std::numeric_limits<int32_t>::max(), 1);
        std::cout << "  MAX_INT + 1 = " << (r1 ? std::to_string(*r1) : "OVERFLOW") << "\n";

        auto r2 = cert_examples::SafeAdd(100, 200);
        std::cout << "  100 + 200 = " << (r2 ? std::to_string(*r2) : "OVERFLOW") << "\n";

        auto r3 = cert_examples::SafeMultiply(100000, 100000);
        std::cout << "  100000 * 100000 = " << (r3 ? std::to_string(*r3) : "OVERFLOW") << "\n";

        auto r4 = cert_examples::SafeMultiply(1000, 2000);
        std::cout << "  1000 * 2000 = " << (r4 ? std::to_string(*r4) : "OVERFLOW") << "\n";
    }

    // --- Demo 6: CERT — Bounds-Checked Array ---
    std::cout << "\n--- Demo 6: CERT ARR30-C — Bounds-Checked Array ---\n";
    {
        cert_examples::SafeArray<int, 5> arr;
        arr.Set(0, 10);
        arr.Set(4, 50);
        bool oob = arr.Set(5, 60);  // out of bounds
        std::cout << "  arr[0] = " << (arr.At(0) ? std::to_string(*arr.At(0)) : "NONE") << "\n";
        std::cout << "  arr[4] = " << (arr.At(4) ? std::to_string(*arr.At(4)) : "NONE") << "\n";
        std::cout << "  arr[5] = " << (arr.At(5) ? "???" : "NONE (out of bounds)") << "\n";
        std::cout << "  Set(5) = " << (oob ? "OK" : "REJECTED") << "\n";
    }

    // --- Demo 7: CERT — RAII Resource Management ---
    std::cout << "\n--- Demo 7: CERT MEM50-CPP — RAII Peripheral Lock ---\n";
    {
        std::cout << "  Entering scope:\n";
        {
            cert_examples::PeripheralLock lock(0x40020000);
            std::cout << "    ... doing SPI transfer ...\n";
            // lock auto-released at scope exit
        }
        std::cout << "  Scope exited — lock released automatically\n";
    }

    // --- Demo 8: Motor Controller (Compliant Example) ---
    std::cout << "\n--- Demo 8: MISRA-Compliant Motor Controller ---\n";
    {
        comparison::MotorController motor;
        auto err1 = motor.SetSpeed(3000);
        std::cout << "  SetSpeed(3000) → "
                  << (err1 == misra_examples::ErrorCode::OK ? "OK" : "ERROR")
                  << "  speed=" << motor.GetSpeed() << "\n";

        auto err2 = motor.SetSpeed(7000);
        std::cout << "  SetSpeed(7000) → "
                  << (err2 == misra_examples::ErrorCode::OK ? "OK" : "OUT_OF_RANGE")
                  << "  speed=" << motor.GetSpeed() << "\n";

        std::cout << "  Max speed (constexpr) = "
                  << comparison::MotorController::GetMaxSpeed() << "\n";
    }

    // --- Demo 9: Type-Safe Register Field ---
    std::cout << "\n--- Demo 9: Type-Safe Register Access ---\n";
    {
        using ModeField = comparison::RegisterField<0x40020000, 0, 4>;
        using SpeedField = comparison::RegisterField<0x40020000, 4, 2>;

        uint32_t reg = 0x00000000;
        reg = ModeField::Write(reg, 0x05);
        reg = SpeedField::Write(reg, 0x02);
        std::cout << "  Register = 0x" << std::hex << reg << std::dec << "\n";
        std::cout << "  Mode  field = " << ModeField::Read(reg) << "\n";
        std::cout << "  Speed field = " << SpeedField::Read(reg) << "\n";
    }

    // --- Demo 10: MISRA Critical Rules Reference Table ---
    std::cout << "\n--- Demo 10: MISRA C++ 2023 Critical Rules ---\n";
    {
        auto rules = misra_rules::GetCriticalRules();
        std::cout << "  ┌─────────────┬───────────┬──────────────────────────────────────────────────────┐\n";
        std::cout << "  │ Rule        │ Category  │ Description                                          │\n";
        std::cout << "  ├─────────────┼───────────┼──────────────────────────────────────────────────────┤\n";
        for (const auto& r : rules) {
            std::cout << "  │ " << std::setw(11) << std::left << r.rule_id
                      << " │ " << std::setw(9) << r.category
                      << " │ " << std::setw(52) << r.description << " │\n";
        }
        std::cout << std::right;
        std::cout << "  └─────────────┴───────────┴──────────────────────────────────────────────────────┘\n";
    }

    std::cout << "\n[All demos completed successfully.]\n";
    return 0;
}
