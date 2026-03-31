/**
 * @file module_11_embedded_systems/01_volatile_hardware.cpp
 * @brief Embedded & Systems: volatile Keyword, Memory-Mapped I/O, ISR Concepts
 *
 * @details
 * =============================================================================
 * [THEORY: What is volatile?]
 * =============================================================================
 * EN: The `volatile` qualifier tells the compiler: "Do NOT optimize reads or writes to this
 * variable — its value can change at ANY time without the program's knowledge." This happens in
 * three critical situations:
 *   1) Memory-mapped hardware registers (GPIO pins, timers, ADC results)
 *   2) Variables modified by Interrupt Service Routines (ISR)
 *   3) Variables shared with DMA controllers or external hardware
 * Without `volatile`, the compiler may cache the value in a CPU register and never re-read it
 * from RAM — missing hardware changes entirely.
 *
 * =============================================================================
 * [THEORY: Memory-Mapped I/O — Hardware as Memory Addresses]
 * =============================================================================
 * EN: In embedded systems, hardware peripherals are controlled by reading from and writing to
 * specific MEMORY ADDRESSES. Writing `0x01` to address `0x40021000` might enable a GPIO clock.
 * The CPU bus routes these addresses directly to hardware registers instead of RAM. These
 * addresses are always accessed through `volatile` pointers to prevent compiler optimization.
 *
 * =============================================================================
 * [THEORY: Interrupt Service Routines (ISR)]
 * =============================================================================
 * EN: An ISR is a function called by the hardware (CPU interrupt controller) when an external
 * event occurs (button press, timer overflow, UART data received). ISRs preempt the main program
 * at any instruction boundary. ISR rules:
 *     1. Keep it SHORT.
 *     2. No heap allocation.
 *     3. No blocking calls.
 *     4. Use `volatile` for shared variables.
 *     5. Consider atomic types for multi-byte values.
 *
 * =============================================================================
 * [CPPREF DEPTH: volatile Does NOT Mean Thread-Safe!]
 * =============================================================================
 * EN: A critical misconception: `volatile` in C++ does NOT provide atomicity or memory ordering
 * guarantees. It prevents compiler optimization but says NOTHING about CPU cache coherency or
 * instruction reordering. For multi-threaded code, use `std::atomic<T>` instead. `volatile` is
 * ONLY for hardware I/O and ISR communication. In Java/C#, `volatile` has different (stronger)
 * semantics — don't confuse them! cppreference.com/w/cpp/language/cv
 *
 * =============================================================================
 * [CPPREF DEPTH: volatile and const Can Coexist]
 * =============================================================================
 * EN: `const volatile int* reg = (const volatile int*)0x40000000;` means: "I promise not to
 * WRITE to this register, but its value can still CHANGE due to hardware." This is common for
 * read-only status registers (e.g., ADC result, timer counter). The compiler won't optimize away
 * reads, and the programmer cannot accidentally write to it. cppreference.com/w/cpp/language/cv
 *
 * @note Compile: g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion 01_volatile_hardware.cpp -o 01_volatile_hardware
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <cstdint>
#include <iostream>

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// [DEMO 1: volatile Prevents Compiler Optimization]
// ═════════════════════════════════════════════════════════════════════════════════════════════════

// EN: Without volatile, the compiler could optimize this to "while(true)" because it sees no
// code path that changes 'flag'.
volatile bool hardwareFlag = false;

void simulateISR() {
  // EN: Simulated ISR — in real embedded, this would be triggered by hardware.
  hardwareFlag = true;
  std::cout << "[ISR] Hardware flag SET to true." << std::endl;
}

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// [DEMO 2: Memory-Mapped I/O Simulation]
// ═════════════════════════════════════════════════════════════════════════════════════════════════

// EN: Simulate memory-mapped hardware registers.
struct GPIO_Registers {
  volatile uint32_t MODER;   // Mode register
  volatile uint32_t ODR;     // Output data register
  volatile uint32_t IDR;     // Input data register (read-only in real HW)
  volatile uint32_t BSRR;   // Bit set/reset register
};

// EN: In real embedded, this would be a hardware address like 0x40020000.
GPIO_Registers simulated_gpio = {0, 0, 0, 0};

void configureGPIO() {
  // EN: Set pin 5 to output mode (bits 11:10 = 01 for output).
  simulated_gpio.MODER |= (1U << 10);
  simulated_gpio.MODER &= ~(1U << 11);
  std::cout << "[GPIO] Pin 5 configured as OUTPUT. MODER=0x"
            << std::hex << simulated_gpio.MODER << std::dec << std::endl;
}

void setGPIOPin(uint32_t pin) {
  // EN: BSRR: writing to lower 16 bits SETS, upper 16 bits RESETS.
  simulated_gpio.BSRR = (1U << pin);
  simulated_gpio.ODR |= (1U << pin);
  std::cout << "[GPIO] Pin " << pin << " SET HIGH. ODR=0x"
            << std::hex << simulated_gpio.ODR << std::dec << std::endl;
}

void resetGPIOPin(uint32_t pin) {
  simulated_gpio.BSRR = (1U << (pin + 16U));
  simulated_gpio.ODR &= ~(1U << pin);
  std::cout << "[GPIO] Pin " << pin << " SET LOW. ODR=0x"
            << std::hex << simulated_gpio.ODR << std::dec << std::endl;
}

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// [DEMO 3: ISR-Safe Shared Variable Pattern]
// ═════════════════════════════════════════════════════════════════════════════════════════════════

// EN: volatile + flag pattern for ISR-main communication.
volatile uint32_t adcResult = 0;
volatile bool adcReady = false;

void simulateADC_ISR() {
  // EN: Simulates ADC conversion complete interrupt.
  adcResult = 3300; // 3.3V in millivolts
  adcReady = true;
  std::cout << "[ADC ISR] Conversion complete: " << adcResult << " mV"
            << std::endl;
}

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// [DEMO 4: const volatile — Read-Only Hardware Register]
// ═════════════════════════════════════════════════════════════════════════════════════════════════

// EN: Simulating a read-only hardware status register.
volatile uint32_t hwStatusReg = 0x0000'00FF;

int main() {
  std::cout << "=== MODULE 11: VOLATILE & HARDWARE I/O ===\n" << std::endl;

  // --- Demo 1: volatile flag ---
  std::cout << "--- DEMO 1: volatile Flag ---\n" << std::endl;
  std::cout << "[Main] Flag before ISR: " << std::boolalpha << hardwareFlag
            << std::endl;
  simulateISR();
  if (hardwareFlag) {
    std::cout << "[Main] Flag detected as TRUE — ISR communication works!\n"
              << std::endl;
  }

  // --- Demo 2: Memory-Mapped GPIO ---
  std::cout << "--- DEMO 2: Memory-Mapped GPIO Simulation ---\n" << std::endl;
  configureGPIO();
  setGPIOPin(5);  // Turn on LED
  setGPIOPin(13); // Turn on buzzer
  resetGPIOPin(5); // Turn off LED
  std::cout << std::endl;

  // --- Demo 3: ISR-Safe ADC Pattern ---
  std::cout << "--- DEMO 3: ISR-Safe ADC Read ---\n" << std::endl;
  std::cout << "[Main] Waiting for ADC..." << std::endl;
  simulateADC_ISR(); // In real code, this happens asynchronously
  if (adcReady) {
    uint32_t voltage = adcResult;
    adcReady = false;
    std::cout << "[Main] Read ADC: " << voltage << " mV (" << (voltage / 1000.0)
              << " V)\n"
              << std::endl;
  }

  // --- Demo 4: const volatile ---
  std::cout << "--- DEMO 4: const volatile (Read-Only Register) ---\n"
            << std::endl;
  const volatile uint32_t *statusReg = &hwStatusReg;
  // *statusReg = 0; // COMPILE ERROR: cannot write to const volatile!
  std::cout << "[Main] Status register value: 0x" << std::hex << *statusReg
            << std::dec << std::endl;
  std::cout << "=> const volatile: compiler re-reads every time, but "
               "writing is forbidden at compile time."
            << std::endl;

  std::cout << "\n=> volatile, Memory-Mapped I/O, and ISR patterns "
               "demonstrated!" << std::endl;

  return 0;
}
