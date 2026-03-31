/**
 * @file module_08_core_guidelines/05_smart_pointer_guidelines.cpp
 * @brief C++ Core Guidelines: Smart Pointer Ownership Rules
 *
 * @details
 * =============================================================================
 * [R.20: Use unique_ptr to represent exclusive ownership]
 * =============================================================================
 * EN: When exactly ONE entity owns a resource, use `std::unique_ptr`. It has ZERO runtime
 * overhead compared to a raw pointer — the compiler generates identical machine code. The
 * destructor automatically frees the resource when the owning scope exits, preventing every
 * possible memory leak.
 *
 * =============================================================================
 * [R.21: Prefer unique_ptr over shared_ptr unless you need shared ownership]
 * =============================================================================
 * EN: `shared_ptr` carries atomic reference-count overhead (control block + two atomic
 * increments/decrements per copy). Default to `unique_ptr`; promote to `shared_ptr` ONLY when
 * multiple owners genuinely need the same resource.
 *
 *
 * =============================================================================
 * [R.22: Use make_shared / make_unique — Exception safety + single allocation]
 * =============================================================================
 * EN: `std::make_unique<T>(args)` and `std::make_shared<T>(args)` prevent memory leaks in
 * complex expressions and, for shared_ptr, fuse the control block with the object into ONE
 * allocation (cache-friendly, fewer calls to the allocator).
 *
 *
 * =============================================================================
 * [R.24: Aliasing constructor — shared_ptr to a sub-object]
 * =============================================================================
 * EN: The aliasing constructor `shared_ptr<T>(owner, &owner->member)` lets you hand out a
 * pointer to a MEMBER while keeping the PARENT alive. No extra allocation. The control block is
 * shared with the parent.
 *
 *
 * =============================================================================
 * [R.30: Take smart pointers as parameters ONLY to express lifetime semantics]
 * =============================================================================
 * EN: If a function only READS or USES an object, take `const T&` or `T*`. Accepting
 * `unique_ptr<T>` means "I will take ownership." Accepting `shared_ptr<T>` means "I may share
 * ownership." Passing smart pointers when you don't need ownership semantics pollutes the API
 * and forces callers into a specific allocation strategy.
 *
 *
 * [CPPREF DEPTH: shared_ptr Control Block and Aliasing Constructor Internals]
 * =============================================================================
 * EN: `std::shared_ptr<T>` internally holds TWO pointers: one to the managed object and one to a
 * control block. The control block stores the strong reference count, the weak reference count,
 * the deleter, and (optionally) the allocator. `std::make_shared<T>(args...)` performs a SINGLE
 * heap allocation for both the object and the control block — more cache-friendly and fewer
 * allocator calls versus `shared_ptr<T>(new T)`. The aliasing constructor
 * `shared_ptr<U>(shared_ptr<T>, U*)` shares the control block of T but points to a different
 * object U — useful for pointing to a member of a shared object without extra ref counting.
 * `enable_shared_from_this<T>` embeds a hidden `weak_ptr<T>`; calling `shared_from_this()` on an
 * object NOT managed by a `shared_ptr` is UB.
 *
 *
 * @note Compile: g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion 05_smart_pointer_guidelines.cpp -o 05_smart_pointer_guidelines
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <chrono>   // EN: high_resolution_clock for timing benchmarks
#include <iostream> // EN: standard I/O
#include <memory>   // EN: smart pointers (unique_ptr, shared_ptr, weak_ptr)
#include <string>   // EN: std::string
#include <vector>   // EN: std::vector

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// PRODUCTION CODE — Automotive Smart Pointer Architecture
// ═════════════════════════════════════════════════════════════════════════════════════════════════

// ─── 1. unique_ptr Factory Pattern ───────────────────────────────────────────────────────────────

enum class ModuleType { BCM, Powertrain, ADAS, Infotainment };

class ECUModule {
public:
    std::string name;
    int firmwareVersion;

    ECUModule(std::string n, int fw) : name(std::move(n)), firmwareVersion(fw) {}
    virtual ~ECUModule() = default;

    virtual void selfTest() const {
        std::cout << "  [ECU] " << name << " v" << firmwareVersion
                  << " — generic self-test PASS" << std::endl;
    }
};

class ADASModule : public ECUModule {
public:
    ADASModule() : ECUModule("ADAS", 3) {}
    void selfTest() const override {
        std::cout << "  [ADAS] Camera + LiDAR calibration — PASS" << std::endl;
    }
};

// EN: R.20 + R.22 — factory returns unique_ptr (exclusive ownership)
std::unique_ptr<ECUModule> createModule(ModuleType type) {
    switch (type) {
        case ModuleType::BCM:
            return std::make_unique<ECUModule>("BCM", 7);
        case ModuleType::Powertrain:
            return std::make_unique<ECUModule>("Powertrain", 12);
        case ModuleType::ADAS:
            return std::make_unique<ADASModule>();
        case ModuleType::Infotainment:
            return std::make_unique<ECUModule>("Infotainment", 5);
    }
    return nullptr;
}

// ─── 2. shared_ptr — Shared Sensor Data Pool ─────────────────────────────────────────────────────

struct SensorDataPool {
    std::vector<double> temperatures;
    std::vector<double> pressures;

    SensorDataPool() : temperatures(128, 0.0), pressures(64, 0.0) {
        std::cout << "  [SensorDataPool] Allocated (128 temp + 64 pressure slots)"
                  << std::endl;
    }
    ~SensorDataPool() {
        std::cout << "  [SensorDataPool] Released — all consumers done" << std::endl;
    }
};

// ─── 3. Aliasing Constructor — Engine / Turbo ────────────────────────────────────────────────────

struct Turbo {
    int boostPSI = 22;
};

struct Engine {
    std::string model = "V6-TwinTurbo";
    Turbo turbo;
};

// ─── 4. R.30 — Correct function signatures ───────────────────────────────────────────────────────

// EN: GOOD — function only reads the object, takes const reference
void diagnose(const ECUModule& module) {
    std::cout << "  [Diag] Running diagnostics on: " << module.name << std::endl;
    module.selfTest();
}

// EN: GOOD — function takes ownership, accepts unique_ptr by value
void installModule(std::unique_ptr<ECUModule> module) {
    std::cout << "  [Install] Installed " << module->name
              << " (fw v" << module->firmwareVersion << ")" << std::endl;
    // EN: module is automatically destroyed at end of scope
}

// ─── 5. weak_ptr Observer — Cycle Breaking ───────────────────────────────────────────────────────

class SensorObserver {
    std::weak_ptr<SensorDataPool> pool_;
    std::string observerName_;

public:
    SensorObserver(std::shared_ptr<SensorDataPool> pool, std::string name)
        : pool_(pool), observerName_(std::move(name)) {}

    void readLatest() const {
        // EN: lock() promotes weak_ptr to shared_ptr if the object is alive
        if (auto sp = pool_.lock()) {
            std::cout << "  [" << observerName_ << "] Pool alive — temp slots: "
                      << sp->temperatures.size() << std::endl;
        } else {
            std::cout << "  [" << observerName_ << "] Pool EXPIRED — nothing to read"
                      << std::endl;
        }
    }
};

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// MAIN — Demonstrations
// ═════════════════════════════════════════════════════════════════════════════════════════════════

int main() {
    std::cout << "=== MODULE 8: C++ CORE GUIDELINES — SMART POINTER OWNERSHIP ===\n"
              << std::endl;

    // ─── Demo 1: unique_ptr Factory & Ownership Transfer ─────────────────────────────────────────
    std::cout << "[Demo 1] unique_ptr Factory & std::move\n";
    {
        auto bcm = createModule(ModuleType::BCM);
        auto adas = createModule(ModuleType::ADAS);

        // EN: R.30 — diagnose does NOT take ownership, receives const ref
        diagnose(*bcm);
        diagnose(*adas);

        // EN: Transfer ownership via std::move — bcm is now nullptr
        installModule(std::move(bcm));
        if (!bcm) {
            std::cout << "  bcm is nullptr after std::move (ownership transferred)\n";
        }
    }

    std::cout << std::endl;

    // ─── Demo 2: shared_ptr Reference Counting ───────────────────────────────────────────────────
    std::cout << "[Demo 2] shared_ptr Reference Counting\n";
    {
        auto pool = std::make_shared<SensorDataPool>();
        std::cout << "  use_count after creation  : " << pool.use_count() << std::endl;

        {
            // EN: Second consumer shares ownership
            auto consumer2 = pool;
            std::cout << "  use_count with 2 consumers: " << pool.use_count() << std::endl;

            auto consumer3 = pool;
            std::cout << "  use_count with 3 consumers: " << pool.use_count() << std::endl;
        }
        // EN: consumer2 and consumer3 destroyed — count drops back to 1
        std::cout << "  use_count after inner scope: " << pool.use_count() << std::endl;
    }

    std::cout << std::endl;

    // ─── Demo 3: Aliasing Constructor ────────────────────────────────────────────────────────────
    std::cout << "[Demo 3] Aliasing Constructor (R.24)\n";
    {
        auto engine = std::make_shared<Engine>();

        // EN: Aliasing — turboPtr points to engine->turbo but shares ownership
        std::shared_ptr<Turbo> turboPtr(engine, &engine->turbo);

        std::cout << "  engine use_count : " << engine.use_count() << std::endl;
        std::cout << "  turbo boost (PSI): " << turboPtr->boostPSI << std::endl;

        engine.reset();
        // EN: Engine object still alive because turboPtr holds shared ownership
        std::cout << "  turbo boost after engine.reset(): " << turboPtr->boostPSI
                  << " (engine kept alive by alias)" << std::endl;
    }

    std::cout << std::endl;

    // ─── Demo 4: weak_ptr Observer — Detect Expired Objects ──────────────────────────────────────
    std::cout << "[Demo 4] weak_ptr Observer Pattern\n";
    {
        SensorObserver observer(nullptr, "LateBinder");

        {
            auto pool = std::make_shared<SensorDataPool>();
            observer = SensorObserver(pool, "TempMonitor");

            // EN: Pool is alive — observer can read
            observer.readLatest();
        }
        // EN: Pool is destroyed — weak_ptr detects expiration
        observer.readLatest();
    }

    std::cout << std::endl;

    // ─── Demo 5: Performance — raw vs unique vs shared creation timing ───────────────────────────
    std::cout << "[Demo 5] Performance Comparison (creation timing)\n";
    {
        constexpr int N = 500000;

        // EN: Raw pointer allocation
        auto t0 = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < N; ++i) {
            auto* p = new ECUModule("raw", i);
            delete p;
        }
        auto t1 = std::chrono::high_resolution_clock::now();

        // EN: unique_ptr allocation (make_unique)
        auto t2 = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < N; ++i) {
            auto p = std::make_unique<ECUModule>("unique", i);
        }
        auto t3 = std::chrono::high_resolution_clock::now();

        // EN: shared_ptr allocation (make_shared)
        auto t4 = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < N; ++i) {
            auto p = std::make_shared<ECUModule>("shared", i);
        }
        auto t5 = std::chrono::high_resolution_clock::now();

        auto rawMs = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();
        auto uniqMs = std::chrono::duration_cast<std::chrono::milliseconds>(t3 - t2).count();
        auto shrMs = std::chrono::duration_cast<std::chrono::milliseconds>(t5 - t4).count();

        std::cout << "  " << N << " allocations each:\n";
        std::cout << "    raw new/delete : " << rawMs << " ms\n";
        std::cout << "    make_unique    : " << uniqMs << " ms\n";
        std::cout << "    make_shared    : " << shrMs << " ms\n";
        std::cout << "  (unique ≈ raw; shared slightly slower due to control block)"
                  << std::endl;
    }

    std::cout << "\n[ARCHITECTURAL CONCLUSION]:" << std::endl;
    std::cout << "1. Default to unique_ptr (zero overhead, exclusive ownership)." << std::endl;
    std::cout << "2. Use shared_ptr ONLY when multiple owners genuinely exist." << std::endl;
    std::cout << "3. Never pass smart pointers to functions that don't need ownership."
              << std::endl;

    return 0;
}

