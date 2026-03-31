/**
 * @file module_09_advanced_mechanics/01_bit_manipulation_and_alignment.cpp
 * @brief Advanced Mechanics: Bit Manipulation & Memory Alignment
 *
 * @details
 * =============================================================================
 * [1. Bitwise Manipulation]
 * =============================================================================
 * EN: Hardware interfaces, embedded systems, and automotive ECU firmware rely on Bit Masking.
 * Doing math at the bit level (`&`, `|`, `^`, `<<`, `>>`) is the absolute fastest way a CPU can
 * operate. Standard C++ also provides `<bitset>` to make handling flags easier.
 *
 *
 * =============================================================================
 * [2. Memory Alignment & Padding (alignas)]
 * =============================================================================
 * EN: Data structures are automatically "padded" by the compiler to align perfectly within the
 * CPU's cache lines. If you organize variables poorly inside a `struct`, you can waste up to 50%
 * of your RAM due to invisible Padding Bytes! We can control this using `alignas` or by ordering
 * members largest-to-smallest.
 *
 *
 * [CPPREF DEPTH: alignas, alignof, and the Strict Aliasing Rule]
 * =============================================================================
 * EN: `alignof(T)` returns the alignment requirement of type T in bytes. `alignas(N)` overrides
 * the default alignment, forcing a variable or type to be aligned to at least N bytes. The
 * Strict Aliasing Rule (§6.10) states: accessing an object through a pointer of incompatible
 * type is Undefined Behavior. Exception: `char*`, `unsigned char*`, and `std::byte*` may alias
 * any type. GCC's `-fno-strict-aliasing` disables this optimization at the cost of performance.
 * `std::aligned_storage` is deprecated in C++23 — prefer `alignas` with a `std::byte` array.
 *
 *
 * @note Compile: g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion 01_bit_manipulation_and_alignment.cpp -o 01_bit_manipulation_and_alignment
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <bitset>
#include <iostream>

// -------------------------------------------------------------------------------------------------
// [SECTION 1] RAM WASTING vs OPTIMAL STRUCT (Padding
// -------------------------------------------------------------------------------------------------
// EN: Order: 1 byte, 8 bytes, 4 bytes.
struct WastedMemoryStruct {
  char a;   // 1 byte (+7 bytes of hidden padding!) -> 8 byte block
  double b; // 8 bytes                              -> 8 byte block
  int c;    // 4 bytes (+4 bytes of hidden padding!)-> 8 byte block
}; // Total Size = 24 Bytes (Almost 50% Memory Wasted!)

// EN: Order: Largest to Smallest (8, 4, 1).
struct OptimizedStruct {
  double b; // 8 bytes
  int c;    // 4 bytes
  char a;   // 1 byte (+3 bytes padding at the end)
}; // Total Size = 16 Bytes (Much better!)

// EN: Forcing absolute cache-line alignment (e.g. 64-bytes for AVX/SIMD instructions)
struct alignas(64) SimdVector {
  float x, y, z, w;
};

// -------------------------------------------------------------------------------------------------
// [SECTION 2] BITWISE FLAGS vs BOOLEANS
// -------------------------------------------------------------------------------------------------
// EN: Instead of using 8 `bool` variables (which take 8 Full Bytes), use 8 Bits (1 Byte). TR: 8
// etmek:
const unsigned char FLAG_RUNNING = 1 << 0;    // 00000001 (Engine Running)
const unsigned char FLAG_ABS_ACTIVE = 1 << 1; // 00000010 (ABS Active)
const unsigned char FLAG_LIGHTS_ON = 1 << 2;  // 00000100 (Headlights On)
const unsigned char FLAG_DOOR_OPEN = 1 << 3;  // 00001000 (Door Open)

int main() {
  std::cout << "=== MODULE 9: BIT MANIPULATION & ALIGNMENT ===\n" << std::endl;

  // --- ALIGNMENT TESTS ---
  std::cout << "[MEMORY ALIGNMENT]" << std::endl;
  std::cout << "Size of WastedMemoryStruct: " << sizeof(WastedMemoryStruct) << " bytes" <<
      std::endl;
  std::cout << "Size of OptimizedStruct   : " << sizeof(OptimizedStruct) << " bytes" << std::endl;
  std::cout << "Size of alignas(64) Vector: " << sizeof(SimdVector) << " bytes\n" << std::endl;

  // --- BITWISE TESTS (C-Style) ---
  std::cout << "[BIT MANIPULATION (C-Style)]" << std::endl;
  unsigned char ecuStatusFlags = 0; // 00000000

  // Set flags (OR `|`)
  ecuStatusFlags |= FLAG_RUNNING;
  ecuStatusFlags |= FLAG_LIGHTS_ON;

  // Check flags (AND `&`)
  if (ecuStatusFlags & FLAG_RUNNING) {
    std::cout << "- ECU Engine is Running!" << std::endl;
  }

  // Toggle flags (XOR `^`)
  ecuStatusFlags ^= FLAG_LIGHTS_ON; // Turns headlights flag OFF

  // --- BITWISE TESTS (Modern C++ std::bitset) ---
  std::cout << "\n[BIT MANIPULATION (Modern C++ <bitset>)]" << std::endl;
  // EN: Modern C++ provides a safe object wrapper around bits.
  std::bitset<8> modernECUStatus;
  modernECUStatus.set(0); // Bit 0 (Engine Running) -> True
  modernECUStatus.set(3); // Bit 3 (Brake Active) -> True

  std::cout << "Current 8-Bit Status: " << modernECUStatus << std::endl;
  std::cout << "Is brake active? : " << (modernECUStatus.test(3) ? "Yes" : "No") << std::endl;

  return 0;
}
