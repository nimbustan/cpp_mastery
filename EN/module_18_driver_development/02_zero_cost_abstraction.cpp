/**
 * @file module_18_driver_development/02_zero_cost_abstraction.cpp
 * @brief CRTP & Policy-Based Design
 *
 * @details
 * =============================================================================
 * [THEORY: Virtual Dispatch vs CRTP — Cost Analysis]
 * =============================================================================
 *
 * EN: Virtual dispatch induces runtime overhead:
 *     - vtable pointer per object (+8 bytes on 64-bit)
 *     - Indirect function call (pipeline stall, cache miss)
 *     - Cannot be inlined by compiler
 *
 *     CRTP eliminates all three by resolving calls at compile time.
 *
 *     Virtual dispatch             CRTP (static polymorphism)
 *     ┌─────────┐                ┌─────────┐
 *     │ obj     │                │ obj     │
 *     │ vptr    │──→ vtable      │ (no ptr)│
 *     └─────────┘   ┌──────┐     └─────────┘
 *                   │func1 │     Compiler resolves:
 *                   │func2 │     Derived::func() inlined
 *                   └──────┘
 *
 *     Overhead comparison:
 *     ┌──────────────────┬───────────────┬──────────────────┐
 *     │ Metric           │ Virtual       │ CRTP             │
 *     ├──────────────────┼───────────────┼──────────────────┤
 *     │ Object size      │ +8 bytes(vptr)│ No overhead      │
 *     │ Call overhead    │ ~2-5 cycles   │ 0 (inlined)      │
 *     │ Inlineable       │ No            │ Yes              │
 *     │ Code size (ROM)  │ vtable + code │ Code per type    │
 *     │ Runtime flexible │ ✅ Yes        │ ❌ Compile-time  │
 *     └──────────────────┴───────────────┴──────────────────┘
 *
 *
 *
 *     Virtual dispatch             CRTP (statik polimorfizm)
 *     ┌─────────┐                ┌──────────┐
 *     │ obj     │                │ obj      │
 *     │ vptr    │──→ vtable      │ (ptr yok)│
 *     └─────────┘   ┌──────┐     └──────────┘
 *                   └──────┘
 *
 *     ┌──────────────────┬───────────────┬──────────────────┐
 *     │ Metrik           │ Virtual       │ CRTP             │
 *     ├──────────────────┼───────────────┼──────────────────┤
 *     └──────────────────┴───────────────┴──────────────────┘
 *
 * =============================================================================
 * [THEORY: Policy-Based Design]
 * =============================================================================
 *
 * EN: Policy-based design separates concerns into independent policy classes:
 *
 *     template<typename CommPolicy, typename ErrorPolicy>
 *     class Sensor : public CommPolicy, public ErrorPolicy { ... };
 *
 *     Sensor<SPI_Comm, LogError>    sensor_spi;
 *     Sensor<I2C_Comm, ResetError>  sensor_i2c;
 *
 *     Each combination generates optimal code with no runtime overhead.
 *
 *
 *     template<typename CommPolicy, typename ErrorPolicy>
 *     class Sensor : public CommPolicy, public ErrorPolicy { ... };
 *
 *     Sensor<SPI_Comm, LogError>    sensor_spi;
 *     Sensor<I2C_Comm, ResetError>  sensor_i2c;
 *
 *
 * =============================================================================
 * [THEORY: Embedded C++ Compile Flags]
 * =============================================================================
 *
 * EN: Common embedded-safe flags:
 *     ┌────────────────────────┬────────────────────────────────────┐
 *     │ Flag                   │ Effect                             │
 *     ├────────────────────────┼────────────────────────────────────┤
 *     │ -fno-exceptions        │ Disable C++ exceptions             │
 *     │ -fno-rtti              │ Disable RTTI (dynamic_cast/typeid) │
 *     │ -fno-threadsafe-statics│ No thread-safe static init guard   │
 *     │ -fno-unwind-tables     │ Remove stack unwinding tables      │
 *     │ -Os / -Oz              │ Optimize for size                  │
 *     │ -ffunction-sections    │ Place each function in own section │
 *     │ -fdata-sections        │ Place each data item in own section│
 *     │ -Wl,--gc-sections      │ Linker removes unused sections     │
 *     └────────────────────────┴────────────────────────────────────┘
 *
 *
 * @note Compile: g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion 02_zero_cost_abstraction.cpp -o 02_zero_cost
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
#include <iomanip>
#include <type_traits>
#include <optional>
#include <cassert>
#include <chrono>

// ═════════════════════════════════════════════════════════════════════════════
// PART 1: Virtual Dispatch vs CRTP — Side-by-Side
// ═════════════════════════════════════════════════════════════════════════════

namespace virtual_vs_crtp {

// EN: --- Virtual version (runtime polymorphism) ---
class ISensor_Virtual {
public:
    virtual ~ISensor_Virtual() = default;
    virtual uint16_t Read() = 0;
    virtual std::string_view Name() const = 0;
};

class TempSensor_V : public ISensor_Virtual {
public:
    uint16_t Read() override { return 2048; }
    std::string_view Name() const override { return "TempSensor(V)"; }
};

class PressureSensor_V : public ISensor_Virtual {
public:
    uint16_t Read() override { return 3500; }
    std::string_view Name() const override { return "PressureSensor(V)"; }
};

// EN: --- CRTP version (compile-time polymorphism) ---
template <typename Derived>
class ISensor_CRTP {
public:
    uint16_t Read() { return static_cast<Derived*>(this)->ReadImpl(); }
    std::string_view Name() const {
        return static_cast<const Derived*>(this)->NameImpl();
    }
};

class TempSensor_C : public ISensor_CRTP<TempSensor_C> {
public:
    uint16_t ReadImpl() { return 2048; }
    static std::string_view NameImpl() { return "TempSensor(CRTP)"; }
};

class PressureSensor_C : public ISensor_CRTP<PressureSensor_C> {
public:
    uint16_t ReadImpl() { return 3500; }
    static std::string_view NameImpl() { return "PressureSensor(CRTP)"; }
};

// EN: Size comparison helper
inline void PrintSizes() {
    std::cout << "  Object sizes:\n";
    std::cout << "    TempSensor_V (virtual):   " << sizeof(TempSensor_V) << " bytes\n";
    std::cout << "    TempSensor_C (CRTP):      " << sizeof(TempSensor_C) << " bytes\n";
    std::cout << "    PressureSensor_V (virtual):" << sizeof(PressureSensor_V) << " bytes\n";
    std::cout << "    PressureSensor_C (CRTP):   " << sizeof(PressureSensor_C) << " bytes\n";
    std::cout << "    vtable pointer overhead:   " << (sizeof(TempSensor_V) - sizeof(TempSensor_C)) << " bytes\n";
}

// EN: Generic function — works with any CRTP sensor
template <typename SensorType>
uint16_t ReadAndLog(ISensor_CRTP<SensorType>& sensor) {
    auto val = sensor.Read();
    std::cout << "    " << sensor.Name() << " = " << val << "\n";
    return val;
}

}  // namespace virtual_vs_crtp

// ═════════════════════════════════════════════════════════════════════════════
// PART 2: CRTP Driver Pattern — SPI/I2C Backend Selection
// ═════════════════════════════════════════════════════════════════════════════

namespace crtp_driver {

// EN: SPI communication backend
class SPI_Backend {
protected:
    static uint16_t TransferRaw(uint8_t reg, [[maybe_unused]] uint8_t dummy) {
        // EN: Simulated SPI register read
        return static_cast<uint16_t>(0x1000 + reg);
    }
    static std::string_view BackendName() { return "SPI"; }
};

// EN: I2C communication backend
class I2C_Backend {
protected:
    static uint16_t TransferRaw(uint8_t reg, [[maybe_unused]] uint8_t dummy) {
        // EN: Simulated I2C register read
        return static_cast<uint16_t>(0x2000 + reg);
    }
    static std::string_view BackendName() { return "I2C"; }
};

// EN: CRTP sensor driver — backend selected at compile time
template <typename Derived, typename CommBackend>
class SensorDriver : public CommBackend {
public:
    uint16_t ReadRegister(uint8_t reg) {
        return CommBackend::TransferRaw(reg, 0);
    }

    float ReadTemperature() {
        uint16_t raw = ReadRegister(0x10);
        return static_cast<Derived*>(this)->ConvertRaw(raw);
    }

    void PrintInfo() {
        std::cout << "    Sensor: " << static_cast<Derived*>(this)->SensorName()
                  << " via " << CommBackend::BackendName() << "\n";
    }
};

// EN: LM75 temperature sensor
template <typename CommBackend>
class LM75 : public SensorDriver<LM75<CommBackend>, CommBackend> {
public:
    float ConvertRaw(uint16_t raw) { return static_cast<float>(raw) * 0.00625f; }
    static std::string_view SensorName() { return "LM75"; }
};

// EN: BMP280 pressure/temp sensor
template <typename CommBackend>
class BMP280 : public SensorDriver<BMP280<CommBackend>, CommBackend> {
public:
    float ConvertRaw(uint16_t raw) { return static_cast<float>(raw) * 0.01f; }
    static std::string_view SensorName() { return "BMP280"; }
};

}  // namespace crtp_driver

// ═════════════════════════════════════════════════════════════════════════════
// PART 3: Policy-Based Design — Sensor with Comm + Error Policies
// ═════════════════════════════════════════════════════════════════════════════

namespace policy_design {

// EN: Communication policies
struct SPI_Policy {
    static uint16_t Read(uint8_t reg) {
        return static_cast<uint16_t>(0x1000 + reg);
    }
    static std::string_view CommName() { return "SPI"; }
};

struct I2C_Policy {
    static uint16_t Read(uint8_t reg) {
        return static_cast<uint16_t>(0x2000 + reg);
    }
    static std::string_view CommName() { return "I2C"; }
};

// EN: Error handling policies
struct LogErrorPolicy {
    static void HandleError(std::string_view msg) {
        std::cout << "      [ERROR-LOG] " << msg << "\n";
    }
    static std::string_view ErrorPolicyName() { return "LogError"; }
};

struct ResetErrorPolicy {
    static void HandleError(std::string_view msg) {
        std::cout << "      [ERROR-RESET] " << msg << " → RESETTING\n";
    }
    static std::string_view ErrorPolicyName() { return "ResetError"; }
};

struct IgnoreErrorPolicy {
    static void HandleError([[maybe_unused]] std::string_view msg) {
        // EN: Silently ignore
    }
    static std::string_view ErrorPolicyName() { return "IgnoreError"; }
};

// EN: Policy-based sensor class
template <typename CommPolicy, typename ErrorPolicy>
class Sensor {
    uint16_t last_value_ = 0;

public:
    uint16_t Read(uint8_t reg) {
        auto val = CommPolicy::Read(reg);
        if (val == 0) {
            ErrorPolicy::HandleError("Read returned zero — possible HW fault");
            return last_value_;
        }
        last_value_ = val;
        return val;
    }

    void PrintConfig() const {
        std::cout << "    Comm=" << CommPolicy::CommName()
                  << " Error=" << ErrorPolicy::ErrorPolicyName() << "\n";
    }
};

}  // namespace policy_design

// ═════════════════════════════════════════════════════════════════════════════
// PART 4: if constexpr — Platform-Specific Code Elimination
// ═════════════════════════════════════════════════════════════════════════════

namespace constexpr_dispatch {

// EN: Platform tags
struct STM32_Platform {};
struct NXP_Platform {};
struct RiscV_Platform {};

// EN: Compile-time clock configuration
template <typename Platform>
constexpr uint32_t GetSystemClock() {
    if constexpr (std::is_same_v<Platform, STM32_Platform>)
        return 168'000'000;  // 168 MHz
    else if constexpr (std::is_same_v<Platform, NXP_Platform>)
        return 120'000'000;  // 120 MHz
    else if constexpr (std::is_same_v<Platform, RiscV_Platform>)
        return 32'000'000;   // 32 MHz
    else
        return 0;
}

template <typename Platform>
constexpr uint32_t GetUARTBaudDivider(uint32_t baud) {
    return GetSystemClock<Platform>() / baud;
}

// EN: Platform-specific initialization (dead code eliminated by compiler)
template <typename Platform>
void InitPeripherals() {
    std::cout << "    System clock: " << GetSystemClock<Platform>() / 1'000'000 << " MHz\n";

    if constexpr (std::is_same_v<Platform, STM32_Platform>) {
        std::cout << "    STM32: Enabling AHB1 bus clocks\n";
        std::cout << "    STM32: Configuring flash wait states = 5\n";
    } else if constexpr (std::is_same_v<Platform, NXP_Platform>) {
        std::cout << "    NXP: Enabling PCC (Peripheral Clock Control)\n";
        std::cout << "    NXP: Configuring flash speculation\n";
    } else if constexpr (std::is_same_v<Platform, RiscV_Platform>) {
        std::cout << "    RISC-V: Configuring PLL from XTAL\n";
        std::cout << "    RISC-V: Setting MSTATUS.MIE = 1\n";
    }

    constexpr auto uart_div = GetUARTBaudDivider<Platform>(115200);
    std::cout << "    UART divider for 115200 baud: " << uart_div << "\n";
}

}  // namespace constexpr_dispatch

// ═════════════════════════════════════════════════════════════════════════════
// PART 5: Tag Dispatch — Communication Type Selection
// ═════════════════════════════════════════════════════════════════════════════

namespace tag_dispatch {

// EN: Tag types for dispatch
struct SPI_tag {};
struct I2C_tag {};
struct UART_tag {};

// EN: Overloaded init functions
inline void InitComm(SPI_tag, uint32_t speed) {
    std::cout << "    SPI init: speed=" << speed / 1000 << " kHz, CPOL=0, CPHA=0\n";
}

inline void InitComm(I2C_tag, uint32_t speed) {
    std::cout << "    I2C init: speed=" << speed / 1000 << " kHz, 7-bit addressing\n";
}

inline void InitComm(UART_tag, uint32_t speed) {
    std::cout << "    UART init: baud=" << speed << ", 8N1\n";
}

// EN: Sensor that uses tag dispatch for communication init
template <typename CommTag>
class TaggedSensor {
    std::string_view name_;
    uint32_t comm_speed_;

public:
    TaggedSensor(std::string_view name, uint32_t speed)
        : name_(name), comm_speed_(speed) {}

    void Init() {
        std::cout << "    Initializing " << name_ << ":\n  ";
        InitComm(CommTag{}, comm_speed_);
    }
};

}  // namespace tag_dispatch

// ═════════════════════════════════════════════════════════════════════════════
// PART 6: constexpr Configuration — Compile-Time Register Layout
// ═════════════════════════════════════════════════════════════════════════════

namespace constexpr_config {

// EN: Register address definitions (compile-time)
struct PeripheralConfig {
    uint32_t base_address;
    uint32_t clock_enable_bit;
    uint32_t irq_number;
    uint32_t dma_channel;
};

// EN: Compile-time peripheral table
constexpr std::array<PeripheralConfig, 4> PERIPHERALS = {{
    {0x40004400, 17, 37, 4},  // USART2
    {0x40004800, 18, 38, 3},  // USART3
    {0x40013000, 14, 35, 2},  // SPI1
    {0x40003800, 15, 36, 0},  // SPI2
}};

constexpr size_t FindPeripheral(uint32_t base) {
    for (size_t i = 0; i < PERIPHERALS.size(); ++i) {
        if (PERIPHERALS[i].base_address == base) return i;
    }
    return PERIPHERALS.size();  // not found
}

// EN: Verify at compile time
static_assert(FindPeripheral(0x40013000) == 2, "SPI1 should be at index 2");
static_assert(FindPeripheral(0x40004400) == 0, "USART2 should be at index 0");

// EN: Buffer sizes — compile-time
constexpr size_t UART_TX_BUF_SIZE = 256;
constexpr size_t UART_RX_BUF_SIZE = 512;
constexpr size_t SPI_BUF_SIZE = 128;

// EN: Compile-time CRC polynomial check
constexpr uint32_t CRC_POLY = 0x04C11DB7;
static_assert(CRC_POLY != 0, "CRC polynomial must not be zero");

}  // namespace constexpr_config

// ═════════════════════════════════════════════════════════════════════════════
// MAIN — Demos
// ═════════════════════════════════════════════════════════════════════════════

int main() {
    std::cout << "╔══════════════════════════════════════════════════════════════╗\n"
              << "║ Module 18: Zero-Cost Abstraction — CRTP & Policy Design      ║\n"
              << "╚══════════════════════════════════════════════════════════════╝\n\n";

    // --- Demo 1: Virtual vs CRTP — Size + Call ---
    std::cout << "--- Demo 1: Virtual vs CRTP — Size Comparison ---\n";
    {
        virtual_vs_crtp::PrintSizes();

        std::cout << "\n  Virtual dispatch:\n";
        virtual_vs_crtp::TempSensor_V temp_v;
        virtual_vs_crtp::PressureSensor_V press_v;
        virtual_vs_crtp::ISensor_Virtual* sensors[] = {&temp_v, &press_v};
        for (auto* s : sensors)
            std::cout << "    " << s->Name() << " = " << s->Read() << "\n";

        std::cout << "  CRTP dispatch:\n";
        virtual_vs_crtp::TempSensor_C temp_c;
        virtual_vs_crtp::PressureSensor_C press_c;
        virtual_vs_crtp::ReadAndLog(temp_c);
        virtual_vs_crtp::ReadAndLog(press_c);
    }

    // --- Demo 2: CRTP Driver — SPI vs I2C Backend ---
    std::cout << "\n--- Demo 2: CRTP Driver — SPI vs I2C Backend ---\n";
    {
        crtp_driver::LM75<crtp_driver::SPI_Backend> lm75_spi;
        crtp_driver::LM75<crtp_driver::I2C_Backend> lm75_i2c;
        crtp_driver::BMP280<crtp_driver::SPI_Backend> bmp_spi;

        lm75_spi.PrintInfo();
        std::cout << "    Temp(SPI) = " << std::fixed << std::setprecision(2)
                  << lm75_spi.ReadTemperature() << " °C\n";

        lm75_i2c.PrintInfo();
        std::cout << "    Temp(I2C) = " << lm75_i2c.ReadTemperature() << " °C\n";

        bmp_spi.PrintInfo();
        std::cout << "    Temp(SPI) = " << bmp_spi.ReadTemperature() << " °C\n";
        std::cout << std::defaultfloat;
    }

    // --- Demo 3: Policy-Based Design ---
    std::cout << "\n--- Demo 3: Policy-Based Design — Comm + Error Policies ---\n";
    {
        policy_design::Sensor<policy_design::SPI_Policy, policy_design::LogErrorPolicy> sensor1;
        policy_design::Sensor<policy_design::I2C_Policy, policy_design::ResetErrorPolicy> sensor2;
        policy_design::Sensor<policy_design::SPI_Policy, policy_design::IgnoreErrorPolicy> sensor3;

        sensor1.PrintConfig();
        auto v1 = sensor1.Read(0x10);
        std::cout << "    Read(0x10) = 0x" << std::hex << v1 << std::dec << "\n";

        sensor2.PrintConfig();
        auto v2 = sensor2.Read(0x20);
        std::cout << "    Read(0x20) = 0x" << std::hex << v2 << std::dec << "\n";

        sensor3.PrintConfig();
        auto v3 = sensor3.Read(0x30);
        std::cout << "    Read(0x30) = 0x" << std::hex << v3 << std::dec << "\n";

        std::cout << "\n  Object sizes (all policies generate separate types):\n";
        std::cout << "    sensor1: " << sizeof(sensor1) << " bytes\n";
        std::cout << "    sensor2: " << sizeof(sensor2) << " bytes\n";
        std::cout << "    sensor3: " << sizeof(sensor3) << " bytes\n";
    }

    // --- Demo 4: if constexpr — Platform-Specific Code ---
    std::cout << "\n--- Demo 4: if constexpr — Platform Code Elimination ---\n";
    {
        std::cout << "  [STM32]\n";
        constexpr_dispatch::InitPeripherals<constexpr_dispatch::STM32_Platform>();
        std::cout << "  [NXP]\n";
        constexpr_dispatch::InitPeripherals<constexpr_dispatch::NXP_Platform>();
        std::cout << "  [RISC-V]\n";
        constexpr_dispatch::InitPeripherals<constexpr_dispatch::RiscV_Platform>();
    }

    // --- Demo 5: Tag Dispatch ---
    std::cout << "\n--- Demo 5: Tag Dispatch — Communication Type Selection ---\n";
    {
        tag_dispatch::TaggedSensor<tag_dispatch::SPI_tag>  accel("Accelerometer", 10'000'000);
        tag_dispatch::TaggedSensor<tag_dispatch::I2C_tag>  temp("TempSensor", 400'000);
        tag_dispatch::TaggedSensor<tag_dispatch::UART_tag> gps("GPS Module", 9600);

        accel.Init();
        temp.Init();
        gps.Init();
    }

    // --- Demo 6: constexpr Configuration ---
    std::cout << "\n--- Demo 6: constexpr Compile-Time Configuration ---\n";
    {
        std::cout << "  Peripheral table (resolved at compile time):\n";
        std::cout << "  ┌────────────┬──────────────┬─────┬─────┐\n";
        std::cout << "  │ Base Addr  │ ClkEn Bit    │ IRQ │ DMA │\n";
        std::cout << "  ├────────────┼──────────────┼─────┼─────┤\n";
        for (const auto& p : constexpr_config::PERIPHERALS) {
            std::cout << "  │ 0x" << std::hex << std::setw(8) << std::setfill('0')
                      << p.base_address << std::dec << std::setfill(' ')
                      << " │ bit " << std::setw(8) << p.clock_enable_bit
                      << " │ " << std::setw(3) << p.irq_number
                      << " │ " << std::setw(3) << p.dma_channel << " │\n";
        }
        std::cout << "  └────────────┴──────────────┴─────┴─────┘\n";

        constexpr auto spi1_idx = constexpr_config::FindPeripheral(0x40013000);
        std::cout << "  SPI1 found at index " << spi1_idx
                  << " (IRQ=" << constexpr_config::PERIPHERALS[spi1_idx].irq_number << ")\n";

        std::cout << "  Buffer sizes: UART_TX=" << constexpr_config::UART_TX_BUF_SIZE
                  << " UART_RX=" << constexpr_config::UART_RX_BUF_SIZE
                  << " SPI=" << constexpr_config::SPI_BUF_SIZE << "\n";
    }

    // --- Demo 7: Zero-Cost Summary ---
    std::cout << "\n--- Demo 7: Zero-Cost Abstraction Summary ---\n";
    {
        struct Technique { std::string_view name; std::string_view overhead; std::string_view use_case; };
        Technique techs[] = {
            {"CRTP",                "0 cycles", "Static polymorphism (sensors, drivers)"},
            {"Policy-based design", "0 cycles", "Configurable behavior (comm, error)"},
            {"if constexpr",        "0 cycles", "Platform-specific code elimination"},
            {"Tag dispatch",        "0 cycles", "Overload resolution by type tag"},
            {"constexpr config",    "0 cycles", "Compile-time peripheral configuration"},
            {"templates",           "0 cycles", "Generic algorithms, type-safe APIs"},
        };
        std::cout << "  ┌──────────────────────┬───────────┬────────────────────────────────────────┐\n";
        std::cout << "  │ Technique            │ Overhead  │ Use Case                               │\n";
        std::cout << "  ├──────────────────────┼───────────┼────────────────────────────────────────┤\n";
        for (const auto& t : techs) {
            std::cout << "  │ " << std::setw(20) << std::left << t.name
                      << " │ " << std::setw(9) << t.overhead
                      << " │ " << std::setw(38) << t.use_case << " │\n";
        }
        std::cout << std::right;
        std::cout << "  └──────────────────────┴───────────┴────────────────────────────────────────┘\n";
    }

    std::cout << "\n[All demos completed successfully.]\n";
    return 0;
}

