/**
 * @file module_03_modern_cpp/05_structured_bindings_optional.cpp
 * @brief Structured Bindings, std::optional, std::variant, std::any
 *
 * @details
 * =============================================================================
 * [THEORY: Structured Bindings (C++17)]
 * =============================================================================
 *
 * EN: Structured bindings allow decomposing an object into its constituent
 *     elements using the syntax: auto [a, b, c] = expression;
 *     They work with std::pair, std::tuple, std::array, and any aggregate type
 *     (struct/class with all public non-static data members). In automotive ECU
 *     software, structured bindings dramatically simplify iteration over maps
 *     of sensor data, DTC records, and calibration tables.
 *
 * =============================================================================
 * [THEORY: std::optional (C++17)]
 * =============================================================================
 *
 * EN: std::optional<T> represents a value that may or may not be present.
 *     It is a safer alternative to raw pointers, sentinel values (-1, NaN),
 *     or out-parameters for expressing "no result".
 *     Key API: has_value(), value(), value_or(default), operator*, operator bool.
 *     In automotive: a sensor may be offline, a DTC lookup may find nothing,
 *     a calibration parameter may be unconfigured — all modeled by optional.
 *
 * =============================================================================
 * [THEORY: std::variant & std::any (C++17)]
 * =============================================================================
 *
 * EN: std::variant<T1,T2,...> is a type-safe union — it holds exactly one of
 *     the listed types at any time. Access via std::get<T>, std::get<I>, or
 *     std::visit() with a visitor (often the "overloaded" lambda pattern).
 *     std::any is a type-safe void* — it can hold any copy-constructible type
 *     and is retrieved via std::any_cast<T>. Throws std::bad_any_cast on mismatch.
 *
 *     When to use which:
 *     optional  → "might not exist" (nullable value semantics)
 *     variant   → "one of several known types" (closed type set)
 *     any       → "truly dynamic type" (open type set, use sparingly)
 *
 * =============================================================================
 * [CPPREF DEPTH: std::optional Deep Dive]
 * =============================================================================
 *
 * EN: `std::optional<T>` stores `T` in-place using aligned storage — there is
 *     NO heap allocation. Its size equals sizeof(T) + 1 byte boolean flag +
 *     padding for alignment. For example:
 *     • `optional<int>` is typically 8 bytes (4 int + 1 flag + 3 pad).
 *     • `optional<char>` is typically 2 bytes (1 char + 1 flag).
 *     `optional<optional<T>>` is valid and semantically different from
 *     `optional<T>`: the outer layer can be engaged holding a disengaged inner
 *     optional — two distinct "empty" states. Accessing a disengaged optional
 *     via `operator*` or `operator->` is undefined behavior (no exception
 *     thrown!). Use `value()` for checked access — it throws
 *     `std::bad_optional_access`.
 *     C++23 adds monadic operations inspired by functional programming:
 *     • `and_then(f)` — flat-map: f returns optional<U>
 *     • `transform(f)` — map: f returns U, wrapped in optional<U>
 *     • `or_else(f)` — called only when disengaged
 *     These enable clean chaining: `opt.and_then(parse).transform(validate)`
 *     without nested if-checks.
 *
 * ⚠  Ref: https://en.cppreference.com/w/cpp/utility/optional
 * ⚠  Ref: https://en.cppreference.com/w/cpp/utility/variant
 * ⚠  Ref: https://en.cppreference.com/w/cpp/utility/any
 *
 * @note Compile: g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion 05_structured_bindings_optional.cpp -o 05_structured_bindings_optional
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <iostream>     // EN: Standard I/O
#include <string>       // EN: std::string
#include <map>          // EN: std::map for sensor readings
#include <optional>     // EN: std::optional — nullable value wrapper
#include <variant>      // EN: std::variant — type-safe union
#include <any>          // EN: std::any — type-erased container
#include <tuple>        // EN: std::tuple for structured bindings demo
#include <array>        // EN: std::array for structured bindings demo
#include <vector>       // EN: std::vector for parameter storage
#include <stdexcept>    // EN: std::runtime_error, std::bad_any_cast

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// 1. STRUCTURED BINDINGS WITH std::map — SENSOR READINGS
// ═════════════════════════════════════════════════════════════════════════════════════════════════

// EN: Build a map of automotive sensor names to their current readings (volts).
std::map<std::string, double> getEngineSensorReadings() {
    return {
        {"CoolantTemp",    92.3},
        {"OilPressure",     3.8},
        {"ThrottlePos",    45.0},
        {"IntakeAirTemp",  38.7},
        {"BatteryVoltage", 13.9}
    };
}

// EN: Print all sensor readings using structured bindings in a range-for loop.
void printSensorMap(const std::map<std::string, double>& sensorMap) {
    std::cout << "  Sensor readings:\n";
    for (const auto& [name, value] : sensorMap) {
        std::cout << "    " << name << " = " << value << "\n";
    }
}

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// 2. STRUCTURED BINDINGS WITH STRUCT — ECU DIAGNOSTIC RESULT
// ═════════════════════════════════════════════════════════════════════════════════════════════════

// EN: Diagnostic Trouble Code result from an ECU self-test.
struct ECUDiagResult {
    int         dtcCode;        // EN: DTC numeric code (e.g., 301 = misfire cyl 1)
    std::string description;    // EN: Human-readable fault description
    bool        isPending;      // EN: true if fault is intermittent / not confirmed
};

// EN: Simulate reading a diagnostic result from ECU.
ECUDiagResult getLatestDiagResult() {
    return {301, "Misfire detected — Cylinder 1", true};
}

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// 3. std::optional — SENSOR READING THAT MAY BE ABSENT
// ═════════════════════════════════════════════════════════════════════════════════════════════════

// EN: Represents a single sensor reading with metadata.
struct SensorReading {
    std::string name;
    double      value;
    std::string unit;
};

// EN: Read sensor by ID. Returns std::nullopt if the sensor is offline.
std::optional<SensorReading> readSensor(int sensorId) {
    // EN: Simulate: even IDs are online, odd IDs are offline.
    if (sensorId % 2 == 0) {
        return SensorReading{"Sensor_" + std::to_string(sensorId),
                             25.0 + sensorId * 1.5,
                             "°C"};
    }
    return std::nullopt;
}

// ─── 3a. Optional Chaining — Multiple Sensors Must Be Online ─────────────────────────────────────

// EN: Perform a combined diagnostic only if ALL required sensors are online.
void combinedDiagnostic(int id1, int id2, int id3) {
    auto s1 = readSensor(id1);
    auto s2 = readSensor(id2);
    auto s3 = readSensor(id3);

    if (s1 && s2 && s3) {
        // EN: All sensors online — proceed with combined check.
        std::cout << "  Combined diagnostic OK:\n";
        std::cout << "    " << s1->name << " = " << s1->value << " " << s1->unit << "\n";
        std::cout << "    " << s2->name << " = " << s2->value << " " << s2->unit << "\n";
        std::cout << "    " << s3->name << " = " << s3->value << " " << s3->unit << "\n";
    } else {
        // EN: At least one sensor is offline — report which ones.
        std::cout << "  Combined diagnostic FAILED — offline sensors:";
        if (!s1) std::cout << " Sensor_" << id1;
        if (!s2) std::cout << " Sensor_" << id2;
        if (!s3) std::cout << " Sensor_" << id3;
        std::cout << "\n";
    }
}

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// 4. std::variant — POLYMORPHIC CAN MESSAGE PAYLOADS
// ═════════════════════════════════════════════════════════════════════════════════════════════════

// EN: CAN bus messages can carry different payload types depending on the signal.
using CANPayload = std::variant<int, double, std::string>;

// EN: Overloaded helper — combines multiple lambdas into a single visitor.
template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

// EN: Decode and display a CAN payload using std::visit + overloaded lambdas.
void decodeCANPayload(const std::string& signalName, const CANPayload& payload) {
    std::cout << "    Signal [" << signalName << "]: ";
    std::visit(overloaded{
        [](int val) {
            // EN: Integer payload — e.g., gear position, RPM count.
            std::cout << "INT = " << val << "\n";
        },
        [](double val) {
            // EN: Floating-point payload — e.g., temperature, voltage.
            std::cout << "DOUBLE = " << val << "\n";
        },
        [](const std::string& val) {
            // EN: String payload — e.g., VIN, diagnostic text.
            std::cout << "STRING = \"" << val << "\"\n";
        }
    }, payload);
}

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// 5. std::any — DYNAMIC ECU PARAMETER STORAGE
// ═════════════════════════════════════════════════════════════════════════════════════════════════

// EN: Generic parameter store for ECU calibration — keys are string names, values can be any
// type (int thresholds, double gains, string labels).
using ECUParamStore = std::map<std::string, std::any>;

// EN: Safely retrieve a parameter from the store with type checking.
template<typename T>
std::optional<T> getParam(const ECUParamStore& store, const std::string& key) {
    auto it = store.find(key);
    if (it == store.end()) {
        return std::nullopt;
    }
    try {
        return std::any_cast<T>(it->second);
    } catch (const std::bad_any_cast&) {
        // EN: Type mismatch — requested type doesn't match stored type.
        return std::nullopt;
    }
}

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// main()
// ═════════════════════════════════════════════════════════════════════════════════════════════════

int main() {
    std::cout << "=== Module 03 | 05: Structured Bindings, optional, variant, any ===\n\n";

    // ─── Demo 1: Structured Bindings — pair, tuple, struct, map ──────────────────────────────────

    std::cout << "[Demo 1] Structured Bindings\n";

    // EN: 1a — Decompose an std::pair (e.g., map insert result).
    std::map<std::string, int> dtcCounts;
    auto [iter, inserted] = dtcCounts.emplace("P0301", 1);
    std::cout << "  Pair: key=" << iter->first
              << ", inserted=" << std::boolalpha << inserted << "\n";

    // EN: 1b — Decompose an std::tuple (e.g., ECU firmware version).
    std::tuple<int, int, int> firmwareVersion{2, 14, 7};
    auto [major, minor, patch] = firmwareVersion;
    std::cout << "  Tuple: firmware v" << major << "." << minor << "." << patch << "\n";

    // EN: 1c — Decompose an std::array.
    std::array<double, 3> pidGains{0.8, 0.1, 0.05};
    auto [kp, ki, kd] = pidGains;
    std::cout << "  Array: PID gains Kp=" << kp << " Ki=" << ki << " Kd=" << kd << "\n";

    // EN: 1d — Decompose a user-defined struct.
    auto [code, desc, pending] = getLatestDiagResult();
    std::cout << "  Struct: DTC " << code << " — \"" << desc
              << "\" pending=" << std::boolalpha << pending << "\n";

    // EN: 1e — Iterate over a map with structured bindings.
    auto sensors = getEngineSensorReadings();
    printSensorMap(sensors);

    std::cout << "\n";

    // ─── Demo 2: std::optional — Sensor Read (present / absent) ──────────────────────────────────

    std::cout << "[Demo 2] std::optional — Sensor Read\n";

    for (int id : {2, 3, 6, 7}) {
        auto reading = readSensor(id);
        if (reading.has_value()) {
            // EN: Sensor is online — display value.
            std::cout << "  Sensor " << id << ": " << reading->name
                      << " = " << reading->value << " " << reading->unit << "\n";
        } else {
            // EN: Sensor is offline — use value_or as fallback demonstration.
            SensorReading fallback{"N/A", 0.0, ""};
            auto safe = reading.value_or(fallback);
            std::cout << "  Sensor " << id << ": OFFLINE (fallback name=\""
                      << safe.name << "\")\n";
        }
    }

    std::cout << "\n";

    // ─── Demo 3: Optional Chaining — Multiple Sensors ────────────────────────────────────────────

    std::cout << "[Demo 3] Optional Chaining — Combined Diagnostic\n";

    // EN: All even IDs → all online → success.
    combinedDiagnostic(2, 4, 6);

    // EN: Mixed IDs → some offline → failure with report.
    combinedDiagnostic(2, 3, 6);

    std::cout << "\n";

    // ─── Demo 4: std::variant — CAN Payload with std::visit ──────────────────────────────────────

    std::cout << "[Demo 4] std::variant — CAN Message Payloads\n";

    // EN: Simulate CAN bus messages with different payload types.
    std::vector<std::pair<std::string, CANPayload>> canMessages = {
        {"GearPosition",    3},
        {"CoolantTemp",     91.7},
        {"VIN",             std::string("WVWZZZ3CZWE123456")},
        {"EngineRPM",       3200},
        {"BatteryVoltage",  13.8}
    };

    for (const auto& [signal, payload] : canMessages) {
        decodeCANPayload(signal, payload);
    }

    // EN: Check which type is currently held using index().
    CANPayload sample = 42;
    std::cout << "  Variant index for int(42): " << sample.index() << "\n";
    sample = 3.14;
    std::cout << "  Variant index for double(3.14): " << sample.index() << "\n";
    sample = std::string("test");
    std::cout << "  Variant index for string: " << sample.index() << "\n";

    std::cout << "\n";

    // ─── Demo 5: std::any — Generic Parameter Map ────────────────────────────────────────────────

    std::cout << "[Demo 5] std::any — ECU Parameter Store\n";

    ECUParamStore ecuParams;
    ecuParams["MaxRPM"]           = 7000;
    ecuParams["IdleTargetRPM"]    = 800;
    ecuParams["FuelTrimGain"]     = 1.05;
    ecuParams["CalibrationLabel"] = std::string("CAL_2026_Q1");

    // EN: Safe retrieval with correct type.
    if (auto rpm = getParam<int>(ecuParams, "MaxRPM")) {
        std::cout << "  MaxRPM = " << *rpm << "\n";
    }

    if (auto gain = getParam<double>(ecuParams, "FuelTrimGain")) {
        std::cout << "  FuelTrimGain = " << *gain << "\n";
    }

    if (auto label = getParam<std::string>(ecuParams, "CalibrationLabel")) {
        std::cout << "  CalibrationLabel = \"" << *label << "\"\n";
    }

    // EN: Wrong type → returns nullopt instead of crashing.
    if (auto bad = getParam<std::string>(ecuParams, "MaxRPM")) {
        std::cout << "  MaxRPM as string = " << *bad << "\n";
    } else {
        std::cout << "  MaxRPM as string: type mismatch (nullopt)\n";
    }

    // EN: Missing key → returns nullopt.
    if (auto missing = getParam<int>(ecuParams, "NonExistentParam")) {
        std::cout << "  NonExistentParam = " << *missing << "\n";
    } else {
        std::cout << "  NonExistentParam: not found (nullopt)\n";
    }

    std::cout << "\n";

    // ─── Demo 6: Comparison — When to Use optional vs variant vs any ─────────────────────────────

    std::cout << "[Demo 6] Comparison — optional vs variant vs any\n\n";

    // EN: optional — "might not exist": a sensor reading that could be absent.
    std::optional<double> oilTemp = 105.3;
    std::optional<double> exhaustTemp = std::nullopt;
    std::cout << "  optional<double> oilTemp: "
              << oilTemp.value_or(-1.0) << "\n";
    std::cout << "  optional<double> exhaustTemp: "
              << exhaustTemp.value_or(-1.0) << " (default = sensor offline)\n";

    // EN: variant — "one of several known types": a CAN signal value.
    std::variant<int, double, std::string> canSignal = 42;
    std::cout << "  variant<int,double,string>: holds index "
              << canSignal.index() << " (int)\n";
    canSignal = std::string("DIAGNOSTIC_OK");
    std::cout << "  variant<int,double,string>: holds index "
              << canSignal.index() << " (string)\n";

    // EN: any — "truly dynamic type": use when type set is open/unknown.
    std::any dynParam = 3.14;
    std::cout << "  any: type = " << dynParam.type().name()
              << ", value = " << std::any_cast<double>(dynParam) << "\n";
    dynParam = std::string("runtime_label");
    std::cout << "  any: type = " << dynParam.type().name()
              << ", value = \"" << std::any_cast<std::string>(dynParam) << "\"\n";

    std::cout << "\n  Rule of thumb:\n";
    std::cout << "    optional  → value may be absent (nullable semantics)\n";
    std::cout << "    variant   → value is one of N known types (closed set)\n";
    std::cout << "    any       → value is truly dynamic (open set, use sparingly)\n";

    std::cout << "\n=== End of Module 03 | 05 ===\n";
    return 0;
}
