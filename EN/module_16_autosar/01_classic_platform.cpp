/**
 * @file module_16_autosar/01_classic_platform.cpp
 * @brief AUTOSAR Classic Platform
 *
 * @details
 * =============================================================================
 * [THEORY: AUTOSAR Classic Platform — Overview]
 * =============================================================================
 *
 * EN: AUTOSAR (AUTomotive Open System ARchitecture) Classic Platform is the
 *     de-facto standard software architecture for microcontroller-based ECUs.
 *     It defines a layered architecture separating application logic from
 *     hardware, enabling portability, reusability, and standardized interfaces.
 *
 *     AUTOSAR Classic Architecture:
 *     ┌──────────────────────────────────────────────────────────────┐
 *     │                  APPLICATION LAYER                           │
 *     │   ┌──────────┐  ┌──────────┐  ┌──────────┐  ┌──────────┐     │
 *     │   │  SWC_1   │  │  SWC_2   │  │  SWC_3   │  │  SWC_N   │     │
 *     │   │(Runnable)│  │(Runnable)│  │(Runnable)│  │(Runnable)│     │
 *     │   └────┬─────┘  └────┬─────┘  └────┬─────┘  └────┬─────┘     │
 *     ├────────┴─────────────┴─────────────┴─────────────┴───────────┤
 *     │              RTE (Runtime Environment)                       │
 *     │  Virtual Function Bus — abstracts all SWC communication      │
 *     ├─────────────────────────────────────────────────────────────-┤
 *     │              BSW (Basic Software)                            │
 *     │  ┌────────────────────────────────────────────────────────┐  │
 *     │  │ Services Layer: OS, COM, NvM, Dcm, Dem, BswM, EcuM     │  │
 *     │  ├────────────────────────────────────────────────────────┤  │
 *     │  │ ECU Abstraction: CanIf, SpiIf, DioIf, AdcIf, PwmIf     │  │
 *     │  ├────────────────────────────────────────────────────────┤  │
 *     │  │ MCAL: Can, Spi, Dio, Adc, Pwm, Gpt, Mcu, Port, Icu     │  │
 *     │  └────────────────────────────────────────────────────────┘  │
 *     ├──────────────────────────────────────────────────────────────┤
 *     │              HARDWARE (MCU + Peripherals)                    │
 *     └──────────────────────────────────────────────────────────────┘
 *
 * =============================================================================
 * [THEORY: SWC — Software Component]
 * =============================================================================
 *
 * EN: SWC is the atomic unit of application software. Each SWC:
 *     - Contains one or more Runnables (executable entities)
 *     - Communicates only via Ports (Sender/Receiver or Client/Server)
 *     - Is hardware-independent (runs on any ECU with matching RTE)
 *
 *     Port Types:
 *     ┌──────────────────┬───────────────────────────────────────────┐
 *     │ Port Type        │ Description                               │
 *     ├──────────────────┼───────────────────────────────────────────┤
 *     │ SenderReceiver   │ Async data passing (signal-level)         │
 *     │ ClientServer     │ Sync function call (request/response)     │
 *     │ ModeSwitchPort   │ Mode change notification (EcuM, BswM)     │
 *     │ NvDataPort       │ Non-volatile data access (NvM interface)  │
 *     └──────────────────┴───────────────────────────────────────────┘
 *
 * =============================================================================
 * [THEORY: COM Stack — PDU Routing]
 * =============================================================================
 *
 * EN: The communication stack routes Protocol Data Units (PDUs) from
 *     application to hardware:
 *
 *     COM (Signal ↔ PDU group)
 *      ↓
 *     PduR (PDU Router — routing table)
 *      ↓
 *     CanIf / LinIf / FrIf / SoAd (Interface layer)
 *      ↓
 *     CanDrv / LinDrv / FrDrv / TcpIp (Driver — MCAL)
 *      ↓
 *     CAN Controller / LIN Transceiver / FlexRay CC / Ethernet PHY
 *
 * =============================================================================
 * [THEORY: AUTOSAR OS — OSEK/VDX Based]
 * =============================================================================
 *
 * EN: AUTOSAR OS extends OSEK/VDX OS with:
 *     - Scalability Classes (SC1-SC4): from basic tasks to memory protection
 *     - Tasks: Basic (non-preemptive) and Extended (can wait on events)
 *     - ISR Category 1 (no OS calls) and Category 2 (OS calls allowed)
 *     - Alarms: periodic activation of tasks/callbacks
 *     - Schedule Tables: deterministic activation sequence
 *     - Counters: hardware or software tick sources
 *
 *     ┌──────────┬─────────────────────────────────────────────────────┐
 *     │ SC Class │ Features                                            │
 *     ├──────────┼─────────────────────────────────────────────────────┤
 *     │ SC1      │ Basic tasks, ISR Cat2, Alarms, Counters             │
 *     │ SC2      │ SC1 + Timing Protection                             │
 *     │ SC3      │ SC1 + Memory Protection                             │
 *     │ SC4      │ SC2 + SC3 (full protection)                         │
 *     └──────────┴─────────────────────────────────────────────────────┘
 *
 * @note Compile: g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion 01_classic_platform.cpp -o 01_classic_platform
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <iostream>
#include <cstdint>
#include <array>
#include <vector>
#include <string>
#include <string_view>
#include <functional>
#include <map>
#include <queue>
#include <iomanip>
#include <algorithm>
#include <cassert>
#include <variant>
#include <optional>

// ═════════════════════════════════════════════════════════════════════════════
// PART 1: AUTOSAR-style SWC and RTE Simulation
// ═════════════════════════════════════════════════════════════════════════════

// EN: Port interfaces — Sender/Receiver and Client/Server abstraction

namespace autosar_classic {

// --- Sender/Receiver Port Data Types ---
struct EngineData {
    uint16_t rpm = 0;
    int8_t   coolant_temp = -40;  // °C, offset -40
    uint8_t  throttle_pct = 0;    // 0-100%
};

struct VehicleSpeed {
    uint16_t speed_kph_x10 = 0;  // 0.1 km/h resolution
    bool     valid = false;
};

// -------------------------------------------------------------------------
// RTE — Runtime Environment
// -------------------------------------------------------------------------
// EN: RTE acts as the "Virtual Function Bus" — all SWC communication
//     passes through RTE. No direct SWC-to-SWC calls allowed.

class RTE {
    EngineData    engine_data_{};
    VehicleSpeed  vehicle_speed_{};

    // EN: Client/Server function registry
    using ServerFunction = std::function<uint8_t(const std::vector<uint8_t>&, std::vector<uint8_t>&)>;
    std::map<std::string, ServerFunction> server_registry_;

public:
    // --- Sender/Receiver API ---
    // EN: Write signal to RTE (sender side)
    void Rte_Write_EngineData(const EngineData& data) { engine_data_ = data; }
    [[nodiscard]] EngineData Rte_Read_EngineData() const { return engine_data_; }

    void Rte_Write_VehicleSpeed(const VehicleSpeed& data) { vehicle_speed_ = data; }
    [[nodiscard]] VehicleSpeed Rte_Read_VehicleSpeed() const { return vehicle_speed_; }

    // --- Client/Server API ---
    void RegisterServer(const std::string& service, ServerFunction func) {
        server_registry_[service] = std::move(func);
    }

    uint8_t Rte_Call(const std::string& service,
                     const std::vector<uint8_t>& request,
                     std::vector<uint8_t>& response) {
        auto it = server_registry_.find(service);
        if (it == server_registry_.end()) return 0xFF;  // E_NOT_OK
        return it->second(request, response);
    }
};

// -------------------------------------------------------------------------
// SWC: Engine Management (Sensor Reading + Data Publishing)
// -------------------------------------------------------------------------

class SWC_EngineManagement {
    RTE& rte_;
    uint32_t cycle_count_ = 0;

public:
    explicit SWC_EngineManagement(RTE& rte) : rte_(rte) {}

    // EN: Runnable — called every 10ms by OS task
    void Runnable_ReadSensors() {
        EngineData data{};
        // EN: Simulate sensor reading (in real system: ADC → scaling → engineering value)
        data.rpm = static_cast<uint16_t>(800 + (cycle_count_ % 50) * 100);
        data.coolant_temp = static_cast<int8_t>(20 + static_cast<int8_t>(cycle_count_ % 30));
        data.throttle_pct = static_cast<uint8_t>(10 + cycle_count_ % 80);
        rte_.Rte_Write_EngineData(data);
        ++cycle_count_;
    }
};

// -------------------------------------------------------------------------
// SWC: Vehicle Speed Calculator
// -------------------------------------------------------------------------

class SWC_SpeedCalculator {
    RTE& rte_;

public:
    explicit SWC_SpeedCalculator(RTE& rte) : rte_(rte) {}

    // EN: Runnable — derives vehicle speed from engine data
    void Runnable_CalcSpeed() {
        auto eng = rte_.Rte_Read_EngineData();
        // EN: Simplified: speed ~proportional to RPM * throttle / gear_ratio
        uint16_t speed = static_cast<uint16_t>(
            (static_cast<uint32_t>(eng.rpm) * eng.throttle_pct) / 500u);
        VehicleSpeed vs{};
        vs.speed_kph_x10 = speed;
        vs.valid = (eng.rpm > 0);
        rte_.Rte_Write_VehicleSpeed(vs);
    }
};

// -------------------------------------------------------------------------
// SWC: Dashboard Display
// -------------------------------------------------------------------------

class SWC_Dashboard {
    RTE& rte_;

public:
    explicit SWC_Dashboard(RTE& rte) : rte_(rte) {}

    void Runnable_UpdateDisplay() {
        auto eng = rte_.Rte_Read_EngineData();
        auto spd = rte_.Rte_Read_VehicleSpeed();
        std::cout << "  [Dashboard] RPM=" << eng.rpm
                  << " Temp=" << static_cast<int>(eng.coolant_temp) << "C"
                  << " Throttle=" << static_cast<int>(eng.throttle_pct) << "%"
                  << " Speed=" << (spd.speed_kph_x10 / 10) << "."
                  << (spd.speed_kph_x10 % 10) << " km/h"
                  << (spd.valid ? " [OK]" : " [INVALID]") << "\n";
    }
};

}  // namespace autosar_classic

// ═════════════════════════════════════════════════════════════════════════════
// PART 2: AUTOSAR OS Simulation (Task, Alarm, Schedule Table)
// ═════════════════════════════════════════════════════════════════════════════

namespace autosar_os {

// EN: Task priority (higher value = higher priority in AUTOSAR OS)

enum class TaskState : uint8_t { SUSPENDED, READY, RUNNING, WAITING };

struct TaskConfig {
    std::string         name;
    uint8_t             priority = 0;
    bool                preemptive = true;
    std::function<void()> entry;  // task body
    uint32_t            stack_size = 1024;
};

struct Task {
    TaskConfig config;
    TaskState  state = TaskState::SUSPENDED;
    uint32_t   activation_count = 0;
};

// EN: Alarm — periodic or one-shot task activation
struct Alarm {
    std::string name;
    std::string task_name;
    uint32_t    cycle_ticks = 0;   // 0 = one-shot
    uint32_t    offset_ticks = 0;
    uint32_t    remaining = 0;
    bool        active = false;
};

// EN: Minimal AUTOSAR OS scheduler simulation
class OS {
    std::vector<Task>  tasks_;
    std::vector<Alarm> alarms_;
    uint32_t           tick_counter_ = 0;

public:
    void DeclareTask(const TaskConfig& cfg) {
        tasks_.push_back({cfg, TaskState::SUSPENDED, 0});
    }

    void SetAlarm(const std::string& alarm_name, const std::string& task_name,
                  uint32_t offset, uint32_t cycle) {
        alarms_.push_back({alarm_name, task_name, cycle, offset, offset, true});
    }

    // EN: Advance system tick and check alarms
    void Tick() {
        ++tick_counter_;
        for (auto& alarm : alarms_) {
            if (!alarm.active) continue;
            if (alarm.remaining > 0) {
                --alarm.remaining;
            }
            if (alarm.remaining == 0) {
                ActivateTask(alarm.task_name);
                if (alarm.cycle_ticks > 0) {
                    alarm.remaining = alarm.cycle_ticks;
                } else {
                    alarm.active = false;  // one-shot
                }
            }
        }
        Schedule();
    }

    void ActivateTask(const std::string& name) {
        for (auto& t : tasks_) {
            if (t.config.name == name && t.state == TaskState::SUSPENDED) {
                t.state = TaskState::READY;
                ++t.activation_count;
            }
        }
    }

    // EN: Priority-based scheduling (run all READY tasks, highest priority first)
    void Schedule() {
        for (;;) {
            Task* best = nullptr;
            for (auto& t : tasks_) {
                if (t.state == TaskState::READY) {
                    if (!best || t.config.priority > best->config.priority)
                        best = &t;
                }
            }
            if (!best) break;
            best->state = TaskState::RUNNING;
            if (best->config.entry) best->config.entry();
            best->state = TaskState::SUSPENDED;  // TerminateTask implicit
        }
    }

    void PrintStatus() const {
        std::cout << "  OS Tick=" << tick_counter_ << " | Tasks: ";
        for (const auto& t : tasks_) {
            std::cout << t.config.name << "(act=" << t.activation_count << ") ";
        }
        std::cout << "\n";
    }
};

}  // namespace autosar_os

// ═════════════════════════════════════════════════════════════════════════════
// PART 3: COM Stack — PDU Router Simulation
// ═════════════════════════════════════════════════════════════════════════════

namespace autosar_com {

// EN: PDU (Protocol Data Unit) — the fundamental data exchange unit

struct PDU {
    uint32_t               id = 0;
    std::vector<uint8_t>   data;
    std::string            source;
    std::string            destination;
};

// EN: PDU Router — routes PDUs between COM, CanIf, LinIf, SoAd

struct RoutingEntry {
    uint32_t    src_pdu_id;
    std::string src_module;
    uint32_t    dst_pdu_id;
    std::string dst_module;
};

class PduRouter {
    std::vector<RoutingEntry> routing_table_;
    uint32_t routed_count_ = 0;

public:
    void AddRoute(uint32_t src_id, const std::string& src_mod,
                  uint32_t dst_id, const std::string& dst_mod) {
        routing_table_.push_back({src_id, src_mod, dst_id, dst_mod});
    }

    std::optional<PDU> Route(const PDU& pdu) {
        for (const auto& entry : routing_table_) {
            if (pdu.id == entry.src_pdu_id && pdu.source == entry.src_module) {
                PDU routed = pdu;
                routed.id = entry.dst_pdu_id;
                routed.source = entry.src_module;
                routed.destination = entry.dst_module;
                ++routed_count_;
                return routed;
            }
        }
        return std::nullopt;  // no route found
    }

    void PrintTable() const {
        std::cout << "  PDU Routing Table (" << routing_table_.size() << " entries):\n";
        std::cout << "  ┌──────────┬────────────┬──────────┬────────────┐\n";
        std::cout << "  │ Src PDU  │ Src Module │ Dst PDU  │ Dst Module │\n";
        std::cout << "  ├──────────┼────────────┼──────────┼────────────┤\n";
        for (const auto& e : routing_table_) {
            std::cout << "  │ 0x" << std::hex << std::setw(4) << std::setfill('0') << e.src_pdu_id
                      << "   │ " << std::setw(10) << std::setfill(' ') << std::left << e.src_module
                      << " │ 0x" << std::setw(4) << std::setfill('0') << std::right << e.dst_pdu_id
                      << "   │ " << std::setw(10) << std::setfill(' ') << std::left << e.dst_module
                      << " │\n" << std::dec;
        }
        std::cout << std::right << std::setfill(' ');
        std::cout << "  └──────────┴────────────┴──────────┴────────────┘\n";
        std::cout << "  Total routed: " << routed_count_ << "\n";
    }
};

// -------------------------------------------------------------------------
// CanIf — CAN Interface Layer (between PduR and CanDrv)
// -------------------------------------------------------------------------

class CanIf {
    uint32_t tx_count_ = 0;
    uint32_t rx_count_ = 0;

public:
    void Transmit(const PDU& pdu) {
        ++tx_count_;
        std::cout << "  [CanIf] TX → CAN ID=0x" << std::hex << pdu.id
                  << " len=" << std::dec << pdu.data.size() << " bytes\n";
    }

    PDU Receive(uint32_t can_id, const std::vector<uint8_t>& data) {
        ++rx_count_;
        PDU pdu{};
        pdu.id = can_id;
        pdu.data = data;
        pdu.source = "CanIf";
        return pdu;
    }

    void PrintStats() const {
        std::cout << "  [CanIf] TX=" << tx_count_ << " RX=" << rx_count_ << "\n";
    }
};

}  // namespace autosar_com

// ═════════════════════════════════════════════════════════════════════════════
// PART 4: Memory Stack — NvM Simulation
// ═════════════════════════════════════════════════════════════════════════════

namespace autosar_nvm {

// EN: NvM (Non-volatile Memory Manager) — manages read/write of NV blocks
//     Architecture: NvM → MemIf → Fee(Flash) or Ea(EEPROM) → Fls/Eep(MCAL)

enum class NvMRequestResult : uint8_t {
    NVM_REQ_OK = 0,
    NVM_REQ_NOT_OK,
    NVM_REQ_PENDING,
    NVM_REQ_INTEGRITY_FAILED,
    NVM_REQ_BLOCK_SKIPPED
};

struct NvMBlock {
    uint16_t              block_id;
    std::string           name;
    std::vector<uint8_t>  ram_mirror;
    std::vector<uint8_t>  nv_data;    // simulated NV storage
    bool                  dirty = false;
    NvMRequestResult      status = NvMRequestResult::NVM_REQ_OK;
    uint16_t              crc = 0;
};

class NvM {
    std::vector<NvMBlock> blocks_;

    static uint16_t CalculateCRC(const std::vector<uint8_t>& data) {
        uint16_t crc = 0xFFFF;
        for (auto byte : data) {
            crc ^= static_cast<uint16_t>(byte);
            for (int i = 0; i < 8; ++i)
                crc = (crc & 1) ? ((crc >> 1) ^ 0xA001u) : (crc >> 1);
        }
        return crc;
    }

public:
    void ConfigureBlock(uint16_t id, const std::string& name, size_t size) {
        NvMBlock block{};
        block.block_id = id;
        block.name = name;
        block.ram_mirror.resize(size, 0xFF);
        block.nv_data.resize(size, 0xFF);
        blocks_.push_back(std::move(block));
    }

    NvMRequestResult ReadBlock(uint16_t id) {
        auto* block = FindBlock(id);
        if (!block) return NvMRequestResult::NVM_REQ_NOT_OK;
        // EN: Copy NV → RAM mirror, verify CRC
        block->ram_mirror = block->nv_data;
        uint16_t calc_crc = CalculateCRC(block->nv_data);
        if (block->crc != 0 && calc_crc != block->crc) {
            block->status = NvMRequestResult::NVM_REQ_INTEGRITY_FAILED;
            return NvMRequestResult::NVM_REQ_INTEGRITY_FAILED;
        }
        block->status = NvMRequestResult::NVM_REQ_OK;
        return NvMRequestResult::NVM_REQ_OK;
    }

    NvMRequestResult WriteBlock(uint16_t id, const std::vector<uint8_t>& data) {
        auto* block = FindBlock(id);
        if (!block) return NvMRequestResult::NVM_REQ_NOT_OK;
        if (data.size() > block->ram_mirror.size())
            return NvMRequestResult::NVM_REQ_NOT_OK;
        std::copy(data.begin(), data.end(), block->ram_mirror.begin());
        block->nv_data = block->ram_mirror;
        block->crc = CalculateCRC(block->nv_data);
        block->dirty = false;
        block->status = NvMRequestResult::NVM_REQ_OK;
        return NvMRequestResult::NVM_REQ_OK;
    }

    void PrintBlocks() const {
        std::cout << "  NvM Block Status:\n";
        for (const auto& b : blocks_) {
            std::cout << "    Block " << b.block_id << " [" << b.name
                      << "] size=" << b.nv_data.size()
                      << " CRC=0x" << std::hex << b.crc << std::dec
                      << " status=" << static_cast<int>(b.status) << "\n";
        }
    }

private:
    NvMBlock* FindBlock(uint16_t id) {
        for (auto& b : blocks_)
            if (b.block_id == id) return &b;
        return nullptr;
    }
};

}  // namespace autosar_nvm

// ═════════════════════════════════════════════════════════════════════════════
// PART 5: Diagnostic — DCM / DEM Simulation
// ═════════════════════════════════════════════════════════════════════════════

namespace autosar_diag {

// EN: DEM — Diagnostic Event Manager: stores DTCs (Diagnostic Trouble Codes)

enum class DTCStatus : uint8_t {
    PASSED     = 0x00,
    FAILED     = 0x01,
    PENDING    = 0x04,
    CONFIRMED  = 0x08,
    TEST_NOT_COMPLETED = 0x50
};

struct DTC {
    uint32_t   dtc_number;        // e.g. 0xC07300
    std::string description;
    DTCStatus  status = DTCStatus::PASSED;
    uint16_t   occurrence_counter = 0;
    uint16_t   aging_counter = 0;
};

class DEM {
    std::vector<DTC> dtc_list_;

public:
    void RegisterDTC(uint32_t number, const std::string& desc) {
        dtc_list_.push_back({number, desc, DTCStatus::PASSED, 0, 0});
    }

    void ReportEvent(uint32_t dtc_number, bool test_failed) {
        for (auto& dtc : dtc_list_) {
            if (dtc.dtc_number == dtc_number) {
                if (test_failed) {
                    dtc.status = DTCStatus::FAILED;
                    ++dtc.occurrence_counter;
                    dtc.aging_counter = 0;
                    if (dtc.occurrence_counter >= 3)
                        dtc.status = DTCStatus::CONFIRMED;
                } else {
                    if (dtc.status == DTCStatus::FAILED)
                        dtc.status = DTCStatus::PENDING;
                    ++dtc.aging_counter;
                    if (dtc.aging_counter >= 40)
                        dtc.status = DTCStatus::PASSED;
                }
                return;
            }
        }
    }

    void ClearAll() {
        for (auto& dtc : dtc_list_) {
            dtc.status = DTCStatus::PASSED;
            dtc.occurrence_counter = 0;
            dtc.aging_counter = 0;
        }
    }

    void PrintAll() const {
        std::cout << "  DEM — DTC Status:\n";
        std::cout << "  ┌──────────┬──────────────────────────────┬──────────┬───────┐\n";
        std::cout << "  │ DTC      │ Description                  │ Status   │ Count │\n";
        std::cout << "  ├──────────┼──────────────────────────────┼──────────┼───────┤\n";
        for (const auto& d : dtc_list_) {
            std::cout << "  │ 0x" << std::hex << std::setw(6) << std::setfill('0')
                      << d.dtc_number << " │ " << std::dec << std::setfill(' ')
                      << std::setw(28) << std::left << d.description << " │ "
                      << std::setw(8) << StatusStr(d.status) << " │ "
                      << std::right << std::setw(5) << d.occurrence_counter << " │\n";
        }
        std::cout << std::right << std::setfill(' ');
        std::cout << "  └──────────┴──────────────────────────────┴──────────┴───────┘\n";
    }

private:
    static std::string_view StatusStr(DTCStatus s) {
        switch (s) {
            case DTCStatus::PASSED:     return "PASSED";
            case DTCStatus::FAILED:     return "FAILED";
            case DTCStatus::PENDING:    return "PENDING";
            case DTCStatus::CONFIRMED:  return "CONFIRM";
            case DTCStatus::TEST_NOT_COMPLETED: return "NOT_CMP";
        }
        return "UNKNOWN";
    }
};

}  // namespace autosar_diag

// ═════════════════════════════════════════════════════════════════════════════
// PART 6: EcuM / BswM — ECU State Management
// ═════════════════════════════════════════════════════════════════════════════

namespace autosar_ecum {

// EN: EcuM phases: STARTUP → UP (RUN/POST_RUN) → SHUTDOWN → SLEEP/OFF

enum class EcuMState : uint8_t {
    STARTUP, RUN, POST_RUN, PREP_SHUTDOWN, GO_SLEEP, SLEEP, GO_OFF, OFF
};

class EcuM {
    EcuMState state_ = EcuMState::STARTUP;
    uint32_t  uptime_ticks_ = 0;

public:
    void Init() {
        state_ = EcuMState::STARTUP;
        std::cout << "  [EcuM] Init → STARTUP\n";
        // EN: Init sequence: Mcu_Init, Port_Init, Dio_Init, SchM_Init, BswM_Init, ...
        state_ = EcuMState::RUN;
        std::cout << "  [EcuM] Startup complete → RUN\n";
    }

    void MainFunction() {
        ++uptime_ticks_;
        // EN: Evaluate wakeup sources, manage run requests
    }

    void RequestShutdown() {
        std::cout << "  [EcuM] Shutdown requested → PREP_SHUTDOWN\n";
        state_ = EcuMState::PREP_SHUTDOWN;
        // EN: NvM_WriteAll, ComM_DeInit, ...
        state_ = EcuMState::GO_OFF;
        std::cout << "  [EcuM] GO_OFF → Uptime was " << uptime_ticks_ << " ticks\n";
        state_ = EcuMState::OFF;
    }

    [[nodiscard]] EcuMState GetState() const { return state_; }

    static std::string_view StateStr(EcuMState s) {
        switch (s) {
            case EcuMState::STARTUP:       return "STARTUP";
            case EcuMState::RUN:           return "RUN";
            case EcuMState::POST_RUN:      return "POST_RUN";
            case EcuMState::PREP_SHUTDOWN: return "PREP_SHUTDOWN";
            case EcuMState::GO_SLEEP:      return "GO_SLEEP";
            case EcuMState::SLEEP:         return "SLEEP";
            case EcuMState::GO_OFF:        return "GO_OFF";
            case EcuMState::OFF:           return "OFF";
        }
        return "UNKNOWN";
    }
};

}  // namespace autosar_ecum

// ═════════════════════════════════════════════════════════════════════════════
// MAIN — Demos
// ═════════════════════════════════════════════════════════════════════════════

int main() {
    std::cout << "╔══════════════════════════════════════════════════════════════╗\n"
              << "║ Module 16: AUTOSAR Classic Platform                          ║\n"
              << "╚══════════════════════════════════════════════════════════════╝\n\n";

    // --- Demo 1: SWC + RTE Communication ---
    std::cout << "--- Demo 1: SWC + RTE Communication (Sender/Receiver) ---\n";
    {
        autosar_classic::RTE rte;
        autosar_classic::SWC_EngineManagement engine_swc(rte);
        autosar_classic::SWC_SpeedCalculator  speed_swc(rte);
        autosar_classic::SWC_Dashboard        dash_swc(rte);

        for (int cycle = 0; cycle < 5; ++cycle) {
            engine_swc.Runnable_ReadSensors();
            speed_swc.Runnable_CalcSpeed();
            dash_swc.Runnable_UpdateDisplay();
        }
    }

    // --- Demo 2: Client/Server via RTE ---
    std::cout << "\n--- Demo 2: Client/Server via RTE ---\n";
    {
        autosar_classic::RTE rte;
        // EN: Register a diagnostic server (SWC_Diag provides ClearDTC service)
        rte.RegisterServer("ClearDTC",
            [](const std::vector<uint8_t>& req, std::vector<uint8_t>& resp) -> uint8_t {
                if (req.size() >= 3) {
                    uint32_t dtc = (static_cast<uint32_t>(req[0]) << 16) |
                                   (static_cast<uint32_t>(req[1]) << 8)  |
                                   static_cast<uint32_t>(req[2]);
                    std::cout << "  [Server] ClearDTC(0x" << std::hex << dtc << std::dec << ") → OK\n";
                    resp = {0x00};  // positive response
                    return 0x00;   // E_OK
                }
                resp = {0x13};  // incorrectMessageLength
                return 0x01;
            });

        // Client call
        std::vector<uint8_t> req = {0xC0, 0x73, 0x00};  // DTC 0xC07300
        std::vector<uint8_t> resp;
        auto result = rte.Rte_Call("ClearDTC", req, resp);
        std::cout << "  Client result: 0x" << std::hex << static_cast<int>(result)
                  << std::dec << "\n";
    }

    // --- Demo 3: AUTOSAR OS — Tasks and Alarms ---
    std::cout << "\n--- Demo 3: AUTOSAR OS — Tasks and Alarms ---\n";
    {
        autosar_os::OS os;

        int task_10ms_count = 0, task_100ms_count = 0;

        os.DeclareTask({"Task_10ms", 10, true, [&]() {
            ++task_10ms_count;
        }, 512});
        os.DeclareTask({"Task_100ms", 5, true, [&]() {
            ++task_100ms_count;
        }, 512});

        os.SetAlarm("Alarm_10ms",  "Task_10ms",  1, 1);   // every tick
        os.SetAlarm("Alarm_100ms", "Task_100ms", 10, 10);  // every 10 ticks

        for (int i = 0; i < 100; ++i) os.Tick();

        os.PrintStatus();
        std::cout << "  Task_10ms executed: " << task_10ms_count << " times\n";
        std::cout << "  Task_100ms executed: " << task_100ms_count << " times\n";
    }

    // --- Demo 4: COM Stack — PDU Routing ---
    std::cout << "\n--- Demo 4: COM Stack — PDU Routing ---\n";
    {
        autosar_com::PduRouter pdur;
        autosar_com::CanIf    can_if;

        // EN: Configure routes: COM → CanIf, LinIf → COM
        pdur.AddRoute(0x0100, "COM",   0x0200, "CanIf");
        pdur.AddRoute(0x0150, "COM",   0x0250, "CanIf");
        pdur.AddRoute(0x0300, "CanIf", 0x0400, "COM");
        pdur.PrintTable();

        // EN: Route a TX PDU from COM to CanIf
        autosar_com::PDU tx_pdu{0x0100, {0x11, 0x22, 0x33, 0x44}, "COM", ""};
        auto routed = pdur.Route(tx_pdu);
        if (routed) can_if.Transmit(*routed);

        // EN: Receive from CAN bus and route to COM
        auto rx_pdu = can_if.Receive(0x0300, {0xAA, 0xBB});
        auto routed_rx = pdur.Route(rx_pdu);
        if (routed_rx)
            std::cout << "  [PduR] Routed RX PDU 0x" << std::hex << routed_rx->id
                      << " → " << routed_rx->destination << std::dec << "\n";
        can_if.PrintStats();
    }

    // --- Demo 5: NvM — Non-Volatile Memory Manager ---
    std::cout << "\n--- Demo 5: NvM — Non-Volatile Memory Manager ---\n";
    {
        autosar_nvm::NvM nvm;
        nvm.ConfigureBlock(1, "CalibData", 16);
        nvm.ConfigureBlock(2, "LearnValues", 8);
        nvm.ConfigureBlock(3, "DTCSnapshot", 32);

        // EN: Write calibration data
        nvm.WriteBlock(1, {0x10, 0x20, 0x30, 0x40, 0x50, 0x60});
        nvm.WriteBlock(2, {0xAB, 0xCD, 0xEF, 0x01});

        // EN: Read back and verify
        auto result = nvm.ReadBlock(1);
        std::cout << "  NvM ReadBlock(1) result: "
                  << (result == autosar_nvm::NvMRequestResult::NVM_REQ_OK ? "OK" : "FAIL")
                  << "\n";
        nvm.PrintBlocks();
    }

    // --- Demo 6: DEM — Diagnostic Event Manager ---
    std::cout << "\n--- Demo 6: DEM — Diagnostic Event Manager (DTC lifecycle) ---\n";
    {
        autosar_diag::DEM dem;
        dem.RegisterDTC(0xC07300, "Lost CAN communication");
        dem.RegisterDTC(0xC10000, "Sensor plausibility");
        dem.RegisterDTC(0xC41400, "Battery voltage low");

        // EN: Simulate events: C07300 fails 3x → confirmed, then heals
        dem.ReportEvent(0xC07300, true);
        dem.ReportEvent(0xC07300, true);
        dem.ReportEvent(0xC07300, true);   // → CONFIRMED
        dem.ReportEvent(0xC10000, true);
        dem.ReportEvent(0xC10000, false);  // → PENDING
        dem.PrintAll();

        std::cout << "  Clearing all DTCs...\n";
        dem.ClearAll();
        dem.PrintAll();
    }

    // --- Demo 7: EcuM — State Management ---
    std::cout << "\n--- Demo 7: EcuM — ECU State Management ---\n";
    {
        autosar_ecum::EcuM ecum;
        ecum.Init();
        for (int i = 0; i < 50; ++i) ecum.MainFunction();
        std::cout << "  Current state: "
                  << autosar_ecum::EcuM::StateStr(ecum.GetState()) << "\n";
        ecum.RequestShutdown();
        std::cout << "  Final state: "
                  << autosar_ecum::EcuM::StateStr(ecum.GetState()) << "\n";
    }

    // --- Demo 8: AUTOSAR vs Bare-metal vs Linux ---
    std::cout << "\n--- Demo 8: AUTOSAR vs Bare-metal vs Linux Comparison ---\n";
    {
        std::cout << "  ┌──────────────────────┬───────────────┬──────────────────┬────────────────┐\n";
        std::cout << "  │ Feature              │ Bare-Metal    │ AUTOSAR Classic  │ Linux/POSIX    │\n";
        std::cout << "  ├──────────────────────┼───────────────┼──────────────────┼────────────────┤\n";
        std::cout << "  │ OS                   │ None/custom   │ AUTOSAR OS(OSEK) │ Linux kernel   │\n";
        std::cout << "  │ Language             │ C / ASM       │ C (SWS), C++ SWC │ C, C++, Rust   │\n";
        std::cout << "  │ Communication        │ Direct reg    │ COM/PduR/CanIf   │ Socket/IPC     │\n";
        std::cout << "  │ Diagnostics          │ Custom        │ DCM/DEM/NRC      │ Custom/SOME-IP │\n";
        std::cout << "  │ NV Storage           │ Direct EEPROM │ NvM/Fee/Ea       │ Filesystem     │\n";
        std::cout << "  │ Configuration        │ #define       │ ARXML/tools      │ Config files   │\n";
        std::cout << "  │ Code Reuse           │ Low           │ High (SWC port.) │ Medium         │\n";
        std::cout << "  │ Boot Time            │ ~1ms          │ ~50ms-300ms      │ ~500ms-5s      │\n";
        std::cout << "  │ Safety (ISO 26262)   │ Possible      │ ASIL-D ready     │ ASIL-B (QNX-D) │\n";
        std::cout << "  │ Typical MCU          │ ATmega/PIC    │ AURIX/RH850/S32K │ i.MX8/SA8155P  │\n";
        std::cout << "  └──────────────────────┴───────────────┴──────────────────┴────────────────┘\n";
    }

    std::cout << "\n[All demos completed successfully.]\n";
    return 0;
}
