/**
 * @file module_15_hw_interfaces/03_serial_protocols_deep.cpp
 * @brief SPI/I2C/UART
 *
 * @details
 * =============================================================================
 * [THEORY: Serial Protocol Register-Level Programming]
 * =============================================================================
 *
 * EN: This file goes deeper than module_14's overview. Here we focus on:
 *     - Register-level peripheral initialization
 *     - Error detection and recovery
 *     - DMA integration patterns
 *     - Multi-master/multi-slave topologies
 *     - Real hardware register maps (STM32-like)
 *
 *     SPI Register Map (STM32):
 *     ┌─────────┬────────┬────────────────────────────────────────┐
 *     │ Offset  │ Name   │ Key Fields                             │
 *     ├─────────┼────────┼────────────────────────────────────────┤
 *     │ 0x00    │ CR1    │ BIDIMODE, BIDIOE, CRCEN, CRCNEXT,      │
 *     │         │        │ DFF, RXONLY, SSM, SSI, LSBFIRST,       │
 *     │         │        │ SPE, BR[2:0], MSTR, CPOL, CPHA         │
 *     │ 0x04    │ CR2    │ TXEIE, RXNEIE, ERRIE, SSOE,            │
 *     │         │        │ TXDMAEN, RXDMAEN                       │
 *     │ 0x08    │ SR     │ BSY, OVR, MODF, CRCERR, UDR,           │
 *     │         │        │ TXE, RXNE                              │
 *     │ 0x0C    │ DR     │ Data register (8/16-bit)               │
 *     │ 0x14    │ CRCPR  │ CRC polynomial register                │
 *     └─────────┴────────┴────────────────────────────────────────┘
 *
 *     I2C Register Map (STM32):
 *     ┌─────────┬────────┬────────────────────────────────────────┐
 *     │ Offset  │ Name   │ Key Fields                             │
 *     ├─────────┼────────┼────────────────────────────────────────┤
 *     │ 0x00    │ CR1    │ SWRST, ALERT, PEC, POS, ACK, STOP,     │
 *     │         │        │ START, NOSTRETCH, ENGC, PE             │
 *     │ 0x04    │ CR2    │ LAST, DMAEN, ITBUFEN, ITEVTEN,         │
 *     │         │        │ ITERREN, FREQ[5:0]                     │
 *     │ 0x08    │ OAR1   │ ADDMODE, ADD[9:0]                      │
 *     │ 0x14    │ SR1    │ SB, ADDR, BTF, ADD10, STOPF,           │
 *     │         │        │ RXNE, TXE, BERR, ARLO, AF, OVR         │
 *     │ 0x18    │ SR2    │ DUALF, SMBHOST, SMBDEFAULT, GENCALL,   │
 *     │         │        │ TRA, BUSY, MSL                         │
 *     │ 0x1C    │ CCR    │ F/S, DUTY, CCR[11:0]                   │
 *     └─────────┴────────┴────────────────────────────────────────┘
 *
 *     UART Register Map (STM32):
 *     ┌─────────┬────────┬────────────────────────────────────────┐
 *     │ Offset  │ Name   │ Key Fields                             │
 *     ├─────────┼────────┼────────────────────────────────────────┤
 *     │ 0x00    │ SR     │ CTS, LBD, TXE, TC, RXNE, IDLE, ORE,    │
 *     │         │        │ NF, FE, PE                             │
 *     │ 0x04    │ DR     │ Data register (9-bit)                  │
 *     │ 0x08    │ BRR    │ DIV_Mantissa[11:0], DIV_Fraction[3:0]  │
 *     │ 0x0C    │ CR1    │ UE, M, WAKE, PCE, PS, PEIE, TXEIE,     │
 *     │         │        │ TCIE, RXNEIE, IDLEIE, TE, RE           │
 *     │ 0x10    │ CR2    │ STOP[1:0], CLKEN, CPOL, CPHA, LBCL     │
 *     │ 0x14    │ CR3    │ DMAT, DMAR, SCEN, NACK, HDSEL,         │
 *     │         │        │ IRLP, IREN, EIE                        │
 *     └─────────┴────────┴────────────────────────────────────────┘
 *
 * @note Compile: g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion 03_serial_protocols_deep.cpp -o 03_serial_deep
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <iostream>
#include <cstdint>
#include <array>
#include <vector>
#include <string>
#include <iomanip>
#include <bitset>
#include <queue>
#include <functional>
#include <optional>
#include <map>
#include <cassert>

// ═════════════════════════════════════════════════════════════════════════════
// PART 1: SPI Register-Level Simulation
// ═════════════════════════════════════════════════════════════════════════════

namespace spi_regs {
    // CR1 bit positions
    constexpr uint32_t CPHA      = 0;
    constexpr uint32_t CPOL      = 1;
    constexpr uint32_t MSTR      = 2;
    constexpr uint32_t BR_SHIFT  = 3;  // BR[2:0]
    constexpr uint32_t BR_MASK   = 0x7;
    constexpr uint32_t SPE       = 6;
    constexpr uint32_t LSBFIRST  = 7;
    constexpr uint32_t SSI       = 8;
    constexpr uint32_t SSM       = 9;
    constexpr uint32_t DFF       = 11; // 0=8-bit, 1=16-bit

    // SR bit positions
    constexpr uint32_t RXNE      = 0;
    constexpr uint32_t TXE       = 1;
    constexpr uint32_t BSY       = 7;
    constexpr uint32_t OVR       = 6;
    constexpr uint32_t MODF      = 5;

    // CR2 bit positions
    constexpr uint32_t RXDMAEN   = 0;
    constexpr uint32_t TXDMAEN   = 1;
    constexpr uint32_t RXNEIE    = 6;
    constexpr uint32_t TXEIE     = 7;
}

class SPIPeripheral {
    uint32_t CR1_ = 0;
    uint32_t CR2_ = 0;
    uint32_t SR_  = (1U << spi_regs::TXE);  // TXE = 1 at reset
    uint32_t DR_  = 0;

    std::queue<uint8_t> rx_fifo_;
    std::function<uint8_t(uint8_t)> slave_handler_;

public:
    // EN: Register write
    void write_CR1(uint32_t val) { CR1_ = val; }
    void write_CR2(uint32_t val) { CR2_ = val; }

    uint32_t read_CR1() const { return CR1_; }
    uint32_t read_SR()  const { return SR_; }

    void set_slave_handler(std::function<uint8_t(uint8_t)> h) {
        slave_handler_ = std::move(h);
    }

    // EN: Write data register → initiates transfer
    void write_DR(uint8_t data) {
        if (!(CR1_ & (1U << spi_regs::SPE))) return;  // SPI not enabled

        SR_ &= ~(1U << spi_regs::TXE);  // TXE = 0 during transfer
        SR_ |= (1U << spi_regs::BSY);     // BSY = 1

        // EN: Simulate full-duplex transfer
        uint8_t rx = slave_handler_ ? slave_handler_(data) : 0xFF;

        rx_fifo_.push(rx);
        SR_ |= (1U << spi_regs::RXNE);   // RXNE = 1
        SR_ |= (1U << spi_regs::TXE);     // TXE = 1 (ready for next)
        SR_ &= ~(1U << spi_regs::BSY);    // BSY = 0
    }

    // EN: Read data register
    uint8_t read_DR() {
        if (rx_fifo_.empty()) return 0;
        uint8_t data = rx_fifo_.front();
        rx_fifo_.pop();
        if (rx_fifo_.empty())
            SR_ &= ~(1U << spi_regs::RXNE);  // RXNE = 0
        return data;
    }

    bool is_tx_empty() const { return SR_ & (1U << spi_regs::TXE); }
    bool is_rx_ready() const { return SR_ & (1U << spi_regs::RXNE); }
    bool is_busy() const { return SR_ & (1U << spi_regs::BSY); }

    void print_config() const {
        bool cpol = CR1_ & (1U << spi_regs::CPOL);
        bool cpha = CR1_ & (1U << spi_regs::CPHA);
        bool master = CR1_ & (1U << spi_regs::MSTR);
        bool enabled = CR1_ & (1U << spi_regs::SPE);
        uint32_t br = (CR1_ >> spi_regs::BR_SHIFT) & spi_regs::BR_MASK;
        bool lsb = CR1_ & (1U << spi_regs::LSBFIRST);
        bool dff16 = CR1_ & (1U << spi_regs::DFF);

        int divisors[] = {2, 4, 8, 16, 32, 64, 128, 256};

        std::cout << "    SPI Configuration:\n"
                  << "      Mode:     " << (master ? "MASTER" : "SLAVE") << "\n"
                  << "      CPOL:     " << cpol << " (idle " << (cpol ? "HIGH" : "LOW") << ")\n"
                  << "      CPHA:     " << cpha << " (sample on " << (cpha ? "2nd" : "1st") << " edge)\n"
                  << "      SPI Mode: " << (cpol * 2 + cpha) << "\n"
                  << "      Baud div: f_PCLK/" << divisors[br] << "\n"
                  << "      Bit order:" << (lsb ? " LSB first" : " MSB first") << "\n"
                  << "      Frame:    " << (dff16 ? "16" : "8") << "-bit\n"
                  << "      Enabled:  " << (enabled ? "YES" : "NO") << "\n";
    }
};

// ═════════════════════════════════════════════════════════════════════════════
// PART 2: I2C Register-Level Simulation
// ═════════════════════════════════════════════════════════════════════════════

namespace i2c_regs {
    // CR1 — EN: Control Register 1
    constexpr uint32_t PE       = 0;
    constexpr uint32_t START    = 8;
    constexpr uint32_t STOP     = 9;
    constexpr uint32_t ACK      = 10;
    // SR1 — EN: Status Register 1
    constexpr uint32_t SB       = 0;   // Start bit generated
    constexpr uint32_t ADDR     = 1;   // Address sent
    constexpr uint32_t BTF      = 2;   // Byte transfer finished
    constexpr uint32_t TXE      = 7;   // TX empty
    constexpr uint32_t RXNE     = 6;   // RX not empty
    constexpr uint32_t AF       = 10;  // Acknowledge failure
    constexpr uint32_t ARLO     = 9;   // Arbitration lost
    constexpr uint32_t BERR     = 8;   // Bus error
}

class I2CPeripheral {
    uint32_t CR1_ = 0;
    uint32_t CR2_ = 0;
    uint32_t SR1_ = 0;
    uint32_t SR2_ = 0;
    uint32_t CCR_ = 0;

    // EN: Simulated slave devices on the bus
    struct I2CSlave {
        uint8_t address;   // 7-bit
        std::map<uint8_t, uint8_t> registers;
    };
    std::vector<I2CSlave> slaves_;

    uint8_t current_addr_ = 0;
    bool write_mode_ = true;
    int current_slave_ = -1;
    uint8_t current_reg_ = 0;
    int byte_count_ = 0;

public:
    void add_slave(uint8_t addr, std::map<uint8_t, uint8_t> regs) {
        slaves_.push_back({addr, std::move(regs)});
    }

    // EN: Enable peripheral
    void enable() { CR1_ |= (1U << i2c_regs::PE); }

    // EN: Generate START condition
    bool start() {
        if (!(CR1_ & (1U << i2c_regs::PE))) return false;
        CR1_ |= (1U << i2c_regs::START);
        SR1_ |= (1U << i2c_regs::SB);
        byte_count_ = 0;
        return true;
    }

    // EN: Send address byte (7-bit addr + R/W)
    bool send_address(uint8_t addr_rw) {
        current_addr_ = static_cast<uint8_t>(addr_rw >> 1);
        write_mode_ = !(addr_rw & 1);

        current_slave_ = -1;
        for (size_t i = 0; i < slaves_.size(); ++i) {
            if (slaves_[i].address == current_addr_) {
                current_slave_ = static_cast<int>(i);
                break;
            }
        }

        SR1_ &= ~(1U << i2c_regs::SB);  // clear SB
        if (current_slave_ >= 0) {
            SR1_ |= (1U << i2c_regs::ADDR);  // address matched
            SR1_ |= (1U << i2c_regs::TXE);
            return true;
        } else {
            SR1_ |= (1U << i2c_regs::AF);  // NACK
            return false;
        }
    }

    // EN: Write data byte
    bool write_data(uint8_t data) {
        if (current_slave_ < 0) return false;

        if (byte_count_ == 0) {
            current_reg_ = data;  // first byte = register address
        } else {
            slaves_[static_cast<size_t>(current_slave_)].registers[current_reg_] = data;
            current_reg_++;
        }
        byte_count_++;
        SR1_ |= (1U << i2c_regs::BTF);
        SR1_ |= (1U << i2c_regs::TXE);
        return true;
    }

    // EN: Read data byte
    std::optional<uint8_t> read_data() {
        if (current_slave_ < 0) return std::nullopt;

        auto& slave = slaves_[static_cast<size_t>(current_slave_)];
        auto it = slave.registers.find(current_reg_);
        uint8_t val = (it != slave.registers.end()) ? it->second : 0xFF;
        current_reg_++;
        SR1_ |= (1U << i2c_regs::RXNE);
        SR1_ |= (1U << i2c_regs::BTF);
        return val;
    }

    // EN: Generate STOP condition
    void stop() {
        CR1_ |= (1U << i2c_regs::STOP);
        SR1_ = 0;
        current_slave_ = -1;
        byte_count_ = 0;
    }

    // EN: Error detection
    struct I2CError {
        bool nack = false;
        bool arbitration_lost = false;
        bool bus_error = false;
    };

    I2CError check_errors() const {
        return {
            (SR1_ & (1U << i2c_regs::AF)) != 0,
            (SR1_ & (1U << i2c_regs::ARLO)) != 0,
            (SR1_ & (1U << i2c_regs::BERR)) != 0,
        };
    }

    void set_clock(uint32_t pclk_mhz, uint32_t target_khz) {
        CR2_ = pclk_mhz & 0x3F;
        // EN: CCR calculation for standard mode
        // CCR = f_PCLK
        uint32_t ccr = (pclk_mhz * 1000) / (2 * target_khz);
        CCR_ = ccr & 0xFFF;
    }

    void print_config() const {
        std::cout << "    I2C Configuration:\n"
                  << "      Enabled: " << ((CR1_ & (1U << i2c_regs::PE)) ? "YES" : "NO") << "\n"
                  << "      PCLK:    " << (CR2_ & 0x3F) << " MHz\n"
                  << "      CCR:     " << (CCR_ & 0xFFF) << "\n"
                  << "      Slaves:  " << slaves_.size() << " on bus\n";
    }
};

// ═════════════════════════════════════════════════════════════════════════════
// PART 3: UART Register-Level Simulation
// ═════════════════════════════════════════════════════════════════════════════

namespace uart_regs {
    // SR — EN: Status Register
    constexpr uint32_t PE_FLAG  = 0;   // Parity error
    constexpr uint32_t FE       = 1;   // Framing error
    constexpr uint32_t NF       = 2;   // Noise flag
    constexpr uint32_t ORE      = 3;   // Overrun error
    constexpr uint32_t IDLE     = 4;
    constexpr uint32_t RXNE     = 5;
    constexpr uint32_t TC       = 6;   // Transmission complete
    constexpr uint32_t TXE      = 7;   // Transmit data register empty
    // CR1 — EN: Control Register 1
    constexpr uint32_t UE       = 13;  // USART enable
    constexpr uint32_t TE       = 3;   // Transmitter enable
    constexpr uint32_t RE       = 2;   // Receiver enable
    constexpr uint32_t RXNEIE   = 5;   // RXNE interrupt enable
    constexpr uint32_t TXEIE    = 7;   // TXE interrupt enable
    constexpr uint32_t PCE      = 10;  // Parity control enable
    constexpr uint32_t M        = 12;  // Word length (0=8bit, 1=9bit)
}

class UARTPeripheral {
    uint32_t SR_  = (1U << uart_regs::TXE) | (1U << uart_regs::TC);
    uint32_t DR_  = 0;
    uint32_t BRR_ = 0;
    uint32_t CR1_ = 0;
    uint32_t CR2_ = 0;
    uint32_t CR3_ = 0;

    std::queue<uint8_t> tx_buffer_;
    std::queue<uint8_t> rx_buffer_;

    UARTPeripheral* connected_ = nullptr;

public:
    void connect(UARTPeripheral& other) {
        connected_ = &other;
        other.connected_ = this;
    }

    // EN: Configure baud rate
    // BRR = f_PCLK
    void set_baud(uint32_t pclk_hz, uint32_t baud) {
        uint32_t div = pclk_hz / baud;
        uint32_t mantissa = div / 16;
        uint32_t fraction = div % 16;
        BRR_ = (mantissa << 4) | (fraction & 0xF);
    }

    void enable(bool tx, bool rx) {
        CR1_ |= (1U << uart_regs::UE);
        if (tx) CR1_ |= (1U << uart_regs::TE);
        if (rx) CR1_ |= (1U << uart_regs::RE);
    }

    // EN: Transmit byte
    bool transmit(uint8_t data) {
        if (!(CR1_ & (1U << uart_regs::UE))) return false;
        if (!(CR1_ & (1U << uart_regs::TE))) return false;

        // Wait for TXE
        if (!(SR_ & (1U << uart_regs::TXE))) return false;

        DR_ = data;
        SR_ &= ~(1U << uart_regs::TXE);
        SR_ &= ~(1U << uart_regs::TC);

        // EN: Simulate transmission → arrives at connected UART
        if (connected_) {
            connected_->receive_byte(data);
        }

        SR_ |= (1U << uart_regs::TXE);
        SR_ |= (1U << uart_regs::TC);
        return true;
    }

    // EN: Receive byte (called internally)
    void receive_byte(uint8_t data) {
        rx_buffer_.push(data);
        SR_ |= (1U << uart_regs::RXNE);
    }

    // EN: Read received byte
    std::optional<uint8_t> read() {
        if (rx_buffer_.empty()) return std::nullopt;
        uint8_t data = rx_buffer_.front();
        rx_buffer_.pop();
        if (rx_buffer_.empty())
            SR_ &= ~(1U << uart_regs::RXNE);
        return data;
    }

    bool is_rx_ready() const { return SR_ & (1U << uart_regs::RXNE); }
    bool is_tx_empty() const { return SR_ & (1U << uart_regs::TXE); }

    struct UARTError {
        bool parity  = false;
        bool framing = false;
        bool noise   = false;
        bool overrun = false;
    };

    UARTError check_errors() const {
        return {
            (SR_ & (1U << uart_regs::PE_FLAG)) != 0,
            (SR_ & (1U << uart_regs::FE)) != 0,
            (SR_ & (1U << uart_regs::NF)) != 0,
            (SR_ & (1U << uart_regs::ORE)) != 0,
        };
    }

    // EN: Simulate overrun error (new data when RXNE still set)
    void simulate_overrun() {
        SR_ |= (1U << uart_regs::ORE);
    }

    void clear_errors() {
        SR_ &= ~((1U << uart_regs::PE_FLAG) | (1U << uart_regs::FE) |
                  (1U << uart_regs::NF) | (1U << uart_regs::ORE));
    }

    void print_config() const {
        uint32_t mantissa = BRR_ >> 4;
        uint32_t fraction = BRR_ & 0xF;
        bool enabled = CR1_ & (1U << uart_regs::UE);
        bool tx_en = CR1_ & (1U << uart_regs::TE);
        bool rx_en = CR1_ & (1U << uart_regs::RE);

        std::cout << "    UART Configuration:\n"
                  << "      Enabled: " << (enabled ? "YES" : "NO") << "\n"
                  << "      TX:      " << (tx_en ? "ON" : "OFF") << "\n"
                  << "      RX:      " << (rx_en ? "ON" : "OFF") << "\n"
                  << "      BRR:     " << mantissa << "." << fraction
                  << " (mantissa.fraction)\n";
    }
};

// ═════════════════════════════════════════════════════════════════════════════
// PART 4: DMA Channel Simulation
// ═════════════════════════════════════════════════════════════════════════════

// EN: Simulates DMA transfer between memory and peripheral
struct DMAChannel {
    enum class Direction { PeriphToMem, MemToPeriph };
    enum class Mode { Normal, Circular };

    Direction direction = Direction::PeriphToMem;
    Mode mode = Mode::Normal;
    uint32_t data_count = 0;
    uint32_t transferred = 0;
    bool enabled = false;
    bool complete = false;

    std::vector<uint8_t> buffer;

    void configure(Direction dir, Mode m, uint32_t count) {
        direction = dir;
        mode = m;
        data_count = count;
        buffer.resize(count, 0);
        transferred = 0;
        complete = false;
    }

    void enable_channel() { enabled = true; complete = false; transferred = 0; }

    // EN: Simulate one DMA transfer step
    bool transfer_step(uint8_t data_in, uint8_t& data_out) {
        if (!enabled || complete) return false;

        if (direction == Direction::PeriphToMem) {
            buffer[transferred] = data_in;
            data_out = data_in;
        } else {
            data_out = buffer[transferred];
        }

        transferred++;
        if (transferred >= data_count) {
            if (mode == Mode::Normal) {
                complete = true;
                enabled = false;
            } else {
                transferred = 0;  // circular
            }
        }
        return true;
    }

    void print_status() const {
        std::cout << "    DMA: " << transferred << "/" << data_count
                  << " [" << (enabled ? "RUNNING" : "STOPPED")
                  << (complete ? " COMPLETE" : "")
                  << "] " << (mode == Mode::Circular ? "CIRCULAR" : "NORMAL") << "\n";
    }
};

// ═════════════════════════════════════════════════════════════════════════════
// MAIN
// ═════════════════════════════════════════════════════════════════════════════
int main() {
    std::cout << "============================================\n";
    std::cout << "   Module 15 - Serial Protocols Deep Dive\n";
    std::cout << "============================================\n\n";

    // ─── Demo 1: SPI Register-Level Init ─────────────────────────────────
    {
        std::cout << "--- Demo 1: SPI Register-Level Initialization ---\n\n";

        SPIPeripheral spi;

        // EN: Step-by-step register configuration (as in STM32 HAL)
        uint32_t cr1 = 0;
        cr1 |= (1U << spi_regs::MSTR);      // Master mode
        cr1 |= (1U << spi_regs::CPOL);       // CPOL=1
        cr1 |= (0U << spi_regs::CPHA);       // CPHA=0 → SPI Mode 2
        cr1 |= (0b010U << spi_regs::BR_SHIFT); // f_PCLK/8
        cr1 |= (1U << spi_regs::SSM);        // Software slave management
        cr1 |= (1U << spi_regs::SSI);        // Internal slave select
        // Do NOT set SPE yet

        spi.write_CR1(cr1);

        std::cout << "  Step 1: CR1 = 0x" << std::hex << std::setw(4)
                  << std::setfill('0') << cr1 << std::dec << std::setfill(' ') << "\n";
        spi.print_config();

        // EN: Enable SPI (set SPE bit)
        cr1 |= (1U << spi_regs::SPE);
        spi.write_CR1(cr1);
        std::cout << "\n  Step 2: SPE=1 → SPI enabled\n\n";

        // EN: Attach a simulated slave (ADXL345 accelerometer)
        spi.set_slave_handler([](uint8_t tx) -> uint8_t {
            static uint8_t reg_addr = 0;
            static bool first = true;
            if (first) { reg_addr = tx; first = false; return 0; }
            first = true;
            if (reg_addr == 0x00) return 0xE5;  // DEVID
            if (reg_addr == 0x32) return 0x12;  // DATAX0
            if (reg_addr == 0x33) return 0x00;  // DATAX1
            return 0xFF;
        });

        // EN: Read DEVID register (0x00 | 0x80 for read)
        spi.write_DR(0x80);  // read command + reg 0x00
        spi.read_DR();        // discard first byte
        spi.write_DR(0x00);   // clock out data
        uint8_t devid = spi.read_DR();
        std::cout << "  ADXL345 DEVID: 0x" << std::hex << static_cast<int>(devid)
                  << std::dec << " (expected: 0xe5)\n\n";
    }

    // ─── Demo 2: SPI Mode Comparison ────────────────────────────────────
    {
        std::cout << "--- Demo 2: All Four SPI Modes ---\n\n";

        std::cout << "  ┌──────────┬──────┬──────┬───────────────────┬───────────────────┐\n";
        std::cout << "  │ SPI Mode │ CPOL │ CPHA │ Clock Idle        │ Data Sample       │\n";
        std::cout << "  ├──────────┼──────┼──────┼───────────────────┼───────────────────┤\n";
        std::cout << "  │ Mode 0   │  0   │  0   │ LOW               │ Rising edge       │\n";
        std::cout << "  │ Mode 1   │  0   │  1   │ LOW               │ Falling edge      │\n";
        std::cout << "  │ Mode 2   │  1   │  0   │ HIGH              │ Falling edge      │\n";
        std::cout << "  │ Mode 3   │  1   │  1   │ HIGH              │ Rising edge       │\n";
        std::cout << "  └──────────┴──────┴──────┴───────────────────┴───────────────────┘\n\n";

        std::cout << "  Timing Diagram (Mode 0: CPOL=0, CPHA=0):\n";
        std::cout << "       ┌──┐  ┌──┐  ┌──┐  ┌──┐  ┌──┐  ┌──┐  ┌──┐  ┌──┐\n";
        std::cout << "  SCK ─┘  └──┘  └──┘  └──┘  └──┘  └──┘  └──┘  └──┘  └─\n";
        std::cout << "  MOSI ╳ D7  ╳ D6  ╳ D5  ╳ D4  ╳ D3  ╳ D2  ╳ D1  ╳ D0\n";
        std::cout << "       ↑    ↑    ↑    ↑    ↑    ↑    ↑    ↑\n";
        std::cout << "     sample points (rising edge)\n\n";

        // EN: Common device SPI modes
        std::cout << "  Common device SPI modes:\n";
        std::cout << "  ┌────────────────────────┬──────────┬────────────┐\n";
        std::cout << "  │ Device                 │ SPI Mode │ Max Clock  │\n";
        std::cout << "  ├────────────────────────┼──────────┼────────────┤\n";
        std::cout << "  │ ADXL345 (Accel)        │ 0 or 3   │ 5 MHz      │\n";
        std::cout << "  │ BME280 (Press/Temp/Hum)│ 0 or 3   │ 10 MHz     │\n";
        std::cout << "  │ MCP2515 (CAN)          │ 0 or 3   │ 10 MHz     │\n";
        std::cout << "  │ W25Q128 (Flash)        │ 0 or 3   │ 133 MHz    │\n";
        std::cout << "  │ MAX31856 (Thermo)      │ 1 or 3   │ 5 MHz      │\n";
        std::cout << "  │ ADS1256 (ADC)          │ 1        │ 7.68 MHz   │\n";
        std::cout << "  └────────────────────────┴──────────┴────────────┘\n\n";
    }

    // ─── Demo 3: I2C Register-Level Transaction ─────────────────────────
    {
        std::cout << "--- Demo 3: I2C Register-Level Read/Write ---\n\n";

        I2CPeripheral i2c;
        i2c.set_clock(42, 400);  // 42 MHz PCLK, 400 kHz Fast Mode

        // EN: Add devices on the bus
        i2c.add_slave(0x48, {{0x00, 0x1A}, {0x01, 0x60}});  // TMP102 temp sensor
        i2c.add_slave(0x68, {{0x75, 0x68}, {0x3B, 0x12}, {0x3C, 0x34}});  // MPU6050

        i2c.enable();
        i2c.print_config();
        std::cout << "\n";

        // EN: Read WHO_AM_I from MPU6050 (addr=0x68, reg=0x75)
        std::cout << "  I2C Read: MPU6050 WHO_AM_I (0x68, reg 0x75):\n";
        std::cout << "    1. START\n";
        i2c.start();

        std::cout << "    2. Send addr 0x68 + WRITE\n";
        i2c.send_address(0x68 << 1 | 0);  // write mode

        std::cout << "    3. Send register addr 0x75\n";
        i2c.write_data(0x75);

        std::cout << "    4. Repeated START\n";
        i2c.start();

        std::cout << "    5. Send addr 0x68 + READ\n";
        i2c.send_address(0x68 << 1 | 1);  // read mode

        auto who_am_i = i2c.read_data();
        std::cout << "    6. Read data: 0x" << std::hex
                  << static_cast<int>(who_am_i.value_or(0xFF))
                  << std::dec << " (expected: 0x68)\n";

        std::cout << "    7. NACK + STOP\n";
        i2c.stop();

        // EN: Write to TMP102 config register
        std::cout << "\n  I2C Write: TMP102 config (0x48, reg 0x01 = 0xA0):\n";
        i2c.start();
        i2c.send_address(0x48 << 1 | 0);
        i2c.write_data(0x01);
        i2c.write_data(0xA0);
        i2c.stop();
        std::cout << "    Written 0xA0 to register 0x01 ✓\n";

        // EN: Test NACK on invalid address
        std::cout << "\n  I2C NACK test (address 0x50, no device):\n";
        i2c.start();
        bool ack = i2c.send_address(0x50 << 1 | 0);
        auto errors = i2c.check_errors();
        std::cout << "    ACK received: " << ack
                  << " | NACK flag: " << errors.nack << "\n";
        i2c.stop();
        std::cout << "\n";
    }

    // ─── Demo 4: UART Register-Level Comms ──────────────────────────────
    {
        std::cout << "--- Demo 4: UART Register-Level Communication ---\n\n";

        UARTPeripheral uart1, uart2;
        uart1.connect(uart2);

        // EN: Configure UART1: 115200 baud, 42 MHz PCLK
        uart1.set_baud(42000000, 115200);
        uart1.enable(true, true);
        uart1.print_config();

        uart2.set_baud(42000000, 115200);
        uart2.enable(true, true);
        std::cout << "\n";

        // EN: Send "HELLO" from UART1 → UART2
        std::string msg = "HELLO";
        std::cout << "  UART1 → UART2: \"" << msg << "\"\n";
        for (char c : msg) {
            uart1.transmit(static_cast<uint8_t>(c));
        }

        // EN: Read on UART2
        std::cout << "  UART2 received: \"";
        while (uart2.is_rx_ready()) {
            auto byte = uart2.read();
            if (byte) std::cout << static_cast<char>(*byte);
        }
        std::cout << "\"\n";

        // EN: Error handling demo
        uart2.simulate_overrun();
        auto err = uart2.check_errors();
        std::cout << "\n  Error flags after overrun:\n"
                  << "    Parity:  " << err.parity << "\n"
                  << "    Framing: " << err.framing << "\n"
                  << "    Noise:   " << err.noise << "\n"
                  << "    Overrun: " << err.overrun << "  ← data lost!\n";
        uart2.clear_errors();
        std::cout << "    After clear: " << uart2.check_errors().overrun << "\n\n";
    }

    // ─── Demo 5: DMA + SPI Transfer ─────────────────────────────────────
    {
        std::cout << "--- Demo 5: DMA-Assisted SPI Transfer ---\n\n";

        std::cout << "  Scenario: Read 8 bytes from SPI Flash via DMA\n\n";

        SPIPeripheral spi;
        uint32_t cr1 = (1U << spi_regs::MSTR) | (1U << spi_regs::SSM) |
                        (1U << spi_regs::SSI)  | (1U << spi_regs::SPE);
        spi.write_CR1(cr1);

        // EN: Flash returns sequential bytes
        uint8_t flash_counter = 0xA0;
        spi.set_slave_handler([&flash_counter](uint8_t) -> uint8_t {
            return flash_counter++;
        });

        DMAChannel dma_rx;
        dma_rx.configure(DMAChannel::Direction::PeriphToMem, DMAChannel::Mode::Normal, 8);
        dma_rx.enable_channel();

        std::cout << "  DMA transfer progress:\n";
        for (uint32_t i = 0; i < 8; ++i) {
            spi.write_DR(0xFF);  // clock out dummy bytes
            uint8_t rx = spi.read_DR();
            uint8_t dma_out = 0;
            dma_rx.transfer_step(rx, dma_out);
            std::cout << "    [" << i << "] SPI_DR=0x" << std::hex
                      << static_cast<int>(rx) << " → DMA buffer\n";
        }
        std::cout << std::dec;
        dma_rx.print_status();

        std::cout << "  DMA Buffer: ";
        for (size_t i = 0; i < 8; ++i) {
            std::cout << "0x" << std::hex << static_cast<int>(dma_rx.buffer[i])
                      << (i < 7 ? " " : "");
        }
        std::cout << std::dec << "\n\n";
    }

    // ─── Demo 6: Multi-Slave SPI Bus ─────────────────────────────────────
    {
        std::cout << "--- Demo 6: Multi-Slave SPI Bus Topology ---\n\n";

        std::cout << "  ┌──────┐   SCK  ┌─────────────┐\n";
        std::cout << "  │      ├───────→│ Slave 0     │ (Accel)  CS=PA4\n";
        std::cout << "  │      │   MOSI ├─────────────┤\n";
        std::cout << "  │ MCU  ├───────→│ Slave 1     │ (Flash)  CS=PA5\n";
        std::cout << "  │(SPI1)│   MISO ├─────────────┤\n";
        std::cout << "  │      │←───────│ Slave 2     │ (CAN)    CS=PA6\n";
        std::cout << "  │      │        └─────────────┘\n";
        std::cout << "  │      │   CS0  CS1  CS2\n";
        std::cout << "  │      ├────┤    ┤    ┤  (active LOW)\n";
        std::cout << "  └──────┘\n\n";

        std::cout << "  Chip Select Management:\n";
        std::cout << "    1. Pull ALL CS pins HIGH (deselect all)\n";
        std::cout << "    2. Pull target CS LOW (select one slave)\n";
        std::cout << "    3. Perform SPI transfer\n";
        std::cout << "    4. Pull CS HIGH (deselect)\n\n";

        std::cout << "  Code pattern:\n";
        std::cout << "    GPIO_SetBits(GPIOA, CS0|CS1|CS2);   // all HIGH\n";
        std::cout << "    GPIO_ResetBits(GPIOA, CS1);          // select Flash\n";
        std::cout << "    SPI_Transmit(&spi1, tx_buf, 4);      // send command\n";
        std::cout << "    SPI_Receive(&spi1, rx_buf, 256);     // read data\n";
        std::cout << "    GPIO_SetBits(GPIOA, CS1);            // deselect\n\n";

        std::cout << "  CAUTION: Never select multiple slaves simultaneously!\n";
        std::cout << "  CAUTION: Ensure CS timing requirements (setup/hold) are met!\n\n";
    }

    // ─── Demo 7: I2C Error Recovery ──────────────────────────────────────
    {
        std::cout << "--- Demo 7: I2C Error Handling & Recovery ---\n\n";

        std::cout << "  ┌──────────────────┬────────────────────────────────────────┐\n";
        std::cout << "  │ Error            │ Recovery Action                        │\n";
        std::cout << "  ├──────────────────┼────────────────────────────────────────┤\n";
        std::cout << "  │ NACK (AF)        │ Send STOP, retry or report error       │\n";
        std::cout << "  │ Arbitration Lost │ Release bus, retry after delay         │\n";
        std::cout << "  │ Bus Error        │ Reset I2C peripheral (SWRST)           │\n";
        std::cout << "  │ Clock Stretching │ Timeout → toggle SCL manually (9×)     │\n";
        std::cout << "  │ SDA stuck LOW    │ Generate 9 SCL clocks + STOP           │\n";
        std::cout << "  │ Bus Locked       │ GPIO bit-bang SCL to unlock            │\n";
        std::cout << "  └──────────────────┴────────────────────────────────────────┘\n\n";

        std::cout << "  I2C Bus Recovery sequence (SDA stuck LOW):\n";
        std::cout << "    1. Configure SCL as GPIO output\n";
        std::cout << "    2. Toggle SCL 9 times (clock out stuck byte)\n";
        std::cout << "    3. Generate STOP condition (SDA rises while SCL high)\n";
        std::cout << "    4. Re-configure SCL/SDA as I2C alternate function\n";
        std::cout << "    5. Re-initialize I2C peripheral\n\n";

        std::cout << "  Pseudo-code:\n";
        std::cout << "    void i2c_bus_recovery() {\n";
        std::cout << "        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;\n";
        std::cout << "        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct); // SCL=PB6\n";
        std::cout << "        for (int i = 0; i < 9; i++) {\n";
        std::cout << "            HAL_GPIO_WritePin(GPIOB, SCL, GPIO_PIN_SET);\n";
        std::cout << "            delay_us(5);\n";
        std::cout << "            HAL_GPIO_WritePin(GPIOB, SCL, GPIO_PIN_RESET);\n";
        std::cout << "            delay_us(5);\n";
        std::cout << "        }\n";
        std::cout << "        // Generate STOP\n";
        std::cout << "        HAL_GPIO_WritePin(GPIOB, SDA, GPIO_PIN_RESET);\n";
        std::cout << "        HAL_GPIO_WritePin(GPIOB, SCL, GPIO_PIN_SET);\n";
        std::cout << "        delay_us(5);\n";
        std::cout << "        HAL_GPIO_WritePin(GPIOB, SDA, GPIO_PIN_SET);\n";
        std::cout << "        MX_I2C1_Init(); // re-init\n";
        std::cout << "    }\n\n";
    }

    // ─── Demo 8: Protocol Comparison ─────────────────────────────────────
    {
        std::cout << "--- Demo 8: SPI vs I2C vs UART Comparison ---\n\n";

        std::cout << "  ┌─────────────────┬───────────────┬───────────────┬──────────────┐\n";
        std::cout << "  │ Feature         │ SPI           │ I2C           │ UART         │\n";
        std::cout << "  ├─────────────────┼───────────────┼───────────────┼──────────────┤\n";
        std::cout << "  │ Wires           │ 4+ (MOSI,     │ 2 (SDA,SCL)   │ 2 (TX, RX)   │\n";
        std::cout << "  │                 │ MISO,SCK,CS)  │               │              │\n";
        std::cout << "  │ Speed           │ Up to 100MHz  │ 100k-3.4MHz   │ Up to 4.5Mbps│\n";
        std::cout << "  │ Duplex          │ Full          │ Half          │ Full         │\n";
        std::cout << "  │ Multi-device    │ CS per slave  │ Address       │ Point-to-pt  │\n";
        std::cout << "  │ Flow control    │ Hardware (CS) │ ACK/NACK      │ RTS/CTS      │\n";
        std::cout << "  │ Max distance    │ ~30 cm PCB    │ ~1 m          │ ~15 m (RS232)│\n";
        std::cout << "  │ Overhead        │ None          │ ACK per byte  │ Start/Stop   │\n";
        std::cout << "  │ Error detection │ None (CRC opt)│ ACK/NACK      │ Parity       │\n";
        std::cout << "  │ Clock           │ Master-driven │ Master-driven │ Async        │\n";
        std::cout << "  │ HW complexity   │ Low           │ Medium        │ Medium       │\n";
        std::cout << "  │ Pin count (3dev)│ 7             │ 2             │ 6            │\n";
        std::cout << "  ├─────────────────┼───────────────┼───────────────┼──────────────┤\n";
        std::cout << "  │ Best for        │ High-speed    │ Multi-sensor  │ Debug/logging│\n";
        std::cout << "  │                 │ flash, ADC,   │ config, EEPROM│ GPS, BT,     │\n";
        std::cout << "  │                 │ display       │               │ console      │\n";
        std::cout << "  └─────────────────┴───────────────┴───────────────┴──────────────┘\n";
    }

    std::cout << "\n============================================\n";
    std::cout << "   End of Serial Protocols Deep Dive\n";
    std::cout << "============================================\n";

    return 0;
}
