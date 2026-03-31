/**
 * @file module_08_core_guidelines/08_maintainable_cpp.cpp
 * @brief Maintainable C++ in Big Projects
 *
 * @details
 * =============================================================================
 * [THEORY: What Makes C++ Hard to Maintain?]
 * =============================================================================
 *
 * EN: Large C++ codebases (100K+ lines) face unique challenges:
 *
 *     1. COMPILE TIMES: A single header change can trigger rebuilds of 1000s of files
 *     2. DEPENDENCY HELL: Circular includes, tight coupling, transitive dependencies
 *     3. BUILD COMPLEXITY: Platform-specific code, conditional compilation (#ifdef soup)
 *     4. IMPLICIT BEHAVIOR: Implicit conversions, copy vs move, ADL surprises
 *     5. ABI FRAGILITY: Changing a class layout breaks binary compatibility
 *
 *     Key strategies:
 *     - Modular architecture with clear dependency graphs
 *     - Forward declarations to minimize includes
 *     - PIMPL for API boundaries
 *     - Static analysis (clang-tidy, cppcheck) in CI
 *     - Code review checklists specific to C++
 *
 * =============================================================================
 * [THEORY: Modular Architecture]
 * =============================================================================
 *
 * EN: Organize code into layers with clear dependency rules:
 *
 *     ┌─────────────────────────────────────────────────────┐
 *     │                  Application Layer                  │
 *     │    (main.cpp, CLI, GUI — depends on everything)     │
 *     ├─────────────────────────────────────────────────────┤
 *     │                  Service Layer                      │
 *     │    (Business logic, orchestration)                  │
 *     ├─────────────────────────────────────────────────────┤
 *     │                  Domain Layer                       │
 *     │    (Core types, interfaces — minimal dependencies)  │
 *     ├─────────────────────────────────────────────────────┤
 *     │                  Infrastructure Layer               │
 *     │    (Database, network, file I/O — adapters)         │
 *     ├─────────────────────────────────────────────────────┤
 *     │                  Platform Layer                     │
 *     │    (OS abstractions, HAL for embedded)              │
 *     └─────────────────────────────────────────────────────┘
 *
 *     Rules:
 *     - Dependencies flow DOWNWARD only (no upward calls)
 *     - Use interfaces (abstract classes) at layer boundaries
 *     - Domain layer has ZERO external dependencies
 *
 * @note Compile: g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion 08_maintainable_cpp.cpp -o 08_maintainable_cpp
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <algorithm>

// ═════════════════════════════════════════════════════════════════════════════
// PART 1: Dependency Inversion with Interfaces
// ═════════════════════════════════════════════════════════════════════════════

/**
 * EN: DEPENDENCY INVERSION PRINCIPLE (DIP):
 *     "High-level modules should not depend on low-level modules.
 *      Both should depend on abstractions."
 *
 *     Instead of:  DiagService → Database        (tight coupling)
 *     Use:         DiagService → IRepository ← Database  (loose coupling)
 *
 *     Benefits:
 *     - Swap implementations without changing business logic
 *     - Test with mock implementations
 *     - Compile only the interface, not the implementation
 */

// EN: DOMAIN LAYER — Pure interface, no implementation details
struct DiagnosticRecord {
    uint32_t dtc_code;
    std::string description;
    bool active;
};

class IDiagRepository {
public:
    virtual ~IDiagRepository() = default;
    virtual void store(const DiagnosticRecord& record) = 0;
    virtual std::vector<DiagnosticRecord> get_active() const = 0;
    virtual size_t count() const = 0;
};

// EN: INFRASTRUCTURE LAYER — Concrete implementation
class InMemoryDiagRepository : public IDiagRepository {
public:
    void store(const DiagnosticRecord& record) override {
        records_.push_back(record);
    }

    std::vector<DiagnosticRecord> get_active() const override {
        std::vector<DiagnosticRecord> result;
        std::copy_if(records_.begin(), records_.end(), std::back_inserter(result),
                     [](const auto& r) { return r.active; });
        return result;
    }

    size_t count() const override { return records_.size(); }

private:
    std::vector<DiagnosticRecord> records_;
};

// EN: SERVICE LAYER — Business logic, depends ONLY on interface
class DiagnosticService {
public:
    // EN: Constructor injection — the standard DI pattern in C++
    //     Takes interface by reference (caller owns the object)
    explicit DiagnosticService(IDiagRepository& repo)
        : repo_(repo) {}

    void report_fault(uint32_t code, const std::string& desc) {
        repo_.store({code, desc, true});
    }

    void clear_fault(uint32_t code, const std::string& desc) {
        repo_.store({code, desc, false});
    }

    void print_active_faults() const {
        auto active = repo_.get_active();
        std::cout << "    Active faults (" << active.size() << "):\n";
        for (const auto& r : active) {
            std::cout << "      DTC-" << r.dtc_code << ": " << r.description << "\n";
        }
    }

private:
    IDiagRepository& repo_;  // EN: Reference to interface, not concrete class
};

// ═════════════════════════════════════════════════════════════════════════════
// PART 2: Include Hygiene & Forward Declarations
// ═════════════════════════════════════════════════════════════════════════════

/**
 * EN: INCLUDE HYGIENE — Rules for fast builds in large projects:
 *
 *     1. FORWARD DECLARE whenever possible (instead of #include):
 *        class Sensor;  // Forward declaration — no include needed!
 *        void process(Sensor* s);  // Only pointer/reference → forward decl OK
 *
 *     2. INCLUDE WHAT YOU USE (IWYU):
 *        - Include the header that DEFINES what you use
 *        - Don't rely on transitive includes (they can change)
 *        - Tool: include-what-you-use (Google IWYU)
 *
 *     3. USE #pragma once or include guards:
 *        #pragma once                  // Modern compilers (simpler)
 *        #ifndef MY_HEADER_HPP         // Portable (works everywhere)
 *        #define MY_HEADER_HPP
 *        ...
 *        #endif
 *
 *     4. MINIMIZE header contents:
 *        - Put implementation in .cpp
 *        - Use PIMPL for complex classes
 *        - Forward declare in headers, include in .cpp
 *
 *     Build time impact of includes:
 *     ┌─────────────────────────┬──────────┬────────────────────────────┐
 *     │ Header                  │ Lines    │ Impact per TU              │
 *     ├─────────────────────────┼──────────┼────────────────────────────┤
 *     │ <iostream>              │ ~28,000  │ Heavy — avoid in headers!  │
 *     │ <vector>                │ ~12,000  │ Medium                     │
 *     │ <string>                │ ~7,000   │ Medium                     │
 *     │ <cstdint>               │ ~50      │ Light — use freely         │
 *     │ Forward declaration     │ 1        │ Zero cost                  │
 *     └─────────────────────────┴──────────┴────────────────────────────┘
 */

// ═════════════════════════════════════════════════════════════════════════════
// PART 3: Static Analysis Integration
// ═════════════════════════════════════════════════════════════════════════════

/**
 * EN: Static analysis catches bugs WITHOUT running the code.
 *     Essential tools for C++ projects:
 *
 *     1. clang-tidy — THE most important C++ linter
 *        - Checks: modernize-*, bugprone-*, performance-*, readability-*
 *        - Config file: .clang-tidy in project root
 *        - IDE integration: VS Code, CLion, Vim
 *        - Run: clang-tidy -p build src/main.cpp
 *
 *     2. cppcheck — Additional checker
 *        - Catches different bugs than clang-tidy
 *        - Run: cppcheck --enable=all --std=c++17 src/
 *
 *     3. Compiler warnings (FREE static analysis!):
 *        g++ -Wall -Wextra -Wpedantic -Wshadow -Wconversion
 *            -Wnon-virtual-dtor -Wold-style-cast -Wcast-align
 *            -Wunused -Woverloaded-virtual -Wnull-dereference
 *            -Wdouble-promotion -Wformat=2
 *
 *     4. AddressSanitizer (runtime, but catches memory bugs):
 *        g++ -fsanitize=address -fno-omit-frame-pointer -g
 */

// EN: Example .clang-tidy configuration
/*
    Checks: >
      -*,
      bugprone-*,
      modernize-*,
      performance-*,
      readability-*,
      cppcoreguidelines-*,
      -modernize-use-trailing-return-type,
      -readability-magic-numbers
    WarningsAsErrors: ''
    HeaderFilterRegex: '.*'
    FormatStyle: 'file'
*/

// ═════════════════════════════════════════════════════════════════════════════
// PART 4: Code Review Checklist for C++
// ═════════════════════════════════════════════════════════════════════════════

/**
 * EN: C++-Specific Code Review Checklist:
 *
 *     MEMORY & OWNERSHIP:
 *     [ ] No raw new/delete (use smart pointers)
 *     [ ] No dangling references or pointers
 *     [ ] Container elements not invalidated during iteration
 *     [ ] Move semantics used where appropriate
 *     [ ] RAII used for all resources (files, locks, sockets)
 *
 *     CORRECTNESS:
 *     [ ] No undefined behavior (signed overflow, null deref, etc.)
 *     [ ] No narrowing conversions (use static_cast or gsl::narrow)
 *     [ ] Rule of 0/3/5 followed (destructors, copy, move)
 *     [ ] Virtual destructors on base classes
 *     [ ] const correctness (const methods, const params)
 *
 *     PERFORMANCE:
 *     [ ] Pass large objects by const& (not by value)
 *     [ ] std::move used for sinks (parameters that will be stored)
 *     [ ] reserve() called on vectors with known sizes
 *     [ ] No unnecessary copies in range-for loops (use const auto&)
 *
 *     API DESIGN:
 *     [ ] explicit on single-arg constructors
 *     [ ] [[nodiscard]] on important return values
 *     [ ] noexcept on move operations and destructors
 *     [ ] Strong types for same-typed parameters
 *
 *     BUILD & MAINTENANCE:
 *     [ ] No new compiler warnings introduced
 *     [ ] Includes minimized (forward declarations used)
 *     [ ] Tests written for new/changed functionality
 *     [ ] No #define macros (use constexpr/templates instead)
 */

// ═════════════════════════════════════════════════════════════════════════════
// PART 5: Large-Scale Refactoring Strategies
// ═════════════════════════════════════════════════════════════════════════════

/**
 * EN: Strategies for refactoring large C++ codebases:
 *
 *     1. STRANGLER FIG PATTERN:
 *        - Don't rewrite everything at once
 *        - Wrap old code with new interface
 *        - Gradually move callers to new interface
 *        - Remove old code when unused
 *
 *     2. EXTRACT INTERFACE:
 *        - Add abstract base class above concrete class
 *        - Move callers to use the interface
 *        - Now multiple implementations possible
 *
 *     3. PIMPL MIGRATION:
 *        - Add Impl struct to class
 *        - Move private members to Impl
 *        - Now header changes don't recompile users
 *
 *     4. COMPILE-TIME SEAM (for testing):
 *        - Template the class on its dependencies
 *        - In production: real implementations
 *        - In tests: mock implementations
 */

// EN: Example of compile-time seam for testability
template<typename Repository>
class TestableService {
public:
    explicit TestableService(Repository& repo) : repo_(repo) {}

    void add_record(uint32_t code, const std::string& desc) {
        repo_.store({code, desc, true});
    }

    size_t total() const { return repo_.count(); }

private:
    Repository& repo_;
};

// EN: In tests: use a mock repository that tracks calls
class MockDiagRepository {
public:
    void store(const DiagnosticRecord& record) {
        stored_records.push_back(record);
    }
    std::vector<DiagnosticRecord> get_active() const { return {}; }
    size_t count() const { return stored_records.size(); }

    // EN: Test helpers
    std::vector<DiagnosticRecord> stored_records;
};

// ═════════════════════════════════════════════════════════════════════════════
// MAIN
// ═════════════════════════════════════════════════════════════════════════════
int main() {
    std::cout << "============================================\n";
    std::cout << "   Module 08 - Maintainable C++\n";
    std::cout << "============================================\n\n";

    // ─── Demo 1: Dependency Inversion ────────────────────────────────────
    {
        std::cout << "--- Demo 1: Dependency Inversion ---\n";

        InMemoryDiagRepository repo;
        DiagnosticService service(repo);

        service.report_fault(0x0100, "Engine coolant temp too high");
        service.report_fault(0x0201, "Oxygen sensor circuit open");
        service.clear_fault(0x0050, "Battery voltage low (cleared)");

        service.print_active_faults();
        std::cout << "    Total records: " << repo.count() << "\n\n";

        std::cout << "    Architecture:\n";
        std::cout << "      DiagnosticService → IDiagRepository (interface)\n";
        std::cout << "                           ↑\n";
        std::cout << "      InMemoryDiagRepository (implementation)\n";
        std::cout << "      (Can swap to DatabaseRepo, FileRepo, etc.)\n\n";
    }

    // ─── Demo 2: Compile-Time Seam (Testing) ────────────────────────────
    {
        std::cout << "--- Demo 2: Compile-Time Seam for Testing ---\n";

        // EN: Production: uses real repository
        InMemoryDiagRepository real_repo;
        TestableService<InMemoryDiagRepository> prod_service(real_repo);
        prod_service.add_record(0x0100, "Engine fault");
        std::cout << "    Production service records: " << prod_service.total() << "\n";

        // EN: Test: uses mock repository (tracks calls)
        MockDiagRepository mock_repo;
        TestableService<MockDiagRepository> test_service(mock_repo);
        test_service.add_record(0x0200, "Test fault");
        test_service.add_record(0x0300, "Another fault");
        std::cout << "    Mock service records: " << test_service.total() << "\n";
        std::cout << "    Mock captured DTC: " << mock_repo.stored_records[0].dtc_code << "\n\n";
    }

    // ─── Demo 3: Static Analysis Commands ────────────────────────────────
    {
        std::cout << "--- Demo 3: Static Analysis Integration ---\n";
        std::cout << "  clang-tidy:\n";
        std::cout << "    clang-tidy -p build src/*.cpp          # Analyze all sources\n";
        std::cout << "    clang-tidy --fix -p build src/main.cpp # Auto-fix issues\n\n";
        std::cout << "  cppcheck:\n";
        std::cout << "    cppcheck --enable=all --std=c++17 -I include/ src/\n\n";
        std::cout << "  Compiler warnings (add to CMake):\n";
        std::cout << "    -Wall -Wextra -Wpedantic -Wshadow -Wconversion\n";
        std::cout << "    -Wnon-virtual-dtor -Wold-style-cast -Wformat=2\n\n";
        std::cout << "  CI pipeline:\n";
        std::cout << "    1. clang-format --dry-run --Werror (formatting)\n";
        std::cout << "    2. clang-tidy -p build (static analysis)\n";
        std::cout << "    3. cmake --build build (compilation)\n";
        std::cout << "    4. ctest --test-dir build (testing)\n";
        std::cout << "    5. valgrind / ASan (memory checking)\n\n";
    }

    // ─── Demo 4: Build Time Optimization ─────────────────────────────────
    {
        std::cout << "--- Demo 4: Build Time Optimization ---\n";
        std::cout << "  ┌───────────────────────────────────────┬─────────────────────────────────┐\n";
        std::cout << "  │ Technique                             │ Impact                          │\n";
        std::cout << "  ├───────────────────────────────────────┼─────────────────────────────────┤\n";
        std::cout << "  │ Forward declarations                  │ Huge (avoids transitive incl.)  │\n";
        std::cout << "  │ PIMPL pattern                         │ Large (breaks header dependency)│\n";
        std::cout << "  │ ccache (compiler cache)               │ Huge for rebuild (90%+ cache)   │\n";
        std::cout << "  │ Ninja generator (vs Make)             │ Medium (better parallelism)     │\n";
        std::cout << "  │ Precompiled headers (PCH)             │ Large (parse STL once)          │\n";
        std::cout << "  │ C++20 Modules                         │ Very large (no re-parsing)      │\n";
        std::cout << "  │ Unity builds (UNITY_BUILD)            │ Large (fewer TU compilations)   │\n";
        std::cout << "  │ -j$(nproc) (parallel build)           │ Linear speedup with cores       │\n";
        std::cout << "  └───────────────────────────────────────┴─────────────────────────────────┘\n\n";
    }

    std::cout << "============================================\n";
    std::cout << "   End of Maintainable C++\n";
    std::cout << "============================================\n";

    return 0;
}
