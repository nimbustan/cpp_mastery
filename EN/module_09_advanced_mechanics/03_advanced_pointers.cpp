/**
 * @file module_09_advanced_mechanics/03_advanced_pointers.cpp
 * @brief Advanced Mechanics: Pointers to Member Functions & std::invoke
 *
 * @details
 * =============================================================================
 * [Pointers to Data Members]
 * =============================================================================
 * EN: Regular pointers (`int*`) point to a specific memory address AT RUNTIME. Pointers to class
 * members (`int ECUModule::*`) don't point to RAM until they are combined with an actual
 * instantiated Object! They just hold an "offset" inside.
 *
 *
 * =============================================================================
 * [Pointers to Member Functions]
 * =============================================================================
 * EN: Used massively in Automotive Middleware (like AUTOSAR Adaptive) or UI frameworks (Qt
 * Signals/Slots) to store a "Function call to be executed later on an object". Modern C++17
 * provides `std::invoke` which makes the ugly syntax beautiful.
 *
 *
 * [CPPREF DEPTH: Pointer Arithmetic, Provenance, and std::launder]
 * =============================================================================
 * EN: Pointer arithmetic is defined only within an array (including one-past- the-end).
 * Dereferencing a pointer outside these bounds is UB. Pointer provenance: even if two pointers
 * hold the same numeric address, they may not be interchangeable — the optimizer tracks "where"
 * a pointer came from. `std::launder` (C++17) creates a new pointer to an object at a given
 * address, bypassing provenance tracking. It is needed after placement-new overwrites an object
 * whose type contains non-trivial `const` or reference members, because the compiler may cache
 * the old values through the original pointer.
 *
 *
 * @note Compile: g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion 03_advanced_pointers.cpp -o 03_advanced_pointers
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <functional> // for std::invoke (C++17)
#include <iostream>
#include <string>

class ECUModule {
public:
  std::string name;
  int messageBudget;
  int uptimeHours;

  // Constructor
  ECUModule(std::string n, int budget, int uptime)
      : name(std::move(n)), messageBudget(budget), uptimeHours(uptime) {}

  // Target Member Function 1
  void sendCANFrame(int priority) {
    if (messageBudget >= priority) {
      messageBudget -= priority;
      std::cout << "[" << name << "] CAN frame sent (Priority: " << priority
                << "). Budget left: " << messageBudget << std::endl;
    } else {
      std::cout << "[" << name << "] CAN budget exhausted!" << std::endl;
    }
  }

  // Target Member Function 2
  void activateDiagMode(int durationSecs) {
    std::cout << "[" << name << "] Diagnostic mode activated for "
              << durationSecs << " seconds." << std::endl;
  }
};

int main() {
  std::cout << "=== MODULE 9: POINTERS TO MEMBERS (ADVANCED) ===\n" << std::endl;

  ECUModule bcm("BCM_BodyControl", 50, 100);
  ECUModule hvac("HVAC_Climate", 10, 50);

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // 1. Pointer to a DATA MEMBER
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // EN: The ugly C++ syntax: Define a pointer that CAN hold an 'int' belonging to 'ECUModule'.
  int ECUModule::*pDataMetric = &ECUModule::messageBudget;

  // Accesses messageBudget!
  std::cout << "[DATA POINTER]: BCM Budget:  " << bcm.*pDataMetric << std::endl;
  // Same pointer, different object!
  std::cout << "[DATA POINTER]: HVAC Budget: " << hvac.*pDataMetric << std::endl;

  // EN: Change the pointer to point to 'uptimeHours' instead of 'messageBudget'.
  pDataMetric = &ECUModule::uptimeHours;
  std::cout << "[DATA POINTER]: BCM Uptime:  " << bcm.*pDataMetric << "\n" << std::endl;

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // 2. Pointer to a MEMBER FUNCTION
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // EN: The UGLIEST Syntax known to mankind. -> return_type (Class::*pointerName)(args); TR: C++
  void (ECUModule::*pAction)(int) = &ECUModule::sendCANFrame;

  // EN: Native execution (Requires the painful `.*` or `->*` operators)
  (bcm.*pAction)(10); // BCM sends a CAN frame!

  // EN: Wait! Let's change the "Action" mapping to diagnostic mode instead!
  pAction = &ECUModule::activateDiagMode;
  (hvac.*pAction)(5); // HVAC activates diagnostic mode!

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // 3. MODERN C++17 `std::invoke` (The Hero
  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // EN: std::invoke completely solves the horrible syntax above.
  std::cout << "\n[C++17 std::invoke MAGIC]" << std::endl;

  // Just invoke (Execute) the functionPointer ON the specific object WITH its arguments!
  // activates diagnostic mode using modern magic.
  std::invoke(pAction, bcm, 15);

  // Also works naturally with Member Data Gets
  // HVAC's uptime smoothly!
  int currentParam = std::invoke(pDataMetric, hvac);
  std::cout << "Invoked HVAC Uptime: " << currentParam << std::endl;

  return 0;
}
