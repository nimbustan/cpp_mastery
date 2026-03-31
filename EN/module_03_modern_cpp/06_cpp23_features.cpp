/**
 * @file module_03_modern_cpp/06_cpp23_features.cpp
 * @brief C++23 Key Features
 *
 * @details
 * =============================================================================
 * [THEORY: C++23 — The Next Standard]
 * =============================================================================
 *
 * EN: C++23 brings significant quality-of-life and power features:
 *
 *     ┌───────────────────────────┬────────────────────────────────────────────┐
 *     │ Feature                   │ Header / Syntax                            │
 *     ├───────────────────────────┼────────────────────────────────────────────┤
 *     │ std::expected<T,E>        │ <expected> — Error handling without throw  │
 *     │ std::print / println      │ <print> — Type-safe I/O (fmtlib-based)     │
 *     │ std::generator<T>         │ <generator> — Standard coroutine generator │
 *     │ Deducing this             │ void f(this auto& self) — explicit self    │
 *     │ if consteval              │ Compile-time vs runtime branching          │
 *     │ std::mdspan               │ <mdspan> — Multidimensional view           │
 *     │ std::flat_map/flat_set    │ <flat_map> — Cache-friendly sorted map     │
 *     │ [[assume(expr)]]          │ Optimizer hint                             │
 *     │ std::stacktrace           │ <stacktrace> — Runtime call stack          │
 *     │ Multidimensional operator │ a[1,2,3] subscript                         │
 *     │ size_t literals           │ 42uz (std::size_t literal)                 │
 *     └───────────────────────────┴────────────────────────────────────────────┘
 *
 * Compiler support (GCC 13): std::expected ✓, deducing this ✓,
 * if consteval ✓. Print/generator/mdspan require GCC 14+.
 *
 * @note Compile: g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion 06_cpp23_features.cpp -o 06_cpp23_features
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <iostream>
#include <expected>
#include <string>
#include <vector>
#include <type_traits>
#include <cmath>

// ═════════════════════════════════════════════════════════════════════════════
// PART 1: std::expected<T,E> — Error Handling Without Exceptions
// ═════════════════════════════════════════════════════════════════════════════

// EN: Error type for sensor operations
enum class SensorError {
    NotCalibrated,
    OutOfRange,
    Disconnected
};

std::string to_string(SensorError e) {
    switch (e) {
        case SensorError::NotCalibrated: return "NotCalibrated";
        case SensorError::OutOfRange:    return "OutOfRange";
        case SensorError::Disconnected:  return "Disconnected";
    }
    return "Unknown";
}

// EN: Functions returning expected instead of throwing
std::expected<double, SensorError> readSensor(int id) {
    if (id < 0) return std::unexpected(SensorError::Disconnected);
    if (id > 100) return std::unexpected(SensorError::OutOfRange);
    return 25.0 + static_cast<double>(id) * 0.5;  // simulated reading
}

std::expected<double, SensorError> calibrate(double raw) {
    if (raw < 0.0) return std::unexpected(SensorError::NotCalibrated);
    return raw * 1.05;  // apply 5% calibration factor
}

std::expected<std::string, SensorError> formatReading(double calibrated) {
    return "Temperature: " + std::to_string(calibrated) + "°C";
}

// ═════════════════════════════════════════════════════════════════════════════
// PART 2: Deducing This
// ═════════════════════════════════════════════════════════════════════════════

// EN: "Deducing this" (explicit object parameter) lets a member function
//     deduce the type of *this, eliminating the need for const/non-const
//     overload pairs and enabling CRTP without inheritance.
//     NOTE: Requires GCC 14+ / Clang 18+ / MSVC 19.36+.
//     We demonstrate the PATTERN here with traditional code + reference syntax.

struct SensorData {
    std::vector<double> readings = {23.5, 24.1, 25.0, 22.8};

    // EN: Traditional C++ — two overloads needed:
    //     double& front() { return readings.front(); }
    //     const double& front() const { return readings.front(); }
    //
    // C++23 deducing this — SINGLE function replaces both:
    //     auto&& front(this auto& self) { return self.readings.front(); }
    //     Deduces const/non-const from caller automatically!

    double& front() { return readings.front(); }
    const double& front() const { return readings.front(); }
    size_t size() const { return readings.size(); }
};

// EN: CRTP-like without inheritance — deducing this reference
//     struct Printable {
//         void print(this auto const& self) {
//             std::cout << self.to_string();  // calls derived's method
//         }
//     };
//     struct Temp : Printable { string to_string() const { ... } };
//     Temp t(36.6); t.print();  // No virtual dispatch!

// ═════════════════════════════════════════════════════════════════════════════
// PART 3: if consteval
// ═════════════════════════════════════════════════════════════════════════════

// EN: consteval forces compile-time evaluation. if consteval branches
//     between compile-time and runtime code paths.

consteval int compile_time_sqrt_approx(int n) {
    // EN: Simple integer sqrt — only available at compile time
    int result = 0;
    while ((result + 1) * (result + 1) <= n) ++result;
    return result;
}

constexpr double flexible_sqrt(double x) {
    if consteval {
        // EN: At compile-time: use consteval-compatible algorithm
        double guess = x / 2.0;
        for (int i = 0; i < 20; ++i) {
            guess = (guess + x / guess) / 2.0;
        }
        return guess;
    } else {
        // EN: At runtime: use optimized library function
        return std::sqrt(x);
    }
}

// ═════════════════════════════════════════════════════════════════════════════
// MAIN
// ═════════════════════════════════════════════════════════════════════════════
int main() {
    std::cout << "============================================\n";
    std::cout << "   Module 03 - C++23 Features\n";
    std::cout << "============================================\n\n";

    // ─── Demo 1: std::expected basics ───────────────────────────────────
    {
        std::cout << "--- Demo 1: std::expected<T,E> ---\n";

        auto r1 = readSensor(42);
        auto r2 = readSensor(-1);
        auto r3 = readSensor(200);

        // EN: Check with has_value() or bool conversion
        if (r1) {
            std::cout << "  Sensor 42: " << r1.value() << "°C\n";
        }
        if (!r2) {
            std::cout << "  Sensor -1: ERROR " << to_string(r2.error()) << "\n";
        }
        if (!r3) {
            std::cout << "  Sensor 200: ERROR " << to_string(r3.error()) << "\n";
        }

        // EN: value_or() — default on error
        double safe = r2.value_or(-999.0);
        std::cout << "  value_or(-999): " << safe << "\n\n";
    }

    // ─── Demo 2: expected monadic operations ────────────────────────────
    {
        std::cout << "--- Demo 2: expected Monadic Chaining ---\n";

        // EN: and_then chains operations that return expected
        //     transform maps the value (like optional::transform)
        //     or_else handles the error case

        auto pipeline = readSensor(42)
            .and_then(calibrate)
            .transform([](double v) { return v + 0.5; })
            .and_then(formatReading);

        if (pipeline) {
            std::cout << "  OK: " << pipeline.value() << "\n";
        }

        // EN: Error pipeline — and_then short-circuits on error
        auto errPipeline = readSensor(-1)
            .and_then(calibrate)
            .and_then(formatReading)
            .or_else([](SensorError e) -> std::expected<std::string, SensorError> {
                return "FALLBACK: error was " + to_string(e);
            });

        if (errPipeline) {
            std::cout << "  Recovered: " << errPipeline.value() << "\n";
        }
        std::cout << "\n";
    }

    // ─── Demo 3: Deducing This ──────────────────────────────────────────
    {
        std::cout << "--- Demo 3: Deducing This (reference) ---\n";

        SensorData data;
        const SensorData cdata;

        // EN: Traditional — needs two overloads. Deducing this replaces both:
        //     auto&& front(this auto& self) { return self.readings.front(); }
        std::cout << "  Mutable front: " << data.front() << "\n";
        std::cout << "  Const front: " << cdata.front() << "\n";
        std::cout << "  Size: " << data.size() << "\n";

        data.front() = 99.9;
        std::cout << "  After modify: " << data.front() << "\n";

        // EN: Deducing this syntax (GCC 14+):
        std::cout << "\n  Deducing this eliminates overload pairs:\n";
        std::cout << "    // Before (2 functions):\n";
        std::cout << "    T& get() { return data_; }\n";
        std::cout << "    const T& get() const { return data_; }\n";
        std::cout << "    // After (1 function, C++23):\n";
        std::cout << "    auto&& get(this auto& self) { return self.data_; }\n\n";
    }

    // ─── Demo 4: if consteval ───────────────────────────────────────────
    {
        std::cout << "--- Demo 4: if consteval ---\n";

        // EN: Compile-time evaluation
        constexpr double ct_sqrt = flexible_sqrt(144.0);
        std::cout << "  constexpr sqrt(144) = " << ct_sqrt << "\n";

        // EN: Runtime evaluation — uses std::sqrt
        double runtime_val = 144.0;
        double rt_sqrt = flexible_sqrt(runtime_val);
        std::cout << "  runtime sqrt(144) = " << rt_sqrt << "\n";

        // EN: consteval integer sqrt
        constexpr int isqrt = compile_time_sqrt_approx(144);
        static_assert(isqrt == 12, "compile-time sqrt check");
        std::cout << "  consteval isqrt(144) = " << isqrt << "\n\n";
    }

    // ─── Demo 5: C++23 Features Reference (not yet in GCC 13) ──────────
    {
        std::cout << "--- Demo 5: C++23 Reference (compiler-dependent) ---\n";
        std::cout << "  std::print / std::println (GCC 14+, <print>):\n";
        std::cout << "    std::println(\"Sensor {}: {:.2f}C\", id, temp);\n";
        std::cout << "    // Type-safe, no format specifiers like printf\n\n";

        std::cout << "  std::generator<T> (GCC 14+, <generator>):\n";
        std::cout << "    std::generator<int> fibonacci() {\n";
        std::cout << "        int a=0, b=1;\n";
        std::cout << "        while (true) { co_yield a; auto t=a; a=b; b+=t; }\n";
        std::cout << "    }\n";
        std::cout << "    for (auto v : fibonacci() | views::take(10)) { ... }\n\n";

        std::cout << "  std::mdspan (GCC 14+, <mdspan>):\n";
        std::cout << "    int data[12];\n";
        std::cout << "    std::mdspan m(data, 3, 4);  // 3x4 matrix view\n";
        std::cout << "    m[1, 2] = 42;  // multi-dimensional subscript\n\n";

        std::cout << "  [[assume(expr)]] — optimizer hint:\n";
        std::cout << "    void process(int x) {\n";
        std::cout << "        [[assume(x > 0)]];  // UB if false, but optimizer uses it\n";
        std::cout << "    }\n\n";

        std::cout << "  std::flat_map / std::flat_set:\n";
        std::cout << "    See module_04_advanced/03_stl_containers_deepdive.cpp Demo 8\n\n";

        std::cout << "  Multidimensional subscript operator:\n";
        std::cout << "    template<class T> struct Matrix {\n";
        std::cout << "        T& operator[](size_t r, size_t c) { return data_[r*cols_+c]; }\n";
        std::cout << "    };\n";
        std::cout << "    m[2, 3] = 42;  // C++23: multiple args to operator[]\n\n";

        std::cout << "  size_t / signed size_t literals:\n";
        std::cout << "    auto x = 42uz;   // std::size_t\n";
        std::cout << "    auto y = 42z;    // std::ssize_t (signed)\n\n";
    }

    std::cout << "============================================\n";
    std::cout << "   End of C++23 Features\n";
    std::cout << "============================================\n";

    return 0;
}
