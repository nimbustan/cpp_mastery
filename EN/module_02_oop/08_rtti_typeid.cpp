/**
 * @file module_02_oop/08_rtti_typeid.cpp
 * @brief OOP Part 8: RTTI & typeid
 *
 * @details
 * =============================================================================
 * [THEORY: What is RTTI?]
 * =============================================================================
 * EN: RTTI (Run-Time Type Information) is a C++ mechanism that allows the program to determine
 * the actual (dynamic) type of a polymorphic object at runtime. When you have a base-class
 * pointer or reference, RTTI lets you ask: "What is the REAL type hiding behind this pointer?"
 * Two primary tools enable this: the `typeid` operator and `dynamic_cast`.
 *
 * =============================================================================
 * [THEORY: typeid Operator]
 * =============================================================================
 * EN: `typeid(expr)` returns a `const std::type_info&`. For polymorphic types (classes with at
 * least one virtual function), it evaluates the runtime type. For non-polymorphic types, it
 * returns the static (compile-time) type. You can compare two type_info objects with `==` and
 * `!=`, and call `.name()` to get an implementation-defined mangled name, or `.hash_code()` for
 * fast comparisons in hash tables.
 *
 * =============================================================================
 * [THEORY: dynamic_cast & Performance]
 * =============================================================================
 * EN: `dynamic_cast<Derived*>(basePtr)` is the safe downcast mechanism. It returns nullptr on
 * failure (for pointers) or throws `std::bad_cast` (for references). RTTI adds a small cost:
 * each polymorphic class carries extra type metadata in its vtable. Compilers offer `-fno-rtti`
 * to disable it entirely (saves binary size, but typeid and dynamic_cast become unavailable).
 *
 * =============================================================================
 * [THEORY: When to Use RTTI]
 * =============================================================================
 * EN: Prefer virtual dispatch or std::variant/std::visit over RTTI when possible. RTTI is
 * appropriate for plugin systems, serialization, debug logging, or when the set of derived types
 * is open-ended. Overusing typeid checks is often a code smell — it bypasses polymorphism
 * entirely.
 *
 * [CPPREF DEPTH: RTTI Cost and Alternatives — When dynamic_cast Hurts Performance]
 * =============================================================================
 * EN: RTTI requires a vtable — it ONLY works on polymorphic types (classes with at least one
 * virtual function). Applying `typeid` to a non- polymorphic type yields the static
 * (compile-time) type, not the runtime type — a subtle but critical distinction. `dynamic_cast`
 * traverses the inheritance graph at runtime. The cost is O(depth) in the inheritance tree — in
 * deep hierarchies with virtual bases, this involves following vbase offsets through the vtable.
 * In tight loops processing millions of objects, this becomes measurable overhead. The
 * `-fno-rtti` compiler flag (GCC/Clang) disables RTTI entirely, removing type_info structures
 * from every polymorphic class. This typically saves ~5% binary size. However, `typeid` and
 * `dynamic_cast` become compile errors, and libraries compiled WITH rtti cannot be mixed safely
 * with code compiled WITHOUT it (ODR violations in type_info). Alternatives to RTTI-based
 * dispatch:
 *   1. Enum-based type tags — manual but fast O(1) switch dispatch.
 *   2. Visitor pattern (double dispatch) — type-safe, no RTTI needed.
 *   3. std::variant + std::visit — compile-time checked, zero overhead.
 *   4. CRTP (Curiously Recurring Template Pattern) — static polymorphism.
 * In automotive (MISRA C++), real-time, and embedded domains, RTTI is often BANNED due to
 * unpredictable runtime cost and binary bloat.
 *
 * ⚠  Ref: https://en.cppreference.com/w/cpp/language/typeid
 *
 * @note Compile: g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion 08_rtti_typeid.cpp -o 08_rtti_typeid
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <iostream>   // EN: Standard I/O for console output
#include <string>     // EN: std::string for type descriptions
#include <typeinfo>   // EN: typeid, std::type_info, std::bad_cast
#include <memory>     // EN: std::unique_ptr for polymorphic ownership
#include <vector>     // EN: std::vector for sensor collection

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// SECTION 1: Polymorphic Sensor Hierarchy (Automotive Domain)
// ═════════════════════════════════════════════════════════════════════════════════════════════════

// EN: Base class with virtual functions — makes the hierarchy polymorphic.
class Sensor {
public:
    virtual ~Sensor() = default;

    virtual std::string getType() const { return "GenericSensor"; }

    virtual double read() const = 0;
};

// ─── 1.1 TemperatureSensor ───────────────────────────────────────────────────────────────────────
class TemperatureSensor : public Sensor {
    double celsius_;
public:
    explicit TemperatureSensor(double c) : celsius_(c) {}

    std::string getType() const override { return "TemperatureSensor"; }
    double read() const override { return celsius_; }

    // EN: Derived-specific method — only accessible after downcast.
    double toFahrenheit() const { return celsius_ * 9.0 / 5.0 + 32.0; }
};

// ─── 1.2 PressureSensor ──────────────────────────────────────────────────────────────────────────
class PressureSensor : public Sensor {
    double kpa_;
public:
    explicit PressureSensor(double kpa) : kpa_(kpa) {}

    std::string getType() const override { return "PressureSensor"; }
    double read() const override { return kpa_; }

    // EN: Convert kilopascals to PSI (pounds per square inch).
    double toPSI() const { return kpa_ * 0.145038; }
};

// ─── 1.3 OxygenSensor ────────────────────────────────────────────────────────────────────────────
class OxygenSensor : public Sensor {
    double voltage_;
public:
    explicit OxygenSensor(double v) : voltage_(v) {}

    std::string getType() const override { return "OxygenSensor"; }
    double read() const override { return voltage_; }

    // EN: Check if the air-fuel mixture is running lean (voltage < 0.45V).
    bool isLean() const { return voltage_ < 0.45; }
};

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// SECTION 2: Non-Polymorphic Type (for comparison)
// ═════════════════════════════════════════════════════════════════════════════════════════════════

// EN: A simple struct without virtual functions — typeid returns static type.
struct DTCCode {
    int code;
    std::string description;
};

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// SECTION 3: Production Functions Using RTTI
// ═════════════════════════════════════════════════════════════════════════════════════════════════

// ─── 3.1 identifySensor ──────────────────────────────────────────────────────────────────────────
// EN: Uses typeid to print the actual runtime type of a sensor.
void identifySensor(const Sensor& s) {
    std::cout << "  typeid name   : " << typeid(s).name() << "\n";
    std::cout << "  hash_code     : " << typeid(s).hash_code() << "\n";
    std::cout << "  getType()     : " << s.getType() << "\n";
}

// ─── 3.2 processSpecificSensor ───────────────────────────────────────────────────────────────────
// EN: Uses dynamic_cast to safely downcast and call derived-specific methods.
void processSpecificSensor(Sensor* s) {
    if (auto* temp = dynamic_cast<TemperatureSensor*>(s)) {
        std::cout << "  [TemperatureSensor] " << temp->read() << " °C = "
                  << temp->toFahrenheit() << " °F\n";
    } else if (auto* pres = dynamic_cast<PressureSensor*>(s)) {
        std::cout << "  [PressureSensor] " << pres->read() << " kPa = "
                  << pres->toPSI() << " PSI\n";
    } else if (auto* oxy = dynamic_cast<OxygenSensor*>(s)) {
        std::cout << "  [OxygenSensor] " << oxy->read() << " V -> "
                  << (oxy->isLean() ? "Lean" : "Rich") << " mixture\n";
    } else {
        std::cout << "  [Unknown sensor type]\n";
    }
}

// ─── 3.3 demonstrateTypeComparison ───────────────────────────────────────────────────────────────
// EN: Shows typeid equality comparison between objects.
void demonstrateTypeComparison(const Sensor& a, const Sensor& b) {
    if (typeid(a) == typeid(b)) {
        std::cout << "  Result: SAME runtime type ("
                  << typeid(a).name() << ")\n";
    } else {
        std::cout << "  Result: DIFFERENT runtime types ("
                  << typeid(a).name() << " vs " << typeid(b).name() << ")\n";
    }
}

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// SECTION 4: main() — Demonstrations
// ═════════════════════════════════════════════════════════════════════════════════════════════════
int main() {
    std::cout << "=== Module 02 OOP | 08: RTTI & typeid ===\n\n";

    // ─── Demo 1: typeid on different sensor types through base pointer ──
    std::cout << "--- Demo 1: typeid through base pointer ---\n";
    std::vector<std::unique_ptr<Sensor>> sensors;
    sensors.push_back(std::make_unique<TemperatureSensor>(92.5));
    sensors.push_back(std::make_unique<PressureSensor>(101.3));
    sensors.push_back(std::make_unique<OxygenSensor>(0.72));

    for (const auto& s : sensors) {
        std::cout << "Sensor [" << s->getType() << "]:\n";
        identifySensor(*s);
        std::cout << "\n";
    }

    // ─── Demo 2: typeid equality comparison ──────────────────────────────────────────────────────
    std::cout << "--- Demo 2: typeid equality comparison ---\n";
    TemperatureSensor coolantTemp(88.0);
    TemperatureSensor intakeTemp(35.0);
    PressureSensor boostPressure(150.0);

    std::cout << "Comparing coolantTemp vs intakeTemp:\n";
    demonstrateTypeComparison(coolantTemp, intakeTemp);

    std::cout << "Comparing coolantTemp vs boostPressure:\n";
    demonstrateTypeComparison(coolantTemp, boostPressure);
    std::cout << "\n";

    // ─── Demo 3: dynamic_cast safe downcast ──────────────────────────────────────────────────────
    std::cout << "--- Demo 3: dynamic_cast safe downcast ---\n";
    Sensor* engineTemp = &coolantTemp;
    processSpecificSensor(engineTemp);

    Sensor* manifold = &boostPressure;
    processSpecificSensor(manifold);

    OxygenSensor lambdaProbe(0.38);
    Sensor* exhaust = &lambdaProbe;
    processSpecificSensor(exhaust);
    std::cout << "\n";

    // ─── Demo 4: dynamic_cast failure (returns nullptr) ──────────────────────────────────────────
    std::cout << "--- Demo 4: dynamic_cast failure ---\n";
    Sensor* sensorPtr = &boostPressure;
    auto* asTemp = dynamic_cast<TemperatureSensor*>(sensorPtr);
    if (asTemp == nullptr) {
        std::cout << "  dynamic_cast<TemperatureSensor*> on PressureSensor "
                  << "returned nullptr (safe failure)\n";
    }

    // EN: Reference cast throws std::bad_cast on failure.
    try {
        [[maybe_unused]] auto& asOxy =
            dynamic_cast<OxygenSensor&>(*sensorPtr);
    } catch (const std::bad_cast& e) {
        std::cout << "  dynamic_cast<OxygenSensor&> threw: "
                  << e.what() << "\n";
    }
    std::cout << "\n";

    // ─── Demo 5: Polymorphic vs non-polymorphic typeid ───────────────────────────────────────────
    std::cout << "--- Demo 5: Polymorphic vs non-polymorphic typeid ---\n";

    // EN: Polymorphic — typeid resolves actual runtime type.
    Sensor* basePtr = &coolantTemp;
    std::cout << "  Declared type : Sensor*\n";
    std::cout << "  typeid(*basePtr): " << typeid(*basePtr).name()
              << "  (runtime type)\n";

    // EN: Non-polymorphic — typeid always returns static (declared) type.
    DTCCode dtc{420, "Catalyst Temp Below Threshold"};
    const DTCCode& dtcRef = dtc;
    std::cout << "  Declared type : const DTCCode&\n";
    std::cout << "  typeid(dtcRef)  : " << typeid(dtcRef).name()
              << "  (static type — no virtual)\n";
    std::cout << "\n";

    // ─── Demo 6: type_info::name() and type_info::hash_code() ────────────────────────────────────
    std::cout << "--- Demo 6: type_info name() & hash_code() ---\n";
    const std::type_info& ti1 = typeid(TemperatureSensor);
    const std::type_info& ti2 = typeid(PressureSensor);
    const std::type_info& ti3 = typeid(TemperatureSensor);

    std::cout << "  TemperatureSensor name : " << ti1.name() << "\n";
    std::cout << "  PressureSensor name    : " << ti2.name() << "\n";
    std::cout << "  TemperatureSensor hash : " << ti1.hash_code() << "\n";
    std::cout << "  PressureSensor hash    : " << ti2.hash_code() << "\n";
    std::cout << "  ti1 == ti3 (same type) : "
              << (ti1 == ti3 ? "true" : "false") << "\n";
    std::cout << "  ti1 == ti2 (diff type) : "
              << (ti1 == ti2 ? "true" : "false") << "\n";

    std::cout << "\n=== RTTI & typeid demonstration complete ===\n";
    return 0;
}
