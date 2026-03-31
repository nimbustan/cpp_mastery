/**
 * @file module_05_certification/07_error_handling_advanced.cpp
 * @brief Advanced Error Handling
 *
 * @details
 * =============================================================================
 * [THEORY: Error Handling Strategies in C++]
 * =============================================================================
 *
 * EN: C++ offers multiple error handling approaches. Each has trade-offs:
 *
 *     ┌────────────────────────┬──────────┬───────────┬───────────────────────┐
 *     │ Approach               │ Speed    │ Safety    │ When to Use           │
 *     ├────────────────────────┼──────────┼───────────┼───────────────────────┤
 *     │ Exceptions             │ Zero-cost│ High      │ Truly exceptional     │
 *     │                        │ (happy   │           │ (file not found, OOM) │
 *     │                        │  path)   │           │                       │
 *     ├────────────────────────┼──────────┼───────────┼───────────────────────┤
 *     │ Return codes (int)     │ Fast     │ Low       │ C interop, embedded   │
 *     │                        │          │ (ignored) │ (no exceptions)       │
 *     ├────────────────────────┼──────────┼───────────┼───────────────────────┤
 *     │ std::error_code        │ Fast     │ Medium    │ System/OS errors      │
 *     │                        │          │           │ (errno, HRESULT)      │
 *     ├────────────────────────┼──────────┼───────────┼───────────────────────┤
 *     │ std::optional<T>       │ Fast     │ High      │ May or may not have   │
 *     │                        │          │           │ a value (no error)    │
 *     ├────────────────────────┼──────────┼───────────┼───────────────────────┤
 *     │ std::expected<T,E>     │ Fast     │ High      │ Value-or-error,       │
 *     │ (C++23)                │          │           │ Rust Result<T,E> like │
 *     └────────────────────────┴──────────┴───────────┴───────────────────────┘
 *
 * =============================================================================
 * [THEORY: std::error_code]
 * =============================================================================
 *
 * EN: The <system_error> framework provides:
 *
 *     std::error_code — A lightweight, non-throwing error container:
 *       - Holds a numeric code + category pointer
 *       - Category identifies the error domain (system, generic, custom)
 *       - Pattern: function returns error_code, caller checks
 *
 *     std::error_condition — Platform-independent error representation
 *     std::error_category — Base class for error domains
 *     std::system_error — Exception that wraps error_code
 *
 *     Why system_error over raw int?
 *     - Type-safe: can't mix errno with HRESULT
 *     - Portable: std::errc maps to platform-specific codes
 *     - Composable: Custom categories for your domain
 *
 * =============================================================================
 * [THEORY: std::expected<T,E> — The Modern Way (C++23)]
 * =============================================================================
 *
 * EN: std::expected<T,E> holds either a T value or an E error:
 *
 *     auto result = parse_sensor_data(raw);
 *     if (result.has_value()) {
 *         use(result.value());    // or *result
 *     } else {
 *         handle(result.error());  // The error
 *     }
 *
 *     Monadic operations (C++23):
 *     result.and_then(f)   — If value, apply f (returns expected)
 *     result.transform(f)  — If value, apply f (returns wrapped value)
 *     result.or_else(f)    — If error, apply f (returns expected)
 *
 *     This enables functional-style chaining:
 *     auto final = read_file("data.bin")
 *                    .and_then(parse_json)
 *                    .transform(extract_temperature)
 *                    .or_else(log_and_default);
 *
 * @note Compile: g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion 07_error_handling_advanced.cpp -o 07_error_handling_advanced
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <iostream>
#include <string>
#include <vector>
#include <system_error>
#include <variant>
#include <optional>
#include <functional>
#include <cmath>
#include <cerrno>
#include <cstring>
#include <cstdint>

// ═════════════════════════════════════════════════════════════════════════════
// PART 1: std::error_code and Custom Error Categories
// ═════════════════════════════════════════════════════════════════════════════

// EN: Define a custom error category for our sensor subsystem
enum class SensorError {
    Success = 0,
    Timeout = 1,
    OutOfRange = 2,
    CalibrationFailed = 3,
    HardwareFault = 4
};

// EN: Custom error category — describes our error domain
class SensorErrorCategory : public std::error_category {
public:
    const char* name() const noexcept override {
        return "SensorError";
    }

    std::string message(int ev) const override {
        switch (static_cast<SensorError>(ev)) {
            case SensorError::Success:           return "Success";
            case SensorError::Timeout:           return "Sensor read timeout";
            case SensorError::OutOfRange:        return "Value out of valid range";
            case SensorError::CalibrationFailed: return "Calibration failed";
            case SensorError::HardwareFault:     return "Hardware fault detected";
            default:                             return "Unknown sensor error";
        }
    }
};

// EN: Global category instance (singleton pattern)
const SensorErrorCategory& sensor_error_category() {
    static SensorErrorCategory instance;
    return instance;
}

std::error_code make_error_code(SensorError e) {
    return {static_cast<int>(e), sensor_error_category()};
}

// EN: Register SensorError with std::error_code system
namespace std {
    template<>
    struct is_error_code_enum<SensorError> : true_type {};
}

// EN: Function that returns error_code instead of throwing
std::error_code read_sensor(int sensor_id, double& value) {
    if (sensor_id < 0) {
        return SensorError::HardwareFault;
    }
    if (sensor_id > 10) {
        return SensorError::Timeout;
    }
    value = 25.0 + static_cast<double>(sensor_id) * 1.5;
    if (value > 35.0) {
        return SensorError::OutOfRange;
    }
    return {};  // EN: Default-constructed = success
}

// ═════════════════════════════════════════════════════════════════════════════
// PART 2: Expected<T,E> — C++23 Polyfill
// ═════════════════════════════════════════════════════════════════════════════

// EN: Simplified std::expected polyfill for C++17 demonstration
//     Real C++23 std::expected has more features, but the pattern is the same
template<typename T, typename E>
class Expected {
public:
    // EN: Value constructors — use in_place_index to disambiguate when T==E
    Expected(T value) : data_(std::in_place_index<0>, std::move(value)) {}

    // EN: Error constructor — use tag type to disambiguate
    struct unexpect_t {};
    Expected(unexpect_t, E error) : data_(std::in_place_index<1>, std::move(error)) {}

    // EN: Static factory for errors
    static Expected unexpected(E error) {
        return Expected(unexpect_t{}, std::move(error));
    }

    bool has_value() const { return data_.index() == 0; }
    explicit operator bool() const { return has_value(); }

    T& value() { return std::get<0>(data_); }
    const T& value() const { return std::get<0>(data_); }

    E& error() { return std::get<1>(data_); }
    const E& error() const { return std::get<1>(data_); }

    T& operator*() { return value(); }
    const T& operator*() const { return value(); }

    // EN: Monadic operations (C++23 style)
    // and_then: If value, apply f (f returns Expected<U,E>)
    template<typename F>
    auto and_then(F&& f) -> decltype(f(std::declval<T>())) {
        if (has_value()) return f(value());
        return decltype(f(std::declval<T>()))::unexpected(error());
    }

    // transform: If value, apply f (wraps result in Expected)
    template<typename F>
    auto transform(F&& f) -> Expected<decltype(f(std::declval<T>())), E> {
        using U = decltype(f(std::declval<T>()));
        if (has_value()) return Expected<U, E>(f(value()));
        return Expected<U, E>::unexpected(error());
    }

    // or_else: If error, apply f
    template<typename F>
    Expected or_else(F&& f) {
        if (has_value()) return *this;
        return f(error());
    }

private:
    std::variant<T, E> data_;
};

// ═════════════════════════════════════════════════════════════════════════════
// PART 3: Practical Error Handling Pipeline
// ═════════════════════════════════════════════════════════════════════════════

struct SensorData {
    double temperature;
    double pressure;
};

// EN: Each function in the pipeline returns Expected
Expected<std::string, std::string> read_raw_data(int sensor_id) {
    if (sensor_id < 0)
        return Expected<std::string, std::string>::unexpected("Invalid sensor ID");
    if (sensor_id > 100)
        return Expected<std::string, std::string>::unexpected("Sensor timeout");
    return std::string("temp=25.3,pressure=1013.25");
}

Expected<SensorData, std::string> parse_data(const std::string& raw) {
    // EN: Simple parsing simulation
    if (raw.empty())
        return Expected<SensorData, std::string>::unexpected("Empty data");
    return SensorData{25.3, 1013.25};
}

Expected<double, std::string> validate_temperature(const SensorData& data) {
    if (data.temperature < -40.0 || data.temperature > 150.0)
        return Expected<double, std::string>::unexpected("Temperature out of range");
    return data.temperature;
}

// ═════════════════════════════════════════════════════════════════════════════
// PART 4: Exception Safety Guarantee Levels
// ═════════════════════════════════════════════════════════════════════════════

/**
 * EN: Exception Safety Guarantees — Every C++ developer must know these:
 *
 *     ┌────────────────────┬─────────────────────────────────────────────────┐
 *     │ Guarantee Level    │ Promise                                         │
 *     ├────────────────────┼─────────────────────────────────────────────────┤
 *     │ No-throw           │ Operation NEVER throws. Declared noexcept.      │
 *     │                    │ Examples: destructors, swap, move operations    │
 *     ├────────────────────┼─────────────────────────────────────────────────┤
 *     │ Strong (rollback)  │ If exception thrown, state is unchanged (as if  │
 *     │                    │ operation never happened). Copy-and-swap idiom. │
 *     ├────────────────────┼─────────────────────────────────────────────────┤
 *     │ Basic              │ No resource leaks, invariants preserved, but    │
 *     │                    │ state may have changed. Minimum acceptable.     │
 *     ├────────────────────┼─────────────────────────────────────────────────┤
 *     │ No guarantee       │ Could leak, corrupt state. Unacceptable!        │
 *     └────────────────────┴─────────────────────────────────────────────────┘
 */

// EN: Strong guarantee example: copy-and-swap idiom
class SensorArray {
public:
    explicit SensorArray(size_t n) : data_(n, 0.0) {}

    // EN: Strong guarantee: If push_back throws, old state preserved
    void add_reading_strong(double value) {
        auto copy = data_;              // 1. Copy current state
        copy.push_back(value);          // 2. Modify copy (may throw)
        data_.swap(copy);               // 3. Swap (noexcept!) — COMMIT
    }

    // EN: Basic guarantee: State is valid but modified on exception
    void add_reading_basic(double value) {
        data_.push_back(value);  // May throw (reallocation), but vector
                                 // provides basic guarantee internally
    }

    size_t size() const noexcept { return data_.size(); }

private:
    std::vector<double> data_;
};

// ═════════════════════════════════════════════════════════════════════════════
// PART 5: noexcept Deep Dive
// ═════════════════════════════════════════════════════════════════════════════

/**
 * EN: noexcept is a CONTRACT and an OPTIMIZATION:
 *
 *     1. noexcept on destructors: ALWAYS (default since C++11)
 *     2. noexcept on move operations: CRITICAL for vector optimization
 *        - vector::push_back uses move IF move ctor is noexcept
 *        - Otherwise falls back to copy (for strong guarantee)
 *     3. noexcept on swap: Always (swap should never fail)
 *     4. noexcept(expr): Conditional noexcept based on expression
 *
 *     Performance impact:
 *     - noexcept functions: compiler can elide stack unwinding code
 *     - vector reallocation: move vs copy (can be 100x difference!)
 *
 */

class EcuFirmware {
    std::string version_;
    std::vector<uint8_t> binary_;

public:
    EcuFirmware(std::string ver, std::vector<uint8_t> bin)
        : version_(std::move(ver)), binary_(std::move(bin)) {}

    // EN: noexcept move — vector will use move during reallocation
    EcuFirmware(EcuFirmware&& other) noexcept
        : version_(std::move(other.version_))
        , binary_(std::move(other.binary_)) {}

    EcuFirmware& operator=(EcuFirmware&& other) noexcept {
        version_ = std::move(other.version_);
        binary_ = std::move(other.binary_);
        return *this;
    }

    // EN: Conditional noexcept — noexcept if string comparison is noexcept
    bool is_same_version(const EcuFirmware& other) const
        noexcept(noexcept(version_ == other.version_)) {
        return version_ == other.version_;
    }

    const std::string& version() const noexcept { return version_; }
    size_t binary_size() const noexcept { return binary_.size(); }
};

// ═════════════════════════════════════════════════════════════════════════════
// MAIN
// ═════════════════════════════════════════════════════════════════════════════
int main() {
    std::cout << "============================================\n";
    std::cout << "   Module 05 - Advanced Error Handling\n";
    std::cout << "============================================\n\n";

    // ─── Demo 1: std::error_code with Custom Category ───────────────────
    {
        std::cout << "--- Demo 1: std::error_code & Custom Category ---\n";

        // EN: Success case
        double value = 0.0;
        std::error_code ec = read_sensor(3, value);
        std::cout << "    Sensor 3: " << (ec ? "FAIL" : "OK")
                  << ", value=" << value << "\n";

        // EN: Error cases
        ec = read_sensor(-1, value);
        std::cout << "    Sensor -1: " << ec.message()
                  << " (category: " << ec.category().name()
                  << ", code: " << ec.value() << ")\n";

        ec = read_sensor(15, value);
        std::cout << "    Sensor 15: " << ec.message() << "\n";

        // EN: System error codes (standard errno mapping)
        auto sys_ec = std::make_error_code(std::errc::no_such_file_or_directory);
        std::cout << "    System: " << sys_ec.message()
                  << " (category: " << sys_ec.category().name() << ")\n\n";
    }

    // ─── Demo 2: Expected<T,E> Monadic Chaining ────────────────────────
    {
        std::cout << "--- Demo 2: Expected<T,E> Monadic Chaining ---\n";

        // EN: Success pipeline: read → parse → validate
        auto result = read_raw_data(5)
            .and_then(parse_data)
            .and_then(validate_temperature);

        if (result) {
            std::cout << "    Pipeline OK: temperature = " << *result << " C\n";
        } else {
            std::cout << "    Pipeline FAIL: " << result.error() << "\n";
        }

        // EN: Error pipeline — first error short-circuits
        auto err_result = read_raw_data(-1)
            .and_then(parse_data)
            .and_then(validate_temperature);

        if (!err_result) {
            std::cout << "    Error pipeline: " << err_result.error() << "\n";
        }

        // EN: transform example — convert temperature to string
        auto str_result = read_raw_data(5)
            .and_then(parse_data)
            .and_then(validate_temperature)
            .transform([](double temp) {
                return std::to_string(temp) + " C";
            });

        if (str_result) {
            std::cout << "    Transform result: " << *str_result << "\n";
        }

        // EN: or_else example — provide fallback on error
        auto fallback_result = read_raw_data(-1)
            .and_then(parse_data)
            .and_then(validate_temperature)
            .or_else([](const std::string& err) -> Expected<double, std::string> {
                std::cout << "    or_else handler: " << err << " → using default\n";
                return 20.0;  // EN: Fallback value
            });

        if (fallback_result) {
            std::cout << "    Fallback value: " << *fallback_result << " C\n\n";
        }
    }

    // ─── Demo 3: Exception Safety Guarantees ────────────────────────────
    {
        std::cout << "--- Demo 3: Exception Safety Guarantees ---\n";
        SensorArray arr(3);
        arr.add_reading_strong(42.0);
        arr.add_reading_basic(43.0);
        std::cout << "    SensorArray size after 2 adds: " << arr.size() << "\n";

        std::cout << "    add_reading_strong: Copy-and-swap → strong guarantee\n";
        std::cout << "    add_reading_basic:  Direct push_back → basic guarantee\n\n";

        std::cout << "    Guarantee levels:\n";
        std::cout << "      No-throw:  destructors, swap, move (noexcept)\n";
        std::cout << "      Strong:    copy-and-swap, commit-or-rollback\n";
        std::cout << "      Basic:     no leaks, invariants hold, state may change\n";
        std::cout << "      None:      UNACCEPTABLE — fix this!\n\n";
    }

    // ─── Demo 4: noexcept Impact on Containers ──────────────────────────
    {
        std::cout << "--- Demo 4: noexcept and Container Optimization ---\n";
        std::cout << "    EcuFirmware move ctor noexcept: "
                  << std::boolalpha
                  << std::is_nothrow_move_constructible<EcuFirmware>::value << "\n";
        std::cout << "    std::string move ctor noexcept: "
                  << std::is_nothrow_move_constructible<std::string>::value << "\n";
        std::cout << "    std::vector<int> move noexcept: "
                  << std::is_nothrow_move_constructible<std::vector<int>>::value << "\n\n";

        // EN: With noexcept move → vector uses move during reallocation
        //     Without noexcept → vector copies (for strong exception guarantee)
        std::vector<EcuFirmware> firmwares;
        firmwares.reserve(2);  // EN: Start small to force reallocation
        firmwares.emplace_back("v1.0", std::vector<uint8_t>(1024));
        firmwares.emplace_back("v2.0", std::vector<uint8_t>(2048));
        firmwares.emplace_back("v3.0", std::vector<uint8_t>(4096));  // Triggers realloc

        std::cout << "    Stored " << firmwares.size() << " firmwares\n";
        std::cout << "    (noexcept move → vector used MOVE during realloc)\n";
        std::cout << "    (without noexcept → vector would COPY → much slower!)\n\n";
    }

    // ─── Demo 5: Error Handling Strategy Decision ───────────────────────
    {
        std::cout << "--- Demo 5: Error Handling Strategy Guide ---\n";
        std::cout << "  ┌────────────────────────┬────────────────────────────────┐\n";
        std::cout << "  │ Situation              │ Use                            │\n";
        std::cout << "  ├────────────────────────┼────────────────────────────────┤\n";
        std::cout << "  │ Programming errors     │ assert() / preconditions       │\n";
        std::cout << "  │ Truly exceptional      │ throw exceptions               │\n";
        std::cout << "  │ Expected failures      │ std::expected<T,E> (C++23)     │\n";
        std::cout << "  │ Missing value          │ std::optional<T>               │\n";
        std::cout << "  │ OS/system errors       │ std::error_code                │\n";
        std::cout << "  │ Embedded / no-except   │ Return codes + error_code      │\n";
        std::cout << "  │ Chained operations     │ expected + and_then/transform  │\n";
        std::cout << "  └────────────────────────┴────────────────────────────────┘\n\n";
    }

    std::cout << "============================================\n";
    std::cout << "   End of Advanced Error Handling\n";
    std::cout << "============================================\n";

    return 0;
}
