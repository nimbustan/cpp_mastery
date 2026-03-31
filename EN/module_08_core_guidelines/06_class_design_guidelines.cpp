/**
 * @file module_08_core_guidelines/06_class_design_guidelines.cpp
 * @brief C++ Core Guidelines: Class Design Rules
 *
 * @details
 * =============================================================================
 * [C.20: Rule of Zero — Avoid defining default operations if you can]
 * =============================================================================
 * EN: If your class members are ALL RAII types (std::string, std::vector, std::unique_ptr), the
 * compiler-generated destructor, copy, and move operations are ALREADY CORRECT. Writing them
 * manually is error-prone and redundant. Let the compiler do its job.
 *
 * =============================================================================
 * [C.21: Define or =delete ALL special members if you define any]
 * =============================================================================
 * EN: If you write a custom destructor, copy constructor, or copy assignment, you almost
 * certainly need to define (or delete) ALL of them. This is the "Rule of Five" in modern C++.
 * Partial definitions lead to subtle bugs like double-free or missing move semantics.
 *
 * =============================================================================
 * [C.35: A base class destructor should be public+virtual or protected+non-virtual]
 * =============================================================================
 * EN: If you delete through a base pointer, the destructor MUST be virtual. Otherwise the
 * derived destructor never runs — resource leak. If deletion through base is not intended, make
 * the destructor protected and non-virtual.
 *
 * =============================================================================
 * [C.67: Polymorphic class should suppress copying]
 * =============================================================================
 * EN: Copying a polymorphic object through a base pointer causes "Object Slicing" — the derived
 * part is silently chopped off. Delete copy operations or make them protected. Use clone()
 * (virtual copy) if deep copying is needed.
 *
 * =============================================================================
 * [C.128 / C.129: virtual → override → final & Interface vs Implementation]
 * =============================================================================
 * EN: Mark every overriding function `override` so the compiler catches typos. Use `final` to
 * prevent further overriding of a specific method or to seal an entire class. Separate INTERFACE
 * inheritance (pure virtuals) from IMPLEMENTATION inheritance (shared base logic) — Liskov
 * Substitution Principle: every subtype must be usable wherever the base type is expected.
 *
 * [CPPREF DEPTH: Liskov Substitution Principle in C++ — Beyond Theory]
 * =============================================================================
 * EN: LSP states that if S is a subtype of T, objects of type T may be replaced with objects of
 * type S without altering program correctness. Common violations include: strengthening
 * preconditions (derived demands more than base promised), weakening postconditions (derived
 * delivers less than base guaranteed), and changing exception specifications. The `override`
 * keyword (C++11) enforces exact signature matching with the base — a misspelled or mistyped
 * override becomes a compile error. `final` prevents further overriding of a particular virtual
 * method or prevents deriving from a class entirely. Composition over inheritance: prefer has-a
 * (member) relationships to is-a (inheritance) except when runtime polymorphism through
 * interfaces (pure virtual base) is needed. A well-designed hierarchy has a thin interface base
 * and concrete leaves.
 *
 * @note Compile: g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion 06_class_design_guidelines.cpp -o 06_class_design_guidelines
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <iostream> // EN: standard I/O
#include <memory>   // EN: smart pointers for RAII members
#include <string>   // EN: std::string
#include <vector>   // EN: std::vector

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// PRODUCTION CODE — Automotive Class Design
// ═════════════════════════════════════════════════════════════════════════════════════════════════

// ─── 1. Rule of Zero: SafeVehicleData ────────────────────────────────────────────────────────────

// EN: C.20 — all members are RAII types; no user-defined special members needed
class SafeVehicleData {
public:
    std::string vin;
    std::vector<double> sensorReadings;
    std::unique_ptr<std::string> diagnosticLog;

    // EN: A regular constructor is fine — Rule of Zero still applies
    SafeVehicleData(std::string v, std::vector<double> readings)
        : vin(std::move(v)),
          sensorReadings(std::move(readings)),
          diagnosticLog(std::make_unique<std::string>("Initial log entry")) {}

    void print() const {
        std::cout << "  VIN: " << vin
                  << " | Sensors: " << sensorReadings.size()
                  << " | Log: " << (diagnosticLog ? *diagnosticLog : "null")
                  << std::endl;
    }
};

// ─── 2. Abstract Interface: IDiagnosticProtocol ──────────────────────────────────────────────────

// EN: C.129 — pure interface (no data, all pure virtual)
class IDiagnosticProtocol {
public:
    virtual ~IDiagnosticProtocol() = default;

    // EN: C.67 — suppress copying on polymorphic base
    IDiagnosticProtocol(const IDiagnosticProtocol&) = delete;
    IDiagnosticProtocol& operator=(const IDiagnosticProtocol&) = delete;

    virtual std::string protocolName() const = 0;
    virtual bool connect(const std::string& target) = 0;
    virtual std::string readDTC() const = 0;
    virtual void clearDTC() = 0;

protected:
    IDiagnosticProtocol() = default;
    IDiagnosticProtocol(IDiagnosticProtocol&&) = default;
    IDiagnosticProtocol& operator=(IDiagnosticProtocol&&) = default;
};

// ─── 3. Concrete: OBD2Protocol ───────────────────────────────────────────────────────────────────

class OBD2Protocol : public IDiagnosticProtocol {
    std::string connectedECU_;
    bool connected_ = false;

public:
    OBD2Protocol() = default;

    // EN: C.128 — every override explicitly marked
    std::string protocolName() const override { return "OBD-II (ISO 15765)"; }

    bool connect(const std::string& target) override {
        connectedECU_ = target;
        connected_ = true;
        std::cout << "  [OBD2] Connected to " << target << std::endl;
        return true;
    }

    std::string readDTC() const override {
        if (!connected_) return "NOT_CONNECTED";
        return "P0301 — Cylinder 1 Misfire Detected";
    }

    void clearDTC() override {
        std::cout << "  [OBD2] DTCs cleared on " << connectedECU_ << std::endl;
    }
};

// ─── 4. Concrete: CANProtocol ────────────────────────────────────────────────────────────────────

class CANProtocol : public IDiagnosticProtocol {
    std::string busName_;
    bool connected_ = false;

public:
    CANProtocol() = default;

    std::string protocolName() const override { return "CAN 2.0B (ISO 11898)"; }

    bool connect(const std::string& target) override {
        busName_ = target;
        connected_ = true;
        std::cout << "  [CAN] Joined bus: " << target << std::endl;
        return true;
    }

    std::string readDTC() const override {
        if (!connected_) return "NOT_CONNECTED";
        return "U0100 — Lost Communication with ECM/PCM";
    }

    void clearDTC() override {
        std::cout << "  [CAN] DTCs cleared on bus " << busName_ << std::endl;
    }
};

// ─── 5. Final Class: HardcodedConfig ─────────────────────────────────────────────────────────────

// EN: C.128 final — sealed class, no further derivation allowed
class HardcodedConfig final {
    int baudRate_;
    std::string protocol_;

public:
    HardcodedConfig(int baud, std::string proto)
        : baudRate_(baud), protocol_(std::move(proto)) {}

    void print() const {
        std::cout << "  [Config] " << protocol_ << " @ " << baudRate_
                  << " baud (FINAL — immutable design)" << std::endl;
    }
};

// ─── 6. Deleted Copy: SingletonECU ───────────────────────────────────────────────────────────────

// EN: C.21 + C.67 — singleton must not be copied or moved
class SingletonECU {
    std::string ecuId_;

    // EN: Private constructor — only accessible via instance()
    explicit SingletonECU(std::string id) : ecuId_(std::move(id)) {}

public:
    // EN: Delete ALL copy and move operations (C.21)
    SingletonECU(const SingletonECU&) = delete;
    SingletonECU& operator=(const SingletonECU&) = delete;
    SingletonECU(SingletonECU&&) = delete;
    SingletonECU& operator=(SingletonECU&&) = delete;

    static SingletonECU& instance() {
        static SingletonECU inst("MasterECU-001");
        return inst;
    }

    void identify() const {
        std::cout << "  [Singleton] I am: " << ecuId_
                  << " (one and only instance)" << std::endl;
    }
};

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// Helper: Polymorphic dispatch through interface pointer
// ═════════════════════════════════════════════════════════════════════════════════════════════════

// EN: Liskov Substitution — works with ANY IDiagnosticProtocol subtype
void runDiagnosticSession(IDiagnosticProtocol& proto, const std::string& target) {
    std::cout << "  Protocol : " << proto.protocolName() << std::endl;
    proto.connect(target);
    std::cout << "  DTC Read : " << proto.readDTC() << std::endl;
    proto.clearDTC();
}

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// MAIN — Demonstrations
// ═════════════════════════════════════════════════════════════════════════════════════════════════

int main() {
    std::cout << "=== MODULE 8: C++ CORE GUIDELINES — CLASS DESIGN ===\n"
              << std::endl;

    // ─── Demo 1: Rule of Zero ────────────────────────────────────────────────────────────────────
    std::cout << "[Demo 1] Rule of Zero — SafeVehicleData\n";
    {
        SafeVehicleData car1("WBA12345678901234", {36.5, 37.1, 38.0});
        car1.print();

        // EN: Move semantics work automatically — no user-defined move ctor
        SafeVehicleData car2 = std::move(car1);
        std::cout << "  After move:\n";
        std::cout << "    car2 -> ";
        car2.print();
        std::cout << "    car1 -> ";
        car1.print();
    }

    std::cout << std::endl;

    // ─── Demo 2: Polymorphism through Interface ──────────────────────────────────────────────────
    std::cout << "[Demo 2] IDiagnosticProtocol — Polymorphic Dispatch\n";
    {
        OBD2Protocol obd2;
        CANProtocol can;

        std::cout << "  --- OBD2 Session ---\n";
        runDiagnosticSession(obd2, "EngineCU");

        std::cout << "  --- CAN Session ---\n";
        runDiagnosticSession(can, "CAN-HS-Bus");
    }

    std::cout << std::endl;

    // ─── Demo 3: Final Class ─────────────────────────────────────────────────────────────────────
    std::cout << "[Demo 3] final class — HardcodedConfig\n";
    {
        HardcodedConfig cfg(500000, "CAN-FD");
        cfg.print();

        // EN: Attempting `class Derived : public HardcodedConfig {}` would be a compile error
        // because HardcodedConfig is final.
        std::cout << "  (Any attempt to inherit from HardcodedConfig → compile error)"
                  << std::endl;
    }

    std::cout << std::endl;

    // ─── Demo 4: Deleted Copy Operations ─────────────────────────────────────────────────────────
    std::cout << "[Demo 4] Deleted Copy — SingletonECU\n";
    {
        SingletonECU& ecu = SingletonECU::instance();
        ecu.identify();

        SingletonECU& ecu2 = SingletonECU::instance();
        ecu2.identify();

        // EN: Both references point to the exact same object
        std::cout << "  Same instance? "
                  << (&ecu == &ecu2 ? "YES" : "NO") << std::endl;

        // EN: Uncommenting the next line would cause a compile error: SingletonECU copy = ecu;
        // // DELETED — C.21 enforced
    }

    std::cout << std::endl;

    // ─── Demo 5: override and final on Virtual Methods ───────────────────────────────────────────
    std::cout << "[Demo 5] override / final Keywords\n";
    {
        // EN: Demonstrate that override catches errors at compile time
        std::unique_ptr<IDiagnosticProtocol> proto =
            std::make_unique<OBD2Protocol>();
        std::cout << "  Protocol via base ptr: " << proto->protocolName() << std::endl;

        auto proto2 = std::make_unique<CANProtocol>();
        std::cout << "  Protocol via base ptr: " << proto2->protocolName() << std::endl;

        // EN: Both correctly dispatch through virtual table thanks to override
        std::cout << "  (Removing 'override' keyword would hide typo-bugs silently)"
                  << std::endl;
    }

    std::cout << "\n[ARCHITECTURAL CONCLUSION]:" << std::endl;
    std::cout << "1. Rule of Zero: Let RAII members handle resource management." << std::endl;
    std::cout << "2. Pure interfaces + override/final = safe polymorphic hierarchies." << std::endl;
    std::cout << "3. Delete copy/move on singletons and polymorphic bases." << std::endl;

    return 0;
}
