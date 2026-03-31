/**
 * @file module_14_automotive_networks/03_flexray.cpp
 * @brief FlexRay — High-Reliability Automotive Bus
 *
 * @details
 * =============================================================================
 * =============================================================================
 *
 * EN: FlexRay (ISO 17458) is a high-speed, deterministic, fault-tolerant
 *     bus designed for safety-critical X-by-wire systems (steer-by-wire,
 *     brake-by-wire). Developed by FlexRay Consortium (BMW, Bosch, Daimler,
 *     Freescale/NXP).
 *
 *     Key characteristics:
 *     - Dual-channel (A + B): redundancy for fault tolerance
 *     - Max speed: 10 Mbps per channel (20 Mbps aggregate)
 *     - Max payload: 254 bytes
 *     - Deterministic: TDMA (static) + FTDMA (dynamic) mixed scheduling
 *     - Time-synchronization: Global clock, macrotick/microtick precision
 *     - Startup protocol: Coldstart nodes synchronize the cluster
 *
 *     Communication Cycle:
 *     ┌──────────────────────────────────────────────────────────────────┐
 *     │                    One Communication Cycle                       │
 *     ├──────────────────┬──────────────────┬──────────┬─────────────────┤
 *     │  Static Segment  │ Dynamic Segment  │ Symbol   │ NIT (Network    │
 *     │  (TDMA)          │ (FTDMA)          │ Window   │  Idle Time)     │
 *     │                  │                  │          │                 │
 *     │ Slot 1 | 2 | 3.. │ MiniSlot 1|2|3.. │ (opt)    │ Clock sync      │
 *     │ Fixed bandwidth  │ Event-driven     │ Wakeup/  │ correction      │
 *     │ Guaranteed       │ Best-effort      │ CAS      │                 │
 *     └──────────────────┴──────────────────┴──────────┴─────────────────┘
 *
 *     FlexRay Frame Format:
 *     ┌─────────┬──────────────────────────────────────────┬──────────┐
 *     │ Header  │ Payload                                  │ Trailer  │
 *     │ (5 B)   │ (0-254 bytes)                            │ (3 B)    │
 *     ├─────────┼──────────────────────────────────────────┼──────────┤
 *     │ Flags   │ Data bytes                               │ CRC-24   │
 *     │ FrameID │                                          │          │
 *     │ PayLen  │                                          │          │
 *     │ HdrCRC  │                                          │          │
 *     │ CycCnt  │                                          │          │
 *     └─────────┴──────────────────────────────────────────┴──────────┘
 *
 *     Header Flags:
 *     - Reserved, Payload Preamble Indicator, Null Frame Indicator,
 *       Sync Frame Indicator, Startup Frame Indicator
 *
 * =============================================================================
 * =============================================================================
 *
 * EN:
 *     ┌────────────────────┬──────────────┬──────────────┬───────────────┐
 *     │ Feature            │ CAN FD       │ FlexRay      │ Eth 100BASE-T1│
 *     ├────────────────────┼──────────────┼──────────────┼───────────────┤
 *     │ Speed              │ 8 Mbps       │ 2x10 Mbps    │ 100 Mbps      │
 *     │ Payload            │ 64 bytes     │ 254 bytes    │ 1500 bytes    │
 *     │ Determinism        │ Priority     │ TDMA ✓       │ TSN needed    │
 *     │ Redundancy         │ No           │ Dual-channel │ Switch-based  │
 *     │ Topology           │ Bus          │ Bus/Star     │ Star/Switch   │
 *     │ Sync precision     │ N/A          │ ~1 μs        │ gPTP ~100ns   │
 *     │ Cost               │ Low          │ Medium-High  │ Medium        │
 *     │ Safety level       │ ASIL-D       │ ASIL-D       │ ASIL-D (TSN)  │
 *     │ Future outlook     │ Maintained   │ Legacy→Eth   │ Growing       │
 *     └────────────────────┴──────────────┴──────────────┴───────────────┘
 *
 */

#include <iostream>
#include <cstdint>
#include <array>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>
#include <cassert>
#include <map>

// ═════════════════════════════════════════════════════════════════════════════
// PART 1: FlexRay Frame Structure
// ═════════════════════════════════════════════════════════════════════════════

// EN: FlexRay frame header flags
struct FlexRayFlags {
    bool startup_frame  = false;
    bool sync_frame     = false;
    bool null_frame     = false;
    bool payload_preamble = false;
};

// EN: Complete FlexRay frame
struct FlexRayFrame {
    FlexRayFlags flags;
    uint16_t frame_id      = 0;
    uint8_t  payload_length = 0;
    uint16_t header_crc    = 0;
    uint8_t  cycle_count   = 0;
    std::vector<uint8_t> payload;
    uint32_t frame_crc     = 0;

    // EN: Actual data length in bytes
    size_t data_bytes() const { return static_cast<size_t>(payload_length) * 2; }

    // EN: Channel assignment
    enum class Channel { A, B, AB };
    Channel channel = Channel::AB;

    void print(const std::string& label) const {
        std::cout << "  [FlexRay] " << label << "\n";
        std::cout << "    FrameID: " << frame_id
                  << " | Cycle: " << static_cast<int>(cycle_count)
                  << " | PayLen: " << static_cast<int>(payload_length)
                  << " (" << data_bytes() << " bytes)"
                  << " | Ch: " << (channel == Channel::A ? "A" :
                                   channel == Channel::B ? "B" : "A+B") << "\n";
        std::cout << "    Flags: "
                  << (flags.startup_frame ? "[STARTUP] " : "")
                  << (flags.sync_frame ? "[SYNC] " : "")
                  << (flags.null_frame ? "[NULL] " : "")
                  << (flags.payload_preamble ? "[PREAMBLE]" : "")
                  << "\n";
        if (!payload.empty() && !flags.null_frame) {
            std::cout << "    Data:";
            size_t show = std::min(payload.size(), size_t{32});
            for (size_t i = 0; i < show; ++i) {
                if (i > 0 && i % 16 == 0) std::cout << "\n          ";
                std::cout << " " << std::hex << std::setw(2) << std::setfill('0')
                          << static_cast<int>(payload[i]);
            }
            if (payload.size() > 32)
                std::cout << " ... (" << std::dec << payload.size() << " total)";
            std::cout << std::dec << "\n";
        }
    }
};

// ═════════════════════════════════════════════════════════════════════════════
// PART 2: FlexRay Timing — Macrotick, Microtick, Cycle
// ═════════════════════════════════════════════════════════════════════════════

// EN: FlexRay time hierarchy:
//     Microtick: smallest time unit (based on oscillator, ~12.5-50 ns)
//     Macrotick: cluster-wide tick (N microticks, typically 1-6 μs)
//     Static Slot: fixed duration in macroticks
//     Communication Cycle: all segments combined (typ. 1-5 ms)

struct FlexRayClusterConfig {
    // EN: Timing parameters
    uint16_t macrotick_ns       = 1000;  // 1 macrotick = 1000 ns = 1 μs
    uint16_t static_slot_mt     = 25;    // static slot = 25 macroticks
    uint16_t num_static_slots   = 20;    // 20 static slots in static segment
    uint16_t dynamic_segment_mt = 100;   // dynamic segment = 100 macroticks
    uint16_t minislot_mt        = 5;     // one minislot = 5 macroticks
    uint16_t symbol_window_mt   = 10;    // symbol window = 10 macroticks
    uint16_t nit_mt             = 30;    // Network Idle Time = 30 macroticks
    uint8_t  max_cycle          = 64;    // cycle counter wraps at 64

    // EN: Calculated cycle length
    uint32_t cycle_length_mt() const {
        return static_cast<uint32_t>(static_slot_mt) * num_static_slots
             + dynamic_segment_mt + symbol_window_mt + nit_mt;
    }

    uint32_t cycle_length_us() const {
        return cycle_length_mt() * macrotick_ns / 1000;
    }

    void print() const {
        std::cout << "  FlexRay Cluster Configuration:\n";
        std::cout << "    Macrotick:        " << macrotick_ns << " ns\n";
        std::cout << "    Static slot:      " << static_slot_mt << " MT ("
                  << static_slot_mt * macrotick_ns / 1000 << " μs)\n";
        std::cout << "    Num static slots: " << num_static_slots << "\n";
        std::cout << "    Dynamic segment:  " << dynamic_segment_mt << " MT\n";
        std::cout << "    Minislot:         " << minislot_mt << " MT\n";
        std::cout << "    Symbol window:    " << symbol_window_mt << " MT\n";
        std::cout << "    NIT:              " << nit_mt << " MT\n";
        std::cout << "    Cycle length:     " << cycle_length_mt() << " MT ("
                  << cycle_length_us() << " μs)\n";
        std::cout << "    Cycle frequency:  " << (1000000.0 / static_cast<double>(cycle_length_us()))
                  << " Hz\n";
    }
};

// ═════════════════════════════════════════════════════════════════════════════
// PART 3: FlexRay Static Segment — TDMA Scheduling
// ═════════════════════════════════════════════════════════════════════════════

// EN: In the static segment, slots are assigned to nodes at design time.
//     Each node knows which slot(s) it owns. Slot = guaranteed bandwidth.
//     TDMA = Time Division Multiple Access — no contention, no collision.

struct StaticSlotAssignment {
    uint16_t slot_id;
    std::string node_name;
    std::string signal_name;
    uint8_t payload_words;
    FlexRayFrame::Channel channel;
};

class FlexRayStaticSchedule {
    std::vector<StaticSlotAssignment> assignments_;

public:
    void assign(uint16_t slot, const std::string& node, const std::string& signal,
                uint8_t payload_words, FlexRayFrame::Channel ch) {
        assignments_.push_back({slot, node, signal, payload_words, ch});
    }

    const std::vector<StaticSlotAssignment>& slots() const { return assignments_; }

    void print() const {
        std::cout << "  Static Segment Schedule:\n";
        std::cout << "  ┌──────┬─────────────────┬──────────────────────┬────────┬─────┐\n";
        std::cout << "  │ Slot │ Node            │ Signal               │ Bytes  │ Ch  │\n";
        std::cout << "  ├──────┼─────────────────┼──────────────────────┼────────┼─────┤\n";
        for (const auto& a : assignments_) {
            std::string ch_str = (a.channel == FlexRayFrame::Channel::A ? "A" :
                                  a.channel == FlexRayFrame::Channel::B ? "B" : "A+B");
            std::cout << "  │ " << std::setw(4) << a.slot_id
                      << " │ " << std::left << std::setw(15) << a.node_name
                      << " │ " << std::setw(20) << a.signal_name
                      << " │ " << std::right << std::setw(6) << static_cast<int>(a.payload_words) * 2
                      << " │ " << std::setw(3) << ch_str << " │\n";
        }
        std::cout << "  └──────┴─────────────────┴──────────────────────┴────────┴─────┘\n";
    }
};

// ═════════════════════════════════════════════════════════════════════════════
// PART 4: FlexRay Startup Protocol
// ═════════════════════════════════════════════════════════════════════════════

// EN: FlexRay startup requires special nodes:
//     - Coldstart nodes (2-3 minimum): initiate cluster startup
//     - Integration nodes: join after coldstart is complete
//
//     Startup State Machine:
//     DEFAULT_CONFIG → READY → STARTUP → NORMAL_ACTIVE
//          (leading coldstart)        (integrated into cluster)
//
//     A leading coldstart node sends CAS (Collision Avoidance Symbol) on
//     the bus, then sends sync+startup frames. Other coldstart nodes
//     integrate. Once all agree, the cluster enters NORMAL_ACTIVE.
//
//     DEFAULT_CONFIG → READY → STARTUP → NORMAL_ACTIVE
//

enum class FlexRayNodeState {
    DefaultConfig,
    Ready,
    WakeupListen,
    WakeupSend,
    StartupPrepare,
    ColdstartListen,
    ColdstartCollisionResolution,
    ColdstartConsistencyCheck,
    ColdstartGap,
    IntegrationColdstartCheck,
    IntegrationListen,
    IntegrationConsistencyCheck,
    NormalActive,
    NormalPassive,
    Halt
};

std::string to_string(FlexRayNodeState s) {
    switch (s) {
        case FlexRayNodeState::DefaultConfig:     return "DEFAULT_CONFIG";
        case FlexRayNodeState::Ready:             return "READY";
        case FlexRayNodeState::WakeupListen:      return "WAKEUP_LISTEN";
        case FlexRayNodeState::WakeupSend:        return "WAKEUP_SEND";
        case FlexRayNodeState::StartupPrepare:    return "STARTUP_PREPARE";
        case FlexRayNodeState::ColdstartListen:   return "COLDSTART_LISTEN";
        case FlexRayNodeState::ColdstartCollisionResolution: return "COLDSTART_COLLISION_RES";
        case FlexRayNodeState::ColdstartConsistencyCheck:    return "COLDSTART_CONSISTENCY";
        case FlexRayNodeState::ColdstartGap:      return "COLDSTART_GAP";
        case FlexRayNodeState::IntegrationColdstartCheck:    return "INTEGRATION_COLDSTART";
        case FlexRayNodeState::IntegrationListen: return "INTEGRATION_LISTEN";
        case FlexRayNodeState::IntegrationConsistencyCheck:  return "INTEGRATION_CONSISTENCY";
        case FlexRayNodeState::NormalActive:       return "NORMAL_ACTIVE";
        case FlexRayNodeState::NormalPassive:      return "NORMAL_PASSIVE";
        case FlexRayNodeState::Halt:               return "HALT";
    }
    return "UNKNOWN";
}

class FlexRayNode {
    std::string name_;
    bool is_coldstart_;
    FlexRayNodeState state_ = FlexRayNodeState::DefaultConfig;
    bool sync_frame_tx_ = false;
    uint8_t current_cycle_ = 0;

public:
    FlexRayNode(std::string name, bool coldstart)
        : name_(std::move(name)), is_coldstart_(coldstart) {}

    const std::string& name() const { return name_; }
    bool is_coldstart() const { return is_coldstart_; }
    FlexRayNodeState state() const { return state_; }

    // EN: Simplified startup sequence
    void configure() { state_ = FlexRayNodeState::Ready; }

    void initiate_startup() {
        if (!is_coldstart_) {
            state_ = FlexRayNodeState::IntegrationListen;
            return;
        }
        state_ = FlexRayNodeState::ColdstartListen;
    }

    void send_cas() {
        if (is_coldstart_ && state_ == FlexRayNodeState::ColdstartListen) {
            state_ = FlexRayNodeState::ColdstartCollisionResolution;
            sync_frame_tx_ = true;
        }
    }

    void coldstart_complete() {
        if (is_coldstart_) {
            state_ = FlexRayNodeState::NormalActive;
        }
    }

    void integrate() {
        if (!is_coldstart_ && state_ == FlexRayNodeState::IntegrationListen) {
            state_ = FlexRayNodeState::IntegrationConsistencyCheck;
        }
    }

    void integration_complete() {
        state_ = FlexRayNodeState::NormalActive;
    }

    void increment_cycle() {
        current_cycle_ = static_cast<uint8_t>((current_cycle_ + 1) % 64);
    }

    void print_status() const {
        std::cout << "    " << std::left << std::setw(20) << name_
                  << std::right
                  << " | State: " << std::left << std::setw(28) << to_string(state_)
                  << std::right
                  << " | " << (is_coldstart_ ? "COLDSTART" : "INTEGRATION")
                  << " | Cycle: " << static_cast<int>(current_cycle_) << "\n";
    }
};

// ═════════════════════════════════════════════════════════════════════════════
// PART 5: FlexRay Dual-Channel Redundancy
// ═════════════════════════════════════════════════════════════════════════════

// EN: FlexRay dual-channel provides:
//     1) Redundancy: Same data on A+B → detect channel failure
//     2) Bandwidth doubling: Different data on A and B → 2x throughput
//     3) Mixed: Some frames on A+B (safety), some on A or B only

struct DualChannelResult {
    bool channel_a_ok = true;
    bool channel_b_ok = true;
    bool data_consistent = true;

    void print() const {
        std::cout << "    Channel A: " << (channel_a_ok ? "OK" : "FAULT")
                  << " | Channel B: " << (channel_b_ok ? "OK" : "FAULT")
                  << " | Consistency: " << (data_consistent ? "MATCH ✓" : "MISMATCH ✗")
                  << "\n";
    }
};

DualChannelResult check_redundancy(const FlexRayFrame& frame_a, const FlexRayFrame& frame_b) {
    DualChannelResult result;
    result.channel_a_ok = !frame_a.flags.null_frame;
    result.channel_b_ok = !frame_b.flags.null_frame;

    if (result.channel_a_ok && result.channel_b_ok) {
        // EN: Compare payloads for consistency
        result.data_consistent = (frame_a.payload == frame_b.payload) &&
                                 (frame_a.frame_id == frame_b.frame_id);
    } else {
        result.data_consistent = false;
    }
    return result;
}

// ═════════════════════════════════════════════════════════════════════════════
// MAIN
// ═════════════════════════════════════════════════════════════════════════════
int main() {
    std::cout << "============================================\n";
    std::cout << "   Module 14 - FlexRay\n";
    std::cout << "============================================\n\n";

    // ─── Demo 1: FlexRay Frame Structure ────────────────────────────────
    {
        std::cout << "--- Demo 1: FlexRay Frame Structure ---\n";

        // EN: Steering angle frame — safety-critical, dual channel
        FlexRayFrame steering;
        steering.frame_id = 1;
        steering.payload_length = 4;  // 8 bytes
        steering.cycle_count = 0;
        steering.flags.sync_frame = true;
        steering.flags.startup_frame = true;
        steering.channel = FlexRayFrame::Channel::AB;
        steering.payload = {0x03, 0xE8, 0x00, 0x50, 0x01, 0x00, 0x00, 0x00};
        steering.print("SteeringAngle (ASIL-D, dual-channel)");

        // EN: Brake pressure frame
        FlexRayFrame brake;
        brake.frame_id = 2;
        brake.payload_length = 3;  // 6 bytes
        brake.cycle_count = 0;
        brake.flags.sync_frame = true;
        brake.channel = FlexRayFrame::Channel::AB;
        brake.payload = {0x07, 0xD0, 0x00, 0x64, 0x01, 0x01};
        brake.print("BrakePressure (ASIL-D, dual-channel)");

        // EN: Non-safety data: infotainment (single channel, larger payload)
        FlexRayFrame infotainment;
        infotainment.frame_id = 15;
        infotainment.payload_length = 64;  // 128 bytes
        infotainment.cycle_count = 0;
        infotainment.channel = FlexRayFrame::Channel::A;
        infotainment.payload.resize(128);
        for (int i = 0; i < 128; ++i) infotainment.payload[static_cast<size_t>(i)] = static_cast<uint8_t>(i & 0xFF);
        infotainment.print("MediaStream (QM, channel A only)");
        std::cout << "\n";
    }

    // ─── Demo 2: Cluster Timing Configuration ──────────────────────────
    {
        std::cout << "--- Demo 2: FlexRay Timing Configuration ---\n";

        FlexRayClusterConfig config;
        config.macrotick_ns = 1000;       // 1 μs per macrotick
        config.static_slot_mt = 25;       // 25 μs per static slot
        config.num_static_slots = 20;     // 20 static slots
        config.dynamic_segment_mt = 100;  // 100 μs dynamic
        config.symbol_window_mt = 10;
        config.nit_mt = 30;

        config.print();

        std::cout << "\n  Communication Cycle Layout:\n";
        uint32_t static_end = static_cast<uint32_t>(config.static_slot_mt) * config.num_static_slots;
        uint32_t dynamic_end = static_end + config.dynamic_segment_mt;
        uint32_t sw_end = dynamic_end + config.symbol_window_mt;
        uint32_t nit_end = sw_end + config.nit_mt;
        std::cout << "  |← Static: 0-" << static_end << " MT →|"
                  << "← Dynamic: " << static_end << "-" << dynamic_end << " MT →|"
                  << "← SW: " << dynamic_end << "-" << sw_end << " →|"
                  << "← NIT: " << sw_end << "-" << nit_end << " →|\n\n";
    }

    // ─── Demo 3: Static Segment TDMA Schedule ──────────────────────────
    {
        std::cout << "--- Demo 3: Static Segment TDMA Schedule ---\n";
        std::cout << "  Scenario: Steer-by-wire system\n\n";

        FlexRayStaticSchedule schedule;

        // EN: Safety-critical signals on A+B (redundant)
        schedule.assign(1, "SteeringECU", "SteeringAngle", 4, FlexRayFrame::Channel::AB);
        schedule.assign(2, "BrakeECU", "BrakePressure", 3, FlexRayFrame::Channel::AB);
        schedule.assign(3, "SteeringECU", "SteeringTorque", 2, FlexRayFrame::Channel::AB);
        schedule.assign(4, "BrakeECU", "WheelSpeed_FL", 2, FlexRayFrame::Channel::AB);
        schedule.assign(5, "BrakeECU", "WheelSpeed_FR", 2, FlexRayFrame::Channel::AB);
        schedule.assign(6, "RackActuator", "RackPosition", 4, FlexRayFrame::Channel::AB);
        schedule.assign(7, "RackActuator", "MotorCurrent", 2, FlexRayFrame::Channel::AB);

        // EN: Medium-priority signals on single channel
        schedule.assign(8, "SuspensionECU", "DamperForce_FL", 2, FlexRayFrame::Channel::A);
        schedule.assign(9, "SuspensionECU", "DamperForce_FR", 2, FlexRayFrame::Channel::A);
        schedule.assign(10, "SuspensionECU", "DamperForce_RL", 2, FlexRayFrame::Channel::B);
        schedule.assign(11, "SuspensionECU", "DamperForce_RR", 2, FlexRayFrame::Channel::B);

        // EN: Lower-priority / diagnostic
        schedule.assign(15, "Gateway", "DiagnosticData", 32, FlexRayFrame::Channel::A);

        schedule.print();
        std::cout << "\n  NOTE: Slots 1-7 on A+B = ASIL-D redundant (steer-by-wire)\n";
        std::cout << "        Slots 8-11 split A/B = bandwidth doubling (suspension)\n";
        std::cout << "        Slot 15 on A only = non-safety diagnostic data\n\n";
    }

    // ─── Demo 4: Startup Protocol Simulation ────────────────────────────
    {
        std::cout << "--- Demo 4: FlexRay Startup Protocol ---\n";

        // EN: Create cluster: 2 coldstart nodes + 2 integration nodes
        FlexRayNode steering("SteeringECU", true);   // coldstart
        FlexRayNode brake("BrakeECU", true);          // coldstart
        FlexRayNode suspension("SuspensionECU", false); // integration
        FlexRayNode gateway("Gateway", false);          // integration

        std::vector<FlexRayNode*> cluster = {&steering, &brake, &suspension, &gateway};

        // EN: Step 1: Configure all nodes
        std::cout << "  Step 1: Configure nodes\n";
        for (auto* node : cluster) {
            node->configure();
            node->print_status();
        }

        // EN: Step 2: Initiate startup
        std::cout << "\n  Step 2: Initiate startup\n";
        for (auto* node : cluster) {
            node->initiate_startup();
            node->print_status();
        }

        // EN: Step 3: Leading coldstart node sends CAS
        std::cout << "\n  Step 3: Coldstart nodes send CAS (Collision Avoidance Symbol)\n";
        steering.send_cas();  // leading coldstart
        brake.send_cas();     // following coldstart
        for (auto* node : cluster) node->print_status();

        // EN: Step 4: Coldstart complete → NORMAL_ACTIVE
        std::cout << "\n  Step 4: Coldstart complete\n";
        steering.coldstart_complete();
        brake.coldstart_complete();
        for (auto* node : cluster) node->print_status();

        // EN: Step 5: Integration nodes join
        std::cout << "\n  Step 5: Integration nodes join cluster\n";
        suspension.integrate();
        gateway.integrate();
        for (auto* node : cluster) node->print_status();

        // EN: Step 6: Integration complete — all NORMAL_ACTIVE
        std::cout << "\n  Step 6: All nodes NORMAL_ACTIVE\n";
        suspension.integration_complete();
        gateway.integration_complete();
        for (auto* node : cluster) node->print_status();
        std::cout << "\n";
    }

    // ─── Demo 5: Dual Channel Redundancy Check ─────────────────────────
    {
        std::cout << "--- Demo 5: Dual Channel Redundancy ---\n";

        // EN: Case 1: Both channels OK, data matches
        std::cout << "  Case 1: Normal operation (both channels OK)\n";
        FlexRayFrame frame_a1;
        frame_a1.frame_id = 1;
        frame_a1.payload = {0x03, 0xE8, 0x00, 0x50};
        FlexRayFrame frame_b1 = frame_a1;  // same data on B
        auto result1 = check_redundancy(frame_a1, frame_b1);
        result1.print();

        // EN: Case 2: Channel B failure (null frame)
        std::cout << "  Case 2: Channel B failure (null frame received)\n";
        FlexRayFrame frame_b2;
        frame_b2.frame_id = 1;
        frame_b2.flags.null_frame = true;
        auto result2 = check_redundancy(frame_a1, frame_b2);
        result2.print();

        // EN: Case 3: Data corruption on Channel B
        std::cout << "  Case 3: Data corruption on Channel B\n";
        FlexRayFrame frame_b3;
        frame_b3.frame_id = 1;
        frame_b3.payload = {0x03, 0xE8, 0x00, 0x51};  // 0x50→0x51 corruption
        auto result3 = check_redundancy(frame_a1, frame_b3);
        result3.print();

        std::cout << "\n  Redundancy strategies:\n";
        std::cout << "    1. Both match → Use data normally\n";
        std::cout << "    2. One channel fault → Use other channel (degraded mode)\n";
        std::cout << "    3. Data mismatch → Enter safe state / re-synchronize\n\n";
    }

    // ─── Demo 6: Steer-by-Wire Simulation ──────────────────────────────
    {
        std::cout << "--- Demo 6: Steer-by-Wire Simulation ---\n";
        std::cout << "  Simulating 10 communication cycles (steering + rack feedback)\n\n";

        double steering_angle = 0.0;   // degrees
        double rack_position = 0.0;    // mm
        constexpr double steering_ratio = 16.0;  // steering:rack ratio
        constexpr double angle_step = 5.0;       // degrees per cycle (driver turning)

        std::cout << "  ┌───────┬──────────────┬──────────────┬───────────────────┐\n";
        std::cout << "  │ Cycle │ SteerAngle(°)│ RackPos(mm)  │ Status            │\n";
        std::cout << "  ├───────┼──────────────┼──────────────┼───────────────────┤\n";

        for (int cycle = 0; cycle < 10; ++cycle) {
            steering_angle += angle_step;
            rack_position = steering_angle / steering_ratio;

            // EN: Pack into FlexRay frame
            FlexRayFrame steer_frame;
            steer_frame.frame_id = 1;
            steer_frame.cycle_count = static_cast<uint8_t>(cycle);
            auto angle_raw = static_cast<int16_t>(steering_angle * 10);  // 0.1° resolution
            auto rack_raw = static_cast<int16_t>(rack_position * 100);    // 0.01mm resolution
            steer_frame.payload = {
                static_cast<uint8_t>(angle_raw & 0xFF),
                static_cast<uint8_t>((angle_raw >> 8) & 0xFF),
                static_cast<uint8_t>(rack_raw & 0xFF),
                static_cast<uint8_t>((rack_raw >> 8) & 0xFF)
            };

            std::string status = "OK";
            if (steering_angle > 40.0) status = "WARN: fast turn";
            if (steering_angle > 45.0) status = "LIMIT: max assist";

            std::cout << "  │ " << std::setw(5) << cycle
                      << " │ " << std::setw(12) << std::fixed << std::setprecision(1)
                      << steering_angle
                      << " │ " << std::setw(12) << std::setprecision(2) << rack_position
                      << " │ " << std::left << std::setw(17) << status << std::right
                      << " │\n";
        }
        std::cout << "  └───────┴──────────────┴──────────────┴───────────────────┘\n\n";
    }

    // ─── Demo 7: FlexRay Future — Migration to Ethernet ─────────────────
    {
        std::cout << "--- Demo 7: FlexRay vs Ethernet — Migration Path ---\n\n";
        std::cout << "  FlexRay is being replaced by Automotive Ethernet + TSN:\n\n";
        std::cout << "  ┌────────────────────────┬──────────────────┬──────────────────┐\n";
        std::cout << "  │ Aspect                 │ FlexRay          │ Ethernet + TSN   │\n";
        std::cout << "  ├────────────────────────┼──────────────────┼──────────────────┤\n";
        std::cout << "  │ Speed                  │ 2x10 Mbps        │ 100M-10G bps     │\n";
        std::cout << "  │ Payload                │ 254 bytes        │ 1500+ bytes      │\n";
        std::cout << "  │ Determinism            │ Native TDMA      │ TSN (802.1Qbv)   │\n";
        std::cout << "  │ Redundancy             │ Dual-channel     │ HSR/PRP (IEC)    │\n";
        std::cout << "  │ Ecosystem              │ Automotive only  │ Universal IT/OT  │\n";
        std::cout << "  │ Cost trend             │ Flat             │ Decreasing       │\n";
        std::cout << "  │ SDV compatible         │ Limited          │ Native SOA       │\n";
        std::cout << "  │ IP-based services      │ No               │ Yes (SOME/IP)    │\n";
        std::cout << "  │ OTA update support     │ Limited          │ Full             │\n";
        std::cout << "  │ New vehicle designs    │ Rare (2024+)     │ Standard         │\n";
        std::cout << "  └────────────────────────┴──────────────────┴──────────────────┘\n\n";

        std::cout << "  Timeline:\n";
        std::cout << "    2006-2015: FlexRay in BMW 5/7 Series, Mercedes S-Class\n";
        std::cout << "    2015-2020: Automotive Ethernet adoption starts (ADAS cameras)\n";
        std::cout << "    2020-2025: TSN matures, replaces FlexRay in new platforms\n";
        std::cout << "    2025+:     FlexRay → legacy support only, Ethernet dominates\n\n";
    }

    std::cout << "============================================\n";
    std::cout << "   End of FlexRay\n";
    std::cout << "============================================\n";

    return 0;
}
