/**
 * @file module_18_driver_development/03_peripheral_driver.cpp
 * @brief Peripheral Driver Development
 *
 * @details
 * =============================================================================
 * [THEORY: Register-Level Driver Architecture]
 * =============================================================================
 *
 * EN: Bare-metal drivers manipulate memory-mapped registers via volatile
 *     pointers. Every peripheral has a register block at a fixed base address.
 *
 *     Register Bit Operations:
 *     ┌──────────────────────────────────────────────────┐
 *     │ SET bit n:    reg |=  (1u << n)                  │
 *     │ CLEAR bit n:  reg &= ~(1u << n)                  │
 *     │ TOGGLE bit n: reg ^=  (1u << n)                  │
 *     │ READ bit n:   (reg >> n) & 1u                    │
 *     │ MODIFY field: reg = (reg & ~mask) | (val << pos) │
 *     └──────────────────────────────────────────────────┘
 *
 *
 * =============================================================================
 * [THEORY: Type-Safe Register Access]
 * =============================================================================
 *
 * EN: Raw bit manipulation is error-prone. A type-safe RegisterField template
 *     encapsulates base address, bit offset and width:
 *
 *     template <uint32_t Base, uint32_t Offset, uint32_t Width>
 *     struct RegisterField {
 *         static void Set(uint32_t val);
 *         static uint32_t Get();
 *     };
 *
 *     This prevents mixing registers from different peripherals.
 *
 *
 * =============================================================================
 * [THEORY: Driver State Machine]
 * =============================================================================
 *
 * EN: UNINITIALIZED ──Init()──► INITIALIZED ──Start()──► RUNNING
 *          ▲                        │                      │
 *   │                     DeInit()              error/Stop()
 *          │                        │                      │
 *          └────── DeInit() ◄── STOPPED ◄────────── ERROR ◄┘
 *     Every driver follows this state machine. Invalid transitions are rejected.
 *
 *
 * =============================================================================
 * [THEORY: ISR Callback Mechanism]
 * =============================================================================
 *
 * EN: In embedded, peripherals trigger hardware interrupts. The ISR (Interrupt
 *     Service Routine) must be fast — typically sets a flag or pushes data to
 *     a ring buffer, then signals the main loop.
 *
 *     ISR callback pattern:
 *     1. Driver registers a callback function pointer
 *     2. Simulated IRQ handler calls the callback
 *     3. Callback stores data / sets flag (minimal work)
 *     4. Main loop processes the flag / data
 *
 *
 * @note Compile: g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion 03_peripheral_driver.cpp -o 03_periph
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <iostream>
#include <cstdint>
#include <string>
#include <string_view>
#include <array>
#include <vector>
#include <functional>
#include <iomanip>
#include <optional>
#include <cassert>

// ═════════════════════════════════════════════════════════════════════════════
// PART 1: Register Bit Manipulation Utilities
// ═════════════════════════════════════════════════════════════════════════════

namespace reg_ops {

// EN: Basic register bit operations on a simulated register variable
struct RegOps {
    static void SetBit(uint32_t& reg, uint32_t bit) { reg |= (1u << bit); }
    static void ClearBit(uint32_t& reg, uint32_t bit) { reg &= ~(1u << bit); }
    static void ToggleBit(uint32_t& reg, uint32_t bit) { reg ^= (1u << bit); }
    static bool ReadBit(uint32_t reg, uint32_t bit) { return (reg >> bit) & 1u; }

    // EN: Modify a multi-bit field within a register
    static void ModifyField(uint32_t& reg, uint32_t pos, uint32_t width, uint32_t value) {
        uint32_t mask = ((1u << width) - 1u) << pos;
        reg = (reg & ~mask) | ((value & ((1u << width) - 1u)) << pos);
    }

    static uint32_t ReadField(uint32_t reg, uint32_t pos, uint32_t width) {
        uint32_t mask = (1u << width) - 1u;
        return (reg >> pos) & mask;
    }
};

// EN: Type-safe register field template
template <uint32_t BitOffset, uint32_t BitWidth>
struct RegisterField {
    static_assert(BitOffset + BitWidth <= 32, "Field exceeds register width");

    static constexpr uint32_t MASK = ((1u << BitWidth) - 1u) << BitOffset;

    static void Set(uint32_t& reg, uint32_t value) {
        reg = (reg & ~MASK) | ((value & ((1u << BitWidth) - 1u)) << BitOffset);
    }

    static uint32_t Get(uint32_t reg) {
        return (reg >> BitOffset) & ((1u << BitWidth) - 1u);
    }
};

}  // namespace reg_ops

// ═════════════════════════════════════════════════════════════════════════════
// PART 2: Complete GPIO Driver
// ═════════════════════════════════════════════════════════════════════════════

namespace gpio_driver {

enum class PinMode : uint8_t { INPUT = 0, OUTPUT = 1, ALT_FUNC = 2, ANALOG = 3 };
enum class PullType : uint8_t { NONE = 0, PULL_UP = 1, PULL_DOWN = 2 };

// EN: Simulated GPIO port register block (mirrors STM32 layout)
struct GPIO_Regs {
    uint32_t MODER   = 0;  // Mode register (2 bits per pin)
    uint32_t OTYPER  = 0;  // Output type
    uint32_t OSPEEDR = 0;  // Output speed
    uint32_t PUPDR   = 0;  // Pull-up
    uint32_t IDR     = 0;  // Input data
    uint32_t ODR     = 0;  // Output data
    uint32_t BSRR    = 0;  // Bit set/reset (write-only)
};

class GPIODriver {
    GPIO_Regs regs_{};
    std::string_view port_name_;

public:
    explicit GPIODriver(std::string_view name) : port_name_(name) {}

    // EN: Configure pin mode
    void ConfigurePin(uint32_t pin, PinMode mode, PullType pull = PullType::NONE) {
        if (pin >= 16) return;
        // EN: MODER: 2 bits per pin
        uint32_t pos = pin * 2;
        reg_ops::RegOps::ModifyField(regs_.MODER, pos, 2, static_cast<uint32_t>(mode));
        reg_ops::RegOps::ModifyField(regs_.PUPDR, pos, 2, static_cast<uint32_t>(pull));
    }

    void WritePin(uint32_t pin, bool state) {
        if (pin >= 16) return;
        if (state)
            regs_.BSRR = (1u << pin);          // Set
        else
            regs_.BSRR = (1u << (pin + 16));   // Reset
        // EN: Apply BSRR to ODR (simulating hardware behavior)
        ApplyBSRR();
    }

    bool ReadPin(uint32_t pin) const {
        if (pin >= 16) return false;
        return (regs_.IDR >> pin) & 1u;
    }

    void TogglePin(uint32_t pin) {
        if (pin >= 16) return;
        regs_.ODR ^= (1u << pin);
        regs_.IDR = regs_.ODR;  // EN: Simulate loopback
    }

    void PrintRegisters() const {
        std::cout << "    " << port_name_ << " registers:\n";
        std::cout << "      MODER  = 0x" << std::hex << std::setw(8) << std::setfill('0')
                  << regs_.MODER << "\n";
        std::cout << "      PUPDR  = 0x" << std::setw(8) << regs_.PUPDR << "\n";
        std::cout << "      ODR    = 0x" << std::setw(8) << regs_.ODR << "\n";
        std::cout << "      IDR    = 0x" << std::setw(8) << regs_.IDR << std::dec
                  << std::setfill(' ') << "\n";
    }

private:
    void ApplyBSRR() {
        // EN: Low 16 bits = set, high 16 bits = reset
        uint32_t set_mask = regs_.BSRR & 0xFFFF;
        uint32_t reset_mask = (regs_.BSRR >> 16) & 0xFFFF;
        regs_.ODR |= set_mask;
        regs_.ODR &= ~reset_mask;
        regs_.IDR = regs_.ODR;  // EN: Simulate loopback for output pins
        regs_.BSRR = 0;
    }
};

}  // namespace gpio_driver

// ═════════════════════════════════════════════════════════════════════════════
// PART 3: UART Driver with TX Polling, RX Ring Buffer + ISR Callback
// ═════════════════════════════════════════════════════════════════════════════

namespace uart_driver {

// EN: Simple ring buffer for RX FIFO
template <size_t N>
class RingBuffer {
    std::array<uint8_t, N> buf_{};
    size_t head_ = 0;
    size_t tail_ = 0;
    size_t count_ = 0;

public:
    bool Push(uint8_t byte) {
        if (count_ >= N) return false;  // Full
        buf_[head_] = byte;
        head_ = (head_ + 1) % N;
        ++count_;
        return true;
    }

    std::optional<uint8_t> Pop() {
        if (count_ == 0) return std::nullopt;
        uint8_t byte = buf_[tail_];
        tail_ = (tail_ + 1) % N;
        --count_;
        return byte;
    }

    [[nodiscard]] size_t Size() const { return count_; }
    [[nodiscard]] bool Empty() const { return count_ == 0; }
    [[nodiscard]] bool Full() const { return count_ >= N; }
};

struct UART_Regs {
    uint32_t CR1 = 0;   // Control register 1
    uint32_t BRR = 0;   // Baud rate register
    uint32_t SR  = 0;   // Status register
    uint32_t DR  = 0;   // Data register
};

// EN: Status register bits
constexpr uint32_t UART_SR_TXE  = (1u << 7);  // TX empty
constexpr uint32_t UART_SR_RXNE = (1u << 5);  // RX not empty
constexpr uint32_t UART_SR_TC   = (1u << 6);  // Transmission complete

// EN: Control register bits
constexpr uint32_t UART_CR1_UE   = (1u << 0); // UART enable
constexpr uint32_t UART_CR1_TE   = (1u << 3); // Transmitter enable
constexpr uint32_t UART_CR1_RE   = (1u << 2); // Receiver enable
constexpr uint32_t UART_CR1_RXNEIE = (1u << 5); // RX interrupt enable

// EN: Callback type for RX interrupt
using RxCallback = std::function<void(uint8_t)>;

class UARTDriver {
    UART_Regs regs_{};
    RingBuffer<64> rx_fifo_;
    RxCallback rx_callback_;
    bool initialized_ = false;

public:
    bool Init(uint32_t sys_clock, uint32_t baud) {
        regs_.BRR = sys_clock / baud;
        regs_.CR1 = UART_CR1_UE | UART_CR1_TE | UART_CR1_RE;
        regs_.SR = UART_SR_TXE | UART_SR_TC;  // Ready to transmit
        initialized_ = true;
        std::cout << "    UART init: BRR=" << regs_.BRR << " (baud=" << baud << ")\n";
        return true;
    }

    void RegisterRxCallback(RxCallback cb) {
        rx_callback_ = std::move(cb);
        regs_.CR1 |= UART_CR1_RXNEIE;
    }

    // EN: Polling TX — wait for TXE, write byte
    bool TransmitByte(uint8_t byte) {
        if (!initialized_) return false;
        // EN: In real HW, we'd spin on !(regs_.SR & UART_SR_TXE)
        regs_.DR = byte;
        regs_.SR |= UART_SR_TC;
        return true;
    }

    bool TransmitString(std::string_view msg) {
        for (char c : msg) {
            if (!TransmitByte(static_cast<uint8_t>(c))) return false;
        }
        return true;
    }

    // EN: Simulate receiving a byte (as if hardware put data in DR)
    void SimulateRxIRQ(uint8_t byte) {
        regs_.DR = byte;
        regs_.SR |= UART_SR_RXNE;
        // EN: ISR handler
        HandleRxIRQ();
    }

    std::optional<uint8_t> ReadByte() { return rx_fifo_.Pop(); }
    [[nodiscard]] size_t RxAvailable() const { return rx_fifo_.Size(); }

private:
    void HandleRxIRQ() {
        if (regs_.SR & UART_SR_RXNE) {
            auto byte = static_cast<uint8_t>(regs_.DR & 0xFF);
            rx_fifo_.Push(byte);
            regs_.SR &= ~UART_SR_RXNE;
            if (rx_callback_) rx_callback_(byte);
        }
    }
};

}  // namespace uart_driver

// ═════════════════════════════════════════════════════════════════════════════
// PART 4: SPI Driver — Full Duplex + Chip Select
// ═════════════════════════════════════════════════════════════════════════════

namespace spi_driver {

struct SPI_Regs {
    uint32_t CR1 = 0;
    uint32_t CR2 = 0;
    uint32_t SR  = 0;
    uint32_t DR  = 0;
};

constexpr uint32_t SPI_SR_TXE  = (1u << 1);
constexpr uint32_t SPI_SR_RXNE = (1u << 0);
constexpr uint32_t SPI_SR_BSY  = (1u << 7);

enum class SPI_Mode : uint8_t { MODE_0 = 0, MODE_1, MODE_2, MODE_3 };

struct SPI_Config {
    uint32_t clock_divider = 8;
    SPI_Mode mode = SPI_Mode::MODE_0;
    bool lsb_first = false;
};

class SPIDriver {
    SPI_Regs regs_{};
    SPI_Config config_{};
    bool cs_active_ = false;
    bool initialized_ = false;

    // EN: Simulated shift register (loopback: MOSI → MISO)
    uint8_t shift_reg_ = 0;

public:
    bool Init(const SPI_Config& cfg) {
        config_ = cfg;
        uint32_t cpol = (static_cast<uint8_t>(cfg.mode) >> 1) & 1u;
        uint32_t cpha = static_cast<uint8_t>(cfg.mode) & 1u;
        regs_.CR1 = (cpol << 1) | cpha | (1u << 6);  // SPE=1
        regs_.SR = SPI_SR_TXE;
        initialized_ = true;
        std::cout << "    SPI init: div=" << cfg.clock_divider
                  << " mode=" << static_cast<int>(cfg.mode)
                  << " LSB=" << (cfg.lsb_first ? "yes" : "no") << "\n";
        return true;
    }

    void ChipSelect(bool active) {
        cs_active_ = active;
        std::cout << "    CS " << (active ? "LOW (active)" : "HIGH (inactive)") << "\n";
    }

    // EN: Full-duplex transfer: send tx_byte, return rx_byte
    uint8_t Transfer(uint8_t tx_byte) {
        if (!initialized_ || !cs_active_) return 0xFF;

        regs_.DR = tx_byte;
        regs_.SR |= SPI_SR_BSY;

        // EN: Simulate loopback (MOSI → MISO with bit inversion for variety)
        shift_reg_ = static_cast<uint8_t>(~tx_byte);

        regs_.SR &= ~SPI_SR_BSY;
        regs_.SR |= SPI_SR_RXNE;

        return shift_reg_;
    }

    // EN: Multi-byte transfer
    std::vector<uint8_t> TransferBlock(const std::vector<uint8_t>& tx_data) {
        std::vector<uint8_t> rx_data;
        rx_data.reserve(tx_data.size());
        for (auto b : tx_data) {
            rx_data.push_back(Transfer(b));
        }
        return rx_data;
    }
};

}  // namespace spi_driver

// ═════════════════════════════════════════════════════════════════════════════
// PART 5: Driver State Machine
// ═════════════════════════════════════════════════════════════════════════════

namespace driver_fsm {

enum class DriverState : uint8_t {
    UNINITIALIZED,
    INITIALIZED,
    RUNNING,
    ERROR,
    STOPPED
};

constexpr std::string_view StateToString(DriverState s) {
    switch (s) {
        case DriverState::UNINITIALIZED: return "UNINITIALIZED";
        case DriverState::INITIALIZED:   return "INITIALIZED";
        case DriverState::RUNNING:       return "RUNNING";
        case DriverState::ERROR:         return "ERROR";
        case DriverState::STOPPED:       return "STOPPED";
    }
    return "UNKNOWN";
}

// EN: Return code for driver operations (instead of exceptions)
enum class DriverResult : uint8_t {
    OK = 0,
    ERROR_INVALID_STATE,
    ERROR_HW_FAULT,
    ERROR_TIMEOUT,
    ERROR_PARAM
};

constexpr std::string_view ResultToString(DriverResult r) {
    switch (r) {
        case DriverResult::OK:                  return "OK";
        case DriverResult::ERROR_INVALID_STATE: return "INVALID_STATE";
        case DriverResult::ERROR_HW_FAULT:      return "HW_FAULT";
        case DriverResult::ERROR_TIMEOUT:       return "TIMEOUT";
        case DriverResult::ERROR_PARAM:         return "BAD_PARAM";
    }
    return "UNKNOWN";
}

// EN: Generic peripheral driver with state machine
class ManagedDriver {
    DriverState state_ = DriverState::UNINITIALIZED;
    std::string_view name_;

    bool IsValidTransition(DriverState to) const {
        switch (state_) {
            case DriverState::UNINITIALIZED:
                return to == DriverState::INITIALIZED;
            case DriverState::INITIALIZED:
                return to == DriverState::RUNNING || to == DriverState::UNINITIALIZED;
            case DriverState::RUNNING:
                return to == DriverState::STOPPED || to == DriverState::ERROR;
            case DriverState::ERROR:
                return to == DriverState::STOPPED;
            case DriverState::STOPPED:
                return to == DriverState::UNINITIALIZED;
        }
        return false;
    }

    DriverResult Transition(DriverState to) {
        if (!IsValidTransition(to)) {
            std::cout << "    [" << name_ << "] REJECTED: "
                      << StateToString(state_) << " → " << StateToString(to) << "\n";
            return DriverResult::ERROR_INVALID_STATE;
        }
        std::cout << "    [" << name_ << "] " << StateToString(state_)
                  << " → " << StateToString(to) << "\n";
        state_ = to;
        return DriverResult::OK;
    }

public:
    explicit ManagedDriver(std::string_view name) : name_(name) {}

    DriverResult Init()   { return Transition(DriverState::INITIALIZED); }
    DriverResult Start()  { return Transition(DriverState::RUNNING); }
    DriverResult Stop()   { return Transition(DriverState::STOPPED); }
    DriverResult DeInit() { return Transition(DriverState::UNINITIALIZED); }
    DriverResult ReportError() { return Transition(DriverState::ERROR); }

    [[nodiscard]] DriverState GetState() const { return state_; }
};

}  // namespace driver_fsm

// ═════════════════════════════════════════════════════════════════════════════
// PART 6: Error Handling with Return Codes and std::expected-like
// ═════════════════════════════════════════════════════════════════════════════

namespace error_handling {

// EN: Lightweight Expected type (C++23 std::expected not always available)
template <typename T, typename E>
class Expected {
    bool has_value_;
    union { T value_; E error_; };

public:
    // EN: Success constructor
    static Expected Success(const T& val) {
        Expected e;
        e.has_value_ = true;
        e.value_ = val;
        return e;
    }

    // EN: Error constructor
    static Expected Failure(const E& err) {
        Expected e;
        e.has_value_ = false;
        e.error_ = err;
        return e;
    }

    [[nodiscard]] bool HasValue() const { return has_value_; }
    [[nodiscard]] const T& Value() const { return value_; }
    [[nodiscard]] const E& Error() const { return error_; }

private:
    Expected() : has_value_(false), error_{} {}
};

// EN: Example: ADC read with error handling
enum class ADCError : uint8_t { TIMEOUT, NOT_CALIBRATED, CHANNEL_INVALID };

constexpr std::string_view ADCErrorStr(ADCError e) {
    switch (e) {
        case ADCError::TIMEOUT:         return "TIMEOUT";
        case ADCError::NOT_CALIBRATED:  return "NOT_CALIBRATED";
        case ADCError::CHANNEL_INVALID: return "CHANNEL_INVALID";
    }
    return "UNKNOWN";
}

class ADCDriver {
    bool calibrated_ = false;
    static constexpr uint32_t MAX_CHANNELS = 16;

public:
    void Calibrate() {
        calibrated_ = true;
        std::cout << "    ADC calibrated\n";
    }

    Expected<uint16_t, ADCError> Read(uint32_t channel) {
        if (!calibrated_)
            return Expected<uint16_t, ADCError>::Failure(ADCError::NOT_CALIBRATED);
        if (channel >= MAX_CHANNELS)
            return Expected<uint16_t, ADCError>::Failure(ADCError::CHANNEL_INVALID);

        // EN: Simulated conversion result
        auto value = static_cast<uint16_t>(1000 + channel * 100);
        return Expected<uint16_t, ADCError>::Success(value);
    }
};

}  // namespace error_handling

// ═════════════════════════════════════════════════════════════════════════════
// MAIN — Demos
// ═════════════════════════════════════════════════════════════════════════════

int main() {
    std::cout << "╔══════════════════════════════════════════════════════════════╗\n"
              << "║ Module 18: Peripheral Driver — GPIO, UART, SPI, FSM          ║\n"
              << "╚══════════════════════════════════════════════════════════════╝\n\n";

    // --- Demo 1: Register Bit Manipulation ---
    std::cout << "--- Demo 1: Register Bit Manipulation ---\n";
    {
        uint32_t reg = 0;
        reg_ops::RegOps::SetBit(reg, 3);
        reg_ops::RegOps::SetBit(reg, 7);
        std::cout << "    SET bits 3,7:   0x" << std::hex << reg << std::dec << "\n";

        reg_ops::RegOps::ClearBit(reg, 3);
        std::cout << "    CLEAR bit 3:    0x" << std::hex << reg << std::dec << "\n";

        reg_ops::RegOps::ToggleBit(reg, 7);
        std::cout << "    TOGGLE bit 7:   0x" << std::hex << reg << std::dec << "\n";

        // EN: Multi-bit field: set bits [5:4] = 0b11
        reg = 0;
        reg_ops::RegOps::ModifyField(reg, 4, 2, 3);
        std::cout << "    ModifyField[5:4]=3: 0x" << std::hex << reg << std::dec << "\n";

        // EN: Type-safe RegisterField
        using MODER_PIN5 = reg_ops::RegisterField<10, 2>;  // Pin 5: bits [11:10]
        uint32_t moder = 0;
        MODER_PIN5::Set(moder, 0b01);  // Output
        std::cout << "    RegisterField<10,2> set to 01: MODER=0x"
                  << std::hex << moder << std::dec
                  << " read=" << MODER_PIN5::Get(moder) << "\n";
    }

    // --- Demo 2: GPIO Driver ---
    std::cout << "\n--- Demo 2: GPIO Driver ---\n";
    {
        gpio_driver::GPIODriver gpioa("GPIOA");

        gpioa.ConfigurePin(5, gpio_driver::PinMode::OUTPUT, gpio_driver::PullType::NONE);
        gpioa.ConfigurePin(13, gpio_driver::PinMode::INPUT, gpio_driver::PullType::PULL_UP);

        gpioa.WritePin(5, true);
        std::cout << "    Pin 5 written HIGH, read=" << gpioa.ReadPin(5) << "\n";

        gpioa.TogglePin(5);
        std::cout << "    Pin 5 toggled,      read=" << gpioa.ReadPin(5) << "\n";

        gpioa.TogglePin(5);
        std::cout << "    Pin 5 toggled,      read=" << gpioa.ReadPin(5) << "\n";

        gpioa.PrintRegisters();
    }

    // --- Demo 3: UART Driver — TX Polling + RX ISR Callback ---
    std::cout << "\n--- Demo 3: UART Driver — TX Polling + RX ISR ---\n";
    {
        uart_driver::UARTDriver uart;
        uart.Init(168'000'000, 115200);

        // EN: Register RX callback
        int rx_count = 0;
        uart.RegisterRxCallback([&rx_count](uint8_t byte) {
            std::cout << "    [ISR] RX byte: 0x" << std::hex
                      << static_cast<int>(byte) << std::dec << "\n";
            ++rx_count;
        });

        // EN: TX (polling)
        std::cout << "    TX: ";
        uart.TransmitString("Hi");
        std::cout << "sent 'Hi'\n";

        // EN: Simulate RX interrupts
        std::cout << "    Simulating 3 RX interrupts:\n";
        uart.SimulateRxIRQ(0x41);  // 'A'
        uart.SimulateRxIRQ(0x42);  // 'B'
        uart.SimulateRxIRQ(0x43);  // 'C'

        std::cout << "    RX FIFO size: " << uart.RxAvailable()
                  << ", callback invoked " << rx_count << " times\n";

        // EN: Read from FIFO
        while (auto byte = uart.ReadByte()) {
            std::cout << "    FIFO pop: '" << static_cast<char>(*byte) << "'\n";
        }
    }

    // --- Demo 4: SPI Driver — Full Duplex ---
    std::cout << "\n--- Demo 4: SPI Driver — Full Duplex ---\n";
    {
        spi_driver::SPIDriver spi;
        spi_driver::SPI_Config cfg{.clock_divider = 4, .mode = spi_driver::SPI_Mode::MODE_0, .lsb_first = false};
        spi.Init(cfg);

        spi.ChipSelect(true);

        // EN: Single byte
        uint8_t rx = spi.Transfer(0xAA);
        std::cout << "    TX=0xAA → RX=0x" << std::hex << static_cast<int>(rx) << std::dec << "\n";

        // EN: Block transfer
        std::vector<uint8_t> tx_data = {0x10, 0x20, 0x30, 0x40};
        auto rx_data = spi.TransferBlock(tx_data);
        std::cout << "    Block TX: ";
        for (auto b : tx_data) std::cout << "0x" << std::hex << static_cast<int>(b) << " ";
        std::cout << "\n    Block RX: ";
        for (auto b : rx_data) std::cout << "0x" << std::hex << static_cast<int>(b) << " ";
        std::cout << std::dec << "\n";

        spi.ChipSelect(false);
    }

    // --- Demo 5: Driver State Machine ---
    std::cout << "\n--- Demo 5: Driver State Machine ---\n";
    {
        driver_fsm::ManagedDriver drv("ADC_Driver");

        std::cout << "  Valid lifecycle:\n";
        auto r1 = drv.Init();
        auto r2 = drv.Start();
        auto r3 = drv.Stop();
        auto r4 = drv.DeInit();
        std::cout << "    Results: " << driver_fsm::ResultToString(r1) << ", "
                  << driver_fsm::ResultToString(r2) << ", "
                  << driver_fsm::ResultToString(r3) << ", "
                  << driver_fsm::ResultToString(r4) << "\n";

        std::cout << "  Invalid transitions:\n";
        drv.Init();
        auto bad1 = drv.Stop();   // Init → Stop = invalid
        auto bad2 = drv.DeInit(); // Init → Uninit (but tested as Stop first)
        std::cout << "    Stop from INITIALIZED: " << driver_fsm::ResultToString(bad1) << "\n";
        std::cout << "    DeInit from INITIALIZED: " << driver_fsm::ResultToString(bad2) << "\n";

        std::cout << "  Error recovery:\n";
        driver_fsm::ManagedDriver drv2("SPI_Driver");
        drv2.Init();
        drv2.Start();
        drv2.ReportError();
        drv2.Stop();
        drv2.DeInit();
    }

    // --- Demo 6: Error Handling with Expected ---
    std::cout << "\n--- Demo 6: Error Handling with Expected ---\n";
    {
        error_handling::ADCDriver adc;

        // EN: Read before calibration
        auto r1 = adc.Read(0);
        if (!r1.HasValue())
            std::cout << "    Read ch0 (uncalibrated): ERROR="
                      << error_handling::ADCErrorStr(r1.Error()) << "\n";

        adc.Calibrate();

        // EN: Valid read
        auto r2 = adc.Read(3);
        if (r2.HasValue())
            std::cout << "    Read ch3: value=" << r2.Value() << "\n";

        // EN: Invalid channel
        auto r3 = adc.Read(20);
        if (!r3.HasValue())
            std::cout << "    Read ch20: ERROR="
                      << error_handling::ADCErrorStr(r3.Error()) << "\n";

        // EN: Read all valid channels
        std::cout << "    All channels: ";
        for (uint32_t ch = 0; ch < 4; ++ch) {
            auto r = adc.Read(ch);
            if (r.HasValue()) std::cout << "ch" << ch << "=" << r.Value() << " ";
        }
        std::cout << "\n";
    }

    // --- Demo 7: Driver Layer Summary ---
    std::cout << "\n--- Demo 7: Peripheral Driver Architecture Summary ---\n";
    {
        struct Layer { std::string_view name; std::string_view description; };
        Layer layers[] = {
            {"Register Ops",    "Raw bit set/clear/toggle/modify"},
            {"RegisterField<>", "Type-safe compile-time field access"},
            {"GPIO Driver",     "Pin config, write, read, toggle via BSRR"},
            {"UART Driver",     "Polling TX, interrupt RX with ring buffer"},
            {"SPI Driver",      "Full-duplex transfer, chip select control"},
            {"State Machine",   "UNINIT→INIT→RUN→STOP→UNINIT lifecycle    "},
            {"Error Handling",  "Return codes + Expected<T,E> pattern"},
        };
        std::cout << "  ┌───────────────────┬───────────────────────────────────────────┐\n";
        std::cout << "  │ Component         │ Description                               │\n";
        std::cout << "  ├───────────────────┼───────────────────────────────────────────┤\n";
        for (const auto& l : layers) {
            std::cout << "  │ " << std::setw(17) << std::left << l.name
                      << " │ " << std::setw(41) << l.description << " │\n";
        }
        std::cout << std::right;
        std::cout << "  └───────────────────┴───────────────────────────────────────────┘\n";
    }

    std::cout << "\n[All demos completed successfully.]\n";
    return 0;
}

