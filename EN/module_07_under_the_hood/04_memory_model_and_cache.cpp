/**
 * @file module_07_under_the_hood/04_memory_model_and_cache.cpp
 * @brief EN: CPU cache hierarchy, memory layout, false sharing, and cache-friendly design
 *
 * @details
 * =============================================================================
 * [THEORY: CPU Cache Hierarchy & Memory Model]
 * =============================================================================
 *
 * EN: Modern CPUs do not access main memory (RAM) directly for every read/write.
 *     Instead, a hierarchy of small, fast caches sits between the core and RAM:
 *     1. L1 cache  — per-core, ~32-64 KB, ~1-4 cycle latency.
 *     2. L2 cache  — per-core, ~256 KB-1 MB, ~10-20 cycles.
 *     3. L3 cache  — shared across cores, ~4-64 MB, ~30-50 cycles.
 *     4. Main RAM  — ~100-300 cycles latency.
 *     Data is transferred in cache lines, typically 64 bytes at a time.
 *
 * =============================================================================
 * [THEORY: Spatial & Temporal Locality]
 * =============================================================================
 *
 * EN: Spatial Locality — When data is accessed, the entire cache line (64B) is fetched.
 * Contiguous memory (std::vector) is fast because the next element is already in cache.
 * Scattered memory (std::list, pointer-chasing) causes cache misses.
 *
 * Temporal Locality — Recently accessed data stays in cache and is fast to re-read. Hot loops
 * over small data sets benefit enormously from temporal locality.
 *
 * =============================================================================
 * [THEORY: False Sharing & SoA vs AoS]
 * =============================================================================
 *
 * EN: False Sharing — When two threads write to different variables on the SAME cache line,
 * hardware forces constant invalidation ("cache line ping-pong"). Fix: pad each variable to its
 * own 64-byte cache line with alignas(64).
 *
 * SoA vs AoS — AoS (Array of Structs): struct { float x,y,z; } arr[N]; Nice OOP, but iterating
 * one field loads unused fields into cache. SoA (Struct of Arrays): struct { float x[N], y[N],
 * z[N]; }; Better cache use when iterating one field across all elements.
 *
 * alignas keyword — alignas(N) forces a variable/struct to start at an N-byte boundary. Use
 * alignas(64) to ensure struct occupies its own cache line.
 *
 * =============================================================================
 * [CPPREF DEPTH: C++ Memory Model — Sequential Consistency and Acquire-Release Semantics]
 * =============================================================================
 *
 * EN: C++11 defines a formal memory model (§1.10) that governs how threads observe writes to
 * shared variables. `memory_order_seq_cst` (the default for std::atomic operations) provides a
 * total order visible to all threads — the simplest mental model but the most expensive on
 * weakly-ordered architectures (ARM, POWER). `memory_order_acquire` / `memory_order_release`
 * establish a happens-before relationship between a store-release and a load-acquire on the SAME
 * atomic variable, without imposing a full fence. `memory_order_relaxed` guarantees only
 * atomicity — no ordering at all — useful for pure counters. False sharing occurs when two
 * independent atomics (or hot variables) reside on the same cache line (typically 64 bytes),
 * forcing cores to bounce the line back and forth. C++17 exposes
 * `std::hardware_destructive_interference_size` so you can pad/align to avoid it. Always profile
 * before weakening memory orders; incorrect relaxed code creates data races that surface only
 * under heavy load.
 *
 * @note Compile: g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion 04_memory_model_and_cache.cpp -o 04_memory_model_and_cache
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <iostream>      // EN: Standard I/O
#include <vector>        // EN: Contiguous dynamic array
#include <list>          // EN: Doubly-linked list for comparison
#include <chrono>        // EN: High-resolution timing
#include <thread>        // EN: Thread support for false sharing demo
#include <numeric>       // EN: std::accumulate, std::iota
#include <algorithm>     // EN: std::generate
#include <cstdint>       // EN: Fixed-width integer types
#include <cstddef>       // EN: std::size_t, alignof

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// SECTION 1: Data Structures — AoS vs SoA (Automotive Sensors)
// ═════════════════════════════════════════════════════════════════════════════════════════════════

// EN: AoS — each sensor reading is one struct with all fields together
struct SensorAoS {
    float temp;
    float pressure;
    float voltage;
};

// EN: SoA — separate arrays for each field, better cache line utilization
struct SensorSoA {
    std::vector<float> temp;
    std::vector<float> pressure;
    std::vector<float> voltage;

    // EN: Resize all channels at once
    void resize(std::size_t n) {
        temp.resize(n);
        pressure.resize(n);
        voltage.resize(n);
    }
};

// ─── 1. False-Sharing Prevention with alignas ────────────────────────────────────────────────────

// EN: Without padding — both counters likely share a cache line
struct NaiveCounter {
    std::uint64_t counter_a = 0;
    std::uint64_t counter_b = 0;
};

// EN: With padding — each counter occupies its own 64-byte cache line
struct alignas(64) PaddedCounter {
    std::uint64_t value = 0;
};

// ─── 2. ECU Configuration with Cache-Line Alignment ──────────────────────────────────────────────

// EN: ECU config aligned to cache line boundary for DMA / shared-memory use
struct alignas(64) EcuConfig {
    std::uint32_t ecu_id;
    float max_rpm;
    float idle_rpm;
    float fuel_trim;
    std::uint8_t  cylinder_count;
    bool          turbo_enabled;
    // EN: Padding is implicit — alignas ensures 64-byte total boundary
};

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// SECTION 2: Benchmark Utilities
// ═════════════════════════════════════════════════════════════════════════════════════════════════

// EN: Simple RAII timer that prints elapsed time on destruction
class ScopedTimer {
public:
    explicit ScopedTimer(const char* label)
        : label_(label), start_(std::chrono::high_resolution_clock::now()) {}

    ~ScopedTimer() {
        auto end = std::chrono::high_resolution_clock::now();
        auto us  = std::chrono::duration_cast<std::chrono::microseconds>(end - start_).count();
        std::cout << "  [" << label_ << "] " << us << " us\n";
    }

    // EN: Non-copyable, non-movable
    ScopedTimer(const ScopedTimer&)            = delete;
    ScopedTimer& operator=(const ScopedTimer&) = delete;

private:
    const char* label_;
    std::chrono::high_resolution_clock::time_point start_;
};

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// SECTION 3: Demonstration Functions
// ═════════════════════════════════════════════════════════════════════════════════════════════════

// ─── 3. AoS vs SoA Benchmark ─────────────────────────────────────────────────────────────────────

// EN: Sum all temperature readings — AoS version (loads pressure & voltage too)
double sumTemperaturesAoS(const std::vector<SensorAoS>& sensors) {
    double total = 0.0;
    for (const auto& s : sensors) {
        total += static_cast<double>(s.temp);
    }
    return total;
}

// EN: Sum all temperature readings — SoA version (only touches temp array)
double sumTemperaturesSoA(const SensorSoA& sensors) {
    double total = 0.0;
    for (const auto& t : sensors.temp) {
        total += static_cast<double>(t);
    }
    return total;
}

// ─── 4. Vector vs List Iteration ─────────────────────────────────────────────────────────────────

// EN: Iterate and sum elements — tests spatial locality difference
void benchmarkVectorVsList(std::size_t n) {
    std::cout << "\n--- Vector vs List Iteration (n=" << n << ") ---\n";

    // EN: Fill a vector with sequential values
    std::vector<int> vec(n);
    std::iota(vec.begin(), vec.end(), 0);

    // EN: Fill a list with the same values
    std::list<int> lst(vec.begin(), vec.end());

    volatile long long sink = 0;

    {
        ScopedTimer t("vector sum");
        long long sum = 0;
        for (const auto& v : vec) { sum += v; }
        sink = sum;
    }

    {
        ScopedTimer t("list   sum");
        long long sum = 0;
        for (const auto& v : lst) { sum += v; }
        sink = sum;
    }

    (void)sink;
}

// ─── 5. False Sharing Demonstration ──────────────────────────────────────────────────────────────

static constexpr std::size_t kIncrements = 50'000'000;

// EN: Two threads hammer the same NaiveCounter — false sharing likely
void falseSharingNaive() {
    NaiveCounter nc;

    auto inc_a = [&]() {
        for (std::size_t i = 0; i < kIncrements; ++i) { ++nc.counter_a; }
    };
    auto inc_b = [&]() {
        for (std::size_t i = 0; i < kIncrements; ++i) { ++nc.counter_b; }
    };

    ScopedTimer t("naive  (shared line)");
    std::thread t1(inc_a);
    std::thread t2(inc_b);
    t1.join();
    t2.join();
}

// EN: Two threads on separate PaddedCounters — no false sharing
void falseSharingPadded() {
    PaddedCounter pa;
    PaddedCounter pb;

    auto inc_a = [&]() {
        for (std::size_t i = 0; i < kIncrements; ++i) { ++pa.value; }
    };
    auto inc_b = [&]() {
        for (std::size_t i = 0; i < kIncrements; ++i) { ++pb.value; }
    };

    ScopedTimer t("padded (own line)   ");
    std::thread t1(inc_a);
    std::thread t2(inc_b);
    t1.join();
    t2.join();
}

// ─── 6. Master Demonstration ─────────────────────────────────────────────────────────────────────

void demonstrateCacheEffects() {
    constexpr std::size_t N = 2'000'000;

    // EN: Prepare AoS data
    std::vector<SensorAoS> aos(N);
    for (std::size_t i = 0; i < N; ++i) {
        aos[i].temp     = static_cast<float>(i) * 0.01f;
        aos[i].pressure = static_cast<float>(i) * 0.02f;
        aos[i].voltage  = static_cast<float>(i) * 0.03f;
    }

    // EN: Prepare SoA data
    SensorSoA soa;
    soa.resize(N);
    for (std::size_t i = 0; i < N; ++i) {
        soa.temp[i]     = static_cast<float>(i) * 0.01f;
        soa.pressure[i] = static_cast<float>(i) * 0.02f;
        soa.voltage[i]  = static_cast<float>(i) * 0.03f;
    }

    std::cout << "--- AoS vs SoA Temperature Sum (N=" << N << ") ---\n";

    volatile double result = 0.0;

    {
        ScopedTimer t("AoS temp sum");
        result = sumTemperaturesAoS(aos);
    }
    {
        ScopedTimer t("SoA temp sum");
        result = sumTemperaturesSoA(soa);
    }

    (void)result;

    // EN: Vector vs list comparison
    benchmarkVectorVsList(N);

    // EN: False sharing comparison
    std::cout << "\n--- False Sharing: Naive vs Padded ---\n";
    falseSharingNaive();
    falseSharingPadded();
}

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// SECTION 4: main() — All Demos
// ═════════════════════════════════════════════════════════════════════════════════════════════════

int main() {
    std::cout << "============================================\n";
    std::cout << " Module 07 — Memory Model & Cache Effects\n";
    std::cout << "============================================\n\n";

    // ─── Demo 1: sizeof and alignment checks ─────────────────────────────────────────────────────
    std::cout << "--- Demo 1: sizeof / alignof ---\n";

    std::cout << "  sizeof(SensorAoS)   = " << sizeof(SensorAoS)   << " bytes\n";
    std::cout << "  alignof(SensorAoS)  = " << alignof(SensorAoS)  << " bytes\n";
    std::cout << "  sizeof(NaiveCounter) = " << sizeof(NaiveCounter) << " bytes\n";
    std::cout << "  sizeof(PaddedCounter)= " << sizeof(PaddedCounter)<< " bytes\n";
    std::cout << "  alignof(PaddedCounter)=" << alignof(PaddedCounter)<< " bytes\n";
    std::cout << "  sizeof(EcuConfig)   = " << sizeof(EcuConfig)   << " bytes\n";
    std::cout << "  alignof(EcuConfig)  = " << alignof(EcuConfig)  << " bytes\n\n";

    // EN: Verify PaddedCounter is on its own cache line
    static_assert(sizeof(PaddedCounter) >= 64,
                  "PaddedCounter must be at least 64 bytes for cache-line isolation");
    static_assert(alignof(PaddedCounter) == 64,
                  "PaddedCounter must be aligned to 64-byte boundary");

    // ─── Demo 2: AoS vs SoA benchmark ────────────────────────────────────────────────────────────
    std::cout << "--- Demo 2 & 3 & 4: Cache Effect Benchmarks ---\n";
    demonstrateCacheEffects();

    // ─── Demo 5: alignas applied to ECU config struct ────────────────────────────────────────────
    std::cout << "\n--- Demo 5: ECU Config Alignment ---\n";

    EcuConfig ecu{};
    ecu.ecu_id         = 0xA0B1;
    ecu.max_rpm        = 7500.0f;
    ecu.idle_rpm       = 800.0f;
    ecu.fuel_trim      = 1.02f;
    ecu.cylinder_count = 4;
    ecu.turbo_enabled  = true;

    // EN: Show that the struct address is 64-byte aligned
    auto addr = reinterpret_cast<std::uintptr_t>(&ecu);
    std::cout << "  EcuConfig address  : 0x" << std::hex << addr << std::dec << "\n";
    std::cout << "  64-byte aligned?   : " << ((addr % 64 == 0) ? "YES" : "NO") << "\n";
    std::cout << "  ecu_id             : 0x" << std::hex << ecu.ecu_id << std::dec << "\n";
    std::cout << "  max_rpm            : " << ecu.max_rpm << "\n";
    std::cout << "  idle_rpm           : " << ecu.idle_rpm << "\n";
    std::cout << "  fuel_trim          : " << ecu.fuel_trim << "\n";
    std::cout << "  cylinder_count     : " << static_cast<int>(ecu.cylinder_count) << "\n";
    std::cout << "  turbo_enabled      : " << std::boolalpha << ecu.turbo_enabled << "\n";

    std::cout << "\n============================================\n";
    std::cout << " All demos complete.\n";
    std::cout << "============================================\n";

    return 0;
}
