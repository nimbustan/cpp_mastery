/**
 * @file module_07_under_the_hood/06_abi_and_name_mangling.cpp
 * @brief EN: ABI (Application Binary Interface) and C++ Name Mangling
 *
 * @details
 * =====================================================================
 * [THEORY: ABI and Name Mangling]
 * =====================================================================
 *
 * EN: ABI (Application Binary Interface) — The Binary-Level Contract
 * An ABI defines how compiled code interacts at the binary level: calling conventions,
 * register usage, struct layout, vtable ordering, and how function names become symbols. Linked
 * object files must agree on the ABI or linker errors / crashes will follow.
 *
 * Name Mangling — Encoding Overloaded Signatures
 * The compiler "mangles" each function into a unique symbol. Under the Itanium ABI: foo(int)
 * → _Z3fooi   |   foo(int,float) → _Z3fooif Each overload gets a distinct mangled name so the
 * linker resolves the correct one.
 *
 * extern "C" — Disabling Name Mangling for Interoperability
 * `extern "C" { ... }` tells the compiler to use C linkage: no mangling. Essential for C
 * interop, shared libraries (.so/.dll), and dlopen/dlsym plugin APIs.
 *
 * Compiler ABI Incompatibility
 * MSVC, GCC, and Clang each use different mangling schemes. Mixing compilers (or major
 * versions) can cause linker failures. In automotive ECU software, the entire toolchain must be
 * locked to ensure ABI consistency.
 *
 * Tools: `c++filt` demangles symbols (echo _Z3fooi | c++filt → foo(int)). `nm` lists symbols
 * in object files. Combine: nm mylib.so | c++filt
 *
 * Shared Libraries: exposing a C ABI (extern "C") for .so files ensures any compiler version
 * can link your functions — standard for UDS, bootloaders, CAN layers.
 *
 * [CPPREF DEPTH: ABI Stability and the Itanium C++ ABI]
 * =============================================================================
 * EN: Most Linux/Unix compilers (GCC, Clang) follow the Itanium C++ ABI, which standardises name
 * mangling, vtable layout, exception handling, and RTTI representation. Name mangling encodes:
 * namespace, enclosing class, function name, parameter types (including cv-qualifiers and
 * references), and template arguments. `extern "C"` suppresses mangling so C code can link the
 * symbol directly. ABI breaks happen when: struct/class layout changes (adding or reordering
 * members), virtual function order changes, or the exception model changes. Compiling with
 * `-fvisibility=hidden` and explicitly exporting only public symbols
 * (`__attribute__((visibility("default")))`) produces smaller and faster shared libraries —
 * fewer relocations, faster `dlopen()`. This is the standard practice for plugin architectures.
 *
 * @note Compile: g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion 06_abi_and_name_mangling.cpp -o 06_abi_and_name_mangling
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <iostream>    // EN: Standard I/O for console output
#include <string>      // EN: std::string for sensor name lookups
#include <functional>  // EN: std::function for modern callback wrappers
#include <typeinfo>    // EN: typeid for RTTI-based type name inspection
#include <cstdio>      // EN: C-style printf for extern "C" callback demos
#include <array>       // EN: std::array for plugin dispatch table

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// SECTION 1: C++ Overloaded Functions — Name Mangling in Action
// ═════════════════════════════════════════════════════════════════════════════════════════════════

// EN: Three overloads share the name "readSensor" — the compiler mangles each into a unique
// symbol.

// ─── 1a. Read sensor by numeric ID ───────────────────────────────────────────────────────────────
int readSensor(int id) {
    // EN: Simulates reading raw ADC value from sensor channel
    int raw_value = 1000 + id * 42;
    std::cout << "  [readSensor(int)] Channel " << id
              << " → raw value: " << raw_value << "\n";
    return raw_value;
}

// ─── 1b. Read sensor by ID with calibration factor ───────────────────────────────────────────────
float readSensor(int id, float calibration) {
    // EN: Applies calibration offset to the sensor reading
    float calibrated = static_cast<float>(1000 + id * 42) * calibration;
    std::cout << "  [readSensor(int, float)] Channel " << id
              << " with cal=" << calibration
              << " → calibrated: " << calibrated << "\n";
    return calibrated;
}

// ─── 1c. Read sensor by name (string lookup) ─────────────────────────────────────────────────────
int readSensor(const std::string& name) {
    // EN: Simulates named sensor lookup (e.g., from config database)
    int resolved_value = static_cast<int>(name.length()) * 100 + 55;
    std::cout << "  [readSensor(string)] Sensor \"" << name
              << "\" → resolved value: " << resolved_value << "\n";
    return resolved_value;
}

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// SECTION 2: extern "C" Block — C-Compatible ECU API Stubs
// ═════════════════════════════════════════════════════════════════════════════════════════════════

// EN: extern "C" prevents name mangling — symbols are exported as plain C names.

extern "C" {

int ecu_init() {
    // EN: Initialize ECU hardware abstraction layer (stub)
    std::cout << "  [ecu_init] ECU initialized (firmware v2.4.1)\n";
    return 0;
}

void ecu_shutdown() {
    // EN: Graceful ECU shutdown — flush CAN buffers, store NVM
    std::cout << "  [ecu_shutdown] ECU shutdown complete\n";
}

int ecu_read_sensor(int channel) {
    // EN: Read raw sensor value via C-linkage API (no mangling)
    int value = 2048 + channel * 17;
    std::cout << "  [ecu_read_sensor] Channel " << channel
              << " → value: " << value << "\n";
    return value;
}

} // extern "C"

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// SECTION 3: C-Style Callback Pattern — Diagnostic Service
// ═════════════════════════════════════════════════════════════════════════════════════════════════

// EN: Function pointer typedef for diagnostic callbacks (C-compatible signature)
typedef void (*DiagCallback)(int code, const char* msg);

// EN: Global callback storage (simulates registration in a diagnostic manager)
static DiagCallback g_diag_callback = nullptr;

void registerCallback(DiagCallback cb) {
    // EN: Register a diagnostic callback function pointer
    g_diag_callback = cb;
    std::cout << "  [registerCallback] Callback registered at address: "
              << reinterpret_cast<void*>(cb) << "\n";
}

void triggerDiagnostic(int fault_code, const char* description) {
    // EN: Trigger the registered diagnostic callback if available
    if (g_diag_callback) {
        g_diag_callback(fault_code, description);
    } else {
        std::cout << "  [triggerDiagnostic] No callback registered!\n";
    }
}

// EN: Sample callback implementations (would be in separate modules in production)
void onDtcDetected(int code, const char* msg) {
    std::printf("  [DTC Handler] Fault 0x%04X: %s\n", code, msg);
}

void onDtcLogged(int code, const char* msg) {
    std::printf("  [DTC Logger]  Fault 0x%04X stored to NVM: %s\n", code, msg);
}

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// SECTION 4: Simulated Plugin System — Function Pointer Dispatch Table
// ═════════════════════════════════════════════════════════════════════════════════════════════════

// EN: Plugin entry point signature — compatible with dlsym/GetProcAddress patterns
using PluginFunc = int (*)(const char* command);

// EN: Simulated plugin implementations (in real systems, loaded from .so files)
int pluginDiagnostics(const char* command) {
    std::cout << "  [Plugin:Diagnostics] Executing: " << command << "\n";
    return 0;
}

int pluginFlasher(const char* command) {
    std::cout << "  [Plugin:Flasher] Executing: " << command << "\n";
    return 0;
}

int pluginCalibration(const char* command) {
    std::cout << "  [Plugin:Calibration] Executing: " << command << "\n";
    return 0;
}

// EN: Plugin dispatch table — maps slot indices to function pointers
struct PluginEntry {
    const char*  name;
    PluginFunc   func;
};

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// MAIN — Demonstration Scenarios
// ═════════════════════════════════════════════════════════════════════════════════════════════════

int main() {
    std::cout << "══════════════════════════════════════════════════════\n";
    std::cout << " ABI & Name Mangling — Automotive ECU Examples\n";
    std::cout << "══════════════════════════════════════════════════════\n\n";

    // ─── Demo 1: Overloaded Function Calls ───────────────────────────────────────────────────────
    // EN: Each call resolves to a different mangled symbol at link time
    std::cout << "── Demo 1: Overloaded readSensor() calls ──\n";
    readSensor(3);
    readSensor(3, 1.05f);
    readSensor(std::string("throttle_position"));
    std::cout << "\n";

    // ─── Demo 2: extern "C" Function Calls ───────────────────────────────────────────────────────
    // EN: These symbols have no mangling — plain C names in the symbol table
    std::cout << "── Demo 2: extern \"C\" ECU API calls ──\n";
    int init_status = ecu_init();
    std::cout << "  Init status: " << init_status << "\n";
    ecu_read_sensor(0);
    ecu_read_sensor(5);
    ecu_shutdown();
    std::cout << "\n";

    // ─── Demo 3: Register and Trigger C-Style Callbacks ──────────────────────────────────────────
    // EN: Function pointer callbacks — standard pattern for C-compatible diagnostic interfaces
    std::cout << "── Demo 3: Diagnostic callback registration ──\n";
    registerCallback(onDtcDetected);
    triggerDiagnostic(0xC410, "Catalyst temperature above threshold");

    registerCallback(onDtcLogged);
    triggerDiagnostic(0xB112, "CAN bus off — ECU communication lost");
    std::cout << "\n";

    // ─── Demo 4: Function Pointer Table (Simulated Plugin Dispatch) ──────────────────────────────
    // EN: In real systems, these pointers come from dlsym() on loaded .so files.
    std::cout << "── Demo 4: Plugin dispatch table ──\n";
    std::array<PluginEntry, 3> plugins = {{
        {"Diagnostics",  pluginDiagnostics},
        {"Flasher",      pluginFlasher},
        {"Calibration",  pluginCalibration}
    }};

    for (const auto& plugin : plugins) {
        std::cout << "  Dispatching to plugin [" << plugin.name << "]:\n";
        int result = plugin.func("self_test");
        std::cout << "    → Return code: " << result << "\n";
    }
    std::cout << "\n";

    // ─── Demo 5: typeid Names of Different Overloads ─────────────────────────────────────────────
    // EN: typeid reveals the compiler's internal type representation for each overload.
    std::cout << "── Demo 5: typeid of overloaded function pointers ──\n";
    int   (*fp_int)(int)                    = readSensor;
    float (*fp_int_float)(int, float)       = readSensor;
    int   (*fp_string)(const std::string&)  = readSensor;

    std::cout << "  readSensor(int)          → typeid: "
              << typeid(fp_int).name() << "\n";
    std::cout << "  readSensor(int, float)   → typeid: "
              << typeid(fp_int_float).name() << "\n";
    std::cout << "  readSensor(string)       → typeid: "
              << typeid(fp_string).name() << "\n";
    std::cout << "\n  Addresses (all distinct due to mangling):\n";
    std::cout << "    readSensor(int):        " << reinterpret_cast<void*>(fp_int) << "\n";
    std::cout << "    readSensor(int, float): " << reinterpret_cast<void*>(fp_int_float) << "\n";
    std::cout << "    readSensor(string):     " << reinterpret_cast<void*>(fp_string) << "\n";

    std::cout << "\n══════════════════════════════════════════════════════\n";
    std::cout << " Tip: compile and inspect mangled symbols with:\n";
    std::cout << "   nm ./06_abi_and_name_mangling | grep readSensor\n";
    std::cout << "   nm ./06_abi_and_name_mangling | grep ecu_ \n";
    std::cout << "   nm ./06_abi_and_name_mangling | c++filt | grep readSensor\n";
    std::cout << "══════════════════════════════════════════════════════\n";

    return 0;
}
