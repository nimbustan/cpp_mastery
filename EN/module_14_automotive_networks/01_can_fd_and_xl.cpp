/**
 * @file module_14_automotive_networks/01_can_fd_and_xl.cpp
 * @brief CAN FD & CAN XL — Modern CAN Protocols
 *
 * @details
 * =============================================================================
 * =============================================================================
 *
 * EN: CAN FD (ISO 11898-1:2015) extends Classic CAN with two key improvements:
 *     1) Higher data rates: arbitration at 500 kbps, data phase up to 8 Mbps
 *     2) Larger payloads: up to 64 bytes (vs 8 bytes in Classic CAN)
 *
 *     ┌─────────────────────────┬──────────────┬──────────────┬────────────────┐
 *     │ Feature                 │ Classic CAN  │ CAN FD       │ CAN XL         │
 *     ├─────────────────────────┼──────────────┼──────────────┼────────────────┤
 *     │ Max Data (bytes)        │ 8            │ 64           │ 2048           │
 *     │ Max Bitrate (data)      │ 1 Mbps       │ 8 Mbps       │ 20 Mbps        │
 *     │ Arbitration             │ ID-based     │ ID-based     │ ID + priority  │
 *     │ CRC Field               │ CRC-15       │ CRC-17/CRC-21│ CRC-32         │
 *     │ Error Handling          │ Error frame  │ Error frame  │ Error frame    │
 *     │ Backward Compat.        │ —            │ Classic CAN  │ CAN FD+Classic │
 *     │ ISO Standard            │ 11898-1:2003 │ 11898-1:2015 │ CiA 610-1      │
 *     │ Typical Use             │ Body, Pwtrain│ ADAS, GW     │ Eth bridge     │
 *     └─────────────────────────┴──────────────┴──────────────┴────────────────┘
 *
 *     CAN FD Frame format (key fields):
 *     ┌─────┬──────┬─────┬─────┬─────┬──────────┬──────────────┬─────┬─────┐
 *     │ SOF │ Arb  │ IDE │ FDF │ BRS │ ESI      │ DLC → Data   │ CRC │ EOF │
 *     │ 1b  │ 11/29│ 1b  │ 1b  │ 1b  │ 1b       │ 0-64 bytes   │17/21│ 7b  │
 *     └─────┴──────┴─────┴─────┴─────┴──────────┴──────────────┴─────┴─────┘
 *     FDF = FD Format (1=FD, 0=Classic)
 *     BRS = Bit Rate Switch (1=higher speed for data phase)
 *     ESI = Error State Indicator (0=active, 1=passive)
 *
 * =============================================================================
 * =============================================================================
 *
 * EN: CAN XL (CiA 610-1, 2024) bridges CAN and Ethernet worlds:
 *     - Up to 2048 bytes payload
 *     - Up to 20 Mbps data rate
 *     - SDU Type field enables Ethernet frame tunneling
 *     - Priority-based arbitration (0=highest)
 *     - Designed for zonal architecture backbone
 *
 * =============================================================================
 * =============================================================================
 *
 * EN: DBC (DataBase CAN) files define signal layout within CAN messages.
 *     Used by Vector CANdb++, PEAK Symbol Editor, Kayak, cantools (Python).
 *
 *     Example DBC entry:
 *       BO_ 0x123 EngineData: 8 Engine_ECU
 *         SG_ RPM : 0|16@1+ (0.25,0) [0|16383.75] "rpm" Gateway
 *         SG_ Temp : 16|8@1+ (1,-40) [-40|215] "degC" Gateway
 *
 *     Format: SG_ <name> : <start_bit>|<length>@<byte_order><sign>
 *             (<factor>,<offset>) [<min>|<max>] "<unit>" <receivers>
 *     byte_order: 1=little-endian(Intel), 0=big-endian(Motorola)
 *
 * =============================================================================
 * =============================================================================
 *
 * EN: SocketCAN is the Linux kernel's CAN subsystem. It provides:
 *     - Standard socket API for CAN (PF_CAN address family)
 *     - Virtual CAN interface (vcan) for testing without hardware
 *     - CAN filters (mask + id) for selective reception
 *     - Support for classic CAN, CAN FD, and raw frames
 *
 *     Setup: sudo modprobe vcan
 *            sudo ip link add dev vcan0 type vcan
 *            sudo ip link set up vcan0
 *
 */

#include <iostream>
#include <cstdint>
#include <cstring>
#include <array>
#include <vector>
#include <string>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <functional>
#include <map>
#include <cassert>

// ═════════════════════════════════════════════════════════════════════════════
// PART 1: CAN Frame Structures — Classic CAN, CAN FD, CAN XL
// ═════════════════════════════════════════════════════════════════════════════

// EN: DLC (Data Length Code) to actual byte count for CAN FD
constexpr uint8_t dlc_to_len(uint8_t dlc) {
    constexpr uint8_t table[] = {0,1,2,3,4,5,6,7,8,12,16,20,24,32,48,64};
    return (dlc < 16) ? table[dlc] : 64;
}

// EN: Reverse: byte count to DLC
constexpr uint8_t len_to_dlc(uint8_t len) {
    if (len <= 8) return len;
    if (len <= 12) return 9;
    if (len <= 16) return 10;
    if (len <= 20) return 11;
    if (len <= 24) return 12;
    if (len <= 32) return 13;
    if (len <= 48) return 14;
    return 15;  // 49-64 bytes
}

// EN: Classic CAN frame (8 bytes max)
struct ClassicCANFrame {
    uint32_t id       = 0;
    bool     extended = false;
    bool     rtr      = false; // Remote Transmission Request — veri yerine uzak istek
    uint8_t  dlc      = 0;
    std::array<uint8_t, 8> data{};

    void print(const std::string& label) const {
        std::cout << "  [Classic CAN] " << label << "\n";
        std::cout << "    ID: 0x" << std::hex << std::setw(3) << std::setfill('0') << id
                  << std::dec << (extended ? " (EXT 29-bit)" : " (STD 11-bit)")
                  << " | DLC: " << static_cast<int>(dlc)
                  << " | RTR: " << (rtr ? "Yes" : "No") << "\n";
        if (!rtr && dlc > 0) {
            std::cout << "    Data:";
            for (uint8_t i = 0; i < dlc && i < 8; ++i)
                std::cout << " " << std::hex << std::setw(2) << std::setfill('0')
                          << static_cast<int>(data[i]);
            std::cout << std::dec << "\n";
        }
    }
};

// EN: CAN FD frame (up to 64 bytes, dual bit rate)
struct CANFDFrame {
    uint32_t id       = 0;
    bool     extended = false;
    bool     brs      = false;
    bool     esi      = false;
    uint8_t  dlc      = 0;
    std::array<uint8_t, 64> data{};

    uint8_t data_length() const { return dlc_to_len(dlc); }

    void print(const std::string& label) const {
        std::cout << "  [CAN FD] " << label << "\n";
        std::cout << "    ID: 0x" << std::hex << std::setw(3) << std::setfill('0') << id
                  << std::dec << (extended ? " (EXT)" : " (STD)")
                  << " | DLC: " << static_cast<int>(dlc)
                  << " (" << static_cast<int>(data_length()) << " bytes)"
                  << " | BRS: " << (brs ? "ON" : "OFF")
                  << " | ESI: " << (esi ? "Passive" : "Active") << "\n";
        uint8_t len = data_length();
        if (len > 0) {
            std::cout << "    Data:";
            for (uint8_t i = 0; i < len; ++i) {
                if (i > 0 && i % 16 == 0) std::cout << "\n          ";
                std::cout << " " << std::hex << std::setw(2) << std::setfill('0')
                          << static_cast<int>(data[i]);
            }
            std::cout << std::dec << "\n";
        }
    }
};

// EN: CAN XL frame (up to 2048 bytes, priority-based)
struct CANXLFrame {
    uint8_t  priority    = 0;
    uint32_t acceptance_field = 0;
    uint8_t  sdu_type    = 0;
    uint16_t data_length = 0;
    std::vector<uint8_t> data;

    static constexpr uint8_t SDU_CAN_FD    = 0x03;
    static constexpr uint8_t SDU_ETHERNET  = 0x05;
    static constexpr uint8_t SDU_RAW       = 0x01;

    void print(const std::string& label) const {
        std::cout << "  [CAN XL] " << label << "\n";
        std::cout << "    Priority: " << static_cast<int>(priority)
                  << " | AF: 0x" << std::hex << acceptance_field << std::dec
                  << " | SDU Type: 0x" << std::hex
                  << static_cast<int>(sdu_type) << std::dec
                  << " | Length: " << data_length << " bytes\n";
        if (!data.empty()) {
            std::cout << "    Data (first 32):";
            size_t show = std::min(data.size(), size_t{32});
            for (size_t i = 0; i < show; ++i) {
                if (i > 0 && i % 16 == 0) std::cout << "\n                    ";
                std::cout << " " << std::hex << std::setw(2) << std::setfill('0')
                          << static_cast<int>(data[i]);
            }
            if (data.size() > 32) std::cout << " ... (" << std::dec << data.size() << " total)";
            std::cout << std::dec << "\n";
        }
    }
};

// ═════════════════════════════════════════════════════════════════════════════
// PART 2: CRC Calculation — CAN FD CRC-17 & CRC-21
// ═════════════════════════════════════════════════════════════════════════════

// EN: CAN FD uses CRC-17 (for ≤16 bytes) or CRC-21 (for >16 bytes).
//     Polynomial CRC-17: x^17 + x^16 + x^14 + x^13 + x^11 + x^6 + x^4 + x^3 + x^1 + 1
//                        = 0x3685B
//     Polynomial CRC-21: x^21 + x^20 + x^13 + x^11 + x^7 + x^4 + x^3 + x^1 + 1
//                        = 0x302899

uint32_t crc17_canfd(const uint8_t* data_bytes, size_t len) {
    // EN: Simplified bit-by-bit CRC-17 calculation for educational purposes
    uint32_t crc = 0;
    constexpr uint32_t poly = 0x3685B;
    for (size_t i = 0; i < len; ++i) {
        crc ^= (static_cast<uint32_t>(data_bytes[i]) << 9); // align to bit 16
        for (int bit = 0; bit < 8; ++bit) {
            if (crc & 0x10000)  // bit 16 set?
                crc = (crc << 1) ^ poly;
            else
                crc <<= 1;
        }
    }
    return crc & 0x1FFFF;  // mask to 17 bits
}

uint32_t crc21_canfd(const uint8_t* data_bytes, size_t len) {
    uint32_t crc = 0;
    constexpr uint32_t poly = 0x302899;
    for (size_t i = 0; i < len; ++i) {
        crc ^= (static_cast<uint32_t>(data_bytes[i]) << 13); // align to bit 20
        for (int bit = 0; bit < 8; ++bit) {
            if (crc & 0x100000)  // bit 20 set?
                crc = (crc << 1) ^ poly;
            else
                crc <<= 1;
        }
    }
    return crc & 0x1FFFFF;  // mask to 21 bits
}

// EN: Auto-select CRC based on CAN FD data length
uint32_t canfd_crc(const CANFDFrame& frame) {
    uint8_t len = frame.data_length();
    if (len <= 16) {
        return crc17_canfd(frame.data.data(), len);
    } else {
        return crc21_canfd(frame.data.data(), len);
    }
}

// ═════════════════════════════════════════════════════════════════════════════
// PART 3: CAN Arbitration Simulation
// ═════════════════════════════════════════════════════════════════════════════

// EN: CAN uses bitwise arbitration — lowest ID wins (dominant='0' wins over recessive='1').
//     This is non-destructive: losing nodes detect their recessive bit was overwritten.

struct ArbitrationEntry {
    uint32_t id;
    std::string ecu_name;
    std::string message_name;
};

// EN: Simulate bitwise arbitration for N ECUs competing simultaneously
ArbitrationEntry simulate_arbitration(std::vector<ArbitrationEntry>& entries) {
    // EN: Sort by ID (lowest wins — dominant)
    std::sort(entries.begin(), entries.end(),
              [](const ArbitrationEntry& a, const ArbitrationEntry& b) {
                  return a.id < b.id;
              });

    std::cout << "  Arbitration race (bit-by-bit):\n";
    for (const auto& e : entries) {
        std::cout << "    ECU [" << e.ecu_name << "] ID=0x"
                  << std::hex << std::setw(3) << std::setfill('0') << e.id
                  << std::dec << " (" << e.message_name << ")\n";
    }
    std::cout << "  Winner: " << entries[0].ecu_name
              << " (lowest ID 0x" << std::hex << entries[0].id << std::dec << ")\n";
    return entries[0];
}

// ═════════════════════════════════════════════════════════════════════════════
// PART 4: DBC Signal Extraction — CAN Database Parsing
// ═════════════════════════════════════════════════════════════════════════════

// EN: A DBC "signal" describes how to extract a physical value from CAN raw bytes.
//     Physical value = (raw_value * factor) + offset

struct DBCSignal {
    std::string name;
    uint8_t  start_bit   = 0;
    uint8_t  bit_length  = 0;
    bool     big_endian  = false; // true=Motorola (big), false=Intel (little-endian)
    bool     is_signed   = false;
    double   factor      = 1.0;
    double   offset      = 0.0;
    double   min_val     = 0.0;
    double   max_val     = 0.0;
    std::string unit;

    // EN: Extract raw value from CAN data (Intel / little-endian byte order)
    uint64_t extract_raw(const uint8_t* frame_data, uint8_t /*frame_len*/) const {
        // EN: For simplicity, implement Intel (little-endian) byte order extraction
        // Start bit is the LSB position in the data
        uint64_t raw = 0;
        for (uint8_t i = 0; i < bit_length; ++i) {
            uint8_t bit_pos = start_bit + i;
            uint8_t byte_idx = bit_pos / 8;
            uint8_t bit_in_byte = bit_pos % 8;
            if (frame_data[byte_idx] & (1u << bit_in_byte)) {
                raw |= (uint64_t{1} << i);
            }
        }
        return raw;
    }

    // EN: Convert raw to physical value
    double to_physical(uint64_t raw) const {
        if (is_signed) {
            // EN: Sign-extend if MSB is set
            if (raw & (uint64_t{1} << (bit_length - 1))) {
                // Set all upper bits to 1 for sign extension
                uint64_t mask = ~((uint64_t{1} << bit_length) - 1);
                raw |= mask;
            }
            return static_cast<double>(static_cast<int64_t>(raw)) * factor + offset;
        }
        return static_cast<double>(raw) * factor + offset;
    }

    // EN: Full extraction pipeline
    double decode(const uint8_t* frame_data, uint8_t frame_len) const {
        uint64_t raw = extract_raw(frame_data, frame_len);
        return to_physical(raw);
    }
};

// EN: CAN message definition (from DBC)
struct DBCMessage {
    uint32_t id;
    std::string name;
    uint8_t dlc;
    std::string transmitter;
    std::vector<DBCSignal> signals;

    void decode_and_print(const uint8_t* frame_data, uint8_t frame_len) const {
        std::cout << "  Message: " << name << " (0x" << std::hex << id << std::dec
                  << ") from " << transmitter << "\n";
        for (const auto& sig : signals) {
            double phys = sig.decode(frame_data, frame_len);
            std::cout << "    " << sig.name << " = " << phys << " " << sig.unit;
            if (phys < sig.min_val || phys > sig.max_val) {
                std::cout << " [OUT OF RANGE!]";
            }
            std::cout << "\n";
        }
    }
};

// ═════════════════════════════════════════════════════════════════════════════
// PART 5: CAN Filter — ID Mask Filtering
// ═════════════════════════════════════════════════════════════════════════════

// EN: CAN controllers use hardware filters to reduce CPU load.
//     Filter passes when: (received_id & mask) == (filter_id & mask)

struct CANFilter {
    uint32_t filter_id;
    uint32_t mask;

    bool matches(uint32_t received_id) const {
        return (received_id & mask) == (filter_id & mask);
    }

    void print() const {
        std::cout << "    Filter: ID=0x" << std::hex << std::setw(3) << std::setfill('0')
                  << filter_id << " Mask=0x" << std::setw(3) << mask << std::dec << "\n";
    }
};

// ═════════════════════════════════════════════════════════════════════════════
// PART 6: CAN Error Handling — Bus-Off Recovery
// ═════════════════════════════════════════════════════════════════════════════

// EN: CAN nodes maintain Transmit Error Counter (TEC) and Receive Error Counter (REC).
//     State machine: Error Active → Error Passive → Bus-Off
//     - Error Active: TEC < 128 AND REC < 128 (normal operation)
//     - Error Passive: TEC ≥ 128 OR REC ≥ 128 (restricted participation)
//     - Bus-Off: TEC ≥ 256 (node disconnects, must recover)
//     Durum makinesi: Error Active → Error Passive → Bus-Off

enum class CANErrorState {
    ErrorActive,
    ErrorPassive,
    BusOff
};

std::string to_string(CANErrorState s) {
    switch (s) {
        case CANErrorState::ErrorActive:  return "Error Active";
        case CANErrorState::ErrorPassive: return "Error Passive";
        case CANErrorState::BusOff:       return "Bus-Off";
    }
    return "Unknown";
}

class CANErrorManager {
    uint16_t tec_ = 0;
    uint16_t rec_ = 0;
    int busoff_recovery_count_ = 0;

public:
    CANErrorState state() const {
        if (tec_ >= 256) return CANErrorState::BusOff;
        if (tec_ >= 128 || rec_ >= 128) return CANErrorState::ErrorPassive;
        return CANErrorState::ErrorActive;
    }

    // EN: Called when a transmit error occurs (+8 per error, per ISO 11898)
    void transmit_error() {
        if (state() != CANErrorState::BusOff)
            tec_ = std::min<uint16_t>(tec_ + 8, 256);
    }

    // EN: Called when a receive error occurs (+1 per error)
    void receive_error() {
        rec_ = std::min<uint16_t>(rec_ + 1, 128);
    }

    // EN: Called on successful transmission (-1, min 0)
    void transmit_success() {
        if (tec_ > 0) --tec_;
    }

    void receive_success() {
        if (rec_ > 0) --rec_;
    }

    // EN: Bus-off recovery: 128 occurrences of 11 consecutive recessive bits
    bool attempt_busoff_recovery() {
        if (state() != CANErrorState::BusOff) return false;
        ++busoff_recovery_count_;
        if (busoff_recovery_count_ >= 128) {
            tec_ = 0;
            rec_ = 0;
            busoff_recovery_count_ = 0;
            return true;  // recovered
        }
        return false;
    }

    void print_status() const {
        std::cout << "    TEC=" << tec_ << " REC=" << rec_
                  << " State=" << to_string(state()) << "\n";
    }
};

// ═════════════════════════════════════════════════════════════════════════════
// PART 7: Mini ECU Network Simulation — 4 ECU Car Network
// ═════════════════════════════════════════════════════════════════════════════

// EN: Simulates a simple car CAN network with 4 ECUs:
//     Engine ECU → publishes RPM, coolant temp, throttle position
//     ABS ECU → publishes wheel speeds
//     HVAC ECU → publishes cabin temperature, fan speed
//     Gateway ECU → receives and routes all messages

struct ECU {
    std::string name;
    std::vector<CANFDFrame> tx_queue;
    std::vector<CANFDFrame> rx_buffer;
    std::vector<CANFilter> filters;

    void queue_tx(CANFDFrame frame) {
        tx_queue.push_back(std::move(frame));
    }

    bool accept(uint32_t msg_id) const {
        if (filters.empty()) return true;  // no filter = accept all (gateway mode)
        for (const auto& f : filters) {
            if (f.matches(msg_id)) return true;
        }
        return false;
    }

    void receive(const CANFDFrame& frame) {
        if (accept(frame.id)) {
            rx_buffer.push_back(frame);
        }
    }
};

// EN: CAN bus — connect ECUs and handle transmission
class CANBus {
    std::vector<ECU*> nodes_;
    size_t total_frames_ = 0;

public:
    void connect(ECU& ecu) { nodes_.push_back(&ecu); }

    // EN: Transmit all queued frames (arbitration: lowest ID first)
    void transmit_cycle() {
        // EN: Collect all pending frames with source info
        struct PendingFrame {
            CANFDFrame frame;
            ECU* source;
        };
        std::vector<PendingFrame> all_pending;

        for (auto* node : nodes_) {
            for (auto& f : node->tx_queue) {
                all_pending.push_back({f, node});
            }
            node->tx_queue.clear();
        }

        // EN: Arbitration — sort by ID (lowest wins)
        std::sort(all_pending.begin(), all_pending.end(),
                  [](const PendingFrame& a, const PendingFrame& b) {
                      return a.frame.id < b.frame.id;
                  });

        // EN: Deliver each frame to all other nodes
        for (const auto& pf : all_pending) {
            for (auto* node : nodes_) {
                if (node != pf.source) {
                    node->receive(pf.frame);
                }
            }
            ++total_frames_;
        }
    }

    size_t total() const { return total_frames_; }
};

// ═════════════════════════════════════════════════════════════════════════════
// PART 8: SocketCAN Reference (Linux Only)
// ═════════════════════════════════════════════════════════════════════════════

// EN: SocketCAN code reference (compiles only on Linux with linux/can.h)
//     Included as commented reference for real-world usage.

/*
 * ==========  SocketCAN Example (Linux Only) ==========
 *
 * #include <linux/can.h>
 * #include <linux/can/raw.h>
 * #include <net/if.h>
 * #include <sys/ioctl.h>
 * #include <sys/socket.h>
 * #include <unistd.h>
 *
 * // --- Setup vcan0 (run as root before this program): ---
 * // sudo modprobe vcan
 * // sudo ip link add dev vcan0 type vcan
 * // sudo ip link set up vcan0
 *
 * // --- CAN Socket (Classic CAN) ---
 * int sock = socket(PF_CAN, SOCK_RAW, CAN_RAW);
 * struct ifreq ifr;
 * std::strcpy(ifr.ifr_name, "vcan0");
 * ioctl(sock, SIOCGIFINDEX, &ifr);
 *
 * struct sockaddr_can addr{};
 * addr.can_family = AF_CAN;
 * addr.can_ifindex = ifr.ifr_ifindex;
 * bind(sock, (struct sockaddr*)&addr, sizeof(addr));
 *
 * // --- Send Classic CAN frame ---
 * struct can_frame frame{};
 * frame.can_id = 0x123;
 * frame.can_dlc = 8;
 * std::memset(frame.data, 0xAA, 8);
 * write(sock, &frame, sizeof(frame));
 *
 * // --- Send CAN FD frame ---
 * int enable_canfd = 1;
 * setsockopt(sock, SOL_CAN_RAW, CAN_RAW_FD_FRAMES,
 *            &enable_canfd, sizeof(enable_canfd));
 * struct canfd_frame fdframe{};
 * fdframe.can_id = 0x456;
 * fdframe.len = 24;
 * fdframe.flags = CANFD_BRS;  // Bit Rate Switch
 * write(sock, &fdframe, sizeof(fdframe));
 *
 * // --- CAN Filter (receive only 0x100-0x1FF) ---
 * struct can_filter rfilter;
 * rfilter.can_id   = 0x100;
 * rfilter.can_mask = 0x700;  // match upper 3 bits of 11-bit ID
 * setsockopt(sock, SOL_CAN_RAW, CAN_RAW_FILTER,
 *            &rfilter, sizeof(rfilter));
 *
 * // --- Receive ---
 * struct can_frame rx_frame;
 * read(sock, &rx_frame, sizeof(rx_frame));
 *
 * close(sock);
 */

// ═════════════════════════════════════════════════════════════════════════════
// MAIN
// ═════════════════════════════════════════════════════════════════════════════
int main() {
    std::cout << "============================================\n";
    std::cout << "   Module 14 - CAN FD & CAN XL\n";
    std::cout << "============================================\n\n";

    // ─── Demo 1: Frame Structures ───────────────────────────────────────
    {
        std::cout << "--- Demo 1: CAN Frame Structures ---\n";

        // EN: Classic CAN — Engine RPM message
        ClassicCANFrame classic;
        classic.id = 0x0C0;
        classic.dlc = 8;
        classic.data = {0x0B, 0xB8, 0x50, 0x00, 0xFF, 0x7F, 0x00, 0x00};  // RPM=3000
        classic.print("Engine RPM (Classic CAN)");

        // EN: CAN FD — ADAS sensor fusion data (needs >8 bytes)
        CANFDFrame fd_frame;
        fd_frame.id = 0x200;
        fd_frame.dlc = 13; // DLC 13 → 32 bytes
        fd_frame.brs = true;
        fd_frame.esi = false;
        for (uint8_t i = 0; i < 32; ++i) fd_frame.data[i] = i;
        fd_frame.print("ADAS Sensor Fusion (CAN FD, 32 bytes)");

        // EN: CAN XL — Tunneled Ethernet frame
        CANXLFrame xl_frame;
        xl_frame.priority = 1;
        xl_frame.acceptance_field = 0x00AA;
        xl_frame.sdu_type = CANXLFrame::SDU_ETHERNET;
        xl_frame.data_length = 128;
        xl_frame.data.resize(128);
        for (int i = 0; i < 128; ++i) xl_frame.data[static_cast<size_t>(i)] = static_cast<uint8_t>(i & 0xFF);
        xl_frame.print("Tunneled Ethernet (CAN XL, 128 bytes)");
        std::cout << "\n";
    }

    // ─── Demo 2: CAN FD CRC Calculation ─────────────────────────────────
    {
        std::cout << "--- Demo 2: CAN FD CRC Calculation ---\n";

        // EN: Short message → CRC-17
        CANFDFrame short_msg;
        short_msg.dlc = 8;  // 8 bytes
        short_msg.data = {};
        for (uint8_t i = 0; i < 8; ++i) short_msg.data[i] = static_cast<uint8_t>(0x10 + i);
        uint32_t crc_short = canfd_crc(short_msg);
        std::cout << "  Short msg (8 bytes) → CRC-17: 0x" << std::hex
                  << std::setw(5) << std::setfill('0') << crc_short << "\n";

        // EN: Long message → CRC-21
        CANFDFrame long_msg;
        long_msg.dlc = 14;  // 48 bytes
        for (uint8_t i = 0; i < 48; ++i) long_msg.data[i] = static_cast<uint8_t>(i);
        uint32_t crc_long = canfd_crc(long_msg);
        std::cout << "  Long msg (48 bytes) → CRC-21: 0x" << std::hex
                  << std::setw(6) << std::setfill('0') << crc_long << "\n";

        // EN: Verify CRC changes with data change
        long_msg.data[0] = 0xFF;  // corrupt one byte
        uint32_t crc_corrupt = canfd_crc(long_msg);
        std::cout << "  Corrupted msg       → CRC-21: 0x" << std::setw(6)
                  << std::setfill('0') << crc_corrupt;
        std::cout << (crc_long != crc_corrupt ? " (DIFFERENT ✓)" : " (SAME — BUG!)") << "\n";
        std::cout << std::dec << "\n";
    }

    // ─── Demo 3: CAN Arbitration ────────────────────────────────────────
    {
        std::cout << "--- Demo 3: CAN Arbitration Simulation ---\n";

        std::vector<ArbitrationEntry> race = {
            {0x200, "ADAS_ECU",    "RadarTarget"},
            {0x0C0, "Engine_ECU",  "EngineRPM"},
            {0x100, "ABS_ECU",     "WheelSpeed"},
            {0x300, "HVAC_ECU",    "CabinTemp"},
            {0x080, "Airbag_ECU",  "CrashSensor"}
        };

        auto winner = simulate_arbitration(race);
        std::cout << "  → " << winner.ecu_name << " transmits first ("
                  << winner.message_name << ")\n";
        std::cout << "  NOTE: Lower ID = Higher priority (safety-critical first)\n\n";
    }

    // ─── Demo 4: DBC Signal Extraction ──────────────────────────────────
    {
        std::cout << "--- Demo 4: DBC Signal Extraction ---\n";
        std::cout << "  DBC definition (simulated):\n";
        std::cout << "    BO_ 0x0C0 EngineData: 8 Engine_ECU\n";
        std::cout << "      SG_ RPM  : 0|16@1+ (0.25,0) [0|16383.75] \"rpm\"\n";
        std::cout << "      SG_ Temp : 16|8@1+ (1,-40) [-40|215] \"degC\"\n";
        std::cout << "      SG_ Load : 24|8@1+ (0.392157,0) [0|100] \"%\"\n\n";

        // EN: Define the message with signals
        DBCMessage engine_msg;
        engine_msg.id = 0x0C0;
        engine_msg.name = "EngineData";
        engine_msg.dlc = 8;
        engine_msg.transmitter = "Engine_ECU";

        // EN: RPM signal: start_bit=0, length=16, Intel byte order, unsigned
        DBCSignal rpm_sig;
        rpm_sig.name = "RPM";
        rpm_sig.start_bit = 0;
        rpm_sig.bit_length = 16;
        rpm_sig.factor = 0.25;
        rpm_sig.offset = 0.0;
        rpm_sig.min_val = 0.0;
        rpm_sig.max_val = 16383.75;
        rpm_sig.unit = "rpm";
        engine_msg.signals.push_back(rpm_sig);

        // EN: Temperature signal: start_bit=16, length=8, offset=-40
        DBCSignal temp_sig;
        temp_sig.name = "CoolantTemp";
        temp_sig.start_bit = 16;
        temp_sig.bit_length = 8;
        temp_sig.factor = 1.0;
        temp_sig.offset = -40.0;
        temp_sig.min_val = -40.0;
        temp_sig.max_val = 215.0;
        temp_sig.unit = "degC";
        engine_msg.signals.push_back(temp_sig);

        // EN: Engine Load: start_bit=24, length=8
        DBCSignal load_sig;
        load_sig.name = "EngineLoad";
        load_sig.start_bit = 24;
        load_sig.bit_length = 8;
        load_sig.factor = 0.392157;
        load_sig.offset = 0.0;
        load_sig.min_val = 0.0;
        load_sig.max_val = 100.0;
        load_sig.unit = "%";
        engine_msg.signals.push_back(load_sig);

        // EN: Simulated CAN data: RPM=3000 → raw=12000 (0x2EE0), Temp=90°C → raw=130 (0x82), Load=75% → raw=191 (0xBF)
        uint8_t can_data[] = {0xE0, 0x2E, 0x82, 0xBF, 0x00, 0x00, 0x00, 0x00};
        engine_msg.decode_and_print(can_data, 8);
        std::cout << "\n";
    }

    // ─── Demo 5: CAN Filter ────────────────────────────────────────────
    {
        std::cout << "--- Demo 5: CAN Filter Matching ---\n";

        // EN: Filter: accept only 0x0C0, 0x0C1, ..., 0x0CF (engine messages)
        CANFilter engine_filter{0x0C0, 0x7F0};  // mask upper 8 bits of 11-bit ID
        engine_filter.print();

        uint32_t test_ids[] = {0x0C0, 0x0C5, 0x0CF, 0x0D0, 0x100, 0x200};
        for (uint32_t tid : test_ids) {
            bool pass = engine_filter.matches(tid);
            std::cout << "    ID 0x" << std::hex << std::setw(3) << std::setfill('0')
                      << tid << std::dec << ": " << (pass ? "PASS ✓" : "REJECT ✗") << "\n";
        }
        std::cout << "\n";
    }

    // ─── Demo 6: Error State Machine ────────────────────────────────────
    {
        std::cout << "--- Demo 6: CAN Error State Machine ---\n";

        CANErrorManager err_mgr;
        std::cout << "  Initial:";
        err_mgr.print_status();

        // EN: Simulate repeated transmit errors
        for (int i = 0; i < 16; ++i) err_mgr.transmit_error();  // 16 * 8 = 128
        std::cout << "  After 16 TX errors (TEC+=128):";
        err_mgr.print_status();

        for (int i = 0; i < 16; ++i) err_mgr.transmit_error();  // 128 more → 256
        std::cout << "  After 16 more TX errors (TEC=256):";
        err_mgr.print_status();

        // EN: Attempt bus-off recovery
        std::cout << "  Attempting bus-off recovery (128 recessive bit sequences)...\n";
        bool recovered = false;
        for (int i = 0; i < 128 && !recovered; ++i) {
            recovered = err_mgr.attempt_busoff_recovery();
        }
        std::cout << "  After recovery:";
        err_mgr.print_status();
        std::cout << "  Recovered: " << (recovered ? "Yes ✓" : "No") << "\n\n";
    }

    // ─── Demo 7: 4-ECU Car Network Simulation ──────────────────────────
    {
        std::cout << "--- Demo 7: 4-ECU Car Network Simulation ---\n";

        ECU engine_ecu{"Engine_ECU", {}, {}, {}};
        ECU abs_ecu{"ABS_ECU", {}, {}, {}};
        ECU hvac_ecu{"HVAC_ECU", {}, {}, {}};
        ECU gateway{"Gateway", {}, {}, {}};  // no filter → accepts all

        // EN: ABS only interested in engine and wheel speed
        abs_ecu.filters.push_back({0x0C0, 0x7F0});  // Engine 0x0Cx
        abs_ecu.filters.push_back({0x100, 0x7F0});  // Wheel  0x10x

        // EN: HVAC only interested in cabin temp request
        hvac_ecu.filters.push_back({0x300, 0x7FF});  // exact match

        CANBus bus;
        bus.connect(engine_ecu);
        bus.connect(abs_ecu);
        bus.connect(hvac_ecu);
        bus.connect(gateway);

        // EN: Engine ECU transmits RPM (3000 rpm) and coolant temp (90°C)
        {
            CANFDFrame rpm_msg;
            rpm_msg.id = 0x0C0;
            rpm_msg.dlc = 8;
            rpm_msg.data[0] = 0xE0; rpm_msg.data[1] = 0x2E;  // RPM raw=12000
            rpm_msg.data[2] = 0x82;  // Temp raw=130 → 90°C
            engine_ecu.queue_tx(rpm_msg);
        }

        // EN: ABS ECU transmits wheel speeds
        {
            CANFDFrame wheel_msg;
            wheel_msg.id = 0x100;
            wheel_msg.dlc = 8;
            wheel_msg.data[0] = 0x40; wheel_msg.data[1] = 0x01;  // FL speed
            wheel_msg.data[2] = 0x40; wheel_msg.data[3] = 0x01;  // FR speed
            wheel_msg.data[4] = 0x38; wheel_msg.data[5] = 0x01;  // RL speed
            wheel_msg.data[6] = 0x38; wheel_msg.data[7] = 0x01;  // RR speed
            abs_ecu.queue_tx(wheel_msg);
        }

        // EN: HVAC transmits cabin temperature
        {
            CANFDFrame cabin_msg;
            cabin_msg.id = 0x300;
            cabin_msg.dlc = 4;
            cabin_msg.data[0] = 0x16;  // 22°C
            cabin_msg.data[1] = 0x03;  // Fan speed level 3
            hvac_ecu.queue_tx(cabin_msg);
        }

        // EN: Run one bus cycle
        bus.transmit_cycle();

        std::cout << "  Results after 1 bus cycle (" << bus.total() << " frames transmitted):\n";
        std::cout << "    Engine_ECU rx: " << engine_ecu.rx_buffer.size() << " msgs\n";
        std::cout << "    ABS_ECU rx:    " << abs_ecu.rx_buffer.size() << " msgs (filtered)\n";
        std::cout << "    HVAC_ECU rx:   " << hvac_ecu.rx_buffer.size() << " msgs (filtered)\n";
        std::cout << "    Gateway rx:    " << gateway.rx_buffer.size() << " msgs (accepts all)\n";

        // EN: Verify gateway received all messages
        std::cout << "\n  Gateway received IDs:";
        for (const auto& f : gateway.rx_buffer) {
            std::cout << " 0x" << std::hex << f.id;
        }
        std::cout << std::dec << "\n";

        // EN: Verify ABS filter — should have engine (0x0C0) but NOT HVAC (0x300)
        std::cout << "  ABS received IDs:";
        for (const auto& f : abs_ecu.rx_buffer) {
            std::cout << " 0x" << std::hex << f.id;
        }
        std::cout << std::dec << "\n";

        // EN: Verify HVAC filter — should NOT have engine or wheel speed
        std::cout << "  HVAC received IDs:";
        for (const auto& f : hvac_ecu.rx_buffer) {
            std::cout << " 0x" << std::hex << f.id;
        }
        std::cout << std::dec << " (only gets 0x300 from others, but none sent it back)\n\n";
    }

    // ─── Demo 8: Comparison Table ───────────────────────────────────────
    {
        std::cout << "--- Demo 8: Classic CAN vs CAN FD vs CAN XL ---\n\n";
        std::cout << "  ┌──────────────────┬──────────────┬──────────────┬──────────────┐\n";
        std::cout << "  │ Feature          │ Classic CAN  │ CAN FD       │ CAN XL       │\n";
        std::cout << "  ├──────────────────┼──────────────┼──────────────┼──────────────┤\n";
        std::cout << "  │ Max Payload      │ 8 bytes      │ 64 bytes     │ 2048 bytes   │\n";
        std::cout << "  │ Max Bitrate      │ 1 Mbps       │ 8 Mbps       │ 20 Mbps      │\n";
        std::cout << "  │ Dual Bitrate     │ No           │ Yes (BRS)    │ Yes          │\n";
        std::cout << "  │ CRC              │ CRC-15       │ CRC-17/21    │ CRC-32       │\n";
        std::cout << "  │ ID Bits          │ 11/29        │ 11/29        │ Priority+AF  │\n";
        std::cout << "  │ Ethernet Bridge  │ No           │ No           │ Yes (SDU)    │\n";
        std::cout << "  │ Backward Compat  │ —            │ ← Classic    │ ← FD+Classic │\n";
        std::cout << "  │ Wiring           │ Twisted pair │ Twisted pair │ Twisted pair │\n";
        std::cout << "  │ Typical ECU      │ Body, BCM    │ ADAS, GW     │ Zone Ctrl    │\n";
        std::cout << "  │ Year Adopted     │ 1993         │ 2012/2015    │ 2024         │\n";
        std::cout << "  └──────────────────┴──────────────┴──────────────┴──────────────┘\n\n";
    }

    std::cout << "============================================\n";
    std::cout << "   End of CAN FD & CAN XL\n";
    std::cout << "============================================\n";

    return 0;
}
