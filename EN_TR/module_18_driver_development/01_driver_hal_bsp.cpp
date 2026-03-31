/**
 * @file module_18_driver_development/01_driver_hal_bsp.cpp
 * @brief Driver / HAL / BSP Layers — C vs C++ in Driver Development
 *
 * @details
 * =============================================================================
 * [THEORY: Driver Layer Hierarchy / TEORİ: Sürücü Katman Hiyerarşisi]
 * =============================================================================
 *
 * EN:
 *     ┌─────────────────────────────────────────────────────┐
 *     │                Application Layer                    │
 *     │  (C++ classes, business logic, AUTOSAR SWC)         │
 *     ├─────────────────────────────────────────────────────┤
 *     │                Framework / Middleware               │
 *     │  (C++: SOME/IP, DDS, Qt, Android HAL)               │
 *     ├─────────────────────────────────────────────────────┤
 *     │                HAL (C++ or C)                       │
 *     │  Hardware Abstraction Layer                         │
 *     │  Platform-independent API                           │
 *     ├─────────────────────────────────────────────────────┤
 *     │                Driver (C)                           │
 *     │  Register-level access, interrupt handlers          │
 *     ├─────────────────────────────────────────────────────┤
 *     │                BSP (Board Support Package)          │
 *     │  Clock config, pin mux, memory map, startup code    │
 *     ├─────────────────────────────────────────────────────┤
 *     │                Hardware (MCU / SoC)                 │
 *     └─────────────────────────────────────────────────────┘
 *
 * TR: Sürücü katman hiyerarşisi (alttan üste):
 *
 *     ┌───────────────────────────────────────────────────┐
 *     │                Uygulama Katmanı                   │
 *     │  (C++ sınıfları, iş mantığı, AUTOSAR SWC)         │
 *     ├───────────────────────────────────────────────────┤
 *     │              Framework / Middleware               │
 *     │  (C++: SOME/IP, DDS, Qt, Android HAL)             │
 *     ├───────────────────────────────────────────────────┤
 *     │              HAL (C++ veya C)                     │
 *     │  Donanım Soyutlama Katmanı — Platform bağımsız API│
 *     ├───────────────────────────────────────────────────┤
 *     │              Sürücü (C)                           │
 *     │  Yazmaç seviyesi erişim, kesme işleyicileri       │
 *     ├───────────────────────────────────────────────────┤
 *     │              BSP (Board Support Package)          │
 *     │  Saat yapılandırma, pin mux, bellek haritası      │
 *     ├───────────────────────────────────────────────────┤
 *     │              Donanım (MCU / SoC)                  │
 *     └───────────────────────────────────────────────────┘
 *
 *     Alt katmanlar genelde C, üst katmanlar C++ ile yazılır.
 *
 * =============================================================================
 * [THEORY: C vs C++ in Driver Development / TEORİ: Sürücü Geliştirmede C ve C++]
 * =============================================================================
 *
 * EN:
 *     ┌─────────────────┬───────────────────┬───────────────────┐
 *     │ Aspect          │ C                 │ C++               │
 *     ├─────────────────┼───────────────────┼───────────────────┤
 *     │ Kernel driver   │ ✅ Required       │ ❌ Not allowed    │
 *     │ MCAL driver     │ ✅ Standard       │ ⚠️ Wrapper only   │
 *     │ HAL abstraction │ ✅ Common         │ ✅ CRTP, templates│
 *     │ BSP init        │ ✅ Standard       │ ⚠️ constexpr conf │
 *     │ Framework       │ ❌ Limited        │ ✅ Preferred      │
 *     │ Application SWC │ ⚠️ Possible       │ ✅ Standard       │
 *     ├─────────────────┼───────────────────┼───────────────────┤
 *     │ ABI stability   │ ✅ Guaranteed     │ ❌ Name mangling  │
 *     │ No exceptions   │ ✅ Default        │ ⚠️ -fno-exception │
 *     │ No RTTI         │ ✅ Default        │ ⚠️ -fno-rtti      │
 *     │ No hidden alloc │ ✅ Explicit       │ ⚠️ Must be aware  │
 *     │ MISRA compliant │ ✅ MISRA C 2012   │ ✅ MISRA C++ 2023 │
 *     └─────────────────┴───────────────────┴───────────────────┘
 *
 * TR: C vs C++ sürücü geliştirmede:
 *     ┌─────────────────┬───────────────────┬───────────────────┐
 *     │ Yön             │ C                 │ C++               │
 *     ├─────────────────┼───────────────────┼───────────────────┤
 *     │ Kernel sürücü   │ ✅ Zorunlu        │ ❌ İzin verilmez  │
 *     │ MCAL sürücü     │ ✅ Standart       │ ⚠️ Yalnızca sarmal│
 *     │ HAL soyutlama   │ ✅ Yaygın         │ ✅ CRTP, template │
 *     │ BSP init        │ ✅ Standart       │ ⚠️ constexpr conf │
 *     │ Framework       │ ❌ Sınırlı        │ ✅ Tercih edilir  │
 *     │ Uygulama SWC    │ ⚠️ Mümkün         │ ✅ Standart       │
 *     ├─────────────────┼───────────────────┼───────────────────┤
 *     │ ABI kararlılığı │ ✅ Garantili      │ ❌ İsim karışt.   │
 *     │ İstisna yok     │ ✅ Varsayılan     │ ⚠️ -fno-exception │
 *     │ RTTI yok        │ ✅ Varsayılan     │ ⚠️ -fno-rtti      │
 *     │ Gizli allok yok │ ✅ Açık           │ ⚠️ Dikkat gerekli │
 *     │ MISRA uyumlu    │ ✅ MISRA C 2012   │ ✅ MISRA C++ 2023 │
 *     └─────────────────┴───────────────────┴───────────────────┘
 *
 * =============================================================================
 * [THEORY: extern "C" Boundary — C Driver + C++ Wrapper / TEORİ: extern "C" Sınırı — C Sürücü + C++ Sarmalayıcı]
 * =============================================================================
 *
 * EN: When C++ code calls a C driver, the boundary is marked with extern "C"
 *     to prevent C++ name mangling and ensure ABI compatibility.
 *
 *     // C header (gpio_driver.h)
 *     #ifdef __cplusplus
 *     extern "C" {
 *     #endif
 *         void GPIO_Init(uint32_t port, uint32_t pin, uint32_t mode);
 *         void GPIO_Write(uint32_t port, uint32_t pin, uint8_t value);
 *         uint8_t GPIO_Read(uint32_t port, uint32_t pin);
 *     #ifdef __cplusplus
 *     }
 *     #endif
 *
 * TR: C++ kodu C sürücüsünü çağırdığında, sınır extern "C" ile işaretlenir.
 *     Bu sayede C++ isim karıştırması (name mangling) engellenir ve
 *     ABI uyumluluğu sağlanır.
 *
 *     // C başlık dosyası (gpio_driver.h)
 *     #ifdef __cplusplus
 *     extern "C" {
 *     #endif
 *         void GPIO_Init(uint32_t port, uint32_t pin, uint32_t mode);
 *         void GPIO_Write(uint32_t port, uint32_t pin, uint8_t value);
 *         uint8_t GPIO_Read(uint32_t port, uint32_t pin);
 *     #ifdef __cplusplus
 *     }
 *     #endif
 *
 * =============================================================================
 * [THEORY: Vendor SDK Patterns / TEORİ: Üretici SDK Kalıpları]
 * =============================================================================
 *
 * EN:
 *     ┌──────────────────┬─────────┬──────────────────────────────┐
 *     │ Vendor / SDK     │ Language│ Pattern                      │
 *     ├──────────────────┼─────────┼──────────────────────────────┤
 *     │ STM32 HAL        │ C       │ HAL_xxx_Init(), callbacks    │
 *     │ Zephyr RTOS      │ C       │ Device tree + driver model   │
 *     │ Android HAL      │ C++     │ HIDL → AIDL interface        │
 *     │ QNX Resource Mgr │ C       │ open/read/write/ioctl        │
 *     │ AUTOSAR MCAL     │ C       │ xxx_Init(), xxx_Read()       │
 *     │ Nordic nRF SDK   │ C       │ nrfx_xxx + app_xxx           │
 *     │ Linux Kernel     │ C       │ file_operations, platform_drv│
 *     └──────────────────┴─────────┴──────────────────────────────┘
 *
 * TR: Satıcı SDK kalıpları:
 *     ┌──────────────────┬─────────┬──────────────────────────────┐
 *     │ Satıcı / SDK     │ Dil     │ Desen                        │
 *     ├──────────────────┼─────────┼──────────────────────────────┤
 *     │ STM32 HAL        │ C       │ HAL_xxx_Init(), callback'ler │
 *     │ Zephyr RTOS      │ C       │ Device tree + sürücü modeli  │
 *     │ Android HAL      │ C++     │ HIDL → AIDL arayüzü          │
 *     │ QNX Resource Mgr │ C       │ open/read/write/ioctl        │
 *     │ AUTOSAR MCAL     │ C       │ xxx_Init(), xxx_Read()       │
 *     │ Nordic nRF SDK   │ C       │ nrfx_xxx + app_xxx           │
 *     │ Linux Kernel     │ C       │ file_operations, platform_drv│
 *     └──────────────────┴─────────┴──────────────────────────────┘
 *
 * @note Compile: g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion 01_driver_hal_bsp.cpp -o 01_driver_hal
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
#include <functional>
#include <optional>
#include <cassert>
#include <cstring>
#include <memory>

// ═════════════════════════════════════════════════════════════════════════════
// PART 1: C-Style Register-Level Driver (simulated)
// ═════════════════════════════════════════════════════════════════════════════

// EN: Simulated hardware registers (in real HW these are memory-mapped)
// TR: Simüle edilmiş donanım register'ları (gerçekte memory-mapped)

namespace c_driver {

// EN: Simulated GPIO register block
// TR: Simüle edilmiş GPIO register bloğu
struct GPIO_RegBlock {
    uint32_t MODER;    // Mode register (input/output/AF/analog) — TR: Mod yazmaçı
    uint32_t ODR;      // Output data register — TR: Çıkış veri yazmaçı
    uint32_t IDR;      // Input data register — TR: Giriş veri yazmaçı
    uint32_t BSRR;     // Bit set/reset register — TR: Bit set/reset yazmaçı
};

// EN: Simulated GPIO peripheral instances
// TR: Simüle edilmiş GPIO çevre birimi örnekleri — gerçek HW'de sabit adreslerdeki register bloklardır
static GPIO_RegBlock GPIOA_REGS = {0, 0, 0, 0};
static GPIO_RegBlock GPIOB_REGS = {0, 0, 0, 0};

// EN: C-style driver API (extern "C" compatible)
// TR: C tarzı sürücü API'si (extern "C" uyumlu)

enum GPIO_Mode : uint32_t { GPIO_MODE_INPUT = 0, GPIO_MODE_OUTPUT = 1, GPIO_MODE_AF = 2, GPIO_MODE_ANALOG = 3 };

inline GPIO_RegBlock* GPIO_GetPort(uint32_t port) {
    switch (port) {
        case 0: return &GPIOA_REGS;
        case 1: return &GPIOB_REGS;
        default: return nullptr;
    }
}

inline int GPIO_Init(uint32_t port, uint32_t pin, GPIO_Mode mode) {
    if (pin > 15) return -1;
    auto* regs = GPIO_GetPort(port);
    if (!regs) return -1;
    uint32_t shift = pin * 2;
    regs->MODER &= ~(0x3u << shift);
    regs->MODER |= (static_cast<uint32_t>(mode) & 0x3u) << shift;
    return 0;
}

inline int GPIO_Write(uint32_t port, uint32_t pin, uint8_t value) {
    if (pin > 15) return -1;
    auto* regs = GPIO_GetPort(port);
    if (!regs) return -1;
    if (value != 0)
        regs->ODR |= (1u << pin);
    else
        regs->ODR &= ~(1u << pin);
    return 0;
}

inline uint8_t GPIO_Read(uint32_t port, uint32_t pin) {
    if (pin > 15) return 0;
    auto* regs = GPIO_GetPort(port);
    if (!regs) return 0;
    return ((regs->IDR >> pin) & 1u) != 0 ? 1 : 0;
}

inline void GPIO_Toggle(uint32_t port, uint32_t pin) {
    if (pin > 15) return;
    auto* regs = GPIO_GetPort(port);
    if (!regs) return;
    regs->ODR ^= (1u << pin);
}

}  // namespace c_driver

// ═════════════════════════════════════════════════════════════════════════════
// PART 2: C++ CRTP HAL Wrapper
// ═════════════════════════════════════════════════════════════════════════════

namespace cpp_hal {

// EN: Pin mode abstraction
// TR: Pin modu soyutlaması
enum class PinMode : uint8_t { INPUT, OUTPUT, AF, ANALOG };

// EN: HAL result type (no exceptions)
// TR: HAL sonuç türü (istisna yok)
enum class HAL_Status : uint8_t { OK, ERROR, INVALID_PARAM };

// EN: CRTP base — provides common interface without virtual dispatch
// TR: CRTP tabanı — virtual dispatch olmadan ortak arayüz sağlar
template <typename Derived>
class GPIOBase {
public:
    HAL_Status Init(uint32_t port, uint32_t pin, PinMode mode) {
        return static_cast<Derived*>(this)->InitImpl(port, pin, mode);
    }
    HAL_Status Write(uint32_t port, uint32_t pin, bool value) {
        return static_cast<Derived*>(this)->WriteImpl(port, pin, value);
    }
    bool Read(uint32_t port, uint32_t pin) {
        return static_cast<Derived*>(this)->ReadImpl(port, pin);
    }
    void Toggle(uint32_t port, uint32_t pin) {
        static_cast<Derived*>(this)->ToggleImpl(port, pin);
    }
};

// EN: STM32-like GPIO implementation (wraps C driver)
// TR: STM32 benzeri GPIO implementasyonu (C sürücüsünü sarar)
class STM32_GPIO : public GPIOBase<STM32_GPIO> {
public:
    HAL_Status InitImpl(uint32_t port, uint32_t pin, PinMode mode) {
        auto c_mode = static_cast<c_driver::GPIO_Mode>(static_cast<uint8_t>(mode));
        int ret = c_driver::GPIO_Init(port, pin, c_mode);
        return ret == 0 ? HAL_Status::OK : HAL_Status::INVALID_PARAM;
    }

    HAL_Status WriteImpl(uint32_t port, uint32_t pin, bool value) {
        int ret = c_driver::GPIO_Write(port, pin, value ? 1 : 0);
        return ret == 0 ? HAL_Status::OK : HAL_Status::INVALID_PARAM;
    }

    bool ReadImpl(uint32_t port, uint32_t pin) {
        return c_driver::GPIO_Read(port, pin) != 0;
    }

    void ToggleImpl(uint32_t port, uint32_t pin) {
        c_driver::GPIO_Toggle(port, pin);
    }
};

// EN: NXP-like GPIO implementation (different register layout, same HAL interface)
// TR: NXP benzeri GPIO implementasyonu (farklı register düzeni, aynı HAL arayüzü)
class NXP_GPIO : public GPIOBase<NXP_GPIO> {
    // EN: NXP uses different register names but same concept
    // TR: NXP farklı register isimleri kullanır ama aynı konsept (PDDR=yön, PDOR=çıkış, PDIR=giriş)
    struct NXP_Regs {
        uint32_t PDDR;  // Port Data Direction Register — TR: Port yön yazmaçı
        uint32_t PDOR;  // Port Data Output Register — TR: Port çıkış yazmaçı
        uint32_t PDIR;  // Port Data Input Register — TR: Port giriş yazmaçı
    };
    std::array<NXP_Regs, 2> ports_{};

public:
    HAL_Status InitImpl(uint32_t port, uint32_t pin, PinMode mode) {
        if (port > 1 || pin > 31) return HAL_Status::INVALID_PARAM;
        if (mode == PinMode::OUTPUT)
            ports_[port].PDDR |= (1u << pin);
        else
            ports_[port].PDDR &= ~(1u << pin);
        return HAL_Status::OK;
    }

    HAL_Status WriteImpl(uint32_t port, uint32_t pin, bool value) {
        if (port > 1 || pin > 31) return HAL_Status::INVALID_PARAM;
        if (value)
            ports_[port].PDOR |= (1u << pin);
        else
            ports_[port].PDOR &= ~(1u << pin);
        return HAL_Status::OK;
    }

    bool ReadImpl(uint32_t port, uint32_t pin) {
        if (port > 1 || pin > 31) return false;
        // EN: For output pins, PDIR reflects PDOR (simulated loopback)
        // TR: Çıkış pinlerinde PDIR, PDOR'u yansıtır (simüle edilmiş geri dönüş)
        ports_[port].PDIR = ports_[port].PDOR;
        return (ports_[port].PDIR >> pin) & 1u;
    }

    void ToggleImpl(uint32_t port, uint32_t pin) {
        if (port > 1 || pin > 31) return;
        ports_[port].PDOR ^= (1u << pin);
    }
};

// EN: Generic function that works with any GPIO implementation (compile-time polymorphism)
// TR: Herhangi bir GPIO implementasyonuyla çalışan genel fonksiyon (derleme zamanı polimorfizm)
template <typename GPIODriver>
void BlinkLED(GPIOBase<GPIODriver>& gpio, uint32_t port, uint32_t pin, int count) {
    gpio.Init(port, pin, PinMode::OUTPUT);
    for (int i = 0; i < count; ++i) {
        gpio.Toggle(port, pin);
        auto state = gpio.Read(port, pin);
        std::cout << "    Blink " << (i + 1) << "/" << count << " → LED "
                  << (state ? "ON" : "OFF") << "\n";
    }
}

}  // namespace cpp_hal

// ═════════════════════════════════════════════════════════════════════════════
// PART 3: BSP — Board Support Package Pattern
// ═════════════════════════════════════════════════════════════════════════════

namespace bsp {

// EN: Board configuration (compile-time via constexpr)
// TR: Board konfigürasyonu (constexpr ile derleme zamanı)
struct PinConfig {
    uint32_t port;
    uint32_t pin;
    cpp_hal::PinMode mode;
    std::string_view label;
};

// EN: Board: "STM32F4 Discovery" pin mapping
// TR: "STM32F4 Discovery" board pin haritası
constexpr std::array<PinConfig, 6> DISCOVERY_BOARD_PINS = {{
    {3, 12, cpp_hal::PinMode::OUTPUT, "LED_GREEN"},
    {3, 13, cpp_hal::PinMode::OUTPUT, "LED_ORANGE"},
    {3, 14, cpp_hal::PinMode::OUTPUT, "LED_RED"},
    {3, 15, cpp_hal::PinMode::OUTPUT, "LED_BLUE"},
    {0,  0, cpp_hal::PinMode::INPUT,  "USER_BUTTON"},
    {0,  5, cpp_hal::PinMode::AF,     "SPI1_SCK"},
}};

// EN: BSP init function — configures all board pins
// TR: BSP başlatma fonksiyonu — tüm board pinlerini yapılandırır
template <typename GPIODriver, size_t N>
void BSP_Init(cpp_hal::GPIOBase<GPIODriver>& gpio,
              const std::array<PinConfig, N>& config) {
    std::cout << "  [BSP] Initializing " << N << " pins:\n";
    for (const auto& pin : config) {
        auto status = gpio.Init(pin.port, pin.pin, pin.mode);
        std::cout << "    " << pin.label << " (P" << pin.port
                  << "." << pin.pin << ") → "
                  << (status == cpp_hal::HAL_Status::OK ? "OK" : "ERROR") << "\n";
    }
}

}  // namespace bsp

// ═════════════════════════════════════════════════════════════════════════════
// PART 4: Linux Device Driver Model (Conceptual)
// ═════════════════════════════════════════════════════════════════════════════

namespace linux_driver {

// EN: Simulated Linux char device file_operations
// TR: Simüle edilmiş Linux karakter cihaz dosya_operasyonları
struct FileOperations {
    std::function<int()>                                   open;
    std::function<int()>                                   release;
    std::function<int(uint8_t* buf, size_t count)>         read;
    std::function<int(const uint8_t* buf, size_t count)>   write;
    std::function<int(uint32_t cmd, uint64_t arg)>         ioctl;
};

// EN: Example: Temperature sensor char device
// TR: Örnek: Sıcaklık sensörü karakter cihazı
class TempSensorDevice {
    bool    opened_ = false;
    int32_t temperature_ = 2500;  // 25.00 °C in centi-degrees — TR: santi-derece cinsinden

public:
    FileOperations GetOps() {
        FileOperations ops;
        ops.open = [this]() {
            opened_ = true;
            std::cout << "    [drv] /dev/temp0 opened\n";
            return 0;
        };
        ops.release = [this]() {
            opened_ = false;
            std::cout << "    [drv] /dev/temp0 closed\n";
            return 0;
        };
        ops.read = [this](uint8_t* buf, size_t count) -> int {
            if (!opened_) return -1;
            if (count < sizeof(temperature_)) return -1;
            std::memcpy(buf, &temperature_, sizeof(temperature_));
            return static_cast<int>(sizeof(temperature_));
        };
        ops.write = [](const uint8_t* /*buf*/, size_t /*count*/) -> int {
            return -1;  // read-only device
        };
        ops.ioctl = [this](uint32_t cmd, uint64_t arg) -> int {
            if (cmd == 0x01) {  // SET_CALIBRATION
                temperature_ += static_cast<int32_t>(arg);
                std::cout << "    [drv] Calibration applied: temp="
                          << temperature_ << " centi-°C\n";
                return 0;
            }
            return -1;
        };
        return ops;
    }
};

// EN: Userspace C++ wrapper (RAII for open/close)
// TR: Kullanıcı alanı C++ sarmalayıcı (open/close için RAII)
class DeviceFile {
    FileOperations ops_;
    bool opened_ = false;

public:
    explicit DeviceFile(FileOperations ops) : ops_(std::move(ops)) {}
    ~DeviceFile() { if (opened_) Close(); }

    DeviceFile(const DeviceFile&) = delete;
    DeviceFile& operator=(const DeviceFile&) = delete;

    int Open() {
        int ret = ops_.open();
        if (ret == 0) opened_ = true;
        return ret;
    }
    int Close() {
        if (!opened_) return -1;
        int ret = ops_.release();
        opened_ = false;
        return ret;
    }
    int Read(uint8_t* buf, size_t count) {
        return ops_.read(buf, count);
    }
    int Ioctl(uint32_t cmd, uint64_t arg) {
        return ops_.ioctl(cmd, arg);
    }
};

}  // namespace linux_driver

// ═════════════════════════════════════════════════════════════════════════════
// PART 5: extern "C" Boundary Demonstration
// ═════════════════════════════════════════════════════════════════════════════

namespace extern_c_demo {

// EN: Simulated C API (in real code, these would be in a .h with extern "C")
// TR: Simüle edilmiş C API'si (gerçekte .h dosyasında extern "C" ile olur)

// Functions declared as if they were C functions
extern "C" {
    // EN: These represent the C driver functions
    // TR: Bunlar C sürücü fonksiyonlarını temsil eder — extern "C" ile C bağlantı uyumu sağlanır
    inline int UART_Init(uint32_t baudrate) {
        std::cout << "    [C-DRV] UART initialized at " << baudrate << " baud\n";
        return 0;
    }
    inline int UART_Transmit(const uint8_t* data, uint32_t len) {
        std::cout << "    [C-DRV] UART TX " << len << " bytes: ";
        for (uint32_t i = 0; i < len && i < 16; ++i)
            std::cout << std::hex << static_cast<int>(data[i]) << " ";
        std::cout << std::dec << "\n";
        return static_cast<int>(len);
    }
    inline int UART_Receive(uint8_t* data, uint32_t max_len) {
        // EN: Simulate receiving "OK\r\n"
        // TR: "OK\r\n" alma simülasyonu — gerçek HW'de UART RX FIFO'dan okunur
        const uint8_t mock[] = {'O', 'K', '\r', '\n'};
        auto copy_len = std::min(max_len, static_cast<uint32_t>(sizeof(mock)));
        std::memcpy(data, mock, copy_len);
        return static_cast<int>(copy_len);
    }
}

// EN: C++ RAII wrapper for the C UART driver
// TR: C UART sürücüsü için C++ RAII sarmalayıcı
class UARTPort {
    uint32_t baudrate_;
    bool     initialized_ = false;

public:
    explicit UARTPort(uint32_t baud) : baudrate_(baud) {
        if (UART_Init(baudrate_) == 0)
            initialized_ = true;
    }

    [[nodiscard]] bool IsReady() const { return initialized_; }

    int Send(std::string_view msg) {
        if (!initialized_) return -1;
        return UART_Transmit(reinterpret_cast<const uint8_t*>(msg.data()),
                              static_cast<uint32_t>(msg.size()));
    }

    std::optional<std::string> Receive(uint32_t max_len = 64) {
        if (!initialized_) return std::nullopt;
        std::vector<uint8_t> buf(max_len);
        int ret = UART_Receive(buf.data(), max_len);
        if (ret <= 0) return std::nullopt;
        return std::string(buf.begin(), buf.begin() + ret);
    }
};

}  // namespace extern_c_demo

// ═════════════════════════════════════════════════════════════════════════════
// MAIN — Demos
// ═════════════════════════════════════════════════════════════════════════════

int main() {
    std::cout << "╔══════════════════════════════════════════════════════════════╗\n"
              << "║ Module 18: Driver / HAL / BSP — C vs C++ Layers              ║\n"
              << "╚══════════════════════════════════════════════════════════════╝\n\n";

    // --- Demo 1: C-Style Register Driver ---
    std::cout << "--- Demo 1: C-Style GPIO Register Driver ---\n";
    {
        c_driver::GPIO_Init(0, 5, c_driver::GPIO_MODE_OUTPUT);
        c_driver::GPIO_Write(0, 5, 1);
        auto val = c_driver::GPIO_Read(0, 5);
        std::cout << "  GPIOA.5: mode=OUTPUT, write=1, read=" << static_cast<int>(val) << "\n";
        // EN: Read back from ODR (simulated IDR = ODR in our simulation)
        // TR: ODR'den geri oku (simülasyonda IDR = ODR) — gerçek HW'de pin durumu IDR'den okunur
        c_driver::GPIOA_REGS.IDR = c_driver::GPIOA_REGS.ODR;
        val = c_driver::GPIO_Read(0, 5);
        std::cout << "  After IDR sync: read=" << static_cast<int>(val) << "\n";

        c_driver::GPIO_Toggle(0, 5);
        c_driver::GPIOA_REGS.IDR = c_driver::GPIOA_REGS.ODR;
        val = c_driver::GPIO_Read(0, 5);
        std::cout << "  After toggle: read=" << static_cast<int>(val) << "\n";

        std::cout << "  GPIOA.MODER = 0x" << std::hex << c_driver::GPIOA_REGS.MODER
                  << "  ODR = 0x" << c_driver::GPIOA_REGS.ODR << std::dec << "\n";
    }

    // --- Demo 2: C++ CRTP HAL Wrapper (STM32) ---
    std::cout << "\n--- Demo 2: C++ CRTP HAL — STM32 GPIO ---\n";
    {
        cpp_hal::STM32_GPIO stm_gpio;
        c_driver::GPIOA_REGS = {0, 0, 0, 0};  // reset
        // EN: Sync IDR with ODR for toggling to be readable
        // TR: Toggle okuması için IDR'yi ODR ile senkronize et — simülasyon gereği
        auto sync = [](){ c_driver::GPIOA_REGS.IDR = c_driver::GPIOA_REGS.ODR; };
        stm_gpio.Init(0, 0, cpp_hal::PinMode::OUTPUT);
        stm_gpio.Write(0, 0, true);
        sync();
        std::cout << "  STM32 PA0 = " << stm_gpio.Read(0, 0) << "\n";
        stm_gpio.Toggle(0, 0);
        sync();
        std::cout << "  After toggle = " << stm_gpio.Read(0, 0) << "\n";
    }

    // --- Demo 3: C++ CRTP HAL — NXP + Generic BlinkLED ---
    std::cout << "\n--- Demo 3: C++ CRTP HAL — NXP GPIO + Generic BlinkLED ---\n";
    {
        cpp_hal::NXP_GPIO nxp_gpio;
        std::cout << "  NXP BlinkLED:\n";
        cpp_hal::BlinkLED(nxp_gpio, 0, 3, 4);
    }

    // --- Demo 4: BSP Board Init ---
    std::cout << "\n--- Demo 4: BSP (Board Support Package) Init ---\n";
    {
        // EN: Using STM32 GPIO with Discovery board config
        // TR: STM32 GPIO'yu Discovery board yapılandırması ile kullan
        // Note: pins with port > 1 are beyond our 2-port simulation, shown for concept
        cpp_hal::STM32_GPIO gpio;
        struct SimplePinConfig {
            uint32_t port; uint32_t pin; cpp_hal::PinMode mode; std::string_view label;
        };
        SimplePinConfig board_pins[] = {
            {0, 12, cpp_hal::PinMode::OUTPUT, "LED_GREEN"},
            {0, 13, cpp_hal::PinMode::OUTPUT, "LED_ORANGE"},
            {0, 14, cpp_hal::PinMode::OUTPUT, "LED_RED"},
            {0,  0, cpp_hal::PinMode::INPUT,  "USER_BUTTON"},
        };
        std::cout << "  [BSP] Board pin initialization:\n";
        for (const auto& pin : board_pins) {
            auto status = gpio.Init(pin.port, pin.pin, pin.mode);
            std::cout << "    " << pin.label << " (P" << pin.port << "." << pin.pin
                      << ") → " << (status == cpp_hal::HAL_Status::OK ? "OK" : "ERROR") << "\n";
        }
    }

    // --- Demo 5: Linux Device Driver Model ---
    std::cout << "\n--- Demo 5: Linux Char Device Model (Simulated) ---\n";
    {
        linux_driver::TempSensorDevice sensor;
        {
            linux_driver::DeviceFile dev(sensor.GetOps());
            dev.Open();

            int32_t temp = 0;
            int ret = dev.Read(reinterpret_cast<uint8_t*>(&temp), sizeof(temp));
            std::cout << "    Read: " << ret << " bytes, temp = "
                      << (temp / 100) << "." << (temp % 100) << " °C\n";

            dev.Ioctl(0x01, 150);  // calibrate +1.50°C
            ret = dev.Read(reinterpret_cast<uint8_t*>(&temp), sizeof(temp));
            std::cout << "    After calibration: temp = "
                      << (temp / 100) << "." << (temp % 100) << " °C\n";
            // RAII: ~DeviceFile closes the device
        }
        std::cout << "    (device auto-closed via RAII)\n";
    }

    // --- Demo 6: extern "C" — C Driver + C++ Wrapper ---
    std::cout << "\n--- Demo 6: extern \"C\" — UART C Driver + C++ Wrapper ---\n";
    {
        extern_c_demo::UARTPort uart(115200);
        std::cout << "  Ready: " << uart.IsReady() << "\n";

        uart.Send("AT+GMR\r\n");

        auto response = uart.Receive();
        if (response)
            std::cout << "    Received: \"" << *response << "\"\n";
    }

    // --- Demo 7: Layer Comparison Summary ---
    std::cout << "\n--- Demo 7: Driver Layer Summary ---\n";
    {
        struct LayerInfo { std::string_view layer; std::string_view lang; std::string_view example; };
        LayerInfo layers[] = {
            {"Application",   "C++",     "AUTOSAR SWC, Qt HMI, business logic"},
            {"Framework",     "C++",     "SOME/IP middleware, Android AIDL"},
            {"HAL",           "C/C++",   "CRTP GPIO, STM32 HAL wrapper"},
            {"Driver",        "C",       "Register-level GPIO, UART, SPI"},
            {"BSP",           "C/ASM",   "Clock config, pin mux, startup.s"},
            {"Hardware",      "Verilog", "MCU silicon, PCB, sensors"},
        };
        std::cout << "  ┌───────────────┬─────────┬─────────────────────────────────────────┐\n";
        std::cout << "  │ Layer         │ Lang    │ Example                                 │\n";
        std::cout << "  ├───────────────┼─────────┼─────────────────────────────────────────┤\n";
        for (const auto& l : layers) {
            std::cout << "  │ " << std::setw(13) << std::left << l.layer
                      << " │ " << std::setw(7) << l.lang
                      << " │ " << std::setw(39) << l.example << " │\n";
        }
        std::cout << std::right;
        std::cout << "  └───────────────┴─────────┴─────────────────────────────────────────┘\n";
    }

    std::cout << "\n[All demos completed successfully.]\n";
    return 0;
}
