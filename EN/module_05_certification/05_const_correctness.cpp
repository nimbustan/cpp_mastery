/**
 * @file module_05_certification/05_const_correctness.cpp
 * @brief CPA/CPP Prep: const Correctness in C++
 *
 * @details
 * =============================================================================
 * [THEORY: const Member Functions]
 * =============================================================================
 * EN: A const member function promises NOT to modify the object's state. When you mark a method
 * as `const`, the compiler enforces that no data member is modified through `this`. Only const
 * member functions can be called on const objects — calling a non-const method on a const object
 * is a compile-time error.
 *
 *
 * =============================================================================
 * [THEORY: The mutable Keyword]
 * =============================================================================
 * EN: `mutable` allows a data member to be modified even inside a const member function. This is
 * used for caches, mutexes, and lazy initialization — things that do NOT change the
 * logical/observable state of the object.
 *
 *
 * =============================================================================
 * [THEORY: const_cast Dangers]
 * =============================================================================
 * EN: `const_cast` can remove the const qualifier, but removing const from data that was
 * ORIGINALLY declared const is Undefined Behavior! The compiler may place truly-const data in
 * read-only memory. The only legitimate use is interfacing with legacy C APIs that forgot const.
 *
 *
 * =============================================================================
 * [THEORY: East-const vs West-const]
 * =============================================================================
 * EN: `const int x` (west-const) and `int const x` (east-const) are identical. East-const reads
 * naturally right-to-left: "x is a const int". With pointers it matters: `int const* p` (pointer
 * to const int) vs `int* const p` (const pointer to int). East-const is consistent.
 *
 *
 * =============================================================================
 * [THEORY: const Propagation & API Design]
 * =============================================================================
 * EN: Passing by `const&` avoids copies while guaranteeing immutability. A well-designed API
 * marks every parameter and method const unless mutation is required. "const is a contract" — it
 * documents intent and enables compiler optimizations. const propagation means if you receive a
 * const ref, everything you pass it to must also accept const.
 *
 *
 * [CPPREF DEPTH: Const Propagation Through Pointers and the Mutable Escape Hatch]
 * =============================================================================
 * EN: `const` on a member function means `this` is `const T*`. BUT: pointers held inside the
 * class are only "shallow const" — `const` on `this` does NOT propagate through `T* member_`.
 * The member pointer itself cannot be reseated, but the pointee remains fully mutable. This is a
 * well-known hole in the const system. `std::experimental::propagate_const<T>` wraps a
 * pointer/smart-pointer and makes `operator*` / `operator->` propagate the constness of the
 * wrapper to the pointee, closing the gap. `mutable` bypasses const entirely — it was designed
 * for mutexes, caches, and lazy initialization (things that do not affect observable state).
 * Overusing `mutable` silently violates the const-correctness contract. Thread-safe const
 * (§17.6.5.9): the standard library assumes `const` member functions are safe to call
 * concurrently. If your `const` methods mutate shared mutable state without synchronization, you
 * have broken this contract and invoked undefined behavior in any STL algorithm.
 *
 *
 * @note Compile: g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion 05_const_correctness.cpp -o 05_const_correctness
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <iostream>   // EN: Standard I/O for console output
#include <string>     // EN: String class for text data
#include <mutex>      // EN: Mutex for thread-safe const access pattern
#include <cmath>      // EN: Math functions for expensive computation demo

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// SECTION 1: const and non-const Overloads — SensorCalibration
// EN: Demonstrates overload resolution based on const-ness of the object
// ═════════════════════════════════════════════════════════════════════════════════════════════════

class SensorCalibration {
private:
    double offset_ = 0.0;
    // EN: Sensor name used in automotive ECU calibration
    std::string sensorName_ = "DefaultSensor";

public:
    explicit SensorCalibration(std::string name, double offset)
        : offset_(offset), sensorName_(std::move(name)) {}

    // EN: const overload — returns by const reference (read-only access)
    const double& getValue() const {
        std::cout << "  [const getValue() called]\n";
        return offset_;
    }

    // EN: non-const overload — returns by reference (allows modification)
    double& getValue() {
        std::cout << "  [non-const getValue() called]\n";
        return offset_;
    }

    const std::string& getName() const { return sensorName_; }
};

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// SECTION 2: mutable Cache — ExpensiveCalculation
// EN: mutable allows caching inside a const method (logical constness)
// ═════════════════════════════════════════════════════════════════════════════════════════════════

class ExpensiveCalculation {
private:
    double input_;
    // EN: mutable — these can change even in a const context
    mutable double cachedResult_ = 0.0;
    mutable bool dirty_ = true;

    // EN: Simulates a heavy computation (e.g., aerodynamic drag coefficient)
    double computeExpensive() const {
        std::cout << "  [Performing expensive calculation...]\n";
        return std::sin(input_) * std::cos(input_) * 42.0;
    }

public:
    explicit ExpensiveCalculation(double input) : input_(input) {}

    // EN: const method but updates cache via mutable members
    double getResult() const {
        if (dirty_) {
            cachedResult_ = computeExpensive();
            dirty_ = false;
        }
        return cachedResult_;
    }

    void setInput(double input) {
        input_ = input;
        dirty_ = true;
    }
};

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// SECTION 3: ThreadSafeCounter with mutable std::mutex
// EN: mutable mutex allows locking in const methods for thread-safe reads
// ═════════════════════════════════════════════════════════════════════════════════════════════════

class ThreadSafeCounter {
private:
    int count_ = 0;
    // EN: mutable so we can lock even in const member functions
    mutable std::mutex mtx_;

public:
    // EN: const method — reads the counter (thread-safe pattern)
    int getCount() const {
        std::lock_guard<std::mutex> lock(mtx_);
        return count_;
    }

    void increment() {
        std::lock_guard<std::mutex> lock(mtx_);
        ++count_;
    }
};

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// SECTION 4: const Propagation Chain — VehicleConfig
// EN: Demonstrates const flowing through multiple function layers
// ═════════════════════════════════════════════════════════════════════════════════════════════════

struct VehicleConfig {
    std::string model = "EV-500";
    int maxSpeedKmh = 250;
    double batteryCapacityKwh = 75.0;
};

// ─── 4a. Low-level reader ────────────────────────────────────────────────────────────────────────
// EN: Accepts const ref — cannot modify the config
void printMaxSpeed(const VehicleConfig& config) {
    std::cout << "  Max speed: " << config.maxSpeedKmh << " km/h\n";
}

// ─── 4b. Mid-level validator ─────────────────────────────────────────────────────────────────────
// EN: Also takes const ref — propagates const guarantee downward
void validateConfig(const VehicleConfig& config) {
    std::cout << "  Validating config for: " << config.model << "\n";
    printMaxSpeed(config);
}

// ─── 4c. Top-level entry point ───────────────────────────────────────────────────────────────────
// EN: Entire chain preserves const — no accidental mutation possible
void processConfig(const VehicleConfig& config) {
    std::cout << "  Processing vehicle configuration...\n";
    validateConfig(config);
}

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// SECTION 5: const_cast Danger (demonstrated safely in comments)
// EN: Removing const from originally-const data is Undefined Behavior!
// ═════════════════════════════════════════════════════════════════════════════════════════════════

// EN: BAD PRACTICE — DO NOT DO THIS IN PRODUCTION:
//
// const int engineCode = 42; int& ref = const_cast<int&>(engineCode);   // UB if we write to
// ref! ref = 99;  // UNDEFINED BEHAVIOR — compiler may have placed 42 in ROM
//
// EN: The only acceptable use: calling a C API that takes non-const but promises not to modify:
//
// void legacy_c_api(char* str);   // C function, doesn't modify str const std::string msg =
// "ECU_OK"; legacy_c_api(const_cast<char*>(msg.c_str()));  // OK if API truly read-only

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// MAIN — Demonstrations
// ═════════════════════════════════════════════════════════════════════════════════════════════════

int main() {
    std::cout << "══════════════════════════════════════════════════════\n";
    std::cout << " const Correctness — C++ Certification Prep\n";
    std::cout << "══════════════════════════════════════════════════════\n\n";

    // ─── Demo 1: const vs non-const overload resolution ──────────────────────────────────────────
    // EN: The compiler picks the overload based on the const-ness of the object
    std::cout << "--- Demo 1: const vs non-const overload ---\n";
    {
        SensorCalibration sensor("TirePressure", 2.5);
        const SensorCalibration constSensor("Throttle", 1.0);

        // EN: Non-const object → non-const overload chosen
        std::cout << "Mutable sensor:\n";
        sensor.getValue() = 3.0;
        std::cout << "  Updated value: " << sensor.getValue() << "\n";

        // EN: const object → const overload chosen (cannot assign!)
        std::cout << "Const sensor:\n";
        double val = constSensor.getValue();
        std::cout << "  Read value: " << val << "\n";
        // constSensor.getValue() = 5.0;  // ERROR: assignment to const ref
    }

    // ─── Demo 2: mutable cache — lazy computation ────────────────────────────────────────────────
    // EN: const method computes on first call, returns cache on second
    std::cout << "\n--- Demo 2: mutable cache (lazy computation) ---\n";
    {
        const ExpensiveCalculation calc(1.5);
        std::cout << "First call:\n";
        std::cout << "  Result: " << calc.getResult() << "\n";
        std::cout << "Second call (cached):\n";
        std::cout << "  Result: " << calc.getResult() << "\n";
    }

    // ─── Demo 3: const reference chains ──────────────────────────────────────────────────────────
    // EN: const flows from top-level to every nested function call
    std::cout << "\n--- Demo 3: const propagation chain ---\n";
    {
        const VehicleConfig config{"EV-900", 300, 100.0};
        processConfig(config);
    }

    // ─── Demo 4: const object restrictions ───────────────────────────────────────────────────────
    // EN: Shows what you CAN and CANNOT do with a const object
    std::cout << "\n--- Demo 4: const object restrictions ---\n";
    {
        const ThreadSafeCounter counter;
        std::cout << "  Count (const access): " << counter.getCount() << "\n";
        // counter.increment();  // ERROR: cannot call non-const on const object

        ThreadSafeCounter mutableCounter;
        mutableCounter.increment();
        mutableCounter.increment();
        mutableCounter.increment();
        std::cout << "  Count (after 3 increments): "
                  << mutableCounter.getCount() << "\n";
    }

    // ─── Demo 5: East-const vs west-const ────────────────────────────────────────────────────────
    // EN: Both styles are identical — east-const reads right-to-left
    std::cout << "\n--- Demo 5: East-const vs West-const ---\n";
    {
        // EN: West-const style (traditional)
        const int westSpeed = 120;

        // EN: East-const style (reads right-to-left: "x is a const int")
        int const eastSpeed = 120;

        std::cout << "  West-const: " << westSpeed << "\n";
        std::cout << "  East-const: " << eastSpeed << "\n";
        std::cout << "  Identical? " << (westSpeed == eastSpeed ? "YES" : "NO")
                  << "\n";

        // EN: Where it really matters — pointers
        int speed = 200;
        int const* ptrToConst = &speed;      // pointer to const int
        int* const constPtr   = &speed;      // const pointer to int

        // *ptrToConst = 300;  // ERROR: can't modify through pointer-to-const
        *constPtr = 300;       // OK: the pointer is const, not the data
        std::cout << "  constPtr modification: speed = " << speed << "\n";
        (void)ptrToConst;
    }

    std::cout << "\n══════════════════════════════════════════════════════\n";
    std::cout << " All const correctness demos completed successfully!\n";
    std::cout << "══════════════════════════════════════════════════════\n";

    return 0;
}
