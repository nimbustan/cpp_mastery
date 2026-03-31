/**
 * @file module_07_under_the_hood/10_linking_deepdive.cpp
 * @brief Linking: Static, Dynamic, LTO
 *
 * @details
 * =============================================================================
 * [THEORY: What Is Linking?]
 * =============================================================================
 *
 * EN: Linking is the FINAL stage of the build pipeline. The linker (ld) takes
 *     multiple object files (.o) and combines them into one executable or library.
 *
 *     Compilation produces .o files with UNRESOLVED symbols:
 *       main.o: calls "parse_frame" but doesn't define it
 *       parser.o: defines "parse_frame"
 *     Linker's job: match every call to its definition → executable.
 *
 *     Two types of linking:
 *     1. STATIC LINKING: Library code is COPIED into the executable at build time
 *     2. DYNAMIC LINKING: Library code stays in a separate .so/.dll file,
 *        loaded at runtime by the OS's dynamic linker (ld-linux.so)
 *
 * =============================================================================
 * [THEORY: Static vs Dynamic Libraries]
 * =============================================================================
 *
 * EN:
 *  ┌────────────────────┬──────────────────────────┬──────────────────────────┐
 *  │ Feature            │ Static (.a / .lib)       │ Dynamic (.so / .dll)     │
 *  ├────────────────────┼──────────────────────────┼──────────────────────────┤
 *  │ How it works       │ Code copied into exe     │ Code loaded at runtime   │
 *  │ File size          │ Larger executable        │ Smaller exe + .so files  │
 *  │ Deployment         │ Single file (easy)       │ Must ship .so files too  │
 *  │ Update library     │ Must RECOMPILE app       │ Just replace .so file    │
 *  │ Memory (N apps)    │ N copies in memory       │ ONE copy shared by all   │
 *  │ Load time          │ Faster (no runtime link) │ Slightly slower          │
 *  │ Symbol visibility  │ All symbols included     │ Only exported symbols    │
 *  │ Use case           │ Embedded/firmware, tests │ Plugins, system libs     │
 *  └────────────────────┴──────────────────────────┴──────────────────────────┘
 *
 * =============================================================================
 * [THEORY: Creating Static Libraries]
 * =============================================================================
 *
 * EN: Step 1: Compile source to object files
 *       g++ -c -o math.o math.cpp
 *       g++ -c -o utils.o utils.cpp
 *
 *     Step 2: Archive objects into .a file using `ar`
 *       ar rcs libmymath.a math.o utils.o
 *       ↑ r=replace, c=create, s=index (for faster linking)
 *
 *     Step 3: Link executable against the static library
 *       g++ -o app main.cpp -L. -lmymath
 *       ↑ -L. = look in current dir, -lmymath = link libmymath.a
 *
 *     CMake:
 *       add_library(mymath STATIC math.cpp utils.cpp)
 *       target_link_libraries(app PRIVATE mymath)
 *
 * =============================================================================
 * [THEORY: Creating Dynamic Libraries]
 * =============================================================================
 *
 * EN: Step 1: Compile with -fPIC (Position-Independent Code)
 *       g++ -c -fPIC -o math.o math.cpp
 *       ↑ -fPIC = code can run at ANY address (required for .so)
 *
 *     Step 2: Create shared library
 *       g++ -shared -o libmymath.so math.o utils.o
 *
 *     Step 3: Link executable
 *       g++ -o app main.cpp -L. -lmymath
 *
 *     Step 4: Runtime — OS must find the .so file!
 *       export LD_LIBRARY_PATH=.:$LD_LIBRARY_PATH  (temporary)
 *       sudo ldconfig                                (system-wide)
 *       g++ -o app main.cpp -L. -lmymath -Wl,-rpath,.  (embed path)
 *
 *     CMake:
 *       add_library(mymath SHARED math.cpp utils.cpp)
 *
 * =============================================================================
 * [THEORY: Symbol Visibility]
 * =============================================================================
 *
 * EN: By default on Linux, ALL symbols in a .so are exported (visible).
 *     This is a security and performance problem.
 *     Best practice: hide everything by default, export only what's needed.
 *
 *     Method 1: Compiler flag
 *       g++ -fvisibility=hidden -shared -o lib.so
 *       Then mark exported functions:
 *       __attribute__((visibility("default"))) void my_api_function();
 *
 *     Method 2: Linker version script
 *       g++ -shared -Wl,--version-script=exports.map -o lib.so
 *       # exports.map:
 *       { global: my_api_*; local: *; };
 *
 *     Method 3: CMake macro (portable)
 *       #if defined(_WIN32)
 *         #define MY_API __declspec(dllexport)
 *       #elif defined(__GNUC__)
 *         #define MY_API __attribute__((visibility("default")))
 *       #else
 *         #define MY_API
 *       #endif
 *
 * =============================================================================
 * [THEORY: rpath vs LD_LIBRARY_PATH vs ldconfig]
 * =============================================================================
 *
 * EN: Three ways the dynamic linker finds .so files at runtime:
 *
 *     1. LD_LIBRARY_PATH (environment variable):
 *        - Checked first by ld-linux.so
 *        - Temporary, per-session
 *        - DON'T use in production (security risk, fragile)
 *
 *     2. rpath (embedded in executable):
 *        - Stored in ELF header of the executable
 *        - Set at link time: -Wl,-rpath,/usr/local/lib
 *        - $ORIGIN trick: -Wl,-rpath,'$ORIGIN/../lib' (relative to exe)
 *        - BEST for application-specific libraries
 *
 *     3. ldconfig (system cache):
 *        - Reads /etc/ld.so.conf and /etc/ld.so.conf.d/*.conf
 *        - Builds cache at /etc/ld.so.cache
 *        - BEST for system-wide libraries
 *        - Run: sudo ldconfig after installing .so to /usr/local/lib
 *
 * =============================================================================
 * [THEORY: Library Versioning (soname)]
 * =============================================================================
 *
 * EN: Linux shared libraries use a versioning scheme:
 *
 *     libmymath.so.2.1.0    ← Real file (MAJOR.MINOR.PATCH)
 *     libmymath.so.2        ← Symlink (soname = MAJOR version)
 *     libmymath.so          ← Symlink (development link)
 *
 *     Rules:
 *     - MAJOR: ABI break (existing code must recompile)
 *     - MINOR: New features, backward compatible
 *     - PATCH: Bug fixes only
 *
 *     Setting soname:
 *       g++ -shared -Wl,-soname,libmymath.so.2 -o libmymath.so.2.1.0
 *       ln -s libmymath.so.2.1.0 libmymath.so.2
 *       ln -s libmymath.so.2 libmymath.so
 *
 *     CMake handles this automatically:
 *       set_target_properties(mymath PROPERTIES
 *           VERSION 2.1.0
 *           SOVERSION 2
 *       )
 *
 * =============================================================================
 * [THEORY: LTO — Link-Time Optimization]
 * =============================================================================
 *
 * EN: LTO lets the compiler optimize ACROSS translation unit boundaries.
 *     Normally, the compiler optimizes each .cpp file independently.
 *     With LTO, the linker sees ALL code and can:
 *     - Inline functions across .cpp files
 *     - Remove unused code more aggressively (dead code elimination)
 *     - Propagate constants across files
 *     - Devirtualize virtual calls when the type is known
 *
 *     Types of LTO:
 *     ┌────────────┬─────────────────────────────────────────────────────┐
 *     │ Type       │ Description                                         │
 *     ├────────────┼─────────────────────────────────────────────────────┤
 *     │ Full LTO   │ All TUs merged into one, then optimized             │
 *     │ (-flto)    │ ✅ Best optimization ❌ Slow, high memory (~2x)     │
 *     │            │                                                     │
 *     │ Thin LTO   │ TUs optimized in parallel with cross-TU info        │
 *     │ (-flto=thin│ ✅ Fast, parallel ❌ Slightly less optimal          │
 *     └────────────┴─────────────────────────────────────────────────────┘
 *
 *     Usage:
 *       g++ -flto -O2 -c main.cpp math.cpp       # Compile with LTO
 *       g++ -flto -O2 main.o math.o -o app        # Link with LTO
 *
 *       # Thin LTO (Clang only):
 *       clang++ -flto=thin -O2 -c main.cpp math.cpp
 *       clang++ -flto=thin -O2 main.o math.o -o app
 *
 *     CMake:
 *       set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)  # Enables -flto
 *       # Or per-target:
 *       set_target_properties(app PROPERTIES INTERPROCEDURAL_OPTIMIZATION ON)
 *
 *     Typical improvements: 5–20% faster, 10–30% smaller binary.
 *
 * @see https://gcc.gnu.org/wiki/LinkTimeOptimization
 * @see https://man7.org/linux/man-pages/man8/ldconfig.8.html
 *
 * @note Compile: g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion 10_linking_deepdive.cpp -o 10_linking_deepdive
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <iostream>
#include <string>
#include <cstdlib>
#include <dlfcn.h>    // EN: dlopen/dlsym for runtime dynamic loading

// ═════════════════════════════════════════════════════════════════════════════
// PART 1: Static Library Simulation
// ═════════════════════════════════════════════════════════════════════════════

/**
 * EN: These would normally live in a separate .cpp file compiled into libmath.a
 *     We keep them here for single-file demonstration.
 *
 */
namespace static_lib_demo {

    // EN: "Library" functions — imagine these are in math_lib.cpp → libmath.a
    int add(int a, int b) { return a + b; }
    int multiply(int a, int b) { return a * b; }

    void demonstrate() {
        std::cout << "  Static library symbol resolution:\n";
        std::cout << "    add(10, 20)      = " << add(10, 20) << "\n";
        std::cout << "    multiply(6, 7)   = " << multiply(6, 7) << "\n\n";

        std::cout << "  Build commands:\n";
        std::cout << "    # Step 1: Compile to .o\n";
        std::cout << "    g++ -c -o math.o math.cpp\n";
        std::cout << "    # Step 2: Archive into .a\n";
        std::cout << "    ar rcs libmath.a math.o\n";
        std::cout << "    # Step 3: Link\n";
        std::cout << "    g++ -o app main.cpp -L. -lmath\n\n";
    }
}

// ═════════════════════════════════════════════════════════════════════════════
// PART 2: Dynamic Library Runtime Loading (dlopen/dlsym)
// ═════════════════════════════════════════════════════════════════════════════

/**
 * EN: dlopen/dlsym let you load shared libraries AT RUNTIME (not at link time).
 *     This is the foundation of plugin systems:
 *
 *     void* handle = dlopen("libplugin.so", RTLD_LAZY);
 *     auto func = (func_ptr)dlsym(handle, "create_plugin");
 *     Plugin* p = func();
 *     // ... use plugin ...
 *     dlclose(handle);
 *
 *     Flags for dlopen:
 *     - RTLD_LAZY:  Resolve symbols on first use (faster startup)
 *     - RTLD_NOW:   Resolve all symbols immediately (safer, catches errors early)
 *     - RTLD_GLOBAL: Make symbols available to subsequently loaded libraries
 *
 */
void demonstrate_dlopen() {
    std::cout << "  Runtime dynamic loading (dlopen/dlsym):\n";

    // EN: Try to load libm.so (math library — always available on Linux)
    void* handle = dlopen("libm.so.6", RTLD_LAZY);
    if (!handle) {
        std::cout << "    dlopen failed: " << dlerror() << "\n";
        return;
    }

    // EN: Look up the cos() function by name
    using cos_func_t = double(*)(double);
    cos_func_t cos_fn = reinterpret_cast<cos_func_t>(dlsym(handle, "cos"));

    const char* error = dlerror();
    if (error) {
        std::cout << "    dlsym failed: " << error << "\n";
        dlclose(handle);
        return;
    }

    // EN: Call the dynamically loaded function
    double result = cos_fn(0.0);
    std::cout << "    Loaded cos() from libm.so.6 via dlsym\n";
    std::cout << "    cos(0.0) = " << result << "\n";

    dlclose(handle);
    std::cout << "    dlclose() called — library unloaded\n\n";
}

// ═════════════════════════════════════════════════════════════════════════════
// PART 3: Symbol Visibility Example
// ═════════════════════════════════════════════════════════════════════════════

/**
 * EN: Demonstrating visibility attributes.
 *     In a real shared library compiled with -fvisibility=hidden:
 *     - Functions WITHOUT the attribute are hidden (internal only)
 *     - Functions WITH __attribute__((visibility("default"))) are exported
 *
 */

// EN: Cross-platform export macro
#if defined(_WIN32)
    #define MY_API __declspec(dllexport)
#elif defined(__GNUC__)
    #define MY_API __attribute__((visibility("default")))
#else
    #define MY_API
#endif

// EN: EXPORTED — visible to users of the library
MY_API int public_api_function(int x) {
    return x * 2;
}

// EN: HIDDEN — internal implementation, not visible outside the library
static int internal_helper(int x) {
    return x + 1;
}

// ═════════════════════════════════════════════════════════════════════════════
// PART 4: Inspecting Binaries
// ═════════════════════════════════════════════════════════════════════════════

/**
 * EN: Essential commands for inspecting compiled binaries:
 *
 *     # See symbols in a binary or library:
 *     nm -C libmath.a           # -C = demangle C++ names
 *     nm -D libmath.so          # -D = dynamic symbols only
 *
 *     # See shared library dependencies:
 *     ldd ./my_app              # Lists all required .so files
 *     readelf -d ./my_app       # ELF dynamic section (rpath, soname)
 *
 *     # See symbol visibility in a .so:
 *     objdump -T libmath.so     # Dynamic symbol table
 *
 *     # See rpath embedded in executable:
 *     readelf -d ./my_app | grep -i path
 *
 *     # Check if LTO was used:
 *     readelf -S main.o | grep gnu.lto
 *
 */

// ═════════════════════════════════════════════════════════════════════════════
// MAIN
// ═════════════════════════════════════════════════════════════════════════════
int main() {
    std::cout << "============================================\n";
    std::cout << "   Module 07 - Linking Deep Dive\n";
    std::cout << "============================================\n\n";

    // ─── Demo 1: Static Library ──────────────────────────────────────────
    {
        std::cout << "--- Demo 1: Static Library (.a) ---\n";
        static_lib_demo::demonstrate();
    }

    // ─── Demo 2: Dynamic Loading ─────────────────────────────────────────
    {
        std::cout << "--- Demo 2: Dynamic Loading (dlopen/dlsym) ---\n";
        demonstrate_dlopen();
    }

    // ─── Demo 3: Symbol Visibility ───────────────────────────────────────
    {
        std::cout << "--- Demo 3: Symbol Visibility ---\n";
        std::cout << "  public_api_function(21) = " << public_api_function(21) << " (exported)\n";
        std::cout << "  internal_helper(21)     = " << internal_helper(21) << " (hidden/static)\n\n";

        std::cout << "  Visibility macro:\n";
        std::cout << "    #define MY_API __attribute__((visibility(\"default\")))\n";
        std::cout << "    Compile with: g++ -fvisibility=hidden -shared ...\n\n";
    }

    // ─── Demo 4: LTO Reference ──────────────────────────────────────────
    {
        std::cout << "--- Demo 4: LTO (Link-Time Optimization) ---\n";
        std::cout << "  Full LTO:\n";
        std::cout << "    g++ -flto -O2 -c main.cpp lib.cpp\n";
        std::cout << "    g++ -flto -O2 main.o lib.o -o app\n\n";
        std::cout << "  Thin LTO (Clang):\n";
        std::cout << "    clang++ -flto=thin -O2 -c main.cpp lib.cpp\n";
        std::cout << "    clang++ -flto=thin -O2 main.o lib.o -o app\n\n";
        std::cout << "  CMake:\n";
        std::cout << "    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)\n\n";
        std::cout << "  Typical gains: 5-20% runtime, 10-30% smaller binary\n\n";
    }

    // ─── Demo 5: Binary Inspection Commands ──────────────────────────────
    {
        std::cout << "--- Demo 5: Binary Inspection Commands ---\n";
        std::cout << "  ┌──────────────────────────────────┬───────────────────────────────────┐\n";
        std::cout << "  │ Command                          │ What it shows                     │\n";
        std::cout << "  ├──────────────────────────────────┼───────────────────────────────────┤\n";
        std::cout << "  │ nm -C libmath.a                  │ All symbols (demangled)           │\n";
        std::cout << "  │ nm -D libmath.so                 │ Dynamic (exported) symbols        │\n";
        std::cout << "  │ ldd ./app                        │ Shared library dependencies       │\n";
        std::cout << "  │ readelf -d ./app                 │ ELF dynamic section (rpath etc)   │\n";
        std::cout << "  │ objdump -T libmath.so            │ Dynamic symbol table              │\n";
        std::cout << "  │ readelf -S main.o | grep lto     │ Check if LTO was used             │\n";
        std::cout << "  │ file ./app                       │ Binary type, architecture         │\n";
        std::cout << "  │ size ./app                       │ Section sizes (text/data/bss)     │\n";
        std::cout << "  └──────────────────────────────────┴───────────────────────────────────┘\n\n";
    }

    // ─── Demo 6: Library Search Order ────────────────────────────────────
    {
        std::cout << "--- Demo 6: Dynamic Linker Search Order ---\n";
        std::cout << "  1. DT_RPATH in executable (deprecated, use DT_RUNPATH)\n";
        std::cout << "  2. LD_LIBRARY_PATH environment variable\n";
        std::cout << "  3. DT_RUNPATH in executable (-Wl,-rpath,...)\n";
        std::cout << "  4. /etc/ld.so.cache (built by ldconfig)\n";
        std::cout << "  5. /lib and /usr/lib (default paths)\n\n";
        std::cout << "  Best practice for deployment:\n";
        std::cout << "    Use rpath with $ORIGIN for portable apps:\n";
        std::cout << "    g++ -o app main.o -Wl,-rpath,'$ORIGIN/../lib' -L./lib -lmylib\n";
        std::cout << "    (finds .so relative to executable location)\n\n";
    }

    std::cout << "============================================\n";
    std::cout << "   End of Linking Deep Dive\n";
    std::cout << "============================================\n";

    return 0;
}

