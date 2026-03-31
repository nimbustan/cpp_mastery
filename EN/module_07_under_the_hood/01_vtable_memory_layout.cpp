/**
 * @file module_07_under_the_hood/01_vtable_memory_layout.cpp
 * @brief Under the Hood: V-Table (Virtual Table) Memory Cost Proof
 *
 * @details
 * =============================================================================
 * [THEORY: Polymorphism is NOT Free! (What is a Vptr?)]
 * =============================================================================
 * EN: Polymorphism requires a mechanism to look up the correct function at RUNTIME. This is done
 * using a "Virtual Table" (V-Table). When a class has AT LEAST ONE `virtual` function, the
 * compiler silently inserts a completely hidden pointer into your class called `vptr` (Virtual
 * Pointer). This pointer points to the V-Table. Since it is a pointer, it adds exactly 8 Bytes
 * (on a 64-bit OS) to the size of EVERY SINGLE OBJECT you create!
 *
 * =============================================================================
 * [CPPREF DEPTH: The Devirtualization Optimization]
 * =============================================================================
 * EN: CppReference notes that modern compilers (GCC/Clang) try to perform "Devirtualization". If
 * the compiler can PROVE at compile-time the exact type of the polymorphic object, it bypasses
 * the V-Table lookup entirely and calls the function directly! (Massive speed boost). Using the
 * `final` keyword helps the compiler prove this.
 *
 * @note Compile: g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion 01_vtable_memory_layout.cpp -o 01_vtable_memory_layout
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <iostream>

// EN: 1. Plain Old Data Class. NO virtual keywords here.
class CheapCharacter {
public:
  int health;  // 4 Bytes
  float speed; // 4 Bytes
  // Expected Total: 8 Bytes.

  // EN: Regular function. Does NOT take up RAM space inside the object. Resolved at
  // Compile-Time.
  void jump() { std::cout << "Jump!" << std::endl; }
};

// EN: 2. Polymorphic Class (Contains a V-Table!).
class ExpensiveCharacter {
public:
  int health;  // 4 Bytes
  float speed; // 4 Bytes
  // Expected Total: 8 Bytes...

  // EN: BUT! The moment we say "virtual", the compiler secretly adds a `void* vptr;` behind the
  // scenes! vptr = 8 Bytes (on 64-bit systems). Total: 8 + 8 = 16 Bytes! (100% RAM overhead!)
  //
  virtual void jump() {
    std::cout << "Flying Jump!" << std::endl;
  }
};

int main() {
  std::cout << "=== MODULE 7: UNDER THE HOOD (V-TABLE PROOF) ===\n" << std::endl;

  std::cout << "[64-Bit OS Memory Estimation]" << std::endl;
  std::cout << "1 int + 1 float = Normally occupies 8 Bytes." << std::endl;

  std::cout << "\n----------------------------------------" << std::endl;
  std::cout << "1. CheapCharacter (NO Polymorphism): " << sizeof(CheapCharacter) << " Bytes." <<
      std::endl;
  std::cout << "-> Only Health (4) + Speed (4). Functions are free." << std::endl;

  std::cout << "\n----------------------------------------" << std::endl;
  std::cout << "2. ExpensiveCharacter (HAS Polymorphism): " << sizeof(ExpensiveCharacter) <<
      " Bytes!" << std::endl;
  std::cout << "-> WHY 16 BYTES? Because an 8-Byte 'vptr' (Virtual Pointer) "
               "was injected!"
            << std::endl;

  std::cout << "EN: If you instantiate 1 Million sensor objects in an ECU "
            << std::endl;
  std::cout << "    DO NOT use 'virtual' in the base class. It wastes RAM and "
               "causes Cache Misses!"
            << std::endl;

  return 0;
}
