/**
 * @file module_14_automotive_networks/05_inter_mcu_communication.cpp
 * @brief Inter-MCU Communication — SPI, I2C, UART, Shared Memory
 *
 * @details
 * =============================================================================
 * [THEORY: Inter-MCU Communication in Automotive / TEORİ: Otomotivde MCU'lar Arası İletişim]
 * =============================================================================
 *
 * EN: While CAN/LIN/Ethernet handle inter-ECU communication, within a single
 *     ECU board, multiple MCUs/processors communicate via hardware peripherals:
 *
 *     ┌──────────────────────────────────────────────────────┐
 *     │ ECU Board (e.g., ADAS Domain Controller)             │
 *     │                                                      │
 *     │  ┌───────────┐    SPI (50MHz)     ┌────────────┐     │
 *     │  │ Main SoC  │───────────────────→│ Safety MCU │     │
 *     │  │(Linux/QNX)│    I2C (400kHz)    │ (ASIL-D)   │     │
 *     │  │           │───────────────────→│            │     │
 *     │  │           │    UART (115200)   │            │     │
 *     │  │           │───────────────────→│            │     │
 *     │  │           │                    └────────────┘     │
 *     │  │           │    Shared Memory                      │
 *     │  │           │←──────────────────→ SRAM (dual-port)  │
 *     │  └───────────┘                                       │
 *     └──────────────────────────────────────────────────────┘
 *
 *     Protocol Comparison:
 *     ┌──────────┬──────────┬──────────┬──────────┬──────────────────────┐
 *     │ Protocol │ Speed    │ Wires    │ Topology │ Use Case             │
 *     ├──────────┼──────────┼──────────┼──────────┼──────────────────────┤
 *     │ SPI      │ 50+ MHz  │ 4 (MOSI  │ Master-  │ Fast data (ADC, DAC, │
 *     │          │          │  MISO    │ Slave    │  Flash, FPGA)        │
 *     │          │          │  SCK,CS) │          │                      │
 *     ├──────────┼──────────┼──────────┼──────────┼──────────────────────┤
 *     │ I2C      │ 100k-    │ 2 (SDA,  │ Multi-   │ Low-speed sensors,   │
 *     │          │ 3.4 MHz  │  SCL)    │ Master   │  EEPROMs, PMICs      │
 *     ├──────────┼──────────┼──────────┼──────────┼──────────────────────┤
 *     │ UART     │ up to    │ 2 (TX,RX)│ Point-to │ Debug console,       │
 *     │          │ 4 Mbps   │          │ Point    │  GPS, Bluetooth      │
 *     ├──────────┼──────────┼──────────┼──────────┼──────────────────────┤
 *     │ Shared   │ Bus speed│ N/A      │ N/A      │ Multi-core IPC,      │
 *     │ Memory   │ (GBps)   │ (on-chip)│          │  DMA transfer        │
 *     └──────────┴──────────┴──────────┴──────────┴──────────────────────┘
 *
 * TR: CAN/LIN/Ethernet ECU'lar arasında haberleşmeyi sağlarken, tek bir ECU
 *     kartı üzerindeki birden fazla MCU/işlemci donanım çevre birimleri ile
 *     iletişim kurar:
 *
 *     ┌────────────────────────────────────────────────────────┐
 *     │ ECU Kartı (ör. ADAS Alan Denetleyicisi)                │
 *     │                                                        │
 *     │  ┌───────────┐    SPI (50MHz)     ┌────────────┐       │
 *     │  │ Ana SoC   │───────────────────→│ Güvenlik   │       │
 *     │  │(Linux/QNX)│    I2C (400kHz)    │ MCU(ASIL-D)│       │
 *     │  │           │───────────────────→│            │       │
 *     │  │           │    UART (115200)   │            │       │
 *     │  │           │───────────────────→│            │       │
 *     │  │           │                    └────────────┘       │
 *     │  │           │    Paylaşımlı Bellek                    │
 *     │  │           │←──────────────────→ SRAM (çift-port)    │
 *     │  └───────────┘                                         │
 *     └────────────────────────────────────────────────────────┘
 *
 *     Protokol Karşılaştırması:
 *     ┌──────────┬──────────┬──────────┬──────────┬──────────────────────┐
 *     │ Protokol │ Hız      │ Kablo    │ Topoloji │ Kullanım Alanı       │
 *     ├──────────┼──────────┼──────────┼──────────┼──────────────────────┤
 *     │ SPI      │ 50+ MHz  │ 4 (MOSI  │ Master-  │ Hızlı veri (ADC,DAC, │
 *     │          │          │  MISO    │ Slave    │  Flash, FPGA)        │
 *     │          │          │  SCK,CS) │          │                      │
 *     ├──────────┼──────────┼──────────┼──────────┼──────────────────────┤
 *     │ I2C      │ 100k-    │ 2 (SDA,  │ Çoklu    │ Düşük hızlı sensör,  │
 *     │          │ 3.4 MHz  │  SCL)    │ Master   │  EEPROM, PMIC        │
 *     ├──────────┼──────────┼──────────┼──────────┼──────────────────────┤
 *     │ UART     │ 4 Mbps'e │ 2 (TX,RX)│ Noktadan │ Debug konsol,        │
 *     │          │ kadar    │          │ Noktaya  │  GPS, Bluetooth      │
 *     ├──────────┼──────────┼──────────┼──────────┼──────────────────────┤
 *     │ Paylaşım │ Bus hızı │ Yok      │ Yok      │ Çok çekirdekli IPC,  │
 *     │ Bellek   │ (GBps)   │ (çip içi)│          │  DMA transferi       │
 *     └──────────┴──────────┴──────────┴──────────┴──────────────────────┘
 * @note Compile: g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion 05_inter_mcu_communication.cpp -o 05_inter_mcu
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <iostream>
#include <cstdint>
#include <vector>
#include <string>
#include <iomanip>
#include <array>
#include <queue>
#include <algorithm>
#include <cassert>
#include <functional>
#include <map>
#include <cstring>

// ═════════════════════════════════════════════════════════════════════════════
// PART 1: SPI — Serial Peripheral Interface
// ═════════════════════════════════════════════════════════════════════════════

// EN: SPI uses 4 wires: MOSI (Master Out Slave In), MISO (Master In Slave Out),
//     SCK (Serial Clock), CS/SS (Chip Select, active low).
//     Full-duplex: Master sends on MOSI, receives on MISO simultaneously.
//     Clock modes: CPOL (idle level) + CPHA (sampling edge) → 4 modes (0-3).
// TR: SPI: 4 kablo (MOSI, MISO, SCK, CS), tam çift yönlü, master-slave mimarisi.
//     Master MOSI'üzerinde gönderir, MISO'üzerinde aynı anda alır.
//     Saat modları: CPOL (boşta seviye) + CPHA (örnekleme kenarı) → 4 mod (0-3).

enum class SPIMode : uint8_t {
    Mode0 = 0,  // CPOL=0, CPHA=0 — idle low, sample on leading edge
                // TR: boşta düşük, yükselen kenarda örnekle
    Mode1 = 1,  // CPOL=0, CPHA=1 — idle low, sample on trailing edge
                // TR: boşta düşük, düşen kenarda örnekle
    Mode2 = 2,  // CPOL=1, CPHA=0 — idle high, sample on leading edge
                // TR: boşta yüksek, düşen kenarda örnekle
    Mode3 = 3,  // CPOL=1, CPHA=1 — idle high, sample on trailing edge
                // TR: boşta yüksek, yükselen kenarda örnekle
};

class SPIMaster {
    std::string name_;
    SPIMode mode_;
    uint32_t clock_hz_;
    int num_slaves_;
    int active_cs_ = -1;

public:
    SPIMaster(std::string name, SPIMode mode, uint32_t clock_hz, int num_slaves)
        : name_(std::move(name)), mode_(mode), clock_hz_(clock_hz), num_slaves_(num_slaves) {}

    bool select_slave(int cs) {
        if (cs < 0 || cs >= num_slaves_) return false;
        active_cs_ = cs;
        return true;
    }

    void deselect() { active_cs_ = -1; }

    // EN: SPI is full-duplex: send tx_data, receive rx_data simultaneously
    // TR: SPI tam çift yönlü: tx_data gönderilirken rx_data aynı anda alınır
    std::vector<uint8_t> transfer(const std::vector<uint8_t>& tx_data,
                                   std::function<uint8_t(uint8_t)> slave_handler) {
        std::vector<uint8_t> rx_data;
        rx_data.reserve(tx_data.size());
        for (auto byte : tx_data) {
            rx_data.push_back(slave_handler(byte));
        }
        return rx_data;
    }

    void print_config() const {
        std::cout << "    SPI Master: " << name_ << "\n";
        std::cout << "      Clock: " << clock_hz_ / 1'000'000 << " MHz"
                  << " | Mode: " << static_cast<int>(mode_)
                  << " (CPOL=" << (static_cast<int>(mode_) >> 1)
                  << " CPHA=" << (static_cast<int>(mode_) & 1) << ")"
                  << " | Slaves: " << num_slaves_
                  << " | Active CS: " << active_cs_ << "\n";
    }
};

// ═════════════════════════════════════════════════════════════════════════════
// PART 2: I2C — Inter-Integrated Circuit
// ═════════════════════════════════════════════════════════════════════════════

// EN: I2C uses 2 wires: SDA (data) + SCL (clock). Open-drain with pull-ups.
//     7-bit addressing (128 devices). Multi-master capable.
//     Transaction: START → Address+R/W → ACK → Data → ACK → ... → STOP
// TR: I2C: 2 kablo (SDA + SCL), açık-drain + pull-up, 7-bit adresleme (128 cihaz).
//     Çoklu master desteği. İşlem: START → Adres+R/W → ACK → Veri → ACK → STOP

enum class I2CSpeed : uint32_t {
    Standard   = 100'000,    // 100 kHz   — TR: Standart mod
    Fast       = 400'000,    // 400 kHz   — TR: Hızlı mod
    FastPlus   = 1'000'000,  // 1 MHz     — TR: Hızlı+ mod
    HighSpeed  = 3'400'000,  // 3.4 MHz   — TR: Yüksek Hız modu
};

std::string to_string(I2CSpeed spd) {
    switch (spd) {
        case I2CSpeed::Standard:  return "Standard (100kHz)";
        case I2CSpeed::Fast:      return "Fast (400kHz)";
        case I2CSpeed::FastPlus:  return "Fast+ (1MHz)";
        case I2CSpeed::HighSpeed: return "High-Speed (3.4MHz)";
    }
    return "Unknown";
}

struct I2CDevice {
    uint8_t address;  // 7-bit — TR: 7-bit I2C adresi (0x00-0x7F)
    std::string name;
    std::map<uint8_t, uint8_t> registers;  // register_addr → value
                                           // TR: yazmaç_adresi → değer eşleme tablosu

    void write_register(uint8_t reg, uint8_t value) {
        registers[reg] = value;
    }

    uint8_t read_register(uint8_t reg) const {
        auto it = registers.find(reg);
        return (it != registers.end()) ? it->second : 0xFF;
    }
};

class I2CBus {
    std::string name_;
    I2CSpeed speed_;
    std::map<uint8_t, I2CDevice*> devices_;

public:
    I2CBus(std::string name, I2CSpeed speed) : name_(std::move(name)), speed_(speed) {}

    bool attach(I2CDevice& dev) {
        if (devices_.count(dev.address)) return false;  // address conflict — TR: adres çakışması
        devices_[dev.address] = &dev;
        return true;
    }

    bool write(uint8_t addr, uint8_t reg, uint8_t data) {
        auto it = devices_.find(addr);
        if (it == devices_.end()) return false;  // NACK — TR: cihaz yanıt vermedi
        it->second->write_register(reg, data);
        return true;  // ACK — TR: cihaz onayladı
    }

    bool read(uint8_t addr, uint8_t reg, uint8_t& data) {
        auto it = devices_.find(addr);
        if (it == devices_.end()) return false;
        data = it->second->read_register(reg);
        return true;
    }

    // EN: Bus scan — detect all devices
    // TR: Bus tarama — bağlı tüm cihazları tespit et (ACK yanıtı kontrolü)
    std::vector<uint8_t> scan() const {
        std::vector<uint8_t> found;
        for (auto& [addr, dev] : devices_) {
            found.push_back(addr);
        }
        return found;
    }

    void print_config() const {
        std::cout << "    I2C Bus: " << name_
                  << " | Speed: " << to_string(speed_)
                  << " | Devices: " << devices_.size() << "\n";
    }
};

// ═════════════════════════════════════════════════════════════════════════════
// PART 3: UART — Universal Asynchronous Receiver-Transmitter
// ═════════════════════════════════════════════════════════════════════════════

// EN: UART uses 2 wires (TX, RX), asynchronous (no clock wire).
//     Frame: [START(1)] [DATA(5-9)] [PARITY(0-1)] [STOP(1-2)]
//     Common config: 8N1 (8 data, No parity, 1 stop bit)
// TR: UART: 2 kablo (TX, RX), asenkron (saat kablosu yok).
//     Çerçeve: [START(1)] [VERİ(5-9)] [PARİTE(0-1)] [STOP(1-2)]
//     Yaygın konfigürasyon: 8N1 (8 veri, parite yok, 1 stop bit)

enum class UARTParity { None, Even, Odd };

struct UARTConfig {
    uint32_t baud_rate  = 115200;
    uint8_t  data_bits  = 8;
    UARTParity parity   = UARTParity::None;
    uint8_t  stop_bits  = 1;

    std::string config_string() const {
        std::string s = std::to_string(data_bits);
        switch (parity) {
            case UARTParity::None: s += "N"; break;
            case UARTParity::Even: s += "E"; break;
            case UARTParity::Odd:  s += "O"; break;
        }
        s += std::to_string(stop_bits);
        return s;  // e.g. "8N1" — TR: ör. "8N1" (8 veri, parite yok, 1 stop)
    }
};

class UARTPort {
    std::string name_;
    UARTConfig config_;
    std::queue<uint8_t> tx_buffer_;
    std::queue<uint8_t> rx_buffer_;

public:
    UARTPort(std::string name, UARTConfig config)
        : name_(std::move(name)), config_(config) {}

    void send(const std::vector<uint8_t>& data) {
        for (auto b : data) tx_buffer_.push(b);
    }

    void send_string(const std::string& str) {
        for (char c : str) tx_buffer_.push(static_cast<uint8_t>(c));
    }

    // EN: Connect two UARTs (TX of one → RX of other)
    // TR: İki UART bağla (birinin TX'ı diğerinin RX'ına)
    void connect_to(UARTPort& other) {
        while (!tx_buffer_.empty()) {
            other.rx_buffer_.push(tx_buffer_.front());
            tx_buffer_.pop();
        }
    }

    std::vector<uint8_t> receive() {
        std::vector<uint8_t> data;
        while (!rx_buffer_.empty()) {
            data.push_back(rx_buffer_.front());
            rx_buffer_.pop();
        }
        return data;
    }

    std::string receive_string() {
        auto data = receive();
        return std::string(data.begin(), data.end());
    }

    size_t rx_available() const { return rx_buffer_.size(); }

    void print_config() const {
        std::cout << "    UART: " << name_
                  << " | Baud: " << config_.baud_rate
                  << " | Config: " << config_.config_string() << "\n";
    }
};

// ═════════════════════════════════════════════════════════════════════════════
// PART 4: Shared Memory IPC
// ═════════════════════════════════════════════════════════════════════════════

// EN: Multi-core MCUs often share SRAM for inter-processor communication.
//     Critical: needs memory barriers, mailbox interrupts, or spinlocks.
//     Common pattern: ring buffer with producer/consumer + doorbell interrupt.
// TR: Çok çekirdekli MCU'lar paylaşımlı SRAM ile haberleşir.
//     Kritik: bellek bariyerleri, mailbox kesmeleri veya spinlock gerekir.
//     Yaygın desen: üretici/tüketici + doorbell kesmesi ile halka tampon.

struct SharedMemoryMailbox {
    static constexpr size_t BUFFER_SIZE = 256;
    static constexpr size_t MSG_MAX_SIZE = 64;

    struct Message {
        uint8_t type;
        uint8_t length;
        std::array<uint8_t, MSG_MAX_SIZE> data {};

        void print(const std::string& label) const {
            std::cout << "      " << label << " Type:0x"
                      << std::hex << std::setw(2) << std::setfill('0')
                      << static_cast<int>(type) << std::dec
                      << " Len:" << static_cast<int>(length) << " Data:";
            for (int i = 0; i < length && i < 16; ++i)
                std::cout << " " << std::hex << std::setw(2) << std::setfill('0')
                          << static_cast<int>(data[static_cast<size_t>(i)]);
            std::cout << std::dec << "\n";
        }
    };

private:
    std::queue<Message> core0_to_core1_;
    std::queue<Message> core1_to_core0_;
    uint32_t msg_count_01_ = 0;
    uint32_t msg_count_10_ = 0;

public:
    // EN: Core 0 sends to Core 1
    // TR: Çekirdek 0, Çekirdek 1'e mesaj gönderir
    bool send_0to1(uint8_t type, const std::vector<uint8_t>& data) {
        if (data.size() > MSG_MAX_SIZE) return false;
        Message msg;
        msg.type = type;
        msg.length = static_cast<uint8_t>(data.size());
        std::copy(data.begin(), data.end(), msg.data.begin());
        core0_to_core1_.push(msg);
        ++msg_count_01_;
        return true;
    }

    bool send_1to0(uint8_t type, const std::vector<uint8_t>& data) {
        if (data.size() > MSG_MAX_SIZE) return false;
        Message msg;
        msg.type = type;
        msg.length = static_cast<uint8_t>(data.size());
        std::copy(data.begin(), data.end(), msg.data.begin());
        core1_to_core0_.push(msg);
        ++msg_count_10_;
        return true;
    }

    // EN: Core 1 reads from Core 0
    // TR: Çekirdek 1, Çekirdek 0'dan okur
    bool receive_on_core1(Message& msg) {
        if (core0_to_core1_.empty()) return false;
        msg = core0_to_core1_.front();
        core0_to_core1_.pop();
        return true;
    }

    bool receive_on_core0(Message& msg) {
        if (core1_to_core0_.empty()) return false;
        msg = core1_to_core0_.front();
        core1_to_core0_.pop();
        return true;
    }

    void print_stats() const {
        std::cout << "    Mailbox stats: Core0→Core1: " << msg_count_01_
                  << " msgs | Core1→Core0: " << msg_count_10_ << " msgs\n";
    }
};

// ═════════════════════════════════════════════════════════════════════════════
// PART 5: Message Framing Protocol
// ═════════════════════════════════════════════════════════════════════════════

// EN: Raw UART/SPI byte streams need framing to detect message boundaries.
//     Common patterns:
//     1) Length-delimited: [HEADER][LENGTH][PAYLOAD][CHECKSUM]
//     2) SLIP/COBS: Byte stuffing to avoid delimiter collisions
//     3) Fixed-length: All messages same size (padding)
// TR: Ham UART/SPI bayt akışlarında mesaj sınırlarını belirlemek için çerçeveleme gerekir.
//     Yaygın desenler:
//     1) Uzunluk-sınırlı: [BAŞLIK][UZUNLUK][VERİ][SAĞLAMA]
//     2) SLIP/COBS: Sınırlayıcı çakışmasını önlemek için bayt doldurma
//     3) Sabit uzunluk: Tüm mesajlar aynı boyutta (dolgulu)

struct FramedMessage {
    static constexpr uint8_t SYNC_BYTE = 0xAA;
    static constexpr uint8_t VERSION   = 0x01;

    uint8_t  msg_id;
    uint8_t  length;
    std::vector<uint8_t> payload;
    uint8_t  checksum;  // XOR of all bytes (msg_id + length + payload)
                        // TR: tüm baytların XOR'u (msg_id + uzunluk + veri)

    static uint8_t calc_checksum(uint8_t id, uint8_t len, const std::vector<uint8_t>& data) {
        uint8_t cs = id ^ len;
        for (auto b : data) cs ^= b;
        return cs;
    }

    // EN: Build a framed message
    // TR: Çerçeveli mesaj oluştur — ID, veri ve sağlama toplamı hesapla
    static FramedMessage build(uint8_t id, const std::vector<uint8_t>& data) {
        FramedMessage msg;
        msg.msg_id = id;
        msg.length = static_cast<uint8_t>(data.size());
        msg.payload = data;
        msg.checksum = calc_checksum(id, msg.length, data);
        return msg;
    }

    bool verify() const {
        return checksum == calc_checksum(msg_id, length, payload);
    }

    // EN: Serialize to byte stream
    // TR: Bayt akışına serileştir — SYNC + VER + ID + LEN + veri + checksum
    std::vector<uint8_t> serialize() const {
        std::vector<uint8_t> bytes;
        bytes.push_back(SYNC_BYTE);
        bytes.push_back(VERSION);
        bytes.push_back(msg_id);
        bytes.push_back(length);
        bytes.insert(bytes.end(), payload.begin(), payload.end());
        bytes.push_back(checksum);
        return bytes;
    }

    // EN: Deserialize from byte stream
    // TR: Bayt akışından çözümle — SYNC/VER doğrula, veriyi çıkar, checksum kontrol et
    static bool deserialize(const std::vector<uint8_t>& bytes, FramedMessage& msg) {
        if (bytes.size() < 5) return false;  // minimum: SYNC+VER+ID+LEN+CS
                                              // TR: minimum: SYNC+VER+ID+UZUNLUK+SAĞLAMA
        if (bytes[0] != SYNC_BYTE) return false;
        if (bytes[1] != VERSION) return false;
        msg.msg_id = bytes[2];
        msg.length = bytes[3];
        if (bytes.size() < static_cast<size_t>(4 + msg.length + 1)) return false;
        msg.payload.assign(bytes.begin() + 4, bytes.begin() + 4 + msg.length);
        msg.checksum = bytes[static_cast<size_t>(4 + msg.length)];
        return msg.verify();
    }

    void print(const std::string& label) const {
        std::cout << "    " << label << " ID:0x"
                  << std::hex << std::setw(2) << std::setfill('0')
                  << static_cast<int>(msg_id) << std::dec
                  << " Len:" << static_cast<int>(length)
                  << " CS:0x" << std::hex << std::setw(2) << std::setfill('0')
                  << static_cast<int>(checksum) << std::dec
                  << " Valid:" << (verify() ? "YES ✓" : "NO ✗") << "\n";
    }
};

// ═════════════════════════════════════════════════════════════════════════════
// MAIN
// ═════════════════════════════════════════════════════════════════════════════
int main() {
    std::cout << "============================================\n";
    std::cout << "   Module 14 - Inter-MCU Communication\n";
    std::cout << "============================================\n\n";

    // ─── Demo 1: SPI Master-Slave Transfer ──────────────────────────────
    {
        std::cout << "--- Demo 1: SPI Master-Slave Transfer ---\n";
        std::cout << "  Scenario: SoC reads accelerometer via SPI\n\n";

        SPIMaster spi("SPI1", SPIMode::Mode0, 10'000'000, 3);
        spi.print_config();

        // EN: Select accelerometer (CS=0) and read WHO_AM_I register
        // TR: İvmeölçerü seç (CS=0) ve WHO_AM_I kaydını oku
        spi.select_slave(0);

        // EN: Simulated accelerometer: responds with register data
        // TR: Simüle ivmeölçer: kayıt verisiyle yanıt verir
        // Read command: 0x80 | reg_addr (bit 7 = read flag)
        // TR: Okuma komutu: 0x80 | yazmaç_adresi (bit 7 = okuma bayrağı)
        std::map<uint8_t, uint8_t> accel_regs = {
            {0x0F, 0x33},  // WHO_AM_I = 0x33 (LIS3DH) — TR: cihaz kimlik kaydı
            {0x28, 0xF0},  // OUT_X_L  — TR: X ekseni alt bayt
            {0x29, 0x01},  // OUT_X_H → X = 0x01F0 = 496 → ~0.12g — TR: X ekseni üst bayt
            {0x2A, 0x20},  // OUT_Y_L  — TR: Y ekseni alt bayt
            {0x2B, 0x00},  // OUT_Y_H → Y = 0x0020 = 32 — TR: Y ekseni üst bayt
            {0x2C, 0x00},  // OUT_Z_L  — TR: Z ekseni alt bayt
            {0x2D, 0x10},  // OUT_Z_H → Z = 0x1000 = 4096 → ~1.0g — TR: Z ekseni üst bayt
        };

        uint8_t last_addr = 0;
        auto slave_handler = [&](uint8_t tx) -> uint8_t {
            if (tx & 0x80) {
                // Read command — TR: Okuma komutu (bit 7 set)
                last_addr = tx & 0x7F;
                return 0xFF;  // dummy byte during address phase
                              // TR: adres fazında kukla bayt
            }
            // Data phase: return register value
            // TR: Veri fazı: yazmaç değerini döndür
            auto it = accel_regs.find(last_addr);
            uint8_t val = (it != accel_regs.end()) ? it->second : 0x00;
            ++last_addr;
            return val;
        };

        // EN: Read WHO_AM_I (addr 0x0F)
        // TR: WHO_AM_I oku (adres 0x0F) — cihaz kimliği doğrulaması
        auto rx1 = spi.transfer({0x8F, 0x00}, slave_handler);
        std::cout << "    WHO_AM_I: TX={0x8F, 0x00} → RX={0x"
                  << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(rx1[0])
                  << ", 0x" << std::setw(2) << static_cast<int>(rx1[1])
                  << "} → ID=0x" << std::setw(2) << static_cast<int>(rx1[1])
                  << std::dec << " (LIS3DH ✓)\n";

        // EN: Read 6 bytes of XYZ data (burst read from 0x28)
        // TR: 6 bayt XYZ verisi oku (0x28'den burst okuma)
        auto rx2 = spi.transfer({0xE8, 0, 0, 0, 0, 0, 0}, slave_handler);
        int16_t x = static_cast<int16_t>(static_cast<uint16_t>(rx2[2]) << 8 | rx2[1]);
        int16_t y = static_cast<int16_t>(static_cast<uint16_t>(rx2[4]) << 8 | rx2[3]);
        int16_t z = static_cast<int16_t>(static_cast<uint16_t>(rx2[6]) << 8 | rx2[5]);
        std::cout << "    Accel XYZ: X=" << x << " Y=" << y << " Z=" << z << "\n";
        std::cout << "    (Z ≈ 1g → correctly oriented, face-up)\n\n";

        spi.deselect();
    }

    // ─── Demo 2: SPI Clock Modes ────────────────────────────────────────
    {
        std::cout << "--- Demo 2: SPI Clock Modes ---\n\n";
        std::cout << "  ┌──────┬──────┬──────┬───────────────────────┬──────────────────┐\n";
        std::cout << "  │ Mode │ CPOL │ CPHA │ Timing                │ Common Devices   │\n";
        std::cout << "  ├──────┼──────┼──────┼───────────────────────┼──────────────────┤\n";
        std::cout << "  │  0   │  0   │  0   │ Idle=LOW, Sample=Rise │ Most sensors     │\n";
        std::cout << "  │  1   │  0   │  1   │ Idle=LOW, Sample=Fall │ Some ADCs        │\n";
        std::cout << "  │  2   │  1   │  0   │ Idle=HIGH,Sample=Fall │ Some DACs        │\n";
        std::cout << "  │  3   │  1   │  1   │ Idle=HIGH,Sample=Rise │ SD cards, Flash  │\n";
        std::cout << "  └──────┴──────┴──────┴───────────────────────┴──────────────────┘\n\n";

        std::cout << "  SPI Signal Diagram (Mode 0, CPOL=0, CPHA=0):\n";
        std::cout << "                 ┌─┐ ┌─┐ ┌─┐ ┌─┐ ┌─┐ ┌─┐ ┌─┐ ┌─┐\n";
        std::cout << "  SCK:      ─────┘ └─┘ └─┘ └─┘ └─┘ └─┘ └─┘ └─┘ └───\n";
        std::cout << "  CS:       ──┐                                     ┌──\n";
        std::cout << "              └─────────────────────────────────────┘\n";
        std::cout << "  MOSI:     ─── D7 ─ D6 ─ D5 ─ D4 ─ D3 ─ D2 ─ D1 ─ D0 ──\n";
        std::cout << "  MISO:     ─── D7 ─ D6 ─ D5 ─ D4 ─ D3 ─ D2 ─ D1 ─ D0 ──\n";
        std::cout << "  Sample:        ↑    ↑    ↑    ↑    ↑    ↑    ↑    ↑\n\n";
    }

    // ─── Demo 3: I2C Bus Scan & Register Access ─────────────────────────
    {
        std::cout << "--- Demo 3: I2C Bus Scan & Register Access ---\n";
        std::cout << "  Scenario: SoC communicates with sensors on I2C bus\n\n";

        I2CBus bus("I2C1", I2CSpeed::Fast);
        bus.print_config();

        // EN: Devices on the bus
        // TR: Bus üzerindeki cihazlar
        I2CDevice temp_sensor;
        temp_sensor.address = 0x48;  // TMP102 — TR: sıcaklık sensörü I2C adresi
        temp_sensor.name = "TMP102 (Temp)";
        temp_sensor.write_register(0x00, 75);   // Temperature = 75 (raw) — TR: ham sıcaklık değeri
        temp_sensor.write_register(0x01, 0x60);  // Config register — TR: yapılandırma kaydı

        I2CDevice accel;
        accel.address = 0x1D;  // ADXL345 — TR: ivmeölçer I2C adresi
        accel.name = "ADXL345 (Accel)";
        accel.write_register(0x00, 0xE5);  // DEVID = 0xE5 — TR: cihaz kimlik değeri

        I2CDevice eeprom;
        eeprom.address = 0x50;  // AT24C02 — TR: EEPROM I2C adresi
        eeprom.name = "AT24C02 (EEPROM)";
        eeprom.write_register(0x00, 0xCA);
        eeprom.write_register(0x01, 0xFE);

        I2CDevice pmic;
        pmic.address = 0x68;  // PMIC — TR: güç yönetim IC'si I2C adresi
        pmic.name = "PMIC (Power)";
        pmic.write_register(0x00, 0x01);  // Status: OK — TR: durum: normal

        bus.attach(temp_sensor);
        bus.attach(accel);
        bus.attach(eeprom);
        bus.attach(pmic);

        // EN: I2C bus scan
        // TR: I2C bus taraması — tüm adreslere ACK sorgusu
        auto found = bus.scan();
        std::cout << "\n    I2C Bus scan results:\n";
        std::cout << "    ┌──────────┬──────────────────────────┐\n";
        std::cout << "    │ Address  │ Device                   │\n";
        std::cout << "    ├──────────┼──────────────────────────┤\n";

        std::map<uint8_t, std::string> addr_names = {
            {0x48, "TMP102 (Temperature)"}, {0x1D, "ADXL345 (Accelerometer)"},
            {0x50, "AT24C02 (EEPROM)"},     {0x68, "PMIC (Power Mgmt)"}
        };
        for (auto addr : found) {
            std::cout << "    │ 0x" << std::hex << std::setw(2) << std::setfill('0')
                      << static_cast<int>(addr) << std::dec
                      << "     │ " << std::left << std::setw(24) << addr_names[addr]
                      << std::right << " │ ACK ✓\n";
        }
        std::cout << "    └──────────┴──────────────────────────┘\n";

        // EN: Read temperature sensor
        // TR: Sıcaklık sensörü oku — TMP102, ham değer × 0.5 = °C
        uint8_t temp_val = 0;
        bus.read(0x48, 0x00, temp_val);
        std::cout << "\n    Read TMP102 [0x48] reg 0x00: " << static_cast<int>(temp_val)
                  << " → " << (static_cast<double>(temp_val) * 0.5) << " °C\n";

        // EN: Write EEPROM
        // TR: EEPROM yaz — AT24C02, bayt bazlı yazma/okuma
        bus.write(0x50, 0x10, 0xBE);
        uint8_t ee_val = 0;
        bus.read(0x50, 0x10, ee_val);
        std::cout << "    Write/Read EEPROM [0x50] reg 0x10: 0x"
                  << std::hex << std::setw(2) << std::setfill('0')
                  << static_cast<int>(ee_val) << std::dec << " ✓\n\n";
    }

    // ─── Demo 4: UART Point-to-Point ────────────────────────────────────
    {
        std::cout << "--- Demo 4: UART Point-to-Point Communication ---\n";
        std::cout << "  Scenario: MCU ↔ GPS module via UART\n\n";

        UARTConfig gps_config{9600, 8, UARTParity::None, 1};
        UARTPort mcu_uart("MCU_UART2", gps_config);
        UARTPort gps_uart("GPS_Module", gps_config);

        mcu_uart.print_config();
        gps_uart.print_config();

        // EN: GPS sends NMEA sentence to MCU
        // TR: GPS, MCU'ya NMEA cümlesi gönderir
        gps_uart.send_string("$GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,47.0,M,,*47\r\n");
        gps_uart.connect_to(mcu_uart);

        std::string nmea = mcu_uart.receive_string();
        std::cout << "\n    GPS → MCU: " << nmea.substr(0, nmea.size() - 2) << "\n";  // strip \r\n
                                                                                        // TR: \r\n kırp

        // EN: MCU sends configuration command to GPS
        // TR: MCU, GPS'çe yapılandırma komutu gönderir
        mcu_uart.send_string("$PMTK314,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*28\r\n");
        mcu_uart.connect_to(gps_uart);

        std::string cmd = gps_uart.receive_string();
        std::cout << "    MCU → GPS: " << cmd.substr(0, cmd.size() - 2) << "\n\n";
    }

    // ─── Demo 5: Shared Memory IPC ──────────────────────────────────────
    {
        std::cout << "--- Demo 5: Shared Memory IPC (Multi-Core MCU) ---\n";
        std::cout << "  Scenario: Core0 (Linux) ↔ Core1 (Safety RTOS) via mailbox\n\n";

        SharedMemoryMailbox mailbox;

        // EN: Core0 → Core1: Send control commands
        // TR: Çekirdek0 → Çekirdek1: Kontrol komutları gönder
        mailbox.send_0to1(0x01, {0x10, 0x20, 0x30});  // Start motor — TR: Motor başlat
        mailbox.send_0to1(0x02, {0x00, 0x64});         // Set speed = 100 — TR: Hız = 100 ayarla
        mailbox.send_0to1(0x03, {0x01});                // Enable watchdog — TR: Watchdog etkinleştir

        std::cout << "    Core0 → Core1 (commands sent):\n";

        // EN: Core1 receives and processes
        // TR: Çekirdek1 alır ve işler
        SharedMemoryMailbox::Message msg;
        int count = 0;
        while (mailbox.receive_on_core1(msg)) {
            msg.print("Received on Core1:");
            ++count;
        }
        std::cout << "    Total received: " << count << "\n\n";

        // EN: Core1 → Core0: Send status updates
        // TR: Çekirdek1 → Çekirdek0: Durum güncellemeleri gönder
        mailbox.send_1to0(0x81, {0x00, 0x01, 0x64});  // Status: OK, motor running, speed=100
                                                        // TR: Durum: OK, motor çalışıyor, hız=100
        mailbox.send_1to0(0x82, {0x25, 0x00});         // Temperature: 37°C — TR: Sıcaklık: 37°C

        std::cout << "    Core1 → Core0 (status sent):\n";
        while (mailbox.receive_on_core0(msg)) {
            msg.print("Received on Core0:");
        }

        mailbox.print_stats();
        std::cout << "\n";
    }

    // ─── Demo 6: Message Framing Protocol ───────────────────────────────
    {
        std::cout << "--- Demo 6: Message Framing Protocol (UART) ---\n\n";

        // EN: Frame format: [SYNC=0xAA][VER=0x01][ID][LEN][PAYLOAD...][CHECKSUM]
        // TR: Çerçeve formatı: [SYNC=0xAA][VER=0x01][ID][UZUNLUK][VERİ...][SAĞLAMA]
        std::cout << "  Frame format:\n";
        std::cout << "  ┌──────┬──────┬──────┬──────┬──────────────┬──────────┐\n";
        std::cout << "  │ SYNC │ VER  │ ID   │ LEN  │ PAYLOAD      │ CHECKSUM │\n";
        std::cout << "  │ 0xAA │ 0x01 │ 1B   │ 1B   │ 0-255 bytes  │ XOR      │\n";
        std::cout << "  └──────┴──────┴──────┴──────┴──────────────┴──────────┘\n\n";

        // EN: Build and serialize messages
        // TR: Mesajları oluştur ve serileştir
        auto msg1 = FramedMessage::build(0x10, {0x01, 0x02, 0x03, 0x04});
        msg1.print("Msg1 (MotorCmd):");

        auto msg2 = FramedMessage::build(0x20, {0x00, 0x64, 0x25});
        msg2.print("Msg2 (SensorData):");

        // EN: Serialize → transmit → deserialize
        // TR: Serileştir → ilet → çözümle
        auto bytes1 = msg1.serialize();
        std::cout << "    Serialized:";
        for (auto b : bytes1)
            std::cout << " " << std::hex << std::setw(2) << std::setfill('0')
                      << static_cast<int>(b);
        std::cout << std::dec << "\n";

        FramedMessage decoded;
        bool ok = FramedMessage::deserialize(bytes1, decoded);
        decoded.print("Decoded:");
        std::cout << "    Decode result: " << (ok ? "SUCCESS ✓" : "FAILED ✗") << "\n";

        // EN: Corrupted message
        // TR: Bozulmuş mesaj — checksum hatası tespit edilmeli
        auto bad_bytes = bytes1;
        bad_bytes[4] = 0xFF;  // corrupt payload — TR: veriyi boz (checksum hatası oluştur)
        FramedMessage bad_decoded;
        bool bad_ok = FramedMessage::deserialize(bad_bytes, bad_decoded);
        std::cout << "    Corrupted decode: " << (bad_ok ? "SUCCESS (BAD!)" : "FAILED ✓ (corruption detected)")
                  << "\n\n";
    }

    // ─── Demo 7: Automotive ECU Internal Architecture ───────────────────
    {
        std::cout << "--- Demo 7: Automotive ECU Internal Architecture ---\n\n";
        std::cout << "  Typical ADAS Domain Controller (internal buses):\n\n";
        std::cout << "  ┌───────────────────────────────────────────────────────────┐\n";
        std::cout << "  │ ADAS ECU Board                                            │\n";
        std::cout << "  │                                                            │\n";
        std::cout << "  │  ┌──────────────┐      SPI (50MHz)      ┌─────────────┐  │\n";
        std::cout << "  │  │ Main SoC     │─────────────────────→ │ Safety MCU  │  │\n";
        std::cout << "  │  │ (S32G/TC397) │      I2C (400kHz)     │ (RL78/STM32)│  │\n";
        std::cout << "  │  │              │─────────────────────→ │ ASIL-D      │  │\n";
        std::cout << "  │  │              │      UART (debug)     │             │  │\n";
        std::cout << "  │  │              │─────────────────────→ │             │  │\n";
        std::cout << "  │  │              │                        └─────────────┘  │\n";
        std::cout << "  │  │              │      SPI (20MHz)                        │\n";
        std::cout << "  │  │              │─────────────────────→  SPI Flash (FW)   │\n";
        std::cout << "  │  │              │      I2C (100kHz)                       │\n";
        std::cout << "  │  │              │─────────────────────→  EEPROM (config)  │\n";
        std::cout << "  │  │              │      I2C (100kHz)                       │\n";
        std::cout << "  │  │              │─────────────────────→  PMIC (power)     │\n";
        std::cout << "  │  │              │      SPI (1MHz)                         │\n";
        std::cout << "  │  │              │─────────────────────→  CAN Transceiver  │\n";
        std::cout << "  │  │              │                        (MCP2515/TJA1145)│\n";
        std::cout << "  │  └──────────────┘                                         │\n";
        std::cout << "  │                                                            │\n";
        std::cout << "  │  External Connectors:                                      │\n";
        std::cout << "  │    CAN FD × 2  |  LIN × 1  |  100BASE-T1 × 4  |  MIPI × 2│\n";
        std::cout << "  └───────────────────────────────────────────────────────────┘\n\n";
    }

    std::cout << "============================================\n";
    std::cout << "   End of Inter-MCU Communication\n";
    std::cout << "============================================\n";

    return 0;
}
