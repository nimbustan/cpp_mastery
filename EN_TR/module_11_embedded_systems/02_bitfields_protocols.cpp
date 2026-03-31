/**
 * @file module_11_embedded_systems/02_bitfields_protocols.cpp
 * @brief Embedded & Systems: Bitfields, Protocol Parsing, Packed Structs — Bit Alanları,
 * Protokol Ayrıştırma, Paketlenmiş Yapılar
 *
 * @details
 * =============================================================================
 * [THEORY: Bitfields — Packing Data into Minimal Space / TEORİ: Bit Alanları — Veriyi Minimum
 * Alana Paketleme]
 * =============================================================================
 * EN: C++ bitfields allow struct members to occupy a specific number of BITS instead of whole
 * bytes. `uint8_t flag : 1;` uses exactly 1 bit. This is critical in embedded systems where
 * every byte matters: network protocol headers, hardware register layouts, command packets over
 * CAN/SPI/I2C buses. A 32-bit register might contain 8 different fields packed into one word.
 *
 * TR: C++ bit alanları, yapı üyelerinin tam baytlar yerine belirli sayıda BİT kaplamasına izin
 * verir. `uint8_t flag : 1;` tam olarak 1 bit kullanır. Bu, her baytın önemli olduğu gömülü
 * sistemlerde kritiktir: ağ protokolü başlıkları, donanım kayıt düzenleri, CAN/SPI/I2C veri
 * yolları üzerinden komut paketleri. Bir 32-bit kayıt, tek bir kelimeye paketlenmiş 8 farklı
 * alan içerebilir.
 *
 * =============================================================================
 * [THEORY: Protocol Parsing with Structs / TEORİ: Yapılarla Protokol Ayrıştırma]
 * =============================================================================
 * EN: Network protocols (TCP/IP, CAN, USB) define exact byte layouts for packets. In C++, we can
 * overlay a struct onto raw byte buffers to parse them directly — zero-copy parsing! The struct
 * fields map directly to protocol fields. This is dramatically faster than parsing byte-by-byte.
 * Requirements: `#pragma pack(1)` or `__attribute__((packed))` to disable compiler padding.
 *
 * TR: Ağ protokolleri (TCP/IP, CAN, USB) paketler için kesin bayt düzenleri tanımlar. C++'ta ham
 * bayt tamponlarına bir yapı bindirerek bunları doğrudan ayrıştırabiliriz — sıfır-kopya
 * ayrıştırma! Yapı alanları doğrudan protokol alanlarına eşlenir. Bu, bayt bayt ayrıştırmadan
 * dramatik şekilde hızlıdır. Gereksinimler: derleyici dolgusunu devre dışı bırakmak için
 * `#pragma pack(1)` veya `__attribute__((packed))`.
 *
 * =============================================================================
 * [THEORY: Endianness — Byte Order Matters / TEORİ: Endianness — Bayt Sırası Önemlidir]
 * =============================================================================
 * EN: CPUs store multi-byte values in either "Little-Endian" (LSB first, x86, ARM) or
 * "Big-Endian" (MSB first, network order). When parsing network protocols, you MUST convert
 * between network byte order (big-endian) and host byte order using `htons()/ntohs()` (16-bit)
 * and `htonl()/ntohl()` (32-bit). Ignoring endianness corrupts multi-byte fields across
 * platforms.
 *
 * TR: CPU'lar çok baytlı değerleri "Little-Endian" (LSB önce, x86, ARM) veya "Big-Endian" (MSB
 * önce, ağ sırası) olarak depolar. Ağ protokollerini ayrıştırırken, ağ bayt sırası (big-endian)
 * ile ana makine bayt sırası arasında `htons()/ntohs()` (16-bit) ve `htonl()/ntohl()` (32-bit)
 * kullanarak dönüştürmeniz ZORUNLUDUR. Endianness'ı görmezden gelmek platformlar arasında çok
 * baytlı alanları bozar.
 *
 * =============================================================================
 * [CPPREF DEPTH: Implementation-Defined Behavior of Bitfields / CPPREF DERİNLİK: Bit Alanlarının
 * Uygulamaya Bağlı Davranışı]
 * =============================================================================
 * EN: The C++ standard leaves several bitfield aspects implementation-defined:
 *     1. Layout ordering (MSB-first or LSB-first within a byte).
 *     2. Whether bitfields can span storage unit boundaries.
 *     3. Alignment of bitfield storage units.
 *     This means bitfield layout is NOT portable across compilers!
 *     For cross-platform protocol parsing, prefer explicit bit manipulation
 *     (`& mask`, `>> shift`) over bitfields.
 *     ⚠  cppreference.com/w/cpp/language/bit_field
 *
 * TR: C++ standardı birçok bit alanı yönünü uygulamaya bağlı bırakır:
 *     1. Düzen sıralaması (bir bayt içinde MSB-önce veya LSB-önce).
 *     2. Bit alanlarının depolama birimi sınırlarını geçip geçemeyeceği.
 *     3. Bit alanı depolama birimlerinin hizalaması.
 *     Bu, bit alanı düzeninin derleyiciler arasında TAŞINABİLİR OLMADIĞI
 *     anlamına gelir! Platformlar arası protokol ayrıştırma için bit alanları
 *     yerine açık bit manipülasyonu (`& maske`, `>> kaydırma`) tercih edin.
 *     ⚠  cppreference.com/w/cpp/language/bit_field
 *
 * =============================================================================
 * [CPPREF DEPTH: std::byte and Type-Safe Byte Manipulation (C++17) / CPPREF DERİNLİK: std::byte
 * ve Tip-Güvenli Bayt Manipülasyonu (C++17)]
 * =============================================================================
 * EN: C++17 introduced `std::byte` as a distinct type for raw byte manipulation. Unlike `char`
 * or `unsigned char`, `std::byte` only supports bitwise operations — no arithmetic! This
 * prevents accidentally treating raw bytes as numbers. `std::byte b{0xFF}; auto v =
 * std::to_integer<uint8_t>(b);` cppreference.com/w/cpp/types/byte
 *
 * TR: C++17, ham bayt manipülasyonu için ayrı bir tip olarak `std::byte`'ı tanıttı. `char` veya
 * `unsigned char`'dan farklı olarak `std::byte` yalnızca bitsel işlemleri destekler — aritmetik
 * yok! Bu, ham baytların yanlışlıkla sayı olarak işlenmesini önler. `std::byte b{0xFF}; auto v =
 * std::to_integer<uint8_t>(b);` cppreference.com/w/cpp/types/byte
 *
 * @note Compile: g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion 02_bitfields_protocols.cpp -o 02_bitfields_protocols
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <cstdint>
#include <cstring>
#include <iomanip>
#include <iostream>

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// [1. BITFIELD DEMO — Hardware Register Layout]
// ═════════════════════════════════════════════════════════════════════════════════════════════════

// EN: Simulates a UART control register with bitfields.
// TR: Bit alanlarıyla bir UART kontrol kaydını simüle eder.
struct UART_CR {
  uint32_t UE : 1;     // UART Enable
  uint32_t RE : 1;     // Receiver Enable
  uint32_t TE : 1;     // Transmitter Enable
  uint32_t IDLEIE : 1; // IDLE Interrupt Enable
  uint32_t RXNEIE : 1; // RX Not Empty Interrupt Enable
  uint32_t TCIE : 1;   // Transmission Complete Interrupt
  uint32_t TXEIE : 1;  // TX Empty Interrupt
  uint32_t PEIE : 1;   // Parity Error Interrupt
  uint32_t PS : 1;     // Parity Selection (0=Even, 1=Odd)
  uint32_t PCE : 1;    // Parity Control Enable
  uint32_t reserved : 22; // Remaining bits
};

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// [2. PACKED STRUCT — Protocol Header (CAN Bus Frame)]
// ═════════════════════════════════════════════════════════════════════════════════════════════════

// EN: #pragma pack(1) prevents compiler from inserting padding bytes.
// TR: #pragma pack(1) derleyicinin dolgu baytları eklemesini önler.
#pragma pack(push, 1)
struct CANFrame {
  uint32_t id : 11;     // CAN Standard ID (11 bits)
  uint32_t rtr : 1;     // Remote Transmission Request
  uint32_t ide : 1;     // Identifier Extension
  uint32_t dlc : 4;     // Data Length Code (0-8)
  uint32_t reserved : 15;
  uint8_t data[8];      // Payload (max 8 bytes)
};
#pragma pack(pop)

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// [3. MANUAL BIT MANIPULATION — Portable Protocol Parsing]
// ═════════════════════════════════════════════════════════════════════════════════════════════════

// EN: Explicit bit manipulation is portable across all compilers.
// TR: Açık bit manipülasyonu tüm derleyicilerde taşınabilirdir.
struct SensorPacket {
  uint8_t raw[4]; // 4 bytes of raw data

  // EN: Extract sensor ID (bits 31:24 of first word)
  // TR: Sensör kimliği çıkar (ilk kelimenin bitleri 31:24)
  uint8_t getSensorId() const { return raw[0]; }

  // EN: Extract temperature (bits 23:12, 12-bit signed value)
  // TR: Sıcaklık çıkar (bitler 23:12, 12-bit işaretli değer)
  int16_t getTemperature() const {
    uint16_t val =
        static_cast<uint16_t>((static_cast<uint16_t>(raw[1]) << 4) |
                               (static_cast<uint16_t>(raw[2]) >> 4));
    // EN: Sign extend 12-bit to 16-bit
    // TR: 12-bit'i 16-bit'e işaret genişlet
    if (val & 0x800U) val |= 0xF000U;
    return static_cast<int16_t>(val);
  }

  // EN: Extract battery level (bits 11:4, 8-bit value, 0-255)
  // TR: Pil seviyesi çıkar (bitler 11:4, 8-bit değer, 0-255)
  uint8_t getBatteryLevel() const {
    return static_cast<uint8_t>(((raw[2] & 0x0FU) << 4) |
                                 (raw[3] >> 4));
  }

  // EN: Extract alert flags (bits 3:0)
  // TR: Uyarı bayrakları çıkar (bitler 3:0)
  uint8_t getAlertFlags() const {
    return static_cast<uint8_t>(raw[3] & 0x0FU);
  }
};

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// [4. ENDIANNESS DEMONSTRATION]
// ═════════════════════════════════════════════════════════════════════════════════════════════════

bool isLittleEndian() {
  uint16_t val = 0x0001;
  // EN: If first byte is 0x01, the LSB is stored first (little-endian).
  // TR: İlk bayt 0x01 ise, LSB önce depolanır (little-endian).
  uint8_t firstByte = 0;
  std::memcpy(&firstByte, &val, 1);
  return firstByte == 0x01;
}

uint16_t swapBytes16(uint16_t val) {
  return static_cast<uint16_t>((val >> 8) | (val << 8));
}

uint32_t swapBytes32(uint32_t val) {
  return ((val >> 24) & 0xFFU) | ((val >> 8) & 0xFF00U) |
         ((val << 8) & 0xFF0000U) | ((val << 24) & 0xFF000000U);
}

int main() {
  std::cout << "=== MODULE 11: BITFIELDS & PROTOCOL PARSING ===\n" << std::endl;

  // --- Demo 1: Bitfield Register ---
  std::cout << "--- DEMO 1: UART Control Register (Bitfield) ---\n" << std::endl;
  UART_CR uart{};
  uart.UE = 1;    // Enable UART
  uart.RE = 1;    // Enable receiver
  uart.TE = 1;    // Enable transmitter
  uart.RXNEIE = 1; // Enable RX interrupt

  std::cout << "UART Enable:    " << uart.UE << std::endl;
  std::cout << "Receiver:       " << uart.RE << std::endl;
  std::cout << "Transmitter:    " << uart.TE << std::endl;
  std::cout << "RX Interrupt:   " << uart.RXNEIE << std::endl;
  std::cout << "sizeof(UART_CR)=" << sizeof(UART_CR) << " bytes\n" << std::endl;

  // --- Demo 2: CAN Frame ---
  std::cout << "--- DEMO 2: CAN Bus Frame (Packed Struct) ---\n" << std::endl;
  CANFrame frame{};
  frame.id = 0x123;    // Standard CAN ID
  frame.rtr = 0;       // Data frame (not remote)
  frame.ide = 0;       // Standard (not extended)
  frame.dlc = 3;       // 3 bytes of data
  frame.data[0] = 0xAA;
  frame.data[1] = 0xBB;
  frame.data[2] = 0xCC;

  std::cout << "CAN ID:   0x" << std::hex << frame.id << std::endl;
  std::cout << "DLC:      " << std::dec << frame.dlc << std::endl;
  std::cout << "Data:     ";
  for (int i = 0; i < frame.dlc; i++) {
    std::cout << "0x" << std::hex << std::setw(2) << std::setfill('0')
              << static_cast<int>(frame.data[i]) << " ";
  }
  std::cout << std::dec << "\nsizeof(CANFrame)=" << sizeof(CANFrame)
            << " bytes\n"
            << std::endl;

  // --- Demo 3: Manual Bit Parsing ---
  std::cout << "--- DEMO 3: Portable Bit Manipulation ---\n" << std::endl;
  SensorPacket packet{};
  // EN: Sensor ID=0x42, Temp=+25 (0x019), Battery=200, Alerts=0x05
  // TR: Sensör ID=0x42, Sıcaklık=+25 (0x019), Pil=200, Uyarılar=0x05
  packet.raw[0] = 0x42;
  packet.raw[1] = 0x01;
  packet.raw[2] = 0x9C;
  packet.raw[3] = 0x85;

  std::cout << "Sensor ID:     0x" << std::hex
            << static_cast<int>(packet.getSensorId()) << std::dec << std::endl;
  std::cout << "Temperature:   " << packet.getTemperature() << " C" << std::endl;
  std::cout << "Battery:       " << static_cast<int>(packet.getBatteryLevel())
            << "/255" << std::endl;
  std::cout << "Alert Flags:   0x" << std::hex
            << static_cast<int>(packet.getAlertFlags()) << std::dec << "\n"
            << std::endl;

  // --- Demo 4: Endianness ---
  std::cout << "--- DEMO 4: Endianness ---\n" << std::endl;
  std::cout << "System is: "
            << (isLittleEndian() ? "Little-Endian" : "Big-Endian") << std::endl;
  uint16_t val16 = 0x1234;
  uint32_t val32 = 0xDEADBEEF;
  std::cout << "0x" << std::hex << val16 << " swapped = 0x"
            << swapBytes16(val16) << std::endl;
  std::cout << "0x" << val32 << " swapped = 0x" << swapBytes32(val32)
            << std::dec << std::endl;

  std::cout << "\n=> Bitfields, packed structs, protocol parsing, and "
               "endianness demonstrated!" << std::endl;

  return 0;
}
