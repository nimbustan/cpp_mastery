/**
 * @file module_07_under_the_hood/02_sigsegv_and_raii.cpp
 * @brief Under the Hood: Segmentation Fault (SIGSEGV) & Prevention via RAII
 *
 * @details
 * =============================================================================
 * [THEORY: Why do we get SIGSEGV (Crash)?]
 * =============================================================================
 * EN: The OS allocates specific RAM pages for your C++ program. If you try to read or write to a
 * memory address that the OS DID NOT GIVE YOU (like `nullptr`, or an array index that is out of
 * bounds), the hardware's MMU (Memory Management Unit) sends an interrupt to the OS. The OS
 * immediately ASSASSINATES your program with a signal called `SIGSEGV` (Segmentation Violation).
 *
 * =============================================================================
 * [THEORY: RAII - How it saves lives!]
 * =============================================================================
 * EN: RAII = "Resource Acquisition Is Initialization". Manual `new` and `delete` leads to 90% of
 * human errors. If you forget `delete`, you get a Memory Leak. If you delete twice, you get a
 * Double Free Crash. RAII dictates: "Entrust the Resource (RAM/File/Mutex) to a C++ Object. When
 * the object goes out of scope {...}, its Destructor AUTOMATICALLY CLEANS IT UP." (e.g.,
 * std::unique_ptr, std::vector).
 *
 * =============================================================================
 * [CPPREF DEPTH: OS Core Dumps]
 * =============================================================================
 * EN: When SIGSEGV happens, Linux doesn't just kill the program. It violently dumps the entire
 * RAM state (Memory map, registers) into a massive file called a "Core Dump" (The corpse of your
 * program). You can later load this Core Dump into GDB (`gdb ./app core`) to do an autopsy and
 * see exactly which line of C++ triggered the assassination!
 *
 * @note Compile: g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion 02_sigsegv_and_raii.cpp -o 02_sigsegv_and_raii
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <iostream>
#include <memory>
#include <vector>

// EN: Dangerous legacy C-style function. WARNING: SIGSEGV RISK!
void dangerousMemoryAccess() {
  int *garbageAddress = nullptr; // Address 0x0000000
  // EN: Suppress warning — demo only
  (void)garbageAddress;

  // EN: BURIED SIGSEGV TRAP: If you uncomment the line below, the program is ASSASSINATED
  // INSTANTLY!
  //
  // *garbageAddress = 55; // SIGSEGV (Segmentation Fault: 11)!!!

  // EN: Why? Because the OS did not grant us permission to write to Address 0.
}

// EN: Memory Leak Trap (What happens without RAII?)
void createAndForget() {
  // EN: Stole 2000 Bytes from OS!
  int *array = new int[500];

  // EN: Imagine an error in the control loop
  bool randomErrorOccurred = true;

  if (randomErrorOccurred) {
    // EN: WARNING! We escape before reaching 'delete[] array'. RAM is lost forever (Leak).
    return;
  }

  // EN: Would have freed it if no error happened...
  delete[] array;
}

// EN: ------ THE ARCHITECTURE OF MODERN C++: RAII ------
class ActuatorController {
public:
  ActuatorController() {
  }
  ~ActuatorController() {
    std::cout << "[RAII] Actuator went out of scope! AUTO-RELEASED!!" << std::endl;
  }
};

int main() {
  std::cout << "=== MODULE 7: SIGSEGV (SEGMENTATION FAULT) & RAII ===\n" << std::endl;

  std::cout << "\n1. Risk of SIGSEGV" << std::endl;
  std::cout << "EN: The OS won't let you write to unowned RAM. It shoots you "
               "with SIGSEGV."
            << std::endl;

  dangerousMemoryAccess();

  std::cout << "\n2. The RAII Solution" << std::endl;

  // EN: Creating a Scope { ... }
  {
    // EN: STEP 1: Smart Pointer is the backbone of C++14 RAII. NO NEW, NO DELETE.
    std::unique_ptr<ActuatorController> safeObject = std::make_unique<ActuatorController>();

    // EN: STEP 2: std::vector is historically a RAII container itself.
    std::vector<int> smartArray(500, 10); // 2000 Bytes managed automatically!

    std::cout << "  -> Inside the Scope, everything is fine..." << std::endl;

    bool suddenCodeCrash = true;
    if (suddenCodeCrash) {
      // EN: Without RAII, breaking here would mean severe leaking...
    }

  // EN: THIS IS RAII! The moment this bracket closes, EVERYTHING is returned to OS
  // automatically!
  }

  return 0;
}
