/**
 * @file module_18_driver_development/04_mini_ecu_project.cpp
 * @brief Capstone Project — Body Control Module (BCM) Simulation
 *
 * @details
 * =============================================================================
 * [THEORY: BCM (Body Control Module) Architecture / TEORİ: BCM (Gövde Kontrol Modülü) Mimarisi]
 * =============================================================================
 *
 * EN: A BCM manages body electronics in a vehicle: lights, wipers, windows,
 *     door locks, interior lighting, etc. It communicates via CAN and LIN
 *     buses, stores diagnostic trouble codes (DTCs), responds to UDS
 *     diagnostic requests, and runs cyclic tasks at different periods.
 *
 *     BCM Software Architecture:
 *     ┌─────────────────────────────────────────────────────┐
 *     │                    APPLICATION                      │
 *     │  ┌─────────┐ ┌──────────┐ ┌─────────┐ ┌──────────┐  │
 *     │  │Headlamp │ │Door Lock │ │Wiper    │ │Interior  │  │
 *     │  │Control  │ │Control   │ │Control  │ │Light     │  │
 *     │  └────┬────┘ └────┬─────┘ └────┬────┘ └────┬─────┘  │
 *     │       │           │            │           │        │
 *     ├───────┴───────────┴────────────┴───────────┴────────┤
 *     │                  SERVICES LAYER                     │
 *     │  ┌─────┐ ┌─────┐ ┌─────┐ ┌─────┐ ┌──────┐ ┌──────┐  │
 *     │  │ CAN │ │ LIN │ │ NvM │ │ DTC │ │  UDS │ │ WDG  │  │
 *     │  └─────┘ └─────┘ └─────┘ └─────┘ └──────┘ └──────┘  │
 *     ├─────────────────────────────────────────────────────┤
 *     │                   HAL / DRIVERS                     │
 *     │       GPIO  │  UART  │  SPI  │  Timer  │  Flash     │
 *     └─────────────────────────────────────────────────────┘
 *
 * =============================================================================
 * [THEORY: Cyclic Task Scheduling / TEORİ: Döngüsel Görev Zamanlama]
 * =============================================================================
 *
 * EN:
 *     ┌───────────┬──────────────────────────────────┐
 *     │ Period    │ Tasks                            │
 *     ├───────────┼──────────────────────────────────┤
 *     │ 10 ms     │ CAN RX, Debounce, Watchdog feed  │
 *     │ 50 ms     │ Lamp control, Wiper control      │
 *     │ 100 ms    │ LIN schedule, DTC check, NvM     │
 *     │ 1000 ms   │ Diagnostics, Health monitoring   │
 *     └───────────┴──────────────────────────────────┘
 *
 * TR: BCM, araç gövde elektroniğini yönetir: farlar, silecekler, camlar,
 *     kapı kilitleri, iç aydınlatma vb. CAN ve LIN bus'ları üzerinden
 *     haberleşir, DTC (arıza kodu) saklar, UDS tanı taleplerine cevap verir.
 *
 *     BCM Yazılım Mimarisi:
 *     ┌─────────────────────────────────────────────────────┐
 *     │                    UYGULAMA                         │
 *     │  ┌─────────┐ ┌──────────┐ ┌─────────┐ ┌──────────┐  │
 *     │  │Far      │ │Kapı Kilit│ │Silecek  │ │İç Aydın. │  │
 *     │  │Kontrol  │ │Kontrol   │ │Kontrol  │ │Kontrol   │  │
 *     │  └────┬────┘ └────┬─────┘ └────┬────┘ └────┬─────┘  │
 *     ├───────┴───────────┴────────────┴───────────┴────────┤
 *     │                SERVİS KATMANI                       │
 *     │  ┌─────┐ ┌─────┐ ┌─────┐ ┌─────┐ ┌──────┐ ┌──────┐  │
 *     │  │ CAN │ │ LIN │ │ NvM │ │ DTC │ │  UDS │ │ WDG  │  │
 *     │  └─────┘ └─────┘ └─────┘ └─────┘ └──────┘ └──────┘  │
 *     ├─────────────────────────────────────────────────────┤
 *     │                 HAL / SÜRÜCÜLER                     │
 *     │     GPIO  │  UART  │  SPI  │  Timer  │  Flash       │
 *     └─────────────────────────────────────────────────────┘
 *
 *     Periyodik görev zamanlası:
 *     ┌───────────┬──────────────────────────────────┐
 *     │ Periyot   │ Görevler                         │
 *     ├───────────┼──────────────────────────────────┤
 *     │ 10 ms     │ CAN RX, debounce, WDG besleme    │
 *     │ 50 ms     │ Lamba kontrolü, silecek kontrolü │
 *     │ 100 ms    │ LIN çizelgesi, DTC kontrol, NvM  │
 *     │ 1000 ms   │ Tanı, sağlık izleme              │
 *     └───────────┴──────────────────────────────────┘
 *
 * @note Compile: g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion 04_mini_ecu_project.cpp -o 04_mini_ecu
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
#include <map>
#include <algorithm>

// ═════════════════════════════════════════════════════════════════════════════
// PART 1: GPIO Module — Digital I/O for BCM
// ═════════════════════════════════════════════════════════════════════════════

namespace bcm {

enum class PinState : uint8_t { LOW = 0, HIGH = 1 };

// EN: Named BCM GPIO pins
// TR: İsimlendirilmiş BCM GPIO pinleri
enum class BCM_Pin : uint8_t {
    HEADLAMP_LEFT = 0,
    HEADLAMP_RIGHT,
    TAIL_LAMP_LEFT,
    TAIL_LAMP_RIGHT,
    INTERIOR_LIGHT,
    DOOR_LOCK_MOTOR,
    WIPER_MOTOR,
    BRAKE_LIGHT,
    DOOR_SWITCH,      // Input
    LIGHT_SENSOR,     // Input (simulated)
    PIN_COUNT
};

constexpr std::string_view PinName(BCM_Pin pin) {
    switch (pin) {
        case BCM_Pin::HEADLAMP_LEFT:   return "HeadlampL";
        case BCM_Pin::HEADLAMP_RIGHT:  return "HeadlampR";
        case BCM_Pin::TAIL_LAMP_LEFT:  return "TailLampL";
        case BCM_Pin::TAIL_LAMP_RIGHT: return "TailLampR";
        case BCM_Pin::INTERIOR_LIGHT:  return "InteriorLt";
        case BCM_Pin::DOOR_LOCK_MOTOR: return "DoorLock";
        case BCM_Pin::WIPER_MOTOR:     return "WiperMotor";
        case BCM_Pin::BRAKE_LIGHT:     return "BrakeLamp";
        case BCM_Pin::DOOR_SWITCH:     return "DoorSwitch";
        case BCM_Pin::LIGHT_SENSOR:    return "LightSens";
        case BCM_Pin::PIN_COUNT:       break;
    }
    return "Unknown";
}

class GPIOModule {
    std::array<PinState, static_cast<size_t>(BCM_Pin::PIN_COUNT)> pins_{};

public:
    GPIOModule() { pins_.fill(PinState::LOW); }

    void Write(BCM_Pin pin, PinState state) {
        pins_[static_cast<size_t>(pin)] = state;
    }

    PinState Read(BCM_Pin pin) const {
        return pins_[static_cast<size_t>(pin)];
    }

    void Toggle(BCM_Pin pin) {
        auto& p = pins_[static_cast<size_t>(pin)];
        p = (p == PinState::HIGH) ? PinState::LOW : PinState::HIGH;
    }

    void DumpOutputs() const {
        std::cout << "    GPIO: ";
        for (size_t i = 0; i < static_cast<size_t>(BCM_Pin::PIN_COUNT); ++i) {
            auto pin = static_cast<BCM_Pin>(i);
            if (pins_[i] == PinState::HIGH)
                std::cout << PinName(pin) << "=ON ";
        }
        std::cout << "\n";
    }
};

// ═════════════════════════════════════════════════════════════════════════════
// PART 2: CAN Module — Message Routing
// ═════════════════════════════════════════════════════════════════════════════

struct CAN_Message {
    uint32_t id = 0;
    uint8_t dlc = 0;
    std::array<uint8_t, 8> data{};
};

// EN: Known CAN message IDs for BCM
// TR: BCM için bilinen CAN mesaj ID'leri — her komut sabit bir ID ile tanımlanır
constexpr uint32_t CAN_ID_LIGHT_CMD    = 0x120;
constexpr uint32_t CAN_ID_DOOR_CMD     = 0x130;
constexpr uint32_t CAN_ID_WIPER_CMD    = 0x140;
constexpr uint32_t CAN_ID_BRAKE_STATUS = 0x150;
constexpr uint32_t CAN_ID_BCM_STATUS   = 0x200;

class CANModule {
    std::vector<CAN_Message> rx_queue_;
    std::vector<CAN_Message> tx_queue_;

public:
    // EN: Simulate receiving a CAN message
    // TR: CAN mesajı alımını simüle et — test amaçlı RX kuyruğuna mesaj ekler
    void SimulateRx(const CAN_Message& msg) { rx_queue_.push_back(msg); }

    std::optional<CAN_Message> GetNextRx() {
        if (rx_queue_.empty()) return std::nullopt;
        auto msg = rx_queue_.front();
        rx_queue_.erase(rx_queue_.begin());
        return msg;
    }

    void Transmit(const CAN_Message& msg) {
        tx_queue_.push_back(msg);
    }

    void PrintTxQueue() const {
        for (const auto& msg : tx_queue_) {
            std::cout << "    CAN TX [0x" << std::hex << msg.id << std::dec
                      << "] DLC=" << static_cast<int>(msg.dlc) << " data:";
            for (uint8_t i = 0; i < msg.dlc && i < 8; ++i)
                std::cout << " " << std::hex << std::setw(2) << std::setfill('0')
                          << static_cast<int>(msg.data[i]);
            std::cout << std::dec << std::setfill(' ') << "\n";
        }
    }

    void ClearTx() { tx_queue_.clear(); }
    [[nodiscard]] size_t RxPending() const { return rx_queue_.size(); }
};

// ═════════════════════════════════════════════════════════════════════════════
// PART 3: LIN Module — Schedule Table Execution
// ═════════════════════════════════════════════════════════════════════════════

struct LIN_Frame {
    uint8_t pid = 0;       // Protected identifier — TR: Korumalı tanımlayıcı (PID)
    uint8_t length = 0;
    std::array<uint8_t, 8> data{};
    std::string_view description;
};

class LINModule {
    struct ScheduleEntry { uint8_t pid; uint16_t slot_ms; };

    std::vector<ScheduleEntry> schedule_;
    size_t schedule_idx_ = 0;
    std::vector<LIN_Frame> responses_;

public:
    void AddScheduleEntry(uint8_t pid, uint16_t slot_ms) {
        schedule_.push_back({pid, slot_ms});
    }

    void AddResponse(const LIN_Frame& frame) {
        responses_.push_back(frame);
    }

    // EN: Execute one slot of the schedule table
    // TR: Çizelge tablosunun bir slotunu çalıştır — sıradaki PID'ye ait yanıt aranır
    std::optional<LIN_Frame> ExecuteSlot() {
        if (schedule_.empty()) return std::nullopt;

        auto& entry = schedule_[schedule_idx_];
        schedule_idx_ = (schedule_idx_ + 1) % schedule_.size();

        // EN: Find matching response
        // TR: Eşleşen yanıtı bul — PID'ye göre önceden kaydedilmiş LIN frame döndürür
        for (const auto& resp : responses_) {
            if (resp.pid == entry.pid) return resp;
        }
        return std::nullopt;
    }

    void PrintSchedule() const {
        std::cout << "    LIN Schedule:\n";
        for (size_t i = 0; i < schedule_.size(); ++i) {
            std::cout << "      Slot " << i << ": PID=0x" << std::hex
                      << static_cast<int>(schedule_[i].pid) << std::dec
                      << " period=" << schedule_[i].slot_ms << "ms\n";
        }
    }
};

// ═════════════════════════════════════════════════════════════════════════════
// PART 4: DTC Storage + NvM (Non-Volatile Memory)
// ═════════════════════════════════════════════════════════════════════════════

enum class DTC_Status : uint8_t {
    PASSIVE = 0,
    ACTIVE  = 1,
    CONFIRMED = 2
};

struct DTC_Entry {
    uint32_t code = 0;
    DTC_Status status = DTC_Status::PASSIVE;
    uint16_t occurrence_count = 0;
    uint32_t first_occurrence_tick = 0;
    std::string_view description;
};

constexpr std::string_view DTCStatusStr(DTC_Status s) {
    switch (s) {
        case DTC_Status::PASSIVE:   return "PASSIVE";
        case DTC_Status::ACTIVE:    return "ACTIVE";
        case DTC_Status::CONFIRMED: return "CONFIRMED";
    }
    return "UNKNOWN";
}

class DTCStore {
    static constexpr size_t MAX_DTCS = 16;
    std::array<DTC_Entry, MAX_DTCS> dtcs_{};
    size_t count_ = 0;

public:
    bool Report(uint32_t code, std::string_view desc, uint32_t tick) {
        // EN: Find existing or add new
        // TR: Mevcut DTC'yi bul veya yeni ekle — aynı kod 3 kez tekrarlanırsa CONFIRMED olur
        for (size_t i = 0; i < count_; ++i) {
            if (dtcs_[i].code == code) {
                dtcs_[i].status = DTC_Status::ACTIVE;
                dtcs_[i].occurrence_count++;
                if (dtcs_[i].occurrence_count >= 3)
                    dtcs_[i].status = DTC_Status::CONFIRMED;
                return true;
            }
        }
        if (count_ >= MAX_DTCS) return false;
        dtcs_[count_++] = {code, DTC_Status::ACTIVE, 1, tick, desc};
        return true;
    }

    void ClearAll() {
        for (size_t i = 0; i < count_; ++i)
            dtcs_[i].status = DTC_Status::PASSIVE;
        count_ = 0;
    }

    void Print() const {
        std::cout << "    DTC Store (" << count_ << " entries):\n";
        for (size_t i = 0; i < count_; ++i) {
            std::cout << "      [0x" << std::hex << std::setw(6) << std::setfill('0')
                      << dtcs_[i].code << std::dec << std::setfill(' ')
                      << "] " << std::setw(10) << DTCStatusStr(dtcs_[i].status)
                      << " cnt=" << dtcs_[i].occurrence_count
                      << " " << dtcs_[i].description << "\n";
        }
    }

    [[nodiscard]] size_t Count() const { return count_; }

    std::optional<DTC_Entry> Get(size_t idx) const {
        if (idx >= count_) return std::nullopt;
        return dtcs_[idx];
    }
};

// EN: Simple NvM simulation (learn values / calibration data)
// TR: Basit NvM simülasyonu (öğrenme değerleri / kalibrasyon verisi)
class NvMStore {
    static constexpr size_t MAX_BLOCKS = 8;
    struct NvMBlock { uint16_t id; uint32_t data; bool valid; };
    std::array<NvMBlock, MAX_BLOCKS> blocks_{};
    size_t count_ = 0;

public:
    bool Write(uint16_t id, uint32_t data) {
        for (size_t i = 0; i < count_; ++i) {
            if (blocks_[i].id == id) {
                blocks_[i].data = data;
                blocks_[i].valid = true;
                return true;
            }
        }
        if (count_ >= MAX_BLOCKS) return false;
        blocks_[count_++] = {id, data, true};
        return true;
    }

    std::optional<uint32_t> Read(uint16_t id) const {
        for (size_t i = 0; i < count_; ++i) {
            if (blocks_[i].id == id && blocks_[i].valid)
                return blocks_[i].data;
        }
        return std::nullopt;
    }

    void Print() const {
        std::cout << "    NvM Store (" << count_ << " blocks):\n";
        for (size_t i = 0; i < count_; ++i) {
            std::cout << "      Block 0x" << std::hex << blocks_[i].id << std::dec
                      << " = " << blocks_[i].data
                      << (blocks_[i].valid ? " [valid]" : " [invalid]") << "\n";
        }
    }
};

// ═════════════════════════════════════════════════════════════════════════════
// PART 5: UDS Diagnostic Handler
// ═════════════════════════════════════════════════════════════════════════════

class UDSHandler {
public:
    // EN: UDS Service IDs
    // TR: UDS Servis ID'leri — ISO 14229 standardına göre tanı hizmet kodları
    static constexpr uint8_t SID_READ_DID       = 0x22;
    static constexpr uint8_t SID_WRITE_DID      = 0x2E;
    static constexpr uint8_t SID_CLEAR_DTC      = 0x14;
    static constexpr uint8_t SID_READ_DTC_INFO  = 0x19;
    static constexpr uint8_t SID_ROUTINE_CTRL   = 0x31;

    // EN: DID identifiers
    // TR: DID tanımlayıcıları — okuma/yazma yapılabilecek veri noktalarının adresleri
    static constexpr uint16_t DID_SW_VERSION    = 0xF189;
    static constexpr uint16_t DID_HW_VERSION    = 0xF191;
    static constexpr uint16_t DID_SERIAL_NUMBER = 0xF18C;
    static constexpr uint16_t DID_LAMP_STATUS   = 0x0100;

    struct UDS_Request  { uint8_t sid; uint16_t param; std::vector<uint8_t> data; };
    struct UDS_Response { uint8_t sid; bool positive; uint8_t nrc; std::vector<uint8_t> data; };

    UDS_Response Process(const UDS_Request& req, DTCStore& dtcs, NvMStore& nvm,
                         const GPIOModule& gpio) {
        switch (req.sid) {
            case SID_READ_DID:    return HandleReadDID(req, nvm, gpio);
            case SID_WRITE_DID:   return HandleWriteDID(req, nvm);
            case SID_CLEAR_DTC:   return HandleClearDTC(dtcs);
            case SID_READ_DTC_INFO: return HandleReadDTCInfo(dtcs);
            case SID_ROUTINE_CTRL:  return HandleRoutineCtrl(req);
            default:
                return {req.sid, false, 0x11, {}};  // serviceNotSupported — TR: servis desteklenmiyor
        }
    }

private:
    UDS_Response HandleReadDID(const UDS_Request& req, NvMStore& nvm,
                               const GPIOModule& gpio) {
        switch (req.param) {
            case DID_SW_VERSION:
                return {static_cast<uint8_t>(req.sid + 0x40), true, 0, {'1', '.', '0', '.', '3'}};
            case DID_HW_VERSION:
                return {static_cast<uint8_t>(req.sid + 0x40), true, 0, {'H', 'W', '2', '.', '1'}};
            case DID_SERIAL_NUMBER: {
                auto sn = nvm.Read(0x0001);
                uint32_t val = sn.value_or(0);
                return {static_cast<uint8_t>(req.sid + 0x40), true, 0,
                    {static_cast<uint8_t>((val >> 24) & 0xFF),
                     static_cast<uint8_t>((val >> 16) & 0xFF),
                     static_cast<uint8_t>((val >> 8) & 0xFF),
                     static_cast<uint8_t>(val & 0xFF)}};
            }
            case DID_LAMP_STATUS: {
                uint8_t status = 0;
                if (gpio.Read(BCM_Pin::HEADLAMP_LEFT) == PinState::HIGH)  status |= 0x01;
                if (gpio.Read(BCM_Pin::HEADLAMP_RIGHT) == PinState::HIGH) status |= 0x02;
                if (gpio.Read(BCM_Pin::TAIL_LAMP_LEFT) == PinState::HIGH) status |= 0x04;
                if (gpio.Read(BCM_Pin::TAIL_LAMP_RIGHT) == PinState::HIGH) status |= 0x08;
                return {static_cast<uint8_t>(req.sid + 0x40), true, 0, {status}};
            }
            default:
                return {req.sid, false, 0x31, {}};  // requestOutOfRange — TR: istek aralık dışı
        }
    }

    static UDS_Response HandleWriteDID(const UDS_Request& req, NvMStore& nvm) {
        if (req.param == DID_SERIAL_NUMBER && req.data.size() >= 4) {
            uint32_t sn = (static_cast<uint32_t>(req.data[0]) << 24) |
                          (static_cast<uint32_t>(req.data[1]) << 16) |
                          (static_cast<uint32_t>(req.data[2]) << 8)  |
                          static_cast<uint32_t>(req.data[3]);
            nvm.Write(0x0001, sn);
            return {static_cast<uint8_t>(req.sid + 0x40), true, 0, {}};
        }
        return {req.sid, false, 0x31, {}};
    }

    static UDS_Response HandleClearDTC(DTCStore& dtcs) {
        dtcs.ClearAll();
        return {static_cast<uint8_t>(SID_CLEAR_DTC + 0x40), true, 0, {}};
    }

    static UDS_Response HandleReadDTCInfo(DTCStore& dtcs) {
        std::vector<uint8_t> data;
        for (size_t i = 0; i < dtcs.Count(); ++i) {
            auto dtc = dtcs.Get(i);
            if (dtc) {
                data.push_back(static_cast<uint8_t>((dtc->code >> 16) & 0xFF));
                data.push_back(static_cast<uint8_t>((dtc->code >> 8) & 0xFF));
                data.push_back(static_cast<uint8_t>(dtc->code & 0xFF));
                data.push_back(static_cast<uint8_t>(dtc->status));
            }
        }
        return {static_cast<uint8_t>(SID_READ_DTC_INFO + 0x40), true, 0, data};
    }

    static UDS_Response HandleRoutineCtrl(const UDS_Request& req) {
        std::cout << "      Routine 0x" << std::hex << req.param << std::dec << " executed\n";
        return {static_cast<uint8_t>(SID_ROUTINE_CTRL + 0x40), true, 0, {0x00}};
    }
};

// ═════════════════════════════════════════════════════════════════════════════
// PART 6: Watchdog Module
// ═════════════════════════════════════════════════════════════════════════════

class Watchdog {
    uint32_t timeout_ticks_;
    uint32_t counter_;
    bool expired_ = false;

public:
    explicit Watchdog(uint32_t timeout) : timeout_ticks_(timeout), counter_(timeout) {}

    void Feed() { counter_ = timeout_ticks_; expired_ = false; }

    void Tick() {
        if (counter_ > 0) --counter_;
        if (counter_ == 0 && !expired_) {
            expired_ = true;
            std::cout << "    [WDG] *** WATCHDOG EXPIRED — SYSTEM RESET ***\n";
        }
    }

    [[nodiscard]] bool IsExpired() const { return expired_; }
    [[nodiscard]] uint32_t Remaining() const { return counter_; }
};

// ═════════════════════════════════════════════════════════════════════════════
// PART 7: BCM State Manager + Cyclic Task Scheduler
// ═════════════════════════════════════════════════════════════════════════════

enum class BCM_State : uint8_t { STARTUP, NORMAL, SLEEP, WAKEUP };

constexpr std::string_view BCMStateStr(BCM_State s) {
    switch (s) {
        case BCM_State::STARTUP: return "STARTUP";
        case BCM_State::NORMAL:  return "NORMAL";
        case BCM_State::SLEEP:   return "SLEEP";
        case BCM_State::WAKEUP:  return "WAKEUP";
    }
    return "UNKNOWN";
}

// EN: Cyclic task entry
// TR: Periyodik görev girişi — isim, periyot, son çalıştırma zamanı ve çalıştırma fonksiyonunu taşır
struct CyclicTask {
    std::string_view name;
    uint32_t period_ms;
    uint32_t last_run;
    std::function<void(uint32_t)> execute;
};

class BCMController {
    GPIOModule gpio_;
    CANModule can_;
    LINModule lin_;
    DTCStore dtcs_;
    NvMStore nvm_;
    UDSHandler uds_;
    Watchdog wdg_{100};  // 100 tick timeout
    BCM_State state_ = BCM_State::STARTUP;
    uint32_t tick_ = 0;
    std::vector<CyclicTask> tasks_;

public:
    BCMController() { SetupTasks(); SetupLIN(); }

    void SetupTasks() {
        // EN: 10ms task group
        // TR: 10ms görev grubu — CAN RX ve watchdog besleme (en yüksek öncelik)
        tasks_.push_back({"CAN_RX", 10, 0, [this](uint32_t t) { Task_CAN_RX(t); }});
        tasks_.push_back({"WDG_Feed", 10, 0, [this](uint32_t) { wdg_.Feed(); }});

        // EN: 50ms task group
        // TR: 50ms görev grubu — lamba ve silecek kontrolü (orta öncelik)
        tasks_.push_back({"LampControl", 50, 0, [this](uint32_t t) { Task_LampControl(t); }});

        // EN: 100ms task group
        // TR: 100ms görev grubu — LIN çizelgesi, DTC kontrolü ve NvM işlemleri
        tasks_.push_back({"LIN_Schedule", 100, 0, [this](uint32_t t) { Task_LIN(t); }});
        tasks_.push_back({"DTC_Check", 100, 0, [this](uint32_t t) { Task_DTCCheck(t); }});

        // EN: 1000ms task group
        // TR: 1000ms görev grubu — durum TX ve sağlık izleme (düşük öncelik)
        tasks_.push_back({"Status_TX", 1000, 0, [this](uint32_t t) { Task_StatusTX(t); }});
    }

    void SetupLIN() {
        lin_.AddScheduleEntry(0x20, 10);   // Rain sensor
        lin_.AddScheduleEntry(0x21, 10);   // Ambient light
        lin_.AddResponse({0x20, 2, {0x00, 0x50, 0,0,0,0,0,0}, "RainSensor"});
        lin_.AddResponse({0x21, 2, {0x03, 0xE8, 0,0,0,0,0,0}, "AmbientLight"});
    }

    // EN: Run scheduler for N ticks (each tick = 10ms)
    // TR: Zamanlayıcıyı N tick çalıştır (her tick = 10ms) — durum makinesini günceller, vadesi gelen görevleri yürütür
    void Run(uint32_t num_ticks) {
        for (uint32_t i = 0; i < num_ticks; ++i) {
            tick_ += 10;  // 10ms per tick
            wdg_.Tick();

            // EN: State machine
            // TR: Durum makinesini güncelle — STARTUP→NORMAL geçişi zamana bağlı
            UpdateState();

            if (state_ == BCM_State::SLEEP) continue;

            // EN: Execute due tasks
            // TR: Vadesi gelen görevleri çalıştır — geçen süre periyodu aştıysa tetiklenir
            for (auto& task : tasks_) {
                if ((tick_ - task.last_run) >= task.period_ms) {
                    task.execute(tick_);
                    task.last_run = tick_;
                }
            }
        }
    }

    // EN: Inject CAN message for processing
    // TR: İşlenmek üzere CAN mesajı enjekte et — test amaçlı RX kuyruğuna ekler
    void InjectCAN(const CAN_Message& msg) { can_.SimulateRx(msg); }

    // EN: Process UDS request
    // TR: UDS talebini işle — DTC, NvM ve GPIO modüllerine erişerek tanı hizmeti sunar
    UDSHandler::UDS_Response ProcessUDS(const UDSHandler::UDS_Request& req) {
        return uds_.Process(req, dtcs_, nvm_, gpio_);
    }

    void PrintStatus() const {
        std::cout << "    BCM State: " << BCMStateStr(state_)
                  << " | Tick: " << tick_ << "ms"
                  << " | WDG: " << wdg_.Remaining() << "\n";
        gpio_.DumpOutputs();
        dtcs_.Print();
        nvm_.Print();
    }

    void PrintCANTx() const { can_.PrintTxQueue(); }
    void ClearCANTx() { can_.ClearTx(); }

    // EN: Report DTC externally (for testing)
    // TR: Dışarıdan DTC bildir (test amaçlı) — arıza kodunu DTC deposuna kaydeder
    void ReportDTC(uint32_t code, std::string_view desc) {
        dtcs_.Report(code, desc, tick_);
    }

    // EN: Write NvM (for testing)
    // TR: NvM'ye yaz (test amaçlı) — seri numarası, kalibrasyon gibi kalıcı verileri saklar
    void WriteNvM(uint16_t id, uint32_t data) { nvm_.Write(id, data); }

private:
    void UpdateState() {
        switch (state_) {
            case BCM_State::STARTUP:
                // EN: Transition to NORMAL after initialization time
                // TR: Başlatma süresi sonrası NORMAL'e geç — tick >= 100 (1 saniye) olunca
                if (tick_ >= 100) {
                    state_ = BCM_State::NORMAL;
                    std::cout << "    [STATE] STARTUP → NORMAL (tick=" << tick_ << ")\n";
                }
                break;
            case BCM_State::NORMAL:
                // EN: Could transition to SLEEP on bus silence or explicit command
                // TR: Bus sessizliğinde veya açık komutla SLEEP'e geçebilir
                break;
            case BCM_State::SLEEP:
                break;
            case BCM_State::WAKEUP:
                state_ = BCM_State::NORMAL;
                break;
        }
    }

    void Task_CAN_RX([[maybe_unused]] uint32_t tick) {
        while (auto msg = can_.GetNextRx()) {
            ProcessCANMessage(*msg, tick);
        }
    }

    void ProcessCANMessage(const CAN_Message& msg, uint32_t tick) {
        switch (msg.id) {
            case CAN_ID_LIGHT_CMD:
                if (msg.dlc >= 1) {
                    bool on = (msg.data[0] & 0x01) != 0;
                    auto s = on ? PinState::HIGH : PinState::LOW;
                    gpio_.Write(BCM_Pin::HEADLAMP_LEFT, s);
                    gpio_.Write(BCM_Pin::HEADLAMP_RIGHT, s);
                    gpio_.Write(BCM_Pin::TAIL_LAMP_LEFT, s);
                    gpio_.Write(BCM_Pin::TAIL_LAMP_RIGHT, s);
                    std::cout << "    [CAN] Light cmd: " << (on ? "ON" : "OFF")
                              << " (tick=" << tick << ")\n";
                }
                break;
            case CAN_ID_DOOR_CMD:
                if (msg.dlc >= 1) {
                    bool lock = (msg.data[0] & 0x01) != 0;
                    gpio_.Write(BCM_Pin::DOOR_LOCK_MOTOR,
                               lock ? PinState::HIGH : PinState::LOW);
                    std::cout << "    [CAN] Door " << (lock ? "LOCK" : "UNLOCK")
                              << " (tick=" << tick << ")\n";
                }
                break;
            case CAN_ID_WIPER_CMD:
                if (msg.dlc >= 1) {
                    bool on = (msg.data[0] & 0x01) != 0;
                    gpio_.Write(BCM_Pin::WIPER_MOTOR,
                               on ? PinState::HIGH : PinState::LOW);
                    std::cout << "    [CAN] Wiper " << (on ? "ON" : "OFF")
                              << " (tick=" << tick << ")\n";
                }
                break;
            case CAN_ID_BRAKE_STATUS:
                if (msg.dlc >= 1) {
                    bool brake = (msg.data[0] & 0x01) != 0;
                    gpio_.Write(BCM_Pin::BRAKE_LIGHT,
                               brake ? PinState::HIGH : PinState::LOW);
                }
                break;
            default:
                break;
        }
    }

    void Task_LampControl([[maybe_unused]] uint32_t tick) {
        // EN: Simple auto-light logic based on light sensor
        // TR: Işık sensörüne dayalı basit otomatik lamba mantığı (gerçek BCM'de ADC ile okunur)
        // (In real BCM, this reads ADC value from ambient light sensor)
    }

    void Task_LIN([[maybe_unused]] uint32_t tick) {
        auto frame = lin_.ExecuteSlot();
        if (frame) {
            // EN: Process LIN response silently in background
            // TR: LIN yanıtını arka planda sessizce işle — sensör verisi alınır
            (void)frame;
        }
    }

    void Task_DTCCheck(uint32_t tick) {
        // EN: Check for faults (simulated: headlamp both must be same state)
        // TR: Arıza kontrolü (simülasyon: her iki far aynı durumda olmalı, asimetri = arıza)
        auto left = gpio_.Read(BCM_Pin::HEADLAMP_LEFT);
        auto right = gpio_.Read(BCM_Pin::HEADLAMP_RIGHT);
        if (left != right) {
            dtcs_.Report(0xC10100, "Headlamp asymmetry", tick);
        }
    }

    void Task_StatusTX(uint32_t tick) {
        CAN_Message status{};
        status.id = CAN_ID_BCM_STATUS;
        status.dlc = 4;
        status.data[0] = static_cast<uint8_t>(state_);
        status.data[1] = static_cast<uint8_t>(dtcs_.Count());
        status.data[2] = static_cast<uint8_t>((tick >> 8) & 0xFF);
        status.data[3] = static_cast<uint8_t>(tick & 0xFF);
        can_.Transmit(status);
    }
};

}  // namespace bcm

// ═════════════════════════════════════════════════════════════════════════════
// MAIN — BCM Integration Demos
// ═════════════════════════════════════════════════════════════════════════════

int main() {
    std::cout << "╔══════════════════════════════════════════════════════════════╗\n"
              << "║ Module 18: Mini ECU Project — BCM (Body Control Module)      ║\n"
              << "╚══════════════════════════════════════════════════════════════╝\n\n";

    bcm::BCMController ecu;

    // --- Demo 1: Startup + State Transition ---
    std::cout << "--- Demo 1: BCM Startup Sequence ---\n";
    {
        ecu.WriteNvM(0x0001, 0x12345678);  // Serial number
        ecu.Run(12);  // 120ms → should transition STARTUP → NORMAL
        ecu.PrintStatus();
    }

    // --- Demo 2: CAN Message Routing ---
    std::cout << "\n--- Demo 2: CAN Message Routing ---\n";
    {
        // EN: Turn on headlamps via CAN
        // TR: Farları CAN üzerinden aç — 0x120 ID'li mesajın data[0]=0x01 gönderilir
        bcm::CAN_Message light_on{bcm::CAN_ID_LIGHT_CMD, 1, {0x01}};
        ecu.InjectCAN(light_on);
        ecu.Run(1);  // Process CAN

        // EN: Lock doors via CAN
        // TR: Kapıları CAN üzerinden kilitle — 0x130 ID'li mesaj ile kilit motoru etkinleşir
        bcm::CAN_Message door_lock{bcm::CAN_ID_DOOR_CMD, 1, {0x01}};
        ecu.InjectCAN(door_lock);
        ecu.Run(1);

        // EN: Enable wiper
        // TR: Sileceği etkinleştir — 0x140 ID'li CAN mesajı ile silecek motoru çalışır
        bcm::CAN_Message wiper_on{bcm::CAN_ID_WIPER_CMD, 1, {0x01}};
        ecu.InjectCAN(wiper_on);
        ecu.Run(1);

        ecu.PrintStatus();
    }

    // --- Demo 3: DTC Storage ---
    std::cout << "\n--- Demo 3: DTC Reporting + Confirmation ---\n";
    {
        //  EN: Report same DTC 3 times → should become CONFIRMED
        //  TR: Aynı DTC'yi 3 kez bildir → CONFIRMED durumuna geçmeli (otomotiv standardı)
        ecu.ReportDTC(0xB10000, "Interior light short circuit");
        ecu.ReportDTC(0xB10000, "Interior light short circuit");
        ecu.ReportDTC(0xB10000, "Interior light short circuit");
        ecu.ReportDTC(0xC20100, "LIN communication timeout");
        ecu.PrintStatus();
    }

    // --- Demo 4: UDS Diagnostics ---
    std::cout << "\n--- Demo 4: UDS Diagnostic Services ---\n";
    {
        // EN: ReadDataByIdentifier — SW Version
        // TR: ReadDataByIdentifier — Yazılım Sürümü (DID=0xF189 ile okunur)
        auto resp1 = ecu.ProcessUDS({bcm::UDSHandler::SID_READ_DID, bcm::UDSHandler::DID_SW_VERSION, {}});
        std::cout << "    ReadDID(SWVer): positive=" << resp1.positive << " data='";
        for (auto b : resp1.data) std::cout << static_cast<char>(b);
        std::cout << "'\n";

        // EN: ReadDID — Lamp Status
        // TR: ReadDID — Lamba Durumu (GPIO pinlerinin bit maskesi olarak döndürür)
        auto resp2 = ecu.ProcessUDS({bcm::UDSHandler::SID_READ_DID, bcm::UDSHandler::DID_LAMP_STATUS, {}});
        std::cout << "    ReadDID(LampStatus): 0x" << std::hex
                  << static_cast<int>(resp2.data.empty() ? 0 : resp2.data[0])
                  << std::dec << "\n";

        // EN: ReadDTCInformation
        // TR: DTC bilgisi oku — tüm kayıtlı arıza kodları ve durumları listelenir
        auto resp3 = ecu.ProcessUDS({bcm::UDSHandler::SID_READ_DTC_INFO, 0, {}});
        std::cout << "    ReadDTCInfo: " << resp3.data.size() / 4 << " DTCs reported\n";

        // EN: WriteDID — Serial Number
        // TR: WriteDID — Seri Numarası (NvM'ye 4 byte veri yazılır)
        auto resp4 = ecu.ProcessUDS({bcm::UDSHandler::SID_WRITE_DID, bcm::UDSHandler::DID_SERIAL_NUMBER, {0xAB, 0xCD, 0xEF, 0x01}});
        std::cout << "    WriteDID(SerialNum): positive=" << resp4.positive << "\n";

        // EN: ReadDID — Serial Number (verify write)
        // TR: ReadDID — Seri Numarası (yazılan değerin doğrulaması)
        auto resp5 = ecu.ProcessUDS({bcm::UDSHandler::SID_READ_DID, bcm::UDSHandler::DID_SERIAL_NUMBER, {}});
        std::cout << "    ReadDID(SerialNum): ";
        for (auto b : resp5.data) std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(b);
        std::cout << std::dec << std::setfill(' ') << "\n";

        // EN: RoutineControl
        // TR: Rutin Kontrol — test veya kalibrasyon rutini başlatır (SID=0x31)
        auto resp6 = ecu.ProcessUDS({bcm::UDSHandler::SID_ROUTINE_CTRL, 0xFF00, {}});
        std::cout << "    RoutineCtrl(0xFF00): positive=" << resp6.positive << "\n";

        // EN: ClearDTC
        // TR: DTC Temizle — tüm kayıtlı arıza kodlarını sıfırlar (SID=0x14)
        auto resp7 = ecu.ProcessUDS({bcm::UDSHandler::SID_CLEAR_DTC, 0, {}});
        std::cout << "    ClearDTC: positive=" << resp7.positive << "\n";
    }

    // --- Demo 5: Cyclic Scheduling ---
    std::cout << "\n--- Demo 5: Cyclic Task Execution (100 ticks) ---\n";
    {
        ecu.ClearCANTx();
        ecu.Run(100);  // 1000ms
        std::cout << "    BCM status TX messages sent:\n";
        ecu.PrintCANTx();
    }

    // --- Demo 6: Turn Off All + Final Status ---
    std::cout << "\n--- Demo 6: Turn Off All Outputs + Final Status ---\n";
    {
        bcm::CAN_Message light_off{bcm::CAN_ID_LIGHT_CMD, 1, {0x00}};
        bcm::CAN_Message door_unlock{bcm::CAN_ID_DOOR_CMD, 1, {0x00}};
        bcm::CAN_Message wiper_off{bcm::CAN_ID_WIPER_CMD, 1, {0x00}};
        ecu.InjectCAN(light_off);
        ecu.InjectCAN(door_unlock);
        ecu.InjectCAN(wiper_off);
        ecu.Run(1);
        ecu.PrintStatus();
    }

    // --- Demo 7: Architecture Summary ---
    std::cout << "\n--- Demo 7: BCM Architecture Summary ---\n";
    {
        struct Module { std::string_view name; std::string_view description; };
        Module modules[] = {
            {"GPIO",         "Digital I/O: lamps, motors, switches"},
            {"CAN",          "Message RX/TX: light, door, wiper, brake, status"},
            {"LIN",          "Schedule table: rain sensor, ambient light"},
            {"DTC Store",    "Fault logging: active→confirmed, occurrence count "},
            {"NvM",          "Non-volatile: serial number, learn values"},
            {"UDS Handler",  "Diagnostics: ReadDID, WriteDID, ClearDTC, Routine"},
            {"Watchdog",     "Supervision: 100-tick timeout, feed in 10ms task"},
            {"State Machine","STARTUP→NORMAL→SLEEP→WAKEUP lifecycle             "},
            {"Scheduler",    "Cyclic tasks: 10ms/50ms/100ms/1000ms groups"},
        };
        std::cout << "  ┌────────────────┬────────────────────────────────────────────────────┐\n";
        std::cout << "  │ Module         │ Description                                        │\n";
        std::cout << "  ├────────────────┼────────────────────────────────────────────────────┤\n";
        for (const auto& m : modules) {
            std::cout << "  │ " << std::setw(14) << std::left << m.name
                      << " │ " << std::setw(50) << m.description << " │\n";
        }
        std::cout << std::right;
        std::cout << "  └────────────────┴────────────────────────────────────────────────────┘\n";
    }

    std::cout << "\n[All demos completed successfully.]\n";
    return 0;
}
