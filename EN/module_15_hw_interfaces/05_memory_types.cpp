/**
 * @file module_15_hw_interfaces/05_memory_types.cpp
 * @brief Memory Technologies
 *
 * @details
 * =============================================================================
 * [THEORY: Embedded Memory Technologies]
 * =============================================================================
 *
 * EN: Modern embedded / automotive systems use multiple memory types, each
 *     with different speed, endurance, power, and retention characteristics.
 *
 *     Memory Taxonomy:
 *     ┌───────────────────────────────────────────────────────────────┐
 *     │                        MEMORY                                 │
 *     ├────────────────────┬──────────────────────────────────────────┤
 *     │     VOLATILE       │           NON-VOLATILE                   │
 *     ├────────────────────┼──────────────────────────────────────────┤
 *     │ SRAM  — MCU cache  │ NOR Flash  — code execution (XIP)        │
 *     │ DRAM  — main RAM   │ NAND Flash — mass storage                │
 *     │ DDR4/5             │ EEPROM     — byte-level config storage   │
 *     │                    │ FRAM (FeRAM) — fast NV, high endurance   │
 *     │                    │ MRAM       — emerging NV, fast           │
 *     │                    │ OTP ROM    — one-time programmable       │
 *     └────────────────────┴──────────────────────────────────────────┘
 *
 *     Key Metrics:
 *     ┌──────────────┬───────────┬────────────┬──────────────┬────────────┐
 *     │ Memory       │ Read      │ Write      │ Endurance    │ Retention  │
 *     ├──────────────┼───────────┼────────────┼──────────────┼────────────┤
 *     │ SRAM         │ ~1 ns     │ ~1 ns      │ Unlimited    │ Volatile   │
 *     │ DRAM/DDR     │ ~10 ns    │ ~10 ns     │ Unlimited    │ Volatile   │
 *     │ NOR Flash    │ ~70 ns    │ ~200 μs    │ 100K cycles  │ 20+ years  │
 *     │ NAND Flash   │ ~25 μs    │ ~200 μs    │ 10K-100K     │ 10+ years  │
 *     │ EEPROM       │ ~200 ns   │ ~5 ms      │ 1M cycles    │ 40+ years  │
 *     │ FRAM         │ ~100 ns   │ ~100 ns    │ 10^14 cycles │ 10+ years  │
 *     │ MRAM         │ ~35 ns    │ ~35 ns     │ 10^15 cycles │ 20+ years  │
 *     │ NVMe SSD     │ ~10 μs    │ ~20 μs     │ 3K-100K TBW  │ Power-off  │
 *     └──────────────┴───────────┴────────────┴──────────────┴────────────┘
 *
 * @note Compile: g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion 05_memory_types.cpp -o 05_memory
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
#include <algorithm>
#include <numeric>
#include <cassert>
#include <cstring>
#include <map>

// ═════════════════════════════════════════════════════════════════════════════
// PART 1: EEPROM Simulator (I2C AT24C256-like)
// ═════════════════════════════════════════════════════════════════════════════

// EN: EEPROM characteristics: byte-addressable, slow write, high endurance

class EEPROM {
    static constexpr uint32_t SIZE = 32768;  // 256 Kbit = 32 KB
    static constexpr int PAGE_SIZE = 64;     // page write size
    static constexpr int MAX_CYCLES = 1000000;

    std::array<uint8_t, SIZE> data_;
    std::array<uint32_t, SIZE / PAGE_SIZE> page_wear_;  // write cycles per page
    uint32_t total_writes_ = 0;

public:
    EEPROM() {
        data_.fill(0xFF);  // erased state
        page_wear_.fill(0);
    }

    // EN: Byte read — fast (~200ns on real HW)
    uint8_t read_byte(uint16_t addr) const {
        return data_[addr % SIZE];
    }

    // EN: Sequential read
    std::vector<uint8_t> read(uint16_t addr, uint16_t len) const {
        std::vector<uint8_t> result(len);
        for (uint16_t i = 0; i < len; ++i)
            result[i] = data_[static_cast<uint16_t>((addr + i) % SIZE)];
        return result;
    }

    // EN: Byte write — slow (~5ms on real HW)
    bool write_byte(uint16_t addr, uint8_t val) {
        uint32_t page = (addr % SIZE) / PAGE_SIZE;
        if (page_wear_[page] >= MAX_CYCLES) return false;  // worn out
        data_[addr % SIZE] = val;
        page_wear_[page]++;
        total_writes_++;
        return true;
    }

    // EN: Page write (up to PAGE_SIZE bytes at once)
    bool write_page(uint16_t addr, const uint8_t* buf, int len) {
        if (len > PAGE_SIZE) return false;
        uint32_t page = (addr % SIZE) / PAGE_SIZE;
        if (page_wear_[page] >= MAX_CYCLES) return false;
        for (int i = 0; i < len; ++i)
            data_[static_cast<uint16_t>((addr + i) % SIZE)] = buf[i];
        page_wear_[page]++;
        total_writes_++;
        return true;
    }

    uint32_t page_cycles(uint16_t addr) const {
        return page_wear_[(addr % SIZE) / PAGE_SIZE];
    }

    uint32_t total_writes() const { return total_writes_; }
    static constexpr uint32_t size() { return SIZE; }
    static constexpr int page_size() { return PAGE_SIZE; }
};

// ═════════════════════════════════════════════════════════════════════════════
// PART 2: NOR Flash Simulator (STM32 Internal Flash-like)
// ═════════════════════════════════════════════════════════════════════════════

class NORFlash {
    static constexpr uint32_t SECTOR_SIZE = 4096;  // 4 KB sector
    static constexpr uint32_t NUM_SECTORS = 64;    // 256 KB total
    static constexpr uint32_t TOTAL_SIZE = SECTOR_SIZE * NUM_SECTORS;
    static constexpr uint32_t MAX_ERASE_CYCLES = 100000;

    std::vector<uint8_t> data_;
    std::array<uint32_t, NUM_SECTORS> erase_count_;

public:
    NORFlash() : data_(TOTAL_SIZE, 0xFF) { erase_count_.fill(0); }

    // EN: Read — fast, supports XIP (execute in place)
    uint8_t read(uint32_t addr) const { return data_[addr % TOTAL_SIZE]; }

    // EN: Program (can only change 1→0, not 0→1!)
    bool program(uint32_t addr, uint8_t val) {
        addr %= TOTAL_SIZE;
        // EN: Flash physics: can only clear bits, not set them
        data_[addr] &= val;
        return true;
    }

    // EN: Program word (4 bytes)
    bool program_word(uint32_t addr, uint32_t val) {
        for (int i = 0; i < 4; ++i) {
            program(addr + static_cast<uint32_t>(i),
                    static_cast<uint8_t>((val >> (i * 8)) & 0xFF));
        }
        return true;
    }

    // EN: Sector erase (must erase before re-programming)
    bool erase_sector(uint32_t sector) {
        if (sector >= NUM_SECTORS) return false;
        if (erase_count_[sector] >= MAX_ERASE_CYCLES) return false;

        uint32_t base = sector * SECTOR_SIZE;
        std::fill(data_.begin() + base,
                  data_.begin() + base + SECTOR_SIZE, static_cast<uint8_t>(0xFF));
        erase_count_[sector]++;
        return true;
    }

    uint32_t sector_erase_count(uint32_t sector) const {
        return (sector < NUM_SECTORS) ? erase_count_[sector] : 0;
    }

    static constexpr uint32_t sector_size() { return SECTOR_SIZE; }
    static constexpr uint32_t total_size() { return TOTAL_SIZE; }
};

// ═════════════════════════════════════════════════════════════════════════════
// PART 3: FRAM Simulator (Ferroelectric RAM)
// ═════════════════════════════════════════════════════════════════════════════

// EN: FRAM: fast as SRAM, non-volatile, extremely high endurance (10^14)

class FRAM {
    static constexpr uint32_t SIZE = 8192;  // 64 Kbit = 8 KB

    std::array<uint8_t, SIZE> data_;
    uint64_t write_cycles_ = 0;

public:
    FRAM() { data_.fill(0x00); }

    // EN: Read — fast (~100ns, same as SRAM)
    uint8_t read(uint16_t addr) const { return data_[addr % SIZE]; }

    // EN: Write — fast (~100ns, no page/sector erase needed!)
    void write(uint16_t addr, uint8_t val) {
        data_[addr % SIZE] = val;
        write_cycles_++;
    }

    // EN: Bulk write — no page boundary limitation
    void write_block(uint16_t addr, const uint8_t* buf, uint16_t len) {
        for (uint16_t i = 0; i < len; ++i) {
            data_[static_cast<uint16_t>((addr + i) % SIZE)] = buf[i];
        }
        write_cycles_ += len;
    }

    uint64_t write_cycles() const { return write_cycles_; }
    static constexpr uint32_t size() { return SIZE; }
};

// ═════════════════════════════════════════════════════════════════════════════
// PART 4: NAND Flash + Wear Leveling
// ═════════════════════════════════════════════════════════════════════════════

class NANDFlash {
    static constexpr uint32_t PAGE_SIZE = 2048;
    static constexpr uint32_t PAGES_PER_BLOCK = 64;
    static constexpr uint32_t BLOCK_SIZE = PAGE_SIZE * PAGES_PER_BLOCK;
    static constexpr uint32_t NUM_BLOCKS = 16;
    static constexpr uint32_t MAX_ERASE = 10000;

    struct Block {
        std::vector<uint8_t> data;
        uint32_t erase_count = 0;
        bool bad = false;
        Block() : data(BLOCK_SIZE, 0xFF) {}
    };

    std::array<Block, NUM_BLOCKS> blocks_;

public:
    NANDFlash() = default;

    // EN: Read page
    std::vector<uint8_t> read_page(uint32_t block, uint32_t page) const {
        if (block >= NUM_BLOCKS || page >= PAGES_PER_BLOCK)
            return {};
        auto& blk = blocks_[block];
        if (blk.bad) return {};
        uint32_t offset = page * PAGE_SIZE;
        return {blk.data.begin() + offset,
                blk.data.begin() + offset + PAGE_SIZE};
    }

    // EN: Program page (must be erased first)
    bool program_page(uint32_t block, uint32_t page,
                       const std::vector<uint8_t>& data) {
        if (block >= NUM_BLOCKS || page >= PAGES_PER_BLOCK) return false;
        auto& blk = blocks_[block];
        if (blk.bad) return false;

        uint32_t offset = page * PAGE_SIZE;
        for (size_t i = 0; i < std::min(data.size(), static_cast<size_t>(PAGE_SIZE)); ++i) {
            blk.data[offset + i] &= data[i];  // can only clear bits
        }
        return true;
    }

    // EN: Block erase
    bool erase_block(uint32_t block) {
        if (block >= NUM_BLOCKS) return false;
        auto& blk = blocks_[block];
        if (blk.erase_count >= MAX_ERASE) {
            blk.bad = true;
            return false;
        }
        std::fill(blk.data.begin(), blk.data.end(), static_cast<uint8_t>(0xFF));
        blk.erase_count++;
        return true;
    }

    uint32_t block_erase_count(uint32_t block) const {
        return (block < NUM_BLOCKS) ? blocks_[block].erase_count : 0;
    }

    bool is_bad_block(uint32_t block) const {
        return (block < NUM_BLOCKS) && blocks_[block].bad;
    }

    static constexpr uint32_t page_size() { return PAGE_SIZE; }
    static constexpr uint32_t block_size() { return BLOCK_SIZE; }
    static constexpr uint32_t num_blocks() { return NUM_BLOCKS; }
};

// EN: Simple wear-leveling: always write to least-erased block
class WearLeveler {
    NANDFlash& flash_;

public:
    explicit WearLeveler(NANDFlash& f) : flash_(f) {}

    // EN: Find block with lowest erase count (not bad)
    uint32_t find_best_block() const {
        uint32_t best = 0;
        uint32_t min_wear = UINT32_MAX;
        for (uint32_t b = 0; b < NANDFlash::num_blocks(); ++b) {
            if (!flash_.is_bad_block(b) && flash_.block_erase_count(b) < min_wear) {
                min_wear = flash_.block_erase_count(b);
                best = b;
            }
        }
        return best;
    }

    void print_wear_map() const {
        std::cout << "    Wear Map [";
        for (uint32_t b = 0; b < NANDFlash::num_blocks(); ++b) {
            if (flash_.is_bad_block(b))
                std::cout << " XX";
            else
                std::cout << " " << std::setw(2) << flash_.block_erase_count(b);
        }
        std::cout << " ]\n";
    }
};

// ═════════════════════════════════════════════════════════════════════════════
// MAIN
// ═════════════════════════════════════════════════════════════════════════════
int main() {
    std::cout << "============================================\n";
    std::cout << "   Module 15 - Memory Types & Technologies\n";
    std::cout << "============================================\n\n";

    // ─── Demo 1: EEPROM Read/Write ──────────────────────────────────────
    {
        std::cout << "--- Demo 1: EEPROM (AT24C256) Simulation ---\n\n";

        EEPROM eeprom;

        // EN: Store calibration data
        struct CalibData {
            uint16_t magic = 0xCAFE;
            float gain = 1.234f;
            int16_t offset = -42;
        };

        CalibData cal;
        const auto* bytes = reinterpret_cast<const uint8_t*>(&cal);
        for (size_t i = 0; i < sizeof(cal); ++i) {
            eeprom.write_byte(static_cast<uint16_t>(i), bytes[i]);
        }

        // EN: Read back
        CalibData read_cal{};
        auto data = eeprom.read(0, static_cast<uint16_t>(sizeof(CalibData)));
        std::memcpy(&read_cal, data.data(), sizeof(CalibData));

        std::cout << "  EEPROM: " << EEPROM::size() << " bytes, "
                  << EEPROM::page_size() << " byte pages\n";
        std::cout << "  Written calibration:\n";
        std::cout << "    magic:  0x" << std::hex << read_cal.magic << std::dec << "\n";
        std::cout << "    gain:   " << read_cal.gain << "\n";
        std::cout << "    offset: " << read_cal.offset << "\n";
        std::cout << "    page wear (addr 0): " << eeprom.page_cycles(0) << " cycles\n";
        std::cout << "    total writes: " << eeprom.total_writes() << "\n\n";
    }

    // ─── Demo 2: Flash Program & Erase ──────────────────────────────────
    {
        std::cout << "--- Demo 2: NOR Flash Program & Erase ---\n\n";

        NORFlash flash;

        std::cout << "  NOR Flash: " << NORFlash::total_size() / 1024 << " KB, "
                  << NORFlash::sector_size() << " byte sectors\n\n";

        // EN: Program some data
        flash.program_word(0x1000, 0xDEADBEEF);

        uint32_t readback = 0;
        for (int i = 0; i < 4; ++i)
            readback |= static_cast<uint32_t>(flash.read(0x1000 + static_cast<uint32_t>(i))) << (i * 8);

        std::cout << "  Program 0xDEADBEEF at 0x1000:\n";
        std::cout << "    Read back: 0x" << std::hex << readback << std::dec << " ✓\n\n";

        // EN: Try to overwrite without erasing — can only clear bits!
        std::cout << "  Overwrite without erase (0x12345678 → 0x1000):\n";
        flash.program_word(0x1000, 0x12345678);
        readback = 0;
        for (int i = 0; i < 4; ++i)
            readback |= static_cast<uint32_t>(flash.read(0x1000 + static_cast<uint32_t>(i))) << (i * 8);
        std::cout << "    Read back: 0x" << std::hex << readback << std::dec << "\n";
        std::cout << "    Expected:  0x" << std::hex
                  << (0xDEADBEEFu & 0x12345678u) << std::dec
                  << " (bits can only go 1→0!)\n\n";

        // EN: Proper sequence: erase then program
        flash.erase_sector(0x1000 / NORFlash::sector_size());
        flash.program_word(0x1000, 0x12345678);
        readback = 0;
        for (int i = 0; i < 4; ++i)
            readback |= static_cast<uint32_t>(flash.read(0x1000 + static_cast<uint32_t>(i))) << (i * 8);
        std::cout << "  After erase + re-program:\n";
        std::cout << "    Read back: 0x" << std::hex << readback << std::dec << " ✓\n";
        std::cout << "    Sector erase count: " << flash.sector_erase_count(1) << "\n\n";
    }

    // ─── Demo 3: FRAM Fast NV Storage ────────────────────────────────────
    {
        std::cout << "--- Demo 3: FRAM — Fast Non-Volatile Memory ---\n\n";

        FRAM fram;

        std::cout << "  FRAM: " << FRAM::size() << " bytes\n";
        std::cout << "  Key advantages over EEPROM:\n";
        std::cout << "    - Write speed: ~100 ns (vs ~5 ms for EEPROM)\n";
        std::cout << "    - No page boundaries for writes\n";
        std::cout << "    - Endurance: 10^14 cycles (vs 10^6 for EEPROM)\n";
        std::cout << "    - No erase needed before write\n\n";

        // EN: Write crash counter (simulating power-loss safe storage)
        uint32_t crash_count = 42;
        auto* p = reinterpret_cast<const uint8_t*>(&crash_count);
        fram.write_block(0x100, p, sizeof(crash_count));

        // EN: Read back
        uint32_t read_val = 0;
        for (int i = 0; i < 4; ++i)
            read_val |= static_cast<uint32_t>(fram.read(static_cast<uint16_t>(0x100 + i))) << (i * 8);

        std::cout << "  Written crash_count=42 at 0x100\n";
        std::cout << "  Read back: " << read_val << " ✓\n";
        std::cout << "  Write cycles: " << fram.write_cycles() << "\n\n";

        std::cout << "  Automotive FRAM uses:\n";
        std::cout << "    - Odometer / trip counter (frequent updates)\n";
        std::cout << "    - DTC freeze-frame data (must survive power-loss)\n";
        std::cout << "    - CAN message counters (rolling counters)\n";
        std::cout << "    - Battery management cell history\n\n";
    }

    // ─── Demo 4: NAND Wear Leveling ─────────────────────────────────────
    {
        std::cout << "--- Demo 4: NAND Flash Wear Leveling ---\n\n";

        NANDFlash nand;
        WearLeveler wl(nand);

        std::cout << "  NAND: " << NANDFlash::num_blocks() << " blocks × "
                  << NANDFlash::block_size() / 1024 << " KB = "
                  << NANDFlash::num_blocks() * NANDFlash::block_size() / 1024 << " KB\n\n";

        // EN: Without wear leveling: hammer block 0
        std::cout << "  Without wear leveling (always use block 0):\n";
        for (int i = 0; i < 5; ++i) nand.erase_block(0);
        wl.print_wear_map();

        // EN: With wear leveling: spread writes
        std::cout << "\n  With wear leveling (least-erased block):\n";
        for (int i = 0; i < 20; ++i) {
            uint32_t best = wl.find_best_block();
            nand.erase_block(best);
        }
        wl.print_wear_map();
        std::cout << "\n";
    }

    // ─── Demo 5: Memory Map — Automotive ECU ────────────────────────────
    {
        std::cout << "--- Demo 5: Automotive ECU Memory Map ---\n\n";

        std::cout << "  ┌─────────────────┬──────────────┬───────────────────────────────┐\n";
        std::cout << "  │ Address Range   │ Size         │ Memory / Purpose              │\n";
        std::cout << "  ├─────────────────┼──────────────┼───────────────────────────────┤\n";
        std::cout << "  │ 0x0000_0000     │ 2 MB         │ Internal Flash (Code)         │\n";
        std::cout << "  │ 0x0800_0000     │ 256 KB       │ Internal Flash (Calibration)  │\n";
        std::cout << "  │ 0x1000_0000     │ 64 KB        │ OTP Memory (keys, serial#)    │\n";
        std::cout << "  │ 0x2000_0000     │ 512 KB       │ SRAM (stack, heap, globals)   │\n";
        std::cout << "  │ 0x2008_0000     │ 128 KB       │ SRAM2 (DMA buffers)           │\n";
        std::cout << "  │ 0x4000_0000     │ -            │ Peripheral registers          │\n";
        std::cout << "  │ 0x6000_0000     │ 4 MB         │ External QSPI Flash (maps)    │\n";
        std::cout << "  │ 0x8000_0000     │ 32 KB        │ External EEPROM (I2C)         │\n";
        std::cout << "  │ 0x9000_0000     │ 8 KB         │ External FRAM (SPI)           │\n";
        std::cout << "  │ 0xE000_0000     │ -            │ Cortex-M System (NVIC, etc.)  │\n";
        std::cout << "  └─────────────────┴──────────────┴───────────────────────────────┘\n\n";

        std::cout << "  Data Placement Strategy:\n";
        std::cout << "    ┌───────────────────────────────┬─────────────┐\n";
        std::cout << "    │ Data Type                     │ Storage     │\n";
        std::cout << "    ├───────────────────────────────┼─────────────┤\n";
        std::cout << "    │ Firmware code                 │ NOR Flash   │\n";
        std::cout << "    │ Bootloader                    │ NOR Flash   │\n";
        std::cout << "    │ Calibration parameters        │ NOR Flash   │\n";
        std::cout << "    │ DTC / Freeze frame            │ EEPROM/FRAM │\n";
        std::cout << "    │ Odometer / trip meter         │ FRAM        │\n";
        std::cout << "    │ Security keys                 │ OTP / HSM   │\n";
        std::cout << "    │ Navigation maps               │ QSPI Flash  │\n";
        std::cout << "    │ Runtime variables             │ SRAM        │\n";
        std::cout << "    │ DMA transfer buffers          │ SRAM2       │\n";
        std::cout << "    │ OTA update staging            │ Ext. Flash  │\n";
        std::cout << "    └───────────────────────────────┴─────────────┘\n\n";
    }

    // ─── Demo 6: ECC & Data Integrity ────────────────────────────────────
    {
        std::cout << "--- Demo 6: ECC & Data Integrity ---\n\n";

        // EN: Simple parity-based ECC simulation
        auto calc_parity = [](const std::vector<uint8_t>& data) -> uint8_t {
            uint8_t parity = 0;
            for (auto b : data) parity ^= b;
            return parity;
        };

        std::vector<uint8_t> orig_data = {0x12, 0x34, 0x56, 0x78, 0x9A};
        uint8_t ecc = calc_parity(orig_data);

        std::cout << "  Original data: ";
        for (auto b : orig_data) std::cout << std::hex << "0x" << static_cast<int>(b) << " ";
        std::cout << "\n  ECC (XOR parity): 0x" << static_cast<int>(ecc) << std::dec << "\n\n";

        // EN: Simulate single-bit corruption
        auto corrupted = orig_data;
        corrupted[2] ^= 0x04;  // flip bit 2 of byte 2

        uint8_t ecc_check = calc_parity(corrupted);
        std::cout << "  Corrupted data: ";
        for (auto b : corrupted) std::cout << std::hex << "0x" << static_cast<int>(b) << " ";
        std::cout << "\n  ECC check: 0x" << static_cast<int>(ecc_check)
                  << " (expected: 0x" << static_cast<int>(ecc) << ")\n"
                  << std::dec;
        std::cout << "  " << ((ecc_check != ecc) ? "CORRUPTION DETECTED ✓" : "OK") << "\n\n";

        std::cout << "  Memory ECC in Automotive:\n";
        std::cout << "  ┌────────────────────┬────────────┬──────────────────────┐\n";
        std::cout << "  │ Memory             │ ECC Type   │ ASIL Requirement     │\n";
        std::cout << "  ├────────────────────┼────────────┼──────────────────────┤\n";
        std::cout << "  │ Internal SRAM      │ SECDED     │ ASIL-B and above     │\n";
        std::cout << "  │ Flash (code)       │ SECDED/BCH │ ASIL-B and above     │\n";
        std::cout << "  │ External DDR       │ ECC-DIMM   │ ASIL-D (autonomous)  │\n";
        std::cout << "  │ External NAND      │ BCH/LDPC   │ Managed by FTL       │\n";
        std::cout << "  │ Cache              │ Parity/ECC │ ASIL-D               │\n";
        std::cout << "  └────────────────────┴────────────┴──────────────────────┘\n";
        std::cout << "  SECDED = Single Error Correct, Double Error Detect\n\n";
    }

    // ─── Demo 7: Memory Comparison Table ─────────────────────────────────
    {
        std::cout << "--- Demo 7: Complete Memory Comparison ---\n\n";

        std::cout << "  ┌───────────┬────────┬──────────┬───────────────┬────────────┬──────────────┐\n";
        std::cout << "  │ Type      │ Vola.  │ Speed    │ Endurance     │ Erase Unit │ Best For     │\n";
        std::cout << "  ├───────────┼────────┼──────────┼───────────────┼────────────┼──────────────┤\n";
        std::cout << "  │ SRAM      │ Yes    │ ~1 ns    │ Unlimited     │ N/A        │ Cache, stack │\n";
        std::cout << "  │ DRAM/DDR  │ Yes    │ ~10 ns   │ Unlimited     │ N/A        │ Main memory  │\n";
        std::cout << "  │ NOR Flash │ No     │ 70 ns R  │ 100K cycles   │ Sector     │ Code, XIP    │\n";
        std::cout << "  │ NAND Flash│ No     │ 25 μs R  │ 10K-100K      │ Block      │ Mass storage │\n";
        std::cout << "  │ EEPROM    │ No     │ 200 ns R │ 1M cycles     │ Byte       │ Config data  │\n";
        std::cout << "  │ FRAM      │ No     │ 100 ns   │ 10^14 cycles  │ Byte       │ Counters, DTC│\n";
        std::cout << "  │ MRAM      │ No     │ 35 ns    │ 10^15+ cycles │ Byte       │ Next-gen NVM │\n";
        std::cout << "  │ OTP ROM   │ No     │ Fast     │ 1 write       │ N/A        │ Keys, serial │\n";
        std::cout << "  │ NVMe SSD  │ No     │ 10 μs    │ 3K-100K TBW   │ Block/page │ ADAS logging │\n";
        std::cout << "  └───────────┴────────┴──────────┴───────────────┴────────────┴──────────────┘\n";
    }

    std::cout << "\n============================================\n";
    std::cout << "   End of Memory Types & Technologies\n";
    std::cout << "============================================\n";

    return 0;
}
