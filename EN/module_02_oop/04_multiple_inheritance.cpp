/**
 * @file module_02_oop/04_multiple_inheritance.cpp
 * @brief OOP Part 4: Multiple Inheritance & The Diamond Problem
 *
 * @details
 * =============================================================================
 * [THEORY: Why is Multiple Inheritance Banned in Many Languages?]
 * =============================================================================
 * EN: C++ is one of the very few languages that allows a class to have TWO or more Base classes.
 * `class FlyingCar : public Car, public Airplane`. This sounds cool but causes a deadly issue
 * known as the "Diamond Problem". Java and C# banned this entirely, relying on Interfaces
 * instead.
 *
 * =============================================================================
 * [THEORY: The Diamond Problem & Virtual Inheritance]
 * =============================================================================
 * EN: Imagine: A Base class 'Device' exists (with an int 'power').
 * - 'Printer' inherits from Device.
 * - 'Scanner' inherits from Device.
 * - 'Copier' inherits from BOTH Printer and Scanner!
 * Result: 'Copier' now contains TWO sets of 'power'! A waste of RAM and ambiguous. C++ solves
 * this using `virtual public` inheritance: "Only make 1 copy of Device."
 *
 * [CPPREF DEPTH: The Diamond Problem and Virtual Base Classes — Memory Layout]
 * =============================================================================
 * EN: With normal (non-virtual) multiple inheritance, each path to a shared base class creates a
 * separate sub-object. If both Printer and Scanner inherit from Device without `virtual`, Copier
 * contains TWO Device sub-objects — sizeof(Copier) includes both. Virtual inheritance changes
 * the ABI layout fundamentally. Instead of embedding the base sub-object directly, each
 * virtual-inheriting class stores a VBASE OFFSET (or vbase pointer) inside its vtable. This
 * offset tells the runtime where the single shared virtual base resides relative to the current
 * sub-object. The shared Device is placed at the END of the most-derived object's memory layout.
 * Construction order with virtual bases: ALL virtual bases are constructed FIRST, in
 * left-to-right, depth-first order — and only by the MOST DERIVED class's constructor (not
 * intermediate classes). Then non-virtual bases are constructed left-to-right, then the class
 * itself. sizeof() increases with virtual inheritance due to the hidden vptr or vbase-offset
 * pointers added by the compiler. On a typical 64-bit ABI, each virtual inheritance level adds 8
 * bytes per sub-object for the vptr. This is why `dynamic_cast` across virtual bases is more
 * expensive — it must traverse offsets stored in the vtable to locate the target.
 *
 * ⚠  Ref: https://en.cppreference.com/w/cpp/language/derived_class
 *
 * @note Compile: g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion 04_multiple_inheritance.cpp -o 04_multiple_inheritance
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <iostream>

// EN: 1. TOP LEVEL ROOT CLASS
class Device {
public:
  int powerConsumption;
  Device() : powerConsumption(100) {
    std::cout << "[Device] Root created." << std::endl;
  }
};

// EN: 2. SUBCLASS 1 (Virtual Inheritance!)
// EN: If we do not say 'virtual' here, the final child class will copy 'powerConsumption' twice!
class Printer : virtual public Device {
public:
  Printer() {
    std::cout << "[Printer] Print capability added." << std::endl;
  }
  void print() { std::cout << "Printing..." << std::endl; }
};

// EN: 3. SUBCLASS 2 (Virtual Inheritance!)
class Scanner : virtual public Device {
public:
  Scanner() {
    std::cout << "[Scanner] Scan capability added." << std::endl;
  }
  void scan() { std::cout << "Scanning..." << std::endl; }
};

// EN: 4. THE NIGHTMARE: MULTIPLE INHERITANCE
class AllInOneCopier : public Printer, public Scanner {
public:
  AllInOneCopier() {
    std::cout << "[AllInOneCopier] Synthesis ready!" << std::endl;
  }

  void showPower() {
    // EN: If we hadn't used 'virtual' inheritance above, 'powerConsumption' would be AMBIGUOUS
    // and the code WOULD NOT COMPILE. (It wouldn't know which 'power' to access).
    std::cout << "Power Consumption: " << powerConsumption
              << " Watt (Diamond Problem Solved!)" << std::endl;
  }
};

int main() {
  std::cout << "=== MODULE 2: MULTIPLE INHERITANCE (DIAMOND PROBLEM) ===\n" << std::endl;

  AllInOneCopier machine;

  std::cout << "\nMachine Tests:" << std::endl;
  machine.print();
  machine.scan();
  machine.showPower();

  return 0;
}
