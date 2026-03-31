/**
 * @file module_15_hw_interfaces/01_gpio_and_registers.cpp
 * @brief GPIO & Register-Level Programming
 *
 * @details
 * =============================================================================
 * [THEORY: Memory-Mapped I/O and Registers]
 * =============================================================================
 *
 * EN: In embedded systems, hardware peripherals (GPIO, UART, SPI, etc.) are
 *     controlled through memory-mapped registers. The CPU accesses these
 *     registers as if they were normal memory addresses.
 *
 *     Memory Map Example (ARM Cortex-M / STM32):
 *     ┌─────────────────┬─────────────────────────────────────────┐
 *     │ Address Range   │ Peripheral                              │
 *     ├─────────────────┼─────────────────────────────────────────┤
 *     │ 0x0000_0000     │ Flash memory (code)                     │
 *     │ 0x2000_0000     │ SRAM                                    │
 *     │ 0x4000_0000     │ APB1 peripherals (UART, I2C, SPI)       │
 *     │ 0x4001_0000     │ APB2 peripherals (GPIO, ADC, TIM)       │
 *     │ 0x4002_0000     │ AHB1 peripherals (DMA, RCC)             │
 *     │ 0xE000_0000     │ Cortex-M system (NVIC, SysTick)         │
 *     └─────────────────┴─────────────────────────────────────────┘
 *
 *     Register Access Rules:
 *     1. Use volatile — compiler must not optimize away reads/writes
 *     2. Use bit manipulation — set/clear individual bits
 *     3. Use read-modify-write (RMW) for shared registers
 *     4. Some registers are write-only or read-only
 *
 *     GPIO Register Set (typical STM32):
 *     ┌─────────┬────────┬──────────────────────────────────────────┐
 *     │ Offset  │ Name   │ Description                              │
 *     ├─────────┼────────┼──────────────────────────────────────────┤
 *     │ 0x00    │ MODER  │ Mode Register (input/output/AF/analog)   │
 *     │ 0x04    │ OTYPER │ Output Type (push-pull / open-drain)     │
 *     │ 0x08    │ OSPEEDR│ Output Speed (low/med/high/very-high)    │
 *     │ 0x0C    │ PUPDR  │ Pull-up/Pull-down                        │
 *     │ 0x10    │ IDR    │ Input Data Register (read-only)          │
 *     │ 0x14    │ ODR    │ Output Data Register                     │
 *     │ 0x18    │ BSRR   │ Bit Set/Reset Register (atomic)          │
 *     │ 0x1C    │ LCKR   │ Lock Register                            │
 *     │ 0x20    │ AFRL   │ Alternate Function Low (pins 0-7)        │
 *     │ 0x24    │ AFRH   │ Alternate Function High (pins 8-15)      │
 *     └─────────┴────────┴──────────────────────────────────────────┘
 *
 * @note Compile: g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion 01_gpio_and_registers.cpp -o 01_gpio_registers
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <iostream>
#include <cstdint>
#include <array>
#include <string>
#include <iomanip>
#include <bitset>
#include <cassert>
#include <functional>
#include <map>

// ═════════════════════════════════════════════════════════════════════════════
// PART 1: Simulated Register — volatile Memory-Mapped I/O
// ═════════════════════════════════════════════════════════════════════════════

// EN: On real hardware: volatile uint32_t* REG = (volatile uint32_t*)0x40020000;
//     We simulate this with a register class that tracks reads/writes.

class SimRegister {
    uint32_t value_;
    std::string name_;
    bool read_only_;
    bool write_only_;
    uint32_t write_mask_;  // bits that can be written

public:
    SimRegister(std::string name, uint32_t initial = 0,
                bool ro = false, bool wo = false, uint32_t mask = 0xFFFFFFFF)
        : value_(initial), name_(std::move(name)),
          read_only_(ro), write_only_(wo), write_mask_(mask) {}

    uint32_t read() const {
        if (write_only_) return 0;  // write-only registers read as 0
        return value_;
    }

    void write(uint32_t val) {
        if (read_only_) return;  // read-only registers ignore writes
        value_ = (value_ & ~write_mask_) | (val & write_mask_);
    }

    // EN: Force-write for simulation (bypasses read-only flag)
    void force_write(uint32_t val) { value_ = val; }

    // EN: Atomic bit set/reset (like BSRR)
    void bit_set(uint32_t mask) { value_ |= mask; }
    void bit_clear(uint32_t mask) { value_ &= ~mask; }

    // EN: Read-Modify-Write
    void rmw(uint32_t clear_mask, uint32_t set_mask) {
        uint32_t tmp = value_;
        tmp &= ~clear_mask;
        tmp |= set_mask;
        value_ = tmp;
    }

    const std::string& name() const { return name_; }

    void print() const {
        std::cout << "    " << std::left << std::setw(8) << name_ << std::right
                  << ": 0x" << std::hex << std::setw(8) << std::setfill('0') << value_
                  << std::dec << std::setfill(' ')
                  << " (" << std::bitset<32>(value_) << ")\n";
    }
};

// ═════════════════════════════════════════════════════════════════════════════
// PART 2: GPIO Port Simulation (STM32-like)
// ═════════════════════════════════════════════════════════════════════════════

enum class GPIOMode : uint8_t {
    Input    = 0b00,
    Output   = 0b01,
    AltFunc  = 0b10,
    Analog   = 0b11,
};

enum class GPIOSpeed : uint8_t {
    Low      = 0b00,
    Medium   = 0b01,
    High     = 0b10,
    VeryHigh = 0b11,
};

enum class GPIOPull : uint8_t {
    None     = 0b00,
    PullUp   = 0b01,
    PullDown = 0b10,
};

enum class GPIOOutputType : uint8_t {
    PushPull  = 0,
    OpenDrain = 1,
};

class GPIOPort {
    std::string port_name_;
    SimRegister MODER;    // offset 0x00
    SimRegister OTYPER;   // offset 0x04
    SimRegister OSPEEDR;  // offset 0x08
    SimRegister PUPDR;    // offset 0x0C
    SimRegister IDR;      // offset 0x10 (read-only)
    SimRegister ODR;      // offset 0x14
    SimRegister BSRR;     // offset 0x18 (write-only)

    uint32_t external_input_ = 0;  // simulated external voltage levels

public:
    GPIOPort(std::string name)
        : port_name_(std::move(name)),
          MODER("MODER", 0),
          OTYPER("OTYPER", 0),
          OSPEEDR("OSPEEDR", 0),
          PUPDR("PUPDR", 0),
          IDR("IDR", 0, true),    // read-only
          ODR("ODR", 0),
          BSRR("BSRR", 0, false, true)  // write-only
    {}

    // EN: Configure pin mode (2 bits per pin in MODER)
    void set_mode(int pin, GPIOMode mode) {
        uint32_t shift = static_cast<uint32_t>(pin) * 2;
        MODER.rmw(0x3U << shift, static_cast<uint32_t>(mode) << shift);
    }

    void set_output_type(int pin, GPIOOutputType type) {
        if (type == GPIOOutputType::OpenDrain)
            OTYPER.bit_set(1U << static_cast<uint32_t>(pin));
        else
            OTYPER.bit_clear(1U << static_cast<uint32_t>(pin));
    }

    void set_speed(int pin, GPIOSpeed speed) {
        uint32_t shift = static_cast<uint32_t>(pin) * 2;
        OSPEEDR.rmw(0x3U << shift, static_cast<uint32_t>(speed) << shift);
    }

    void set_pull(int pin, GPIOPull pull) {
        uint32_t shift = static_cast<uint32_t>(pin) * 2;
        PUPDR.rmw(0x3U << shift, static_cast<uint32_t>(pull) << shift);
    }

    // EN: Write output pin (via ODR)
    void write_pin(int pin, bool high) {
        if (high)
            ODR.bit_set(1U << static_cast<uint32_t>(pin));
        else
            ODR.bit_clear(1U << static_cast<uint32_t>(pin));
        // Update IDR to reflect output (for output pins)
        update_idr();
    }

    // EN: Atomic write via BSRR (no read-modify-write needed)
    void atomic_set(int pin) {
        ODR.bit_set(1U << static_cast<uint32_t>(pin));
        update_idr();
    }

    void atomic_reset(int pin) {
        ODR.bit_clear(1U << static_cast<uint32_t>(pin));
        update_idr();
    }

    // EN: Read input pin
    bool read_pin(int pin) const {
        return (IDR.read() >> static_cast<uint32_t>(pin)) & 1;
    }

    // EN: Simulate external input (for input pins)
    void set_external_input(int pin, bool high) {
        if (high)
            external_input_ |= (1U << static_cast<uint32_t>(pin));
        else
            external_input_ &= ~(1U << static_cast<uint32_t>(pin));
        update_idr();
    }

    void print_registers() const {
        std::cout << "  " << port_name_ << " Registers:\n";
        MODER.print();
        OTYPER.print();
        OSPEEDR.print();
        PUPDR.print();
        IDR.print();
        ODR.print();
    }

    void print_pin_config(int pin) const {
        uint32_t shift = static_cast<uint32_t>(pin) * 2;
        auto mode = static_cast<GPIOMode>((MODER.read() >> shift) & 0x3);
        auto speed = static_cast<GPIOSpeed>((OSPEEDR.read() >> shift) & 0x3);
        auto pull = static_cast<GPIOPull>((PUPDR.read() >> shift) & 0x3);
        auto otype = static_cast<GPIOOutputType>(
            (OTYPER.read() >> static_cast<uint32_t>(pin)) & 0x1);

        std::string mode_str, speed_str, pull_str, otype_str;
        switch (mode) {
            case GPIOMode::Input:   mode_str = "INPUT";  break;
            case GPIOMode::Output:  mode_str = "OUTPUT"; break;
            case GPIOMode::AltFunc: mode_str = "AF";     break;
            case GPIOMode::Analog:  mode_str = "ANALOG"; break;
        }
        switch (speed) {
            case GPIOSpeed::Low:      speed_str = "LOW";       break;
            case GPIOSpeed::Medium:   speed_str = "MEDIUM";    break;
            case GPIOSpeed::High:     speed_str = "HIGH";      break;
            case GPIOSpeed::VeryHigh: speed_str = "VERY_HIGH"; break;
        }
        switch (pull) {
            case GPIOPull::None:     pull_str = "NONE";      break;
            case GPIOPull::PullUp:   pull_str = "PULL_UP";   break;
            case GPIOPull::PullDown: pull_str = "PULL_DOWN"; break;
        }
        otype_str = (otype == GPIOOutputType::PushPull) ? "PP" : "OD";

        std::cout << "    Pin " << std::setw(2) << pin << ": " << std::left
                  << std::setw(6) << mode_str << " " << std::setw(9) << speed_str
                  << " " << std::setw(9) << pull_str << " " << otype_str
                  << std::right;
        if (mode == GPIOMode::Output) {
            std::cout << " → " << (read_pin(pin) ? "HIGH" : "LOW");
        } else if (mode == GPIOMode::Input) {
            std::cout << " ← " << (read_pin(pin) ? "HIGH" : "LOW");
        }
        std::cout << "\n";
    }

private:
    void update_idr() {
        // EN: IDR reflects output for output pins, external for input pins
        uint32_t idr_val = 0;
        for (int i = 0; i < 16; ++i) {
            auto shift = static_cast<uint32_t>(i) * 2;
            auto mode = static_cast<GPIOMode>((MODER.read() >> shift) & 0x3);
            if (mode == GPIOMode::Output || mode == GPIOMode::AltFunc) {
                idr_val |= (ODR.read() & (1U << static_cast<uint32_t>(i)));
            } else {
                idr_val |= (external_input_ & (1U << static_cast<uint32_t>(i)));
            }
        }
        // EN: Force-update IDR in simulation (on real HW, hardware drives IDR)
        IDR.force_write(idr_val);
    }
};

// ═════════════════════════════════════════════════════════════════════════════
// PART 3: Bit Manipulation Utilities
// ═════════════════════════════════════════════════════════════════════════════

// EN: Common bit manipulation patterns used in register programming
namespace BitOps {
    // Set bit at position
    constexpr uint32_t set_bit(uint32_t reg, int bit) {
        return reg | (1U << static_cast<uint32_t>(bit));
    }
    // Clear bit at position
    constexpr uint32_t clear_bit(uint32_t reg, int bit) {
        return reg & ~(1U << static_cast<uint32_t>(bit));
    }
    // Toggle bit
    constexpr uint32_t toggle_bit(uint32_t reg, int bit) {
        return reg ^ (1U << static_cast<uint32_t>(bit));
    }
    // Read bit
    constexpr bool read_bit(uint32_t reg, int bit) {
        return (reg >> static_cast<uint32_t>(bit)) & 1U;
    }
    // Set bitfield: clear [pos+width-1:pos], then set value
    constexpr uint32_t set_field(uint32_t reg, int pos, int width, uint32_t value) {
        uint32_t mask = ((1U << static_cast<uint32_t>(width)) - 1) << static_cast<uint32_t>(pos);
        return (reg & ~mask) | ((value << static_cast<uint32_t>(pos)) & mask);
    }
    // Read bitfield
    constexpr uint32_t read_field(uint32_t reg, int pos, int width) {
        uint32_t mask = (1U << static_cast<uint32_t>(width)) - 1;
        return (reg >> static_cast<uint32_t>(pos)) & mask;
    }
}

// ═════════════════════════════════════════════════════════════════════════════
// PART 4: Clock Enable (RCC) Simulation
// ═════════════════════════════════════════════════════════════════════════════

// EN: Before using any peripheral, its clock must be enabled in the RCC
//     (Reset and Clock Control) register. Forgetting this is a classic bug!

class RCCSimulator {
    SimRegister AHB1ENR;  // AHB1 Enable Register (GPIO clocks)
    SimRegister APB1ENR;  // APB1 Enable (UART, I2C, SPI)
    SimRegister APB2ENR;  // APB2 Enable (ADC, TIM)

    // EN: Clock enable bit positions
    static constexpr int GPIOA_EN = 0;
    static constexpr int GPIOB_EN = 1;
    static constexpr int GPIOC_EN = 2;
    static constexpr int DMA1_EN  = 21;

public:
    RCCSimulator()
        : AHB1ENR("AHB1ENR", 0),
          APB1ENR("APB1ENR", 0),
          APB2ENR("APB2ENR", 0) {}

    void enable_gpio(char port) {
        int bit = port - 'A';
        AHB1ENR.bit_set(1U << static_cast<uint32_t>(bit));
    }

    bool is_gpio_enabled(char port) const {
        int bit = port - 'A';
        return (AHB1ENR.read() >> static_cast<uint32_t>(bit)) & 1;
    }

    void print() const {
        std::cout << "  RCC Registers:\n";
        AHB1ENR.print();
        APB1ENR.print();
        APB2ENR.print();
    }
};

// ═════════════════════════════════════════════════════════════════════════════
// MAIN
// ═════════════════════════════════════════════════════════════════════════════
int main() {
    std::cout << "============================================\n";
    std::cout << "   Module 15 - GPIO & Register Programming\n";
    std::cout << "============================================\n\n";

    // ─── Demo 1: Register Bit Manipulation ──────────────────────────────
    {
        std::cout << "--- Demo 1: Bit Manipulation Operations ---\n";
        uint32_t reg = 0x00000000;

        reg = BitOps::set_bit(reg, 5);
        std::cout << "    After set_bit(5):     0x" << std::hex << std::setw(8)
                  << std::setfill('0') << reg << std::dec << "\n";

        reg = BitOps::set_bit(reg, 13);
        std::cout << "    After set_bit(13):    0x" << std::hex << std::setw(8)
                  << std::setfill('0') << reg << std::dec << "\n";

        reg = BitOps::clear_bit(reg, 5);
        std::cout << "    After clear_bit(5):   0x" << std::hex << std::setw(8)
                  << std::setfill('0') << reg << std::dec << "\n";

        reg = BitOps::toggle_bit(reg, 13);
        std::cout << "    After toggle_bit(13): 0x" << std::hex << std::setw(8)
                  << std::setfill('0') << reg << std::dec << "\n";

        // EN: Bitfield operations
        reg = 0;
        reg = BitOps::set_field(reg, 4, 3, 0b101);  // set bits [6:4] = 5
        std::cout << "    After set_field(4,3,0b101): 0x" << std::hex << std::setw(8)
                  << std::setfill('0') << reg << std::dec << "\n";
        uint32_t field = BitOps::read_field(reg, 4, 3);
        std::cout << "    read_field(4,3): " << field << " (expected: 5)\n\n";
    }

    // ─── Demo 2: RCC Clock Enable ──────────────────────────────────────
    {
        std::cout << "--- Demo 2: RCC Clock Enable ---\n";
        std::cout << "  IMPORTANT: Always enable peripheral clock before access!\n\n";

        RCCSimulator rcc;
        std::cout << "  Before enabling clocks:\n";
        rcc.print();

        rcc.enable_gpio('A');
        rcc.enable_gpio('B');
        rcc.enable_gpio('C');

        std::cout << "\n  After enabling GPIOA, GPIOB, GPIOC clocks:\n";
        rcc.print();

        std::cout << "\n  Clock status: GPIOA=" << rcc.is_gpio_enabled('A')
                  << " GPIOB=" << rcc.is_gpio_enabled('B')
                  << " GPIOC=" << rcc.is_gpio_enabled('C')
                  << " GPIOD=" << rcc.is_gpio_enabled('D') << "\n\n";
    }

    // ─── Demo 3: GPIO Output Configuration ─────────────────────────────
    {
        std::cout << "--- Demo 3: GPIO Output (LED Control) ---\n";
        std::cout << "  Scenario: LED on PA5 (STM32 Nucleo built-in LED)\n\n";

        GPIOPort gpioa("GPIOA");

        // EN: Configure PA5 as output, push-pull, medium speed, no pull
        gpioa.set_mode(5, GPIOMode::Output);
        gpioa.set_output_type(5, GPIOOutputType::PushPull);
        gpioa.set_speed(5, GPIOSpeed::Medium);
        gpioa.set_pull(5, GPIOPull::None);

        gpioa.print_pin_config(5);

        // EN: Turn LED ON
        gpioa.write_pin(5, true);
        std::cout << "    LED ON  → Pin 5 = " << gpioa.read_pin(5) << "\n";

        // EN: Turn LED OFF
        gpioa.write_pin(5, false);
        std::cout << "    LED OFF → Pin 5 = " << gpioa.read_pin(5) << "\n";

        // EN: Atomic operations (BSRR-style)
        gpioa.atomic_set(5);
        std::cout << "    atomic_set(5)   → Pin 5 = " << gpioa.read_pin(5) << "\n";
        gpioa.atomic_reset(5);
        std::cout << "    atomic_reset(5) → Pin 5 = " << gpioa.read_pin(5) << "\n\n";
    }

    // ─── Demo 4: GPIO Input (Button) ────────────────────────────────────
    {
        std::cout << "--- Demo 4: GPIO Input (Button Read) ---\n";
        std::cout << "  Scenario: Button on PC13 (active-low with pull-up)\n\n";

        GPIOPort gpioc("GPIOC");

        // EN: Configure PC13 as input with pull-up
        gpioc.set_mode(13, GPIOMode::Input);
        gpioc.set_pull(13, GPIOPull::PullUp);

        gpioc.print_pin_config(13);

        // EN: Simulate button states
        gpioc.set_external_input(13, true);  // not pressed (pull-up)
        std::cout << "    Button released (HIGH): " << gpioc.read_pin(13) << "\n";

        gpioc.set_external_input(13, false);  // pressed (active-low)
        std::cout << "    Button pressed (LOW):   " << gpioc.read_pin(13) << "\n\n";
    }

    // ─── Demo 5: Multiple Pin Configuration ─────────────────────────────
    {
        std::cout << "--- Demo 5: Multi-Pin Automotive ECU GPIO Layout ---\n\n";

        GPIOPort gpiob("GPIOB");

        // EN: Typical ECU GPIO assignment
        struct PinConfig {
            int pin;
            GPIOMode mode;
            GPIOSpeed speed;
            GPIOPull pull;
            GPIOOutputType otype;
            std::string function;
        };

        std::vector<PinConfig> pins = {
            {0,  GPIOMode::Analog,  GPIOSpeed::Low,      GPIOPull::None,     GPIOOutputType::PushPull, "ADC_IN8 (Temp Sensor)"},
            {1,  GPIOMode::Analog,  GPIOSpeed::Low,      GPIOPull::None,     GPIOOutputType::PushPull, "ADC_IN9 (Batt Voltage)"},
            {3,  GPIOMode::AltFunc, GPIOSpeed::VeryHigh,  GPIOPull::None,     GPIOOutputType::PushPull, "SPI1_SCK"},
            {4,  GPIOMode::AltFunc, GPIOSpeed::VeryHigh,  GPIOPull::None,     GPIOOutputType::PushPull, "SPI1_MISO"},
            {5,  GPIOMode::AltFunc, GPIOSpeed::VeryHigh,  GPIOPull::None,     GPIOOutputType::PushPull, "SPI1_MOSI"},
            {6,  GPIOMode::AltFunc, GPIOSpeed::High,      GPIOPull::PullUp,   GPIOOutputType::OpenDrain, "I2C1_SCL"},
            {7,  GPIOMode::AltFunc, GPIOSpeed::High,      GPIOPull::PullUp,   GPIOOutputType::OpenDrain, "I2C1_SDA"},
            {8,  GPIOMode::Output,  GPIOSpeed::Low,       GPIOPull::None,     GPIOOutputType::PushPull, "LED_Status (Green)"},
            {9,  GPIOMode::Output,  GPIOSpeed::Low,       GPIOPull::None,     GPIOOutputType::PushPull, "LED_Error (Red)"},
            {10, GPIOMode::AltFunc, GPIOSpeed::High,      GPIOPull::PullUp,   GPIOOutputType::PushPull, "USART3_TX (Debug)"},
            {11, GPIOMode::AltFunc, GPIOSpeed::High,      GPIOPull::PullUp,   GPIOOutputType::PushPull, "USART3_RX (Debug)"},
            {12, GPIOMode::Input,   GPIOSpeed::Low,       GPIOPull::PullDown, GPIOOutputType::PushPull, "CAN_STB (Standby)"},
            {13, GPIOMode::AltFunc, GPIOSpeed::VeryHigh,  GPIOPull::None,     GPIOOutputType::PushPull, "CAN2_TX"},
            {14, GPIOMode::AltFunc, GPIOSpeed::VeryHigh,  GPIOPull::None,     GPIOOutputType::PushPull, "CAN2_RX"},
        };

        for (auto& p : pins) {
            gpiob.set_mode(p.pin, p.mode);
            gpiob.set_speed(p.pin, p.speed);
            gpiob.set_pull(p.pin, p.pull);
            gpiob.set_output_type(p.pin, p.otype);
        }

        std::cout << "  GPIOB Pin Assignments (Automotive ECU):\n";
        std::cout << "  ┌──────┬────────┬───────────┬───────────┬────┬─────────────────────────┐\n";
        std::cout << "  │ Pin  │ Mode   │ Speed     │ Pull      │ OT │ Function                │\n";
        std::cout << "  ├──────┼────────┼───────────┼───────────┼────┼─────────────────────────┤\n";
        for (auto& p : pins) {
            std::string mode_s, speed_s, pull_s, ot_s;
            switch (p.mode) {
                case GPIOMode::Input:   mode_s = "INPUT";  break;
                case GPIOMode::Output:  mode_s = "OUTPUT"; break;
                case GPIOMode::AltFunc: mode_s = "AF";     break;
                case GPIOMode::Analog:  mode_s = "ANALOG"; break;
            }
            switch (p.speed) {
                case GPIOSpeed::Low:      speed_s = "LOW";       break;
                case GPIOSpeed::Medium:   speed_s = "MEDIUM";    break;
                case GPIOSpeed::High:     speed_s = "HIGH";      break;
                case GPIOSpeed::VeryHigh: speed_s = "VERY_HIGH"; break;
            }
            switch(p.pull) {
                case GPIOPull::None:     pull_s = "NONE";      break;
                case GPIOPull::PullUp:   pull_s = "PULL_UP";   break;
                case GPIOPull::PullDown: pull_s = "PULL_DOWN"; break;
            }
            ot_s = (p.otype == GPIOOutputType::PushPull) ? "PP" : "OD";

            std::cout << "  │ " << std::setw(4) << ("PB" + std::to_string(p.pin))
                      << " │ " << std::left << std::setw(6) << mode_s
                      << " │ " << std::setw(9) << speed_s
                      << " │ " << std::setw(9) << pull_s
                      << " │ " << std::setw(2) << ot_s
                      << " │ " << std::setw(23) << p.function
                      << std::right << " │\n";
        }
        std::cout << "  └──────┴────────┴───────────┴───────────┴────┴─────────────────────────┘\n";

        std::cout << "\n  Register state after configuration:\n";
        gpiob.print_registers();
        std::cout << "\n";
    }

    // ─── Demo 6: volatile Keyword Importance ─────────────────────────────
    {
        std::cout << "--- Demo 6: volatile Keyword in Register Access ---\n\n";
        std::cout << "  Why volatile is ESSENTIAL for hardware registers:\n\n";
        std::cout << "  WITHOUT volatile (WRONG):\n";
        std::cout << "    uint32_t* REG = (uint32_t*)0x40020014;\n";
        std::cout << "    *REG = 0x01;  // compiler may optimize this away!\n";
        std::cout << "    while (*REG & 0x02) {}  // infinite loop: compiler\n";
        std::cout << "                            // caches value, never re-reads\n\n";

        std::cout << "  WITH volatile (CORRECT):\n";
        std::cout << "    volatile uint32_t* REG = (volatile uint32_t*)0x40020014;\n";
        std::cout << "    *REG = 0x01;  // always written to hardware\n";
        std::cout << "    while (*REG & 0x02) {}  // re-reads every iteration\n\n";

        std::cout << "  C++ best practice — use std::atomic or volatile:\n";
        std::cout << "    template<typename T>\n";
        std::cout << "    struct HWRegister {\n";
        std::cout << "        volatile T value;\n";
        std::cout << "        void write(T v) { value = v; }\n";
        std::cout << "        T read() const  { return value; }\n";
        std::cout << "    };\n\n";

        // EN: Demonstrate the concept (without real hardware)
        volatile uint32_t sim_reg = 0;
        sim_reg = 0xDEADBEEF;
        uint32_t read_back = sim_reg;
        std::cout << "  Simulated volatile register:\n";
        std::cout << "    Write: 0xDEADBEEF → Read: 0x"
                  << std::hex << std::setw(8) << std::setfill('0')
                  << read_back << std::dec << " ✓\n\n";
    }

    // ─── Demo 7: Common Register Patterns ───────────────────────────────
    {
        std::cout << "--- Demo 7: Common Register Programming Patterns ---\n\n";

        std::cout << "  ┌─────────────────────────┬──────────────────────────────────┐\n";
        std::cout << "  │ Pattern                 │ Code                             │\n";
        std::cout << "  ├─────────────────────────┼──────────────────────────────────┤\n";
        std::cout << "  │ Set single bit          │ REG |= (1 << bit)                │\n";
        std::cout << "  │ Clear single bit        │ REG &= ~(1 << bit)               │\n";
        std::cout << "  │ Toggle bit              │ REG ^= (1 << bit)                │\n";
        std::cout << "  │ Check bit               │ if (REG & (1 << bit))            │\n";
        std::cout << "  │ Set multi-bit field     │ REG = (REG & ~mask) | (val<<pos) │\n";
        std::cout << "  │ Atomic set (BSRR)       │ BSRR = (1 << bit)                │\n";
        std::cout << "  │ Atomic reset (BSRR)     │ BSRR = (1 << (bit+16))           │\n";
        std::cout << "  │ Read-Modify-Write       │ tmp=REG; tmp|=val; REG=tmp;      │\n";
        std::cout << "  │ Wait for flag           │ while(!(REG & FLAG)) {}          │\n";
        std::cout << "  │ Write-only register     │ REG = value; (no read first!)    │\n";
        std::cout << "  └─────────────────────────┴──────────────────────────────────┘\n\n";

        std::cout << "  Common mistakes:\n";
        std::cout << "    1. Forgetting volatile → compiler optimizes away HW access\n";
        std::cout << "    2. Forgetting RCC clock enable → registers read as 0\n";
        std::cout << "    3. Wrong bit position → corrupts adjacent peripheral config\n";
        std::cout << "    4. RMW on write-only registers → read returns 0, destroys data\n";
        std::cout << "    5. Not waiting for ready flag → using peripheral before it's ready\n";
    }

    std::cout << "\n============================================\n";
    std::cout << "   End of GPIO & Register Programming\n";
    std::cout << "============================================\n";

    return 0;
}
