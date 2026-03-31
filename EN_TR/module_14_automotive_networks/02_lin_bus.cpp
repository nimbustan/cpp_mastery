/**
 * @file module_14_automotive_networks/02_lin_bus.cpp
 * @brief LIN Bus — Low-Cost Automotive Sub-Network — LIN Bus — Düşük Maliyetli Otomotiv Alt Ağı
 *
 * @details
 * =============================================================================
 * [THEORY: LIN — Local Interconnect Network / TEORİ: LIN — Yerel Bağlantı Ağı]
 * =============================================================================
 *
 * EN: LIN (Local Interconnect Network) is a low-cost, single-wire serial bus
 *     used for non-safety-critical body electronics. Standardized by LIN Consortium
 *     (now part of ISO 17987).
 *
 *     Key characteristics:
 *     - Single-wire + ground (very cheap wiring)
 *     - Master-slave architecture: 1 master, up to 16 slaves
 *     - Max speed: 20 kbps (typical: 9.6 or 19.2 kbps)
 *     - Deterministic: master controls ALL communication via schedule table
 *     - No arbitration needed (unlike CAN) — master decides who talks when
 *
 *     LIN Frame Structure:
 *     ┌────────┬──────────┬──────────────────────────────────────────────┐
 *     │ HEADER │          │  RESPONSE                                    │
 *     │(Master)│          │  (Slave or Master)                           │
 *     ├────────┼──────────┼────────────┬──────────────┬──────────────────┤
 *     │ Break  │ Sync     │ PID        │ Data         │ Checksum         │
 *     │(≥13bit)│ (0x55)   │ (6bit ID   │ (1-8 bytes)  │ (Classic/        │
 *     │        │          │  +2 parity)│              │  Enhanced)       │
 *     └────────┴──────────┴────────────┴──────────────┴──────────────────┘
 *
 *     PID (Protected Identifier):
 *     - 6-bit frame ID (0x00-0x3F) = 64 possible frames
 *     - 2 parity bits: P0 = ID0⊕ID1⊕ID2⊕ID4, P1 = ¬(ID1⊕ID3⊕ID4⊕ID5)
 *     - ID 0x3C-0x3D: Diagnostic frames (master request / slave response)
 *     - ID 0x3E-0x3F: Reserved
 *
 *     Frame Types:
 *     ┌────────────────────┬──────────────────────────────────────────────┐
 *     │ Type               │ Description                                  │
 *     ├────────────────────┼──────────────────────────────────────────────┤
 *     │ Unconditional      │ Fixed schedule, slave always responds        │
 *     │ Event-triggered    │ Multiple slaves, respond only if data changed│
 *     │ Sporadic           │ Master sends own data when slot available    │
 *     │ Diagnostic         │ ID=0x3C (MasterReq), ID=0x3D (SlaveResp)     │
 *     └────────────────────┴──────────────────────────────────────────────┘
 *
 * TR: LIN, güvenlik-kritik olmayan gövde elektroniği için kullanılan düşük
 *     maliyetli, tek telli seri bus'tır (ISO 17987). Master-slave mimarisinde
 *     master tüm iletişimi schedule table ile yönetir.
 *
 *     Temel özellikler:
 *     - Tek tel + toprak (oldukça ucuz kablolama)
 *     - Master-slave mimari: 1 master, 16'ya kadar slave
 *     - Maks hız: 20 kbps (tipik: 9.6 veya 19.2 kbps)
 *     - Belirlenimci: master schedule table ile tüm iletişimi kontrol eder
 *     - Arbitrasyon yok (CAN'dan farklı) — master kimin ne zaman konuşacağına karar verir
 *
 *     LIN Çerçeve Yapısı:
 *     ┌────────┬──────────┬──────────────────────────────────────────────┐
 *     │ BAŞLIK │          │  YANIT                                       │
 *     │(Master)│          │  (Slave veya Master)                         │
 *     ├────────┼──────────┼────────────┬──────────────┬──────────────────┤
 *     │ Break  │ Sync     │ PID        │ Veri         │ Checksum         │
 *     │(≥13bit)│ (0x55)   │ (6bit ID   │ (1-8 bayt)   │ (Classic/        │
 *     │        │          │  +2 parite)│              │  Enhanced)       │
 *     └────────┴──────────┴────────────┴──────────────┴──────────────────┘
 *
 *     PID (Korumalı Tanımlayıcı):
 *     - 6-bit frame ID (0x00-0x3F) = 64 olası çerçeve
 *     - 2 parite biti: P0 = ID0⊕ID1⊕ID2⊕ID4, P1 = ¬(ID1⊕ID3⊕ID4⊕ID5)
 *     - ID 0x3C-0x3D: Teşhis çerçeveleri (master isteği / slave yanıtı)
 *     - ID 0x3E-0x3F: Ayrılmış (reserved)
 *
 *     Çerçeve Türleri:
 *     - Unconditional: Sabit zamanlama, slave her zaman yanıt verir
 *     - Event-triggered: Birden fazla slave, sadece veri değişirse yanıt
 *     - Sporadic: Master, slot müsaitse kendi verisini gönderir
 *     - Diagnostic: ID=0x3C (MasterReq), ID=0x3D (SlaveResp)
 *
 * =============================================================================
 * [THEORY: LIN Checksum — Classic vs Enhanced / TEORİ: LIN Sağlama Toplamı — Klasik ve Gelişmiş]
 * =============================================================================
 *
 * EN: Two checksum types exist:
 *     - Classic (LIN 1.x): checksum over DATA bytes only
 *     - Enhanced (LIN 2.x): checksum over PID + DATA bytes
 *     Both use inverted modulo-256 sum with carry propagation.
 *
 * TR: İki checksum türü vardır:
 *     - Classic (LIN 1.x): sadece DATA baytları üzerinden checksum
 *     - Enhanced (LIN 2.x): PID + DATA baytları üzerinden checksum
 *     Her ikisi de elde-yayılmalı ters modülü-256 toplamı kullanır.
 *
 * =============================================================================
 * [THEORY: LIN vs CAN — When to Use Which / TEORİ: LIN ve CAN — Hangisi Ne Zaman Kullanılır]
 * =============================================================================
 *
 * EN:
 *     ┌──────────────────────┬──────────────┬─────────────────────────────┐
 *     │ Feature              │ LIN          │ CAN                         │
 *     ├──────────────────────┼──────────────┼─────────────────────────────┤
 *     │ Speed                │ 20 kbps      │ 1 Mbps (Classic), 8M (FD)   │
 *     │ Wiring               │ Single wire  │ Twisted pair (2 wires)      │
 *     │ Cost per node        │ ~$0.5        │ ~$2-5                       │
 *     │ Topology             │ Master-slave │ Multi-master (peer-to-peer) │
 *     │ Arbitration          │ None (sched) │ Bitwise ID-based            │
 *     │ Error detection      │ Checksum     │ CRC, ACK, error frame       │
 *     │ Max nodes            │ 1+16         │ Theoretically unlimited     │
 *     │ Deterministic        │ Yes (table)  │ Not guaranteed (priority)   │
 *     │ Typical use          │ Window, seat │ Engine, ABS, ADAS           │
 *     │ Safety level         │ QM, ASIL-A   │ Up to ASIL-D                │
 *     │ Wake-up              │ Bus wakeup   │ Bus wakeup                  │
 *     └──────────────────────┴──────────────┴─────────────────────────────┘
 *
 * TR: LIN vs CAN karşılaştırması:
 *     - Hız: LIN 20 kbps, CAN 1-8 Mbps — LIN düşük hız yeterli olduğunda
 *     - Maliyet: LIN ~$0.5/düğüm, CAN ~$2-5 — LIN kablolama maliyetini düşürür
 *     - Topoloji: LIN master-slave, CAN multi-master — LIN basit kontrol için
 *     - Arbitrasyon: LIN yok (schedule), CAN bit-bazlı ID — LIN çakışmayı önler
 *     - Güvenlik: LIN QM/ASIL-A, CAN ASIL-D'ye kadar — LIN güvenlik-kritik değil
 *     - Tipik kullanım: LIN cam/koltuk/ayna, CAN motor/ABS/ADAS
 *     - Uyandırma: Her ikisi de bus wakeup destekler
 *
 * @note Compile: g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion 02_lin_bus.cpp -o 02_lin_bus
 *
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <iostream>
#include <cstdint>
#include <array>
#include <vector>
#include <string>
#include <functional>
#include <iomanip>
#include <cassert>
#include <map>

// ═════════════════════════════════════════════════════════════════════════════
// PART 1: LIN Frame Structures
// ═════════════════════════════════════════════════════════════════════════════

// EN: Frame type classification
// TR: Frame tipi sınıflandırması
enum class LINFrameType {
    Unconditional,   // Sabit zamanlama, slave her zaman yanıt verir
    EventTriggered,  // Sadece veri değiştiğinde yanıt (bus yükü azalması)
    Sporadic,        // Master slot müsaitse kendi verisini gönderir
    DiagRequest,     // Master teşhis isteği (ID=0x3C, her zaman 8 bayt)
    DiagResponse     // Slave teşhis yanıtı (ID=0x3D, her zaman 8 bayt)
};

std::string to_string(LINFrameType t) {
    switch (t) {
        case LINFrameType::Unconditional:  return "Unconditional";
        case LINFrameType::EventTriggered: return "Event-Triggered";
        case LINFrameType::Sporadic:       return "Sporadic";
        case LINFrameType::DiagRequest:    return "Diagnostic Request";
        case LINFrameType::DiagResponse:   return "Diagnostic Response";
    }
    return "Unknown";
}

// EN: Protected Identifier (PID) calculation
// TR: Korumalı Tanımlayıcı (PID) hesaplaması
class LINPID {
public:
    // EN: Calculate PID from 6-bit frame ID (0x00-0x3F)
    // TR: 6-bit frame ID'den PID hesapla (parite bitleri P0, P1 eklenir)
    //     PID = ID[5:0] + P0 + P1
    //     P0 = ID0 ⊕ ID1 ⊕ ID2 ⊕ ID4
    //     P1 = ¬(ID1 ⊕ ID3 ⊕ ID4 ⊕ ID5)
    static uint8_t calculate(uint8_t id) {
        id &= 0x3F;  // mask to 6 bits

        uint8_t p0 = ((id >> 0) ^ (id >> 1) ^ (id >> 2) ^ (id >> 4)) & 1u;
        uint8_t p1 = static_cast<uint8_t>(~((id >> 1) ^ (id >> 3) ^ (id >> 4) ^ (id >> 5)) & 1u);

        return static_cast<uint8_t>(id | (p0 << 6) | (p1 << 7));
    }

    // EN: Extract frame ID from PID
    // TR: PID'den frame ID'yi çıkar (alt 6 bit maskesi)
    static uint8_t extract_id(uint8_t pid) {
        return pid & 0x3F;
    }

    // EN: Verify PID parity
    // TR: PID parite doğrulaması (ıletim hatası tespiti için)
    static bool verify(uint8_t pid) {
        uint8_t id = extract_id(pid);
        return calculate(id) == pid;
    }
};

// EN: LIN Checksum calculation
// TR: LIN Checksum hesaplaması
class LINChecksum {
public:
    // EN: Classic checksum (LIN 1.x): sum over DATA only
    // TR: Klasik checksum (LIN 1.x): sadece DATA baytları üzerine toplam
    static uint8_t classic(const uint8_t* data, uint8_t len) {
        uint16_t sum = 0;
        for (uint8_t i = 0; i < len; ++i) {
            sum += data[i];
            if (sum > 0xFF) sum = (sum & 0xFF) + 1;  // elde yayılması (carry propagation)
        }
        return static_cast<uint8_t>(~sum & 0xFF);
    }

    // EN: Enhanced checksum (LIN 2.x): sum over PID + DATA
    // TR: Gelişmiş checksum (LIN 2.x): PID + DATA baytları üzerine toplam
    static uint8_t enhanced(uint8_t pid, const uint8_t* data, uint8_t len) {
        uint16_t sum = pid;
        for (uint8_t i = 0; i < len; ++i) {
            sum += data[i];
            if (sum > 0xFF) sum = (sum & 0xFF) + 1;
        }
        return static_cast<uint8_t>(~sum & 0xFF);
    }
};

// EN: Complete LIN frame
// TR: Tam LIN frame yapısı
struct LINFrame {
    uint8_t      id      = 0;   // 6-bit frame ID (0x00-0x3F), slot numarasını belirler
    uint8_t      pid     = 0;   // Korumalı ID (ID + P0/P1 parite bitleri)
    uint8_t      dlc     = 0;   // Veri uzunluğu (1-8 bayt)
    std::array<uint8_t, 8> data{};
    uint8_t      checksum = 0;
    bool         enhanced_checksum = true;  // LIN 2.x varsayılan (PID dahil)
    LINFrameType type = LINFrameType::Unconditional;
    std::string  name;

    void compute() {
        pid = LINPID::calculate(id);
        if (enhanced_checksum) {
            checksum = LINChecksum::enhanced(pid, data.data(), dlc);
        } else {
            checksum = LINChecksum::classic(data.data(), dlc);
        }
    }

    bool verify() const {
        if (!LINPID::verify(pid)) return false;
        uint8_t expected;
        if (enhanced_checksum) {
            expected = LINChecksum::enhanced(pid, data.data(), dlc);
        } else {
            expected = LINChecksum::classic(data.data(), dlc);
        }
        return checksum == expected;
    }

    void print() const {
        std::cout << "    [LIN] " << name << " (ID=0x"
                  << std::hex << std::setw(2) << std::setfill('0')
                  << static_cast<int>(id)
                  << " PID=0x" << static_cast<int>(pid)
                  << ") Type=" << to_string(type)
                  << " DLC=" << std::dec << static_cast<int>(dlc) << "\n";
        std::cout << "      Data:";
        for (uint8_t i = 0; i < dlc; ++i) {
            std::cout << " " << std::hex << std::setw(2) << std::setfill('0')
                      << static_cast<int>(data[i]);
        }
        std::cout << std::dec << " | Chk=0x" << std::hex
                  << static_cast<int>(checksum)
                  << (enhanced_checksum ? " (enhanced)" : " (classic)")
                  << std::dec << " | Valid=" << (verify() ? "YES" : "NO") << "\n";
    }
};

// ═════════════════════════════════════════════════════════════════════════════
// PART 2: LIN Slave Node Simulation
// ═════════════════════════════════════════════════════════════════════════════

// EN: A LIN slave node responds to specific frame IDs from the master.
//     When the master sends a header with a matching ID, the slave fills the response.
// TR: LIN slave düğümü, master'dan gelen belirli frame ID'lerine yanıt verir.

class LINSlaveNode {
    std::string name_;
    // EN: Map of frame ID → response callback
    // TR: Frame ID → yanıt callback haritası (slave hangi ID'ye yanıt veriyor)
    std::map<uint8_t, std::function<LINFrame()>> response_handlers_;
    bool awake_ = true;

public:
    explicit LINSlaveNode(std::string name) : name_(std::move(name)) {}

    const std::string& name() const { return name_; }

    // EN: Register a response handler for a frame ID
    // TR: Bir frame ID için yanıt işleyicisi kaydet
    void on_frame(uint8_t frame_id, std::function<LINFrame()> handler) {
        response_handlers_[frame_id] = std::move(handler);
    }

    // EN: Master sends header — slave responds if it owns this frame
    // TR: Master header gönderir — bu frame'in sahibi slave ise yanıt verir
    bool responds_to(uint8_t frame_id) const {
        return response_handlers_.count(frame_id) > 0;
    }

    LINFrame respond(uint8_t frame_id) {
        auto it = response_handlers_.find(frame_id);
        if (it != response_handlers_.end()) {
            LINFrame frame = it->second();
            frame.id = frame_id;
            frame.compute();
            return frame;
        }
        return {};
    }

    // EN: Sleep/wakeup mechanism
    // TR: Uyku/uyanma mekanizması (LIN bus güç tasarrufu için uyutulabilir)
    void sleep() { awake_ = false; }
    void wakeup() { awake_ = true; }
    bool is_awake() const { return awake_; }
};

// ═════════════════════════════════════════════════════════════════════════════
// PART 3: LIN Schedule Table
// ═════════════════════════════════════════════════════════════════════════════

// EN: The schedule table defines WHEN each frame is transmitted.
//     The master cycles through the table, sending headers at fixed intervals.
//     This makes LIN fully deterministic — no collisions possible.
// TR: Schedule table her frame'in NE ZAMAN iletileceğini tanımlar.
//     Master, tabloyu döngüsel olarak takip eder.

struct ScheduleEntry {
    uint8_t frame_id;
    uint16_t delay_ms;    // delay after this frame before next
    LINFrameType type;
    std::string description;
};

class LINScheduleTable {
    std::string name_;
    std::vector<ScheduleEntry> entries_;
    size_t current_index_ = 0;

public:
    explicit LINScheduleTable(std::string name) : name_(std::move(name)) {}

    void add(uint8_t frame_id, uint16_t delay_ms, LINFrameType type,
             const std::string& desc) {
        entries_.push_back({frame_id, delay_ms, type, desc});
    }

    const ScheduleEntry& next() {
        const auto& entry = entries_[current_index_];
        current_index_ = (current_index_ + 1) % entries_.size();
        return entry;
    }

    size_t size() const { return entries_.size(); }
    const std::string& name() const { return name_; }

    void print() const {
        std::cout << "  Schedule Table: " << name_ << " (" << entries_.size() << " slots)\n";
        std::cout << "  ┌──────┬──────────┬──────────────────┬───────────────────────────┐\n";
        std::cout << "  │ Slot │ Frame ID │ Delay (ms)       │ Description               │\n";
        std::cout << "  ├──────┼──────────┼──────────────────┼───────────────────────────┤\n";
        for (size_t i = 0; i < entries_.size(); ++i) {
            const auto& e = entries_[i];
            std::cout << "  │ " << std::dec << std::setfill(' ') << std::setw(4) << i
                      << " │ 0x" << std::hex << std::setfill('0') << std::setw(2)
                      << static_cast<int>(e.frame_id) << std::dec << std::setfill(' ')
                      << "     │ " << std::setw(16) << e.delay_ms
                      << " │ " << std::left << std::setw(25) << e.description
                      << std::right << " │\n";
        }
        std::cout << "  └──────┴──────────┴──────────────────┴───────────────────────────┘\n";
    }
};

// ═════════════════════════════════════════════════════════════════════════════
// PART 4: LIN Master Node
// ═════════════════════════════════════════════════════════════════════════════

// EN: The master node controls all communication:
//     1) Follows the schedule table
//     2) Sends break + sync + PID header
//     3) Waits for the designated slave to respond with data + checksum
// TR: Master tüm iletişimi kontrol eder: schedule table'ı takip eder,
//     header gönderir, slave'in yanıt vermesini bekler.

class LINMaster {
    std::string name_;
    std::vector<LINSlaveNode*> slaves_;
    LINScheduleTable* active_schedule_ = nullptr;
    std::vector<LINFrame> message_log_;  // all received responses

public:
    explicit LINMaster(std::string name) : name_(std::move(name)) {}

    void add_slave(LINSlaveNode& slave) { slaves_.push_back(&slave); }
    void set_schedule(LINScheduleTable& table) { active_schedule_ = &table; }

    // EN: Execute one schedule cycle (all entries once)
    // TR: Bir schedule döngüsü çalıştır (tüm girişler sırayla)
    void execute_cycle() {
        if (!active_schedule_) return;

        size_t num_slots = active_schedule_->size();
        uint32_t total_time_ms = 0;

        std::cout << "  Master [" << name_ << "] executing schedule '"
                  << active_schedule_->name() << "' (" << num_slots << " slots)\n";

        for (size_t slot = 0; slot < num_slots; ++slot) {
            const auto& entry = active_schedule_->next();
            total_time_ms += entry.delay_ms;

            // EN: Master sends header (break + sync + PID)
            // TR: Master header gönderir (break + sync + PID)
            uint8_t pid = LINPID::calculate(entry.frame_id);

            // EN: Find responding slave
            // TR: Yanıt veren slave'i bul
            bool responded = false;
            for (auto* slave : slaves_) {
                if (slave->is_awake() && slave->responds_to(entry.frame_id)) {
                    LINFrame response = slave->respond(entry.frame_id);
                    response.name = entry.description;
                    response.type = entry.type;
                    message_log_.push_back(response);

                    std::cout << "    t=" << std::setw(5) << total_time_ms << "ms | "
                              << "Header PID=0x" << std::hex << std::setw(2) << std::setfill('0')
                              << static_cast<int>(pid) << std::dec << std::setfill(' ')
                              << " → " << slave->name() << " responds: ";

                    // EN: Print decoded data summary
                    // TR: Çözümlü veri özetini yazdır
                    std::cout << "[";
                    for (uint8_t i = 0; i < response.dlc; ++i) {
                        if (i > 0) std::cout << " ";
                        std::cout << std::hex << std::setw(2) << std::setfill('0')
                                  << static_cast<int>(response.data[i]);
                    }
                    std::cout << std::dec << std::setfill(' ') << "] ✓\n";
                    responded = true;
                    break;
                }
            }
            if (!responded) {
                std::cout << "    t=" << std::setw(5) << total_time_ms << "ms | "
                          << "Header PID=0x" << std::hex << std::setw(2) << std::setfill('0')
                          << static_cast<int>(pid) << std::dec << std::setfill(' ')
                          << " → No response\n";
            }
        }
        std::cout << "  Cycle complete: " << message_log_.size() << " responses, "
                  << total_time_ms << "ms total\n";
    }

    const std::vector<LINFrame>& log() const { return message_log_; }
    void clear_log() { message_log_.clear(); }
};

// ═════════════════════════════════════════════════════════════════════════════
// PART 5: LIN Sleep/Wakeup
// ═════════════════════════════════════════════════════════════════════════════

// EN: LIN supports sleep mode for power saving:
//     - Master sends go-to-sleep command: Diagnostic frame ID=0x3C, data[0]=0x00
//     - Slave wakes up by pulling bus dominant for 250μs-5ms
//     - After wakeup, master resumes schedule table execution
// TR: LIN güç tasarrufu için uyku modunu destekler:
//     - Master uyku komutu gönderir: ID=0x3C, data[0]=0x00
//     - Slave, bus'ı dominant çekerek uyanır

struct LINBusState {
    bool bus_sleeping = false;
    uint32_t sleep_count = 0;
    uint32_t wakeup_count = 0;

    void go_to_sleep() {
        bus_sleeping = true;
        ++sleep_count;
    }

    void wakeup() {
        bus_sleeping = false;
        ++wakeup_count;
    }

    void print() const {
        std::cout << "    Bus state: " << (bus_sleeping ? "SLEEPING" : "ACTIVE")
                  << " | Sleep count: " << sleep_count
                  << " | Wakeup count: " << wakeup_count << "\n";
    }
};

// ═════════════════════════════════════════════════════════════════════════════
// MAIN
// ═════════════════════════════════════════════════════════════════════════════
int main() {
    std::cout << "============================================\n";
    std::cout << "   Module 14 - LIN Bus\n";
    std::cout << "============================================\n\n";

    // ─── Demo 1: PID Calculation & Verification ─────────────────────────
    {
        std::cout << "--- Demo 1: LIN PID Calculation ---\n";

        std::cout << "  ┌──────────┬──────────┬───────────┬────────────┐\n";
        std::cout << "  │ Frame ID │ PID (hex)│ P0 P1     │ Verify     │\n";
        std::cout << "  ├──────────┼──────────┼───────────┼────────────┤\n";

        uint8_t test_ids[] = {0x00, 0x01, 0x10, 0x20, 0x3C, 0x3D, 0x3F};
        for (uint8_t fid : test_ids) {
            uint8_t pid = LINPID::calculate(fid);
            uint8_t p0 = (pid >> 6) & 1;
            uint8_t p1 = (pid >> 7) & 1;
            bool valid = LINPID::verify(pid);
            std::cout << "  │ 0x" << std::hex << std::setw(2) << std::setfill('0')
                      << static_cast<int>(fid)
                      << "     │ 0x" << std::setw(2) << static_cast<int>(pid)
                      << "     │ P0=" << static_cast<int>(p0) << " P1=" << static_cast<int>(p1)
                      << " │ " << std::dec << (valid ? "VALID ✓   " : "INVALID ✗ ")
                      << " │\n";
        }
        std::cout << "  └──────────┴──────────┴───────────┴────────────┘\n\n";
    }

    // ─── Demo 2: Checksum Calculation ───────────────────────────────────
    {
        std::cout << "--- Demo 2: LIN Checksum (Classic vs Enhanced) ---\n";

        uint8_t data[] = {0x40, 0x01, 0x20, 0xFF, 0x00, 0x00, 0x00, 0x00};
        uint8_t pid = LINPID::calculate(0x10);

        uint8_t chk_classic  = LINChecksum::classic(data, 4);
        uint8_t chk_enhanced = LINChecksum::enhanced(pid, data, 4);

        std::cout << "  Data: [40 01 20 FF], Frame ID=0x10, PID=0x"
                  << std::hex << static_cast<int>(pid) << "\n";
        std::cout << "  Classic checksum  (data only):     0x"
                  << std::setw(2) << std::setfill('0') << static_cast<int>(chk_classic) << "\n";
        std::cout << "  Enhanced checksum (PID + data):    0x"
                  << std::setw(2) << static_cast<int>(chk_enhanced) << "\n";
        std::cout << std::dec;
        std::cout << "  Classic ≠ Enhanced: " << (chk_classic != chk_enhanced ? "Yes ✓" : "Bug!")
                  << " (PID changes the sum)\n\n";
    }

    // ─── Demo 3: Complete LIN Frame ─────────────────────────────────────
    {
        std::cout << "--- Demo 3: Complete LIN Frame ---\n";

        // EN: Window position frame from door module slave
        // TR: Kapı modülü slave'inden cam pozisyonu frame'i
        LINFrame window_frame;
        window_frame.id = 0x10;
        window_frame.dlc = 2;
        window_frame.data[0] = 0x50;  // window position: 80%
        window_frame.data[1] = 0x00;  // status: no error
        window_frame.enhanced_checksum = true;
        window_frame.type = LINFrameType::Unconditional;
        window_frame.name = "Window Position";
        window_frame.compute();
        window_frame.print();

        // EN: Corrupt the checksum and verify
        // TR: Checksum'u boz ve doğrulama hatası göster
        LINFrame corrupt = window_frame;
        corrupt.checksum = 0xFF;
        corrupt.name = "Corrupted Frame";
        corrupt.print();
        std::cout << "\n";
    }

    // ─── Demo 4: Schedule Table ─────────────────────────────────────────
    {
        std::cout << "--- Demo 4: LIN Schedule Table ---\n";

        // EN: BCM (Body Control Module) schedule for door/window/seat control
        // TR: BCM schedule tablosu — kapı/cam/koltuk kontrol döngüsü
        LINScheduleTable bcm_schedule("BCM_Main_Schedule");

        bcm_schedule.add(0x10, 10, LINFrameType::Unconditional, "WindowPos_FL");
        bcm_schedule.add(0x11, 10, LINFrameType::Unconditional, "WindowPos_FR");
        bcm_schedule.add(0x12, 10, LINFrameType::Unconditional, "MirrorPos_L");
        bcm_schedule.add(0x13, 10, LINFrameType::Unconditional, "MirrorPos_R");
        bcm_schedule.add(0x20, 15, LINFrameType::EventTriggered, "SeatPosition");
        bcm_schedule.add(0x30, 20, LINFrameType::Sporadic, "InteriorLight");
        bcm_schedule.add(0x3C, 10, LINFrameType::DiagRequest, "DiagRequest");
        bcm_schedule.add(0x3D, 10, LINFrameType::DiagResponse, "DiagResponse");

        bcm_schedule.print();
        std::cout << "\n";
    }

    // ─── Demo 5: Master-Slave Communication Simulation ──────────────────
    {
        std::cout << "--- Demo 5: Master-Slave LIN Network ---\n";

        // EN: Create master (BCM) and 3 slaves (door module, mirror, seat)
        // TR: Master (BCM) ve 3 slave (kapı, ayna, koltuk) oluştur
        LINMaster bcm("BCM_Master");

        LINSlaveNode door_fl("Door_FL");
        LINSlaveNode mirror_l("Mirror_L");
        LINSlaveNode seat_driver("Seat_Driver");

        // EN: Door slave responds to frame 0x10 with window position
        // TR: Kapı slave'i frame 0x10'a cam pozisyonuyla yanıt verir
        door_fl.on_frame(0x10, []() {
            LINFrame f;
            f.dlc = 3;
            f.data[0] = 0x50;  // window: 80%
            f.data[1] = 0x00;  // motor: stopped
            f.data[2] = 0x01;  // obstacle detected: no
            return f;
        });

        // EN: Door slave also responds to frame 0x11 (lock status)
        // TR: Kapı slave'i frame 0x11'e kilit durumu ile yanıt verir
        door_fl.on_frame(0x11, []() {
            LINFrame f;
            f.dlc = 2;
            f.data[0] = 0x01;  // locked: yes
            f.data[1] = 0x00;  // child lock: off
            return f;
        });

        // EN: Mirror slave responds to frame 0x12
        // TR: Ayna slave'i frame 0x12'ye pozisyon ve ısıtıcı bilgisi ile yanıt verir
        mirror_l.on_frame(0x12, []() {
            LINFrame f;
            f.dlc = 4;
            f.data[0] = 0x32;  // horizontal position: 50
            f.data[1] = 0x28;  // vertical position: 40
            f.data[2] = 0x00;  // fold state: unfolded
            f.data[3] = 0x01;  // heater: on
            return f;
        });

        // EN: Seat slave responds to frame 0x20
        // TR: Koltuk slave'i frame 0x20'ye 6 byte pozisyon verisiyle yanıt verir
        seat_driver.on_frame(0x20, []() {
            LINFrame f;
            f.dlc = 6;
            f.data[0] = 0x64;  // fore-aft: 100mm
            f.data[1] = 0x50;  // height: 80mm
            f.data[2] = 0x28;  // recline: 40 degrees
            f.data[3] = 0x3C;  // lumbar: 60%
            f.data[4] = 0x01;  // heater: level 1
            f.data[5] = 0x00;  // ventilation: off
            return f;
        });

        bcm.add_slave(door_fl);
        bcm.add_slave(mirror_l);
        bcm.add_slave(seat_driver);

        // EN: Create schedule
        // TR: Schedule tablosu oluştur (sabit zamanlamalı döngü)
        LINScheduleTable schedule("BCM_Schedule");
        schedule.add(0x10, 10, LINFrameType::Unconditional, "WindowPos_FL");
        schedule.add(0x11, 10, LINFrameType::Unconditional, "DoorLock_FL");
        schedule.add(0x12, 10, LINFrameType::Unconditional, "MirrorPos_L");
        schedule.add(0x20, 15, LINFrameType::EventTriggered, "SeatPos_Driver");
        schedule.add(0x25, 10, LINFrameType::Unconditional, "AmbientLight");

        bcm.set_schedule(schedule);
        bcm.execute_cycle();

        // EN: Verify responses
        // TR: Yanıtları doğrula
        std::cout << "\n  Detailed responses:\n";
        for (const auto& frame : bcm.log()) {
            frame.print();
        }
        std::cout << "\n";
    }

    // ─── Demo 6: Sleep/Wakeup ───────────────────────────────────────────
    {
        std::cout << "--- Demo 6: LIN Sleep/Wakeup ---\n";

        LINBusState bus_state;
        bus_state.print();

        // EN: Master sends go-to-sleep command
        // TR: Master uyku komutu gönderir (ID=0x3C, data[0]=0x00)
        std::cout << "  Master sends go-to-sleep (ID=0x3C, data[0]=0x00)\n";
        bus_state.go_to_sleep();

        LINSlaveNode test_slave("TestSlave");
        test_slave.sleep();
        std::cout << "  Slave state: " << (test_slave.is_awake() ? "AWAKE" : "SLEEPING") << "\n";
        bus_state.print();

        // EN: Wakeup signal from slave
        // TR: Slave'den uyanma sinyali (dominant darbe 250μs-5ms)
        std::cout << "  Slave sends wakeup signal (dominant pulse 250us-5ms)\n";
        test_slave.wakeup();
        bus_state.wakeup();
        std::cout << "  Slave state: " << (test_slave.is_awake() ? "AWAKE" : "SLEEPING") << "\n";
        bus_state.print();
        std::cout << "\n";
    }

    // ─── Demo 7: Diagnostic Frames ──────────────────────────────────────
    {
        std::cout << "--- Demo 7: LIN Diagnostic Frames ---\n";
        std::cout << "  LIN diagnostic uses ID=0x3C (MasterRequest) and ID=0x3D (SlaveResponse)\n";
        std::cout << "  Both are always 8 bytes.\n\n";

        // EN: Master diagnostic request: Read Supplier ID
        // TR: Master teşhis isteği: Tedarikçi ID'sini oku (SID=0xB2)
        LINFrame diag_req;
        diag_req.id = 0x3C;
        diag_req.dlc = 8;
        diag_req.data = {0x06, 0xB2, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF};
        // NAD=0x06, SID=0xB2 (ReadByIdentifier), ID=0x0000 (Supplier ID)
        diag_req.enhanced_checksum = false;  // Diagnostic frames use classic checksum
        diag_req.type = LINFrameType::DiagRequest;
        diag_req.name = "DiagRequest: ReadByID (SupplierID)";
        diag_req.compute();
        diag_req.print();

        // EN: Slave diagnostic response
        // TR: Slave teşhis yanıtı (RSID=0xF2, SupplierID=0x1234)
        LINFrame diag_resp;
        diag_resp.id = 0x3D;
        diag_resp.dlc = 8;
        diag_resp.data = {0x06, 0xF2, 0x00, 0x12, 0x34, 0x01, 0xFF, 0xFF};
        // NAD=0x06, RSID=0xF2, SupplierID=0x1234, FunctionID=0x01
        diag_resp.enhanced_checksum = false;
        diag_resp.type = LINFrameType::DiagResponse;
        diag_resp.name = "DiagResponse: SupplierID=0x1234";
        diag_resp.compute();
        diag_resp.print();

        std::cout << "\n  LIN Diagnostic Service IDs:\n";
        std::cout << "  ┌──────────┬──────────────────────────────────────────┐\n";
        std::cout << "  │ SID      │ Service Name                             │\n";
        std::cout << "  ├──────────┼──────────────────────────────────────────┤\n";
        std::cout << "  │ 0xB0     │ AssignNAD (Node Address assignment)      │\n";
        std::cout << "  │ 0xB2     │ ReadByIdentifier                         │\n";
        std::cout << "  │ 0xB3     │ ConditionalChangeNAD                     │\n";
        std::cout << "  │ 0xB4     │ DataDump (vendor-specific)               │\n";
        std::cout << "  │ 0xB5     │ AssignNAD via SNPD                       │\n";
        std::cout << "  │ 0xB6     │ SaveConfiguration                        │\n";
        std::cout << "  │ 0xB7     │ AssignFrameIdRange                       │\n";
        std::cout << "  └──────────┴──────────────────────────────────────────┘\n\n";
    }

    // ─── Demo 8: LIN vs CAN Comparison ──────────────────────────────────
    {
        std::cout << "--- Demo 8: LIN vs CAN Comparison ---\n\n";
        std::cout << "  ┌──────────────────────┬──────────────┬──────────────────────────┐\n";
        std::cout << "  │ Feature              │ LIN          │ CAN                      │\n";
        std::cout << "  ├──────────────────────┼──────────────┼──────────────────────────┤\n";
        std::cout << "  │ Speed                │ 20 kbps      │ 1 Mbps / 8 Mbps (FD)     │\n";
        std::cout << "  │ Wiring               │ 1 wire + GND │ 2 wires (twisted pair)   │\n";
        std::cout << "  │ Cost/node            │ ~$0.50       │ ~$2-5                    │\n";
        std::cout << "  │ Architecture         │ Master-Slave │ Multi-master             │\n";
        std::cout << "  │ Arbitration          │ Schedule     │ ID-based bitwise         │\n";
        std::cout << "  │ Max payload          │ 8 bytes      │ 8B / 64B (FD)            │\n";
        std::cout << "  │ Max nodes            │ 1+16         │ Unlimited (practical ~30)│\n";
        std::cout << "  │ Deterministic        │ Yes (100%)   │ No (priority-based)      │\n";
        std::cout << "  │ Error detection      │ Checksum     │ CRC + ACK + error frame  │\n";
        std::cout << "  │ Safety level         │ QM / ASIL-A  │ Up to ASIL-D             │\n";
        std::cout << "  │ Typical use          │ Window, seat │ Engine, brakes, ADAS     │\n";
        std::cout << "  │                      │ mirror, light│                          │\n";
        std::cout << "  └──────────────────────┴──────────────┴──────────────────────────┘\n\n";

        std::cout << "  Typical car LIN topology:\n";
        std::cout << "  ┌──────────────────┐\n";
        std::cout << "  │   BCM            │  Body Control Module = LIN Master\n";
        std::cout << "  │  (Master)        │  Also CAN node → bridges LIN↔CAN\n";
        std::cout << "  └──┬──┬────┬─────┬─┘\n";
        std::cout << "     │  │    │     │   Single wire LIN bus\n";
        std::cout << "  ┌──▼┐┌▼──┐┌▼───┐┌▼────┐\n";
        std::cout << "  │Win││Mir││Seat││Light│  LIN Slaves\n";
        std::cout << "  │Mot││Adj││Ctl ││Ctrl │  (sub-$1 MCU each)\n";
        std::cout << "  └───┘└───┘└────┘└─────┘\n\n";
    }

    std::cout << "============================================\n";
    std::cout << "   End of LIN Bus\n";
    std::cout << "============================================\n";

    return 0;
}
