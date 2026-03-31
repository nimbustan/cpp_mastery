/**
 * @file module_07_under_the_hood/05_compilation_pipeline.cpp
 * @brief EN: C++ Compilation Pipeline
 *
 * @details
 * =====================================================================
 * [THEORY: C++ Compilation Pipeline]
 * =====================================================================
 *
 * EN: The C++ build process transforms human-readable source into machine code through four
 * distinct stages. Understanding this pipeline is critical for diagnosing linker errors,
 * managing build times, and writing robust firmware for automotive ECUs.
 *
 * ── Stage 1: PREPROCESSING ── The preprocessor (cpp) runs before actual compilation. It
 * handles:
 * • #include — textual inclusion of header files
 * • #define / #ifdef / #ifndef — macro expansion & conditional compilation
 * • #pragma — compiler-specific directives
 * • __FILE__, __LINE__, __func__ — predefined macros
 * In automotive, conditional compilation selects HW-specific code paths (e.g.,
 * ECU_VARIANT_A vs ECU_VARIANT_B) from one codebase.
 *
 * ── Stage 2: COMPILATION ── The compiler (cc1plus) translates each preprocessed translation
 * unit (TU) into an object file (.o). Each .cpp file is ONE translation unit. Templates are
 * instantiated per-TU, which can cause code bloat if many TUs instantiate the same template with
 * the same types.
 *
 * ── Stage 3: LINKING ── The linker (ld) merges object files and resolves symbol references.
 * The One Definition Rule (ODR) states: every used entity must have exactly one definition
 * across all TUs. Violations cause undefined behavior or linker errors. `extern` declares a
 * symbol without defining it; the definition lives elsewhere. `inline` functions/variables are
 * ODR-exempt — the linker picks one copy. C++17 inline variables enable header-only definitions
 * of global state. `static` at file/namespace scope gives internal linkage — the symbol is
 * invisible outside its TU.
 *
 * ── Stage 4: LOADING ── The OS loader maps the executable into memory, resolves dynamic
 * library symbols, sets up the stack, and transfers control to _start → main(). In automotive,
 * ECU firmware is often flashed directly to ROM; the "loader" is the bootloader that copies code
 * to execution addresses.
 *
 * ── Header-Only Libraries ── Placing all code in headers is convenient (no separate .cpp to
 * compile) but can increase build times and binary size (code bloat) since every TU that
 * includes the header gets its own copy of non-inline definitions.
 *
 * [CPPREF DEPTH: One Definition Rule (ODR) — The Silent Killer of Large Projects]
 * =============================================================================
 * EN: The One Definition Rule states every entity must have exactly one definition across all
 * Translation Units. An ODR violation is undefined behavior — and no diagnostic is required!
 * Inline functions must have identical token-for-token definitions in every TU; otherwise the
 * linker silently picks one and discards the rest. Header-only libraries rely on the
 * inline/template ODR exemption: the definition may appear in multiple TUs provided every copy
 * is identical. In C++17, `inline` on a variable or function means "allow multiple definitions"
 * — it does NOT mean "please inline this call". Link-Time Optimization (LTO) can detect some ODR
 * violations that normal linking misses. C++20 Modules eliminate the textual inclusion model
 * entirely, making ODR violations structurally impossible for module-exported entities.
 *
 * @note Compile: g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion 05_compilation_pipeline.cpp -o 05_compilation_pipeline
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <iostream>   // EN: Standard I/O for console output
#include <string>     // EN: std::string for text handling
#include <sstream>    // EN: std::ostringstream for string building
#include <cstdint>    // EN: Fixed-width integer types (uint16_t, etc.)
#include <vector>     // EN: Dynamic array container
#include <array>      // EN: Fixed-size array container
#include <typeinfo>   // EN: typeid for RTTI demonstration

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// SECTION 1: INLINE CONSTEXPR AUTOMOTIVE CONFIGURATION
// ═════════════════════════════════════════════════════════════════════════════════════════════════

// EN: inline constexpr values can be defined in headers safely (ODR-exempt).

// ─── 1.1 Engine Parameters ───────────────────────────────────────────────────────────────────────

inline constexpr uint16_t MAX_RPM           = 7500;
// EN: Maximum engine revolutions per minute

inline constexpr uint16_t IDLE_RPM          = 800;
// EN: Idle engine speed target

inline constexpr float    MIN_VOLTAGE       = 9.0f;
// EN: Minimum battery voltage before ECU triggers low-voltage mode

inline constexpr float    MAX_COOLANT_TEMP  = 110.0f;
// EN: Coolant temperature threshold for overheating alarm (°C)

inline constexpr uint8_t  NUM_CYLINDERS     = 4;
// EN: Number of engine cylinders

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// SECTION 2: PREPROCESSOR — CONDITIONAL COMPILATION
// ═════════════════════════════════════════════════════════════════════════════════════════════════

// EN: ECU_DEBUG enables verbose diagnostics; undef for production builds.

#define ECU_DEBUG

#ifdef ECU_DEBUG
inline constexpr bool IS_DEBUG_BUILD = true;
#else
inline constexpr bool IS_DEBUG_BUILD = false;
#endif

// ─── 2.1 ECU_LOG Macro ───────────────────────────────────────────────────────────────────────────

// EN: Logging macro using __FILE__, __LINE__, __func__ predefined macros.

#define ECU_LOG(msg)                                                          \
    do {                                                                      \
        if constexpr (IS_DEBUG_BUILD) {                                       \
            std::cout << "[ECU_LOG] " << __FILE__                             \
                      << ":" << __LINE__                                      \
                      << " (" << __func__ << ") — " << msg << "\n";          \
        }                                                                     \
    } while (0)

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// SECTION 3: STATIC AT NAMESPACE SCOPE — INTERNAL LINKAGE
// ═════════════════════════════════════════════════════════════════════════════════════════════════

// EN: 'static' at namespace scope gives internal linkage — the variable exists only in this
// translation unit. Another TU can define its own copy without causing an ODR violation.

static int s_ecu_boot_count = 0;

// EN: Each TU that includes such a definition gets its own independent copy.

static void increment_boot_count() {
    ++s_ecu_boot_count;
}

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// SECTION 4: TEMPLATE INSTANTIATION — CODE GENERATION & BLOAT
// ═════════════════════════════════════════════════════════════════════════════════════════════════

// EN: Each unique template instantiation generates its own copy of code. If SensorBuffer<float,
// 64> and SensorBuffer<double, 64> are both used, the compiler emits TWO complete class
// implementations.

template <typename T, std::size_t Capacity>
class SensorBuffer {
public:
    // EN: Push a sensor reading into the circular buffer.
    void push(T value) {
        buffer_[write_idx_ % Capacity] = value;
        ++write_idx_;
    }

    // EN: Get the most recent reading.
    T latest() const {
        return (write_idx_ > 0)
            ? buffer_[(write_idx_ - 1) % Capacity]
            : T{};
    }

    // EN: Return how many elements have been pushed (may exceed Capacity).
    std::size_t total_writes() const { return write_idx_; }

    // EN: Return the fixed capacity of this buffer.
    static constexpr std::size_t capacity() { return Capacity; }

private:
    std::array<T, Capacity> buffer_{};
    std::size_t write_idx_ = 0;
};

// ─── 4.1 Extern Declaration Example ──────────────────────────────────────────────────────────────

// EN: 'extern' keyword declares a variable/function defined elsewhere. In a multi-TU project,
// the definition would be in exactly one .cpp file. Here we provide the definition immediately
// (single-TU demo).

extern const char* const ECU_FIRMWARE_VERSION;
const char* const ECU_FIRMWARE_VERSION = "v3.2.1-AUTOSAR";

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// SECTION 5: MAIN — DEMONSTRATIONS
// ═════════════════════════════════════════════════════════════════════════════════════════════════

int main() {
    std::cout << "╔═════════════════════════════════════════════════════════╗\n";
    std::cout << "║  Module 07 — 05: C++ Compilation Pipeline               ║\n";
    std::cout << "║  EN: From Source Code to Running Binary                 ║\n";
    std::cout << "╚═════════════════════════════════════════════════════════╝\n\n";

    // ─── Demo 1: Preprocessor Macros & Conditional Compilation ───────────────────────────────────

    std::cout << "── Demo 1: Preprocessor & Conditional Compilation ──\n";
    // EN: Show whether ECU_DEBUG is active at compile time.

    std::cout << "  IS_DEBUG_BUILD = " << std::boolalpha << IS_DEBUG_BUILD << "\n";

#ifdef ECU_DEBUG
    std::cout << "  [ECU_DEBUG] Diagnostic mode ACTIVE — extra checks enabled.\n";
    // EN: In production, #undef ECU_DEBUG removes this entire block.
#endif

    std::cout << "  Firmware: " << ECU_FIRMWARE_VERSION << "\n\n";

    // ─── Demo 2: ECU_LOG Macro with File/Line/Func Info ──────────────────────────────────────────

    std::cout << "── Demo 2: ECU_LOG Macro ──\n";
    // EN: ECU_LOG expands __FILE__, __LINE__, __func__ at the call site.

    ECU_LOG("Engine control module initialized");
    ECU_LOG("CAN bus ready — baud rate 500kbps");
    ECU_LOG("Sensor calibration complete");
    std::cout << "\n";

    // ─── Demo 3: inline constexpr Configuration Values ───────────────────────────────────────────

    std::cout << "── Demo 3: Inline Constexpr Configuration ──\n";
    // EN: These values are compile-time constants with no storage overhead.

    std::cout << "  MAX_RPM          = " << MAX_RPM << " rpm\n";
    std::cout << "  IDLE_RPM         = " << IDLE_RPM << " rpm\n";
    std::cout << "  MIN_VOLTAGE      = " << MIN_VOLTAGE << " V\n";
    std::cout << "  MAX_COOLANT_TEMP = " << MAX_COOLANT_TEMP << " °C\n";
    std::cout << "  NUM_CYLINDERS    = " << static_cast<int>(NUM_CYLINDERS) << "\n";

    // EN: Simulate an RPM range check using the constants.
    uint16_t current_rpm = 3200;
    if (current_rpm >= IDLE_RPM && current_rpm <= MAX_RPM) {
        std::cout << "  RPM " << current_rpm << " is within operating range.\n";
    }
    std::cout << "\n";

    // ─── Demo 4: Static Namespace-Scope Variable Behavior ────────────────────────────────────────

    std::cout << "── Demo 4: Static Internal Linkage ──\n";
    // EN: s_ecu_boot_count has internal linkage — it only exists in this TU. In a multi-file
    // project, each TU would have its own independent copy.

    std::cout << "  Boot count before: " << s_ecu_boot_count << "\n";
    increment_boot_count();
    increment_boot_count();
    increment_boot_count();
    std::cout << "  Boot count after 3 increments: " << s_ecu_boot_count << "\n";
    std::cout << "  (Each TU would maintain its own static copy)\n\n";

    // ─── Demo 5: Template Instantiation & Code Bloat ─────────────────────────────────────────────

    std::cout << "── Demo 5: Template Instantiation Sizes ──\n";
    // EN: Different instantiations create separate code — observe sizes.

    SensorBuffer<float, 32>    rpm_buffer;
    SensorBuffer<double, 32>   temp_buffer;
    SensorBuffer<uint16_t, 64> voltage_buffer;
    SensorBuffer<float, 128>   accel_buffer;

    // EN: Push some sample automotive sensor data.
    rpm_buffer.push(3200.0f);
    rpm_buffer.push(3450.5f);
    temp_buffer.push(92.3);
    voltage_buffer.push(1380);
    accel_buffer.push(0.98f);

    std::cout << "  sizeof SensorBuffer<float,   32>  = "
              << sizeof(rpm_buffer)     << " bytes\n";
    std::cout << "  sizeof SensorBuffer<double,  32>  = "
              << sizeof(temp_buffer)    << " bytes\n";
    std::cout << "  sizeof SensorBuffer<uint16_t,64>  = "
              << sizeof(voltage_buffer) << " bytes\n";
    std::cout << "  sizeof SensorBuffer<float,  128>  = "
              << sizeof(accel_buffer)   << " bytes\n";

    std::cout << "\n  Latest RPM reading:     " << rpm_buffer.latest() << " rpm\n";
    std::cout << "  Latest coolant temp:    " << temp_buffer.latest() << " °C\n";
    std::cout << "  Latest voltage (raw):   " << voltage_buffer.latest() << "\n";
    std::cout << "  Accel buffer capacity:  " << accel_buffer.capacity() << "\n";

    // EN: Each unique <T, N> combination generates a full class in the object file. With 4
    // instantiations, 4 copies of push(), latest(), etc. exist.

    std::cout << "\n  4 instantiations = 4× full class code in the object file.\n";
    std::cout << "  This is why header-only template libraries can cause bloat.\n";

    std::cout << "\n══════════════════════════════════════════════════════════════\n";
    std::cout << "  Pipeline: .cpp → Preprocessor → Compiler → Linker → Binary\n";
    std::cout << "                        ↑ macros     ↑ .o files  ↑ executable\n";
    std::cout << "══════════════════════════════════════════════════════════════\n";

    return 0;
}
