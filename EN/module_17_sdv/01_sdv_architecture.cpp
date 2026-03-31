/**
 * @file module_17_sdv/01_sdv_architecture.cpp
 * @brief Software-Defined Vehicle (SDV) Architecture & C++ Role
 *
 * @details
 * =============================================================================
 * [THEORY: SDV — Software-Defined Vehicle]
 * =============================================================================
 *
 * EN: An SDV treats the vehicle as a software platform where features are
 *     defined, updated, and enhanced through software — not hardware changes.
 *
 *     Traditional ECU Architecture        SDV Architecture
 *     ┌──────────────────────┐          ┌──────────────────────────┐
 *     │  ECU1 (Engine)       │          │  HPC (Central Computer)  │
 *     │  ECU2 (Brakes/ABS)   │          │  ┌─────────────────────┐ │
 *     │  ECU3 (ADAS)         │          │  │ ADAS  │ Cockpit     │ │
 *     │  ECU4 (Body/BCM)     │   →→→    │  │ Body  │ Powertrain  │ │
 *     │  ECU5 (Infotainment) │          │  └─────────────────────┘ │
 *     │  ECU6 (Gateway)      │          │  + Zone Controllers (ZC) │
 *     │  ... (70-150 ECUs)   │          │    ZC1  ZC2  ZC3  ZC4    │
 *     └──────────────────────┘          └──────────────────────────┘
 *
 *
 *     Geleneksel ECU Mimarisi:       SDV Mimarisi:
 *     ┌──────────────────────┐      ┌──────────────────────────┐
 *     │ ECU1 (Motor)         │      │ HPC (Merkezi Bilgisayar) │
 *     │ ECU2 (Fren/ABS)      │      │ ┌─────────────────────┐  │
 *     │ ECU4     │      │ └─────────────────────┘  │
 *     │ ... (70-150 ECU)     │      │   ZC1  ZC2  ZC3  ZC4     │
 *     └──────────────────────┘      └──────────────────────────┘
 *
 * =============================================================================
 * [THEORY: Domain → Zonal Architecture Transition]
 * =============================================================================
 *
 * EN: DOMAIN-based (traditional):
 *     ┌──────────┐ ┌──────────┐ ┌──────────┐ ┌──────────┐
 *     │Powertrain│ │  Chassis │ │   Body   │ │   ADAS   │
 *     │ Domain   │ │  Domain  │ │  Domain  │ │  Domain  │
 *     │Controller│ │Controller│ │Controller│ │Controller│
 *     └────┬─────┘ └────┬─────┘ └────┬─────┘ └────┬─────┘
 *          │            │            │                  │
 *     ─────┴────────────┴────────────┴────────────┴─── CAN/LIN
 *
 *     ZONAL (SDV):
 *                    ┌──────────────────────┐
 *                    │   HPC / Vehicle      │
 *                    │   Computer           │
 *                    └─┬──────┬──────┬────┬─┘
 *                      │      │      │    │  Ethernet backbone
 *           ┌──────────┴┐  ┌──┴───┐ ┌┴──┐┌┴──────────┐
 *           │Zone Front │  │Zone  │ │ZR ││Zone Rear  │
 *           │Left  (ZFL)│  │FR    │ │L  ││Right (ZRR)│
 *           └───┬───┬───┘  └──┬───┘ └┬──┘└───┬───┬───┘
 *           Sensors Actuators  ...       Sensors Actuators
 *
 *
 * =============================================================================
 * [THEORY: SOA — Service-Oriented Architecture in Automotive]
 * =============================================================================
 *
 * EN:
 *     ┌───────────────────────────────────────────────────────┐
 *     │                  Service Consumer                     │
 *     │  Dashboard App    Navigation    Autonomous Driving    │
 *     └───────┬──────────────┬──────────────┬─────────────────┘
 *             │              │              │
 *     ┌───────┴──────────────┴──────────────┴─────────────────┐
 *     │              Service Middleware (SOA)                 │
 *     │             SOME/IP  |  DDS  |  iceoryx               │
 *     └───────┬──────────────┬──────────────┬─────────────────┘
 *             │              │              │
 *     ┌───────┴────┐   ┌─────┴─────┐  ┌─────┴──────┐
 *     │VehicleSpeed│   │  Radar    │  │  Camera    │
 *     │  Service   │   │  Service  │  │  Service   │
 *     └────────────┘   └───────────┘  └────────────┘
 *
 *
 * =============================================================================
 * [THEORY: Vehicle Signal Specification (VSS)]
 * =============================================================================
 *
 * EN: COVESA VSS is a standardized tree-based vehicle data model:
 *     Vehicle.Speed              → km/h (uint16)
 *     Vehicle.Chassis.SteeringWheel.Angle → degrees (float)
 *     Vehicle.Powertrain.FuelSystem.Level → percent (uint8)
 *     Vehicle.ADAS.AEB.IsEnabled → bool
 *
 *
 * =============================================================================
 * [THEORY: C++ Role in SDV]
 * =============================================================================
 *
 * EN:
 *     ┌─────────────────────────────────────────────────────────┐
 *     │ Layer              │ Language    │ Why                  │
 *     ├────────────────────┼─────────────┼──────────────────────┤
 *     │ ML Training        │ Python      │ Framework ecosystem  │
 *     │ ML Inference       │ C++ / CUDA  │ Real-time latency    │
 *     │ Sensor Fusion      │ C++         │ Deterministic timing │
 *     │ Middleware         │ C++         │ Performance + SOA    │
 *     │ AUTOSAR Adaptive   │ C++14/17    │ ara::* APIs          │
 *     │ Real-time Services │ C++         │ RTOS integration     │
 *     │ HMI / Qt           │ C++ / QML   │ GPU rendering        │
 *     │ Android Automotive │ Java/Kotlin │ App framework        │
 *     │ Safety-critical new│ Rust        │ Memory safety        │
 *     │ Driver/HAL         │ C           │ Kernel requirement   │
 *     └────────────────────┴─────────────┴──────────────────────┘
 *
 *
 * @note Compile: g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion 01_sdv_architecture.cpp -o 01_sdv_arch
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <iostream>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>
#include <map>
#include <unordered_map>
#include <functional>
#include <memory>
#include <optional>
#include <variant>
#include <iomanip>
#include <algorithm>
#include <chrono>
#include <array>
#include <queue>
#include <sstream>
#include <cassert>

// ═════════════════════════════════════════════════════════════════════════════
// PART 1: SDV Architecture Model — Domain vs Zonal
// ═════════════════════════════════════════════════════════════════════════════

namespace sdv_arch {

// EN: ECU types in traditional vs SDV architecture
enum class ECUType : uint8_t {
    TRADITIONAL_DOMAIN, HPC, ZONE_CONTROLLER
};

struct ECUNode {
    std::string         name;
    ECUType             type;
    std::vector<std::string> functions;
    uint32_t            compute_gflops;  // processing power
};

// EN: Demonstrate domain→zonal consolidation
struct VehicleArchitecture {
    std::string               arch_name;
    std::vector<ECUNode>      nodes;
    uint32_t                  total_ecus;
    std::string               backbone;

    void Print() const {
        std::cout << "  Architecture: " << arch_name << "\n"
                  << "  Backbone: " << backbone << "\n"
                  << "  Total nodes: " << total_ecus << "\n";
        for (const auto& n : nodes) {
            std::cout << "    [" << n.name << "] ("
                      << n.compute_gflops << " GFLOPS) — ";
            for (size_t i = 0; i < n.functions.size(); ++i) {
                if (i > 0) std::cout << ", ";
                std::cout << n.functions[i];
            }
            std::cout << "\n";
        }
    }
};

inline VehicleArchitecture CreateTraditional() {
    return {"Domain-Based (Traditional)", {
        {"Engine ECU",       ECUType::TRADITIONAL_DOMAIN, {"Fuel injection", "Ignition"}, 1},
        {"ABS/ESP ECU",      ECUType::TRADITIONAL_DOMAIN, {"Brake control", "Stability"}, 2},
        {"BCM",              ECUType::TRADITIONAL_DOMAIN, {"Lights", "Wipers", "Doors"}, 1},
        {"ADAS ECU",         ECUType::TRADITIONAL_DOMAIN, {"AEB", "LKA", "ACC"}, 20},
        {"Infotainment",     ECUType::TRADITIONAL_DOMAIN, {"Nav", "Media", "Phone"}, 10},
        {"Gateway",          ECUType::TRADITIONAL_DOMAIN, {"CAN routing"}, 1},
    }, 80, "CAN + LIN"};
}

inline VehicleArchitecture CreateZonal() {
    return {"Zonal SDV", {
        {"Vehicle HPC",      ECUType::HPC, {"ADAS", "Cockpit", "Body logic", "OTA"}, 200},
        {"Zone Front-Left",  ECUType::ZONE_CONTROLLER, {"FL sensors", "FL actuators"}, 5},
        {"Zone Front-Right", ECUType::ZONE_CONTROLLER, {"FR sensors", "FR actuators"}, 5},
        {"Zone Rear-Left",   ECUType::ZONE_CONTROLLER, {"RL sensors", "RL actuators"}, 5},
        {"Zone Rear-Right",  ECUType::ZONE_CONTROLLER, {"RR sensors", "RR actuators"}, 5},
    }, 12, "Automotive Ethernet (10GBASE-T1)"};
}

}  // namespace sdv_arch

// ═════════════════════════════════════════════════════════════════════════════
// PART 2: Vehicle Signal Specification (VSS) — Signal Tree
// ═════════════════════════════════════════════════════════════════════════════

namespace vss {

// EN: VSS data types — mirrors COVESA/GENIVI VSS standard
using SignalValue = std::variant<bool, int32_t, uint32_t, float, std::string>;

struct VSSNode {
    std::string  path;         // e.g. "Vehicle.Speed"
    std::string  description;
    std::string  unit;
    SignalValue  value;
    std::string  data_type;
};

class VSSTree {
    std::map<std::string, VSSNode> signals_;

public:
    void AddSignal(VSSNode node) {
        auto path = node.path;
        signals_[path] = std::move(node);
    }

    std::optional<SignalValue> Get(const std::string& path) const {
        auto it = signals_.find(path);
        if (it == signals_.end()) return std::nullopt;
        return it->second.value;
    }

    bool Set(const std::string& path, SignalValue val) {
        auto it = signals_.find(path);
        if (it == signals_.end()) return false;
        it->second.value = std::move(val);
        return true;
    }

    void PrintAll() const {
        for (const auto& [path, node] : signals_) {
            std::cout << "  " << std::setw(45) << std::left << path;
            std::visit([&](const auto& v) {
                using T = std::decay_t<decltype(v)>;
                if constexpr (std::is_same_v<T, bool>)
                    std::cout << (v ? "true" : "false");
                else if constexpr (std::is_same_v<T, std::string>)
                    std::cout << v;
                else
                    std::cout << v;
            }, node.value);
            std::cout << " " << node.unit << "\n";
        }
    }
};

inline VSSTree CreateDefaultVSS() {
    VSSTree tree;
    tree.AddSignal({"Vehicle.Speed",                        "Current speed",       "km/h",   uint32_t{0},    "uint16"});
    tree.AddSignal({"Vehicle.Powertrain.FuelSystem.Level",  "Fuel level",          "%",      float{75.0f},   "float"});
    tree.AddSignal({"Vehicle.Chassis.SteeringWheel.Angle",  "Steering angle",      "deg",    float{0.0f},    "float"});
    tree.AddSignal({"Vehicle.ADAS.AEB.IsEnabled",           "AEB status",          "",       true,           "bool"});
    tree.AddSignal({"Vehicle.Body.Lights.Beam.Low.IsOn",    "Low beam status",     "",       false,          "bool"});
    tree.AddSignal({"Vehicle.Cabin.HVAC.Temperature",       "Cabin temperature",   "°C",     float{22.0f},   "float"});
    tree.AddSignal({"Vehicle.OBD.EngineLoad",               "Engine load",         "%",      float{35.0f},   "float"});
    tree.AddSignal({"Vehicle.CurrentLocation.Latitude",     "GPS latitude",        "deg",    float{39.92f},  "float"});
    tree.AddSignal({"Vehicle.CurrentLocation.Longitude",    "GPS longitude",       "deg",    float{32.85f},  "float"});
    tree.AddSignal({"Vehicle.VehicleIdentification.VIN",    "VIN",                 "",       std::string{"WVWZZZ3CZWE123456"}, "string"});
    return tree;
}

}  // namespace vss

// ═════════════════════════════════════════════════════════════════════════════
// PART 3: SOA — Mini Service-Oriented Architecture Framework
// ═════════════════════════════════════════════════════════════════════════════

namespace soa {

// EN: Service message types
enum class MessageType : uint8_t {
    REQUEST, RESPONSE, EVENT, SUBSCRIBE, UNSUBSCRIBE
};

struct ServiceMessage {
    std::string  service_name;
    std::string  method_or_event;
    MessageType  type;
    std::string  payload;
    uint32_t     client_id;
};

// EN: Service interface — all services implement this
class IService {
public:
    virtual ~IService() = default;
    virtual std::string_view GetName() const = 0;
    virtual std::string HandleRequest(const std::string& method,
                                       const std::string& payload) = 0;
};

// EN: SOA Service Registry — central service directory
class ServiceRegistry {
    std::unordered_map<std::string, IService*> services_;
    std::unordered_map<std::string,
        std::vector<std::function<void(const std::string&, const std::string&)>>> subscribers_;

public:
    bool Register(IService* svc) {
        auto name = std::string(svc->GetName());
        if (services_.count(name) > 0) return false;
        services_[name] = svc;
        std::cout << "  [Registry] Service registered: " << name << "\n";
        return true;
    }

    std::optional<std::string> Call(const std::string& service,
                                     const std::string& method,
                                     const std::string& payload) {
        auto it = services_.find(service);
        if (it == services_.end()) return std::nullopt;
        return it->second->HandleRequest(method, payload);
    }

    void Subscribe(const std::string& event_topic,
                   std::function<void(const std::string&, const std::string&)> handler) {
        subscribers_[event_topic].push_back(std::move(handler));
    }

    void Publish(const std::string& event_topic, const std::string& data) {
        auto it = subscribers_.find(event_topic);
        if (it != subscribers_.end()) {
            for (auto& handler : it->second) {
                handler(event_topic, data);
            }
        }
    }

    [[nodiscard]] size_t ServiceCount() const { return services_.size(); }
};

// ═════════════════════════════════════════════════════════════════════════════
// PART 4: Concrete Services — VehicleSpeed, Dashboard, Logger
// ═════════════════════════════════════════════════════════════════════════════

// EN: Vehicle Speed Service (provider)
class VehicleSpeedService : public IService {
    uint32_t speed_kmh_ = 0;
    ServiceRegistry& registry_;

public:
    explicit VehicleSpeedService(ServiceRegistry& reg) : registry_(reg) {}

    std::string_view GetName() const override { return "VehicleSpeedService"; }

    std::string HandleRequest(const std::string& method,
                               const std::string& /*payload*/) override {
        if (method == "GetSpeed") {
            return std::to_string(speed_kmh_);
        }
        return "ERROR:UNKNOWN_METHOD";
    }

    // EN: Simulate speed change and publish event
    void UpdateSpeed(uint32_t new_speed) {
        speed_kmh_ = new_speed;
        registry_.Publish("VehicleSpeed.Changed", std::to_string(speed_kmh_));
    }
};

// EN: Dashboard Service (consumer / observer)
class DashboardService : public IService {
    uint32_t displayed_speed_ = 0;

public:
    std::string_view GetName() const override { return "DashboardService"; }

    std::string HandleRequest(const std::string& method,
                               const std::string& /*payload*/) override {
        if (method == "GetDisplayedSpeed")
            return std::to_string(displayed_speed_);
        return "ERROR:UNKNOWN_METHOD";
    }

    void OnSpeedChanged(const std::string& /*topic*/, const std::string& data) {
        displayed_speed_ = static_cast<uint32_t>(std::stoul(data));
        std::cout << "    [Dashboard] Speed updated: " << displayed_speed_ << " km/h\n";
    }
};

// EN: Data Logger Service (observer)
class DataLoggerService : public IService {
    std::vector<std::string> log_entries_;

public:
    std::string_view GetName() const override { return "DataLoggerService"; }

    std::string HandleRequest(const std::string& method,
                               const std::string& /*payload*/) override {
        if (method == "GetLogCount")
            return std::to_string(log_entries_.size());
        if (method == "GetLogs") {
            std::string result;
            for (const auto& e : log_entries_) {
                result += e + "; ";
            }
            return result;
        }
        return "ERROR:UNKNOWN_METHOD";
    }

    void OnEvent(const std::string& topic, const std::string& data) {
        auto entry = "[LOG] " + topic + "=" + data;
        log_entries_.push_back(entry);
        std::cout << "    [Logger] " << entry << "\n";
    }

    [[nodiscard]] size_t EntryCount() const { return log_entries_.size(); }
};

}  // namespace soa

// ═════════════════════════════════════════════════════════════════════════════
// PART 5: HPC Software Stack Model
// ═════════════════════════════════════════════════════════════════════════════

namespace hpc_stack {

struct SoftwareLayer {
    std::string name;
    std::string language;
    std::string description;
};

inline std::vector<SoftwareLayer> GetHPCStack() {
    return {
        {"Vehicle Apps",      "C++/Python/Java", "ADAS, Cockpit HMI, Body logic"},
        {"ara::* / AOS API",  "C++17",           "AUTOSAR Adaptive / Android Automotive"},
        {"Middleware",         "C++",             "SOME/IP, DDS, iceoryx, gRPC"},
        {"OS Framework",      "C/C++",           "POSIX, QNX/Linux, hypervisor"},
        {"Hypervisor",        "C/ASM",           "QNX Hypervisor, Xen, COQOS"},
        {"BSP / Drivers",     "C",               "Board support, MCAL, kernel modules"},
        {"Hardware SoC",      "RTL",             "Qualcomm SA8295P / NVIDIA Orin / NXP S32G"},
    };
}

}  // namespace hpc_stack

// ═════════════════════════════════════════════════════════════════════════════
// PART 6: Industry SDV Platform Comparison
// ═════════════════════════════════════════════════════════════════════════════

namespace industry {

struct SDVPlatform {
    std::string oem;
    std::string platform_name;
    std::string soc;
    std::string os;
    std::string middleware;
    std::string note;
};

inline std::vector<SDVPlatform> GetPlatforms() {
    return {
        {"Volkswagen",  "vw.os",          "Qualcomm SA8295P",  "VW.OS (Linux-based)", "SOME/IP + DDS",  "Cariad development"},
        {"Mercedes",    "MB.OS",          "NVIDIA Orin",       "QNX + Linux",         "SOME/IP",        "Full stack in-house"},
        {"BMW",         "Neue Klasse OS", "Qualcomm SA8295P",  "QNX + Android",       "SOME/IP + DDS",  "2025 launch with iX"},
        {"Tesla",       "Tesla OS",       "HW4.0 (custom)",    "Linux (custom)",      "Custom IPC",     "Full vertical integ."},
        {"Rivian",      "Rivian SW",      "Qualcomm SA8295P",  "Linux + QNX",         "DDS",            "Startup approach"},
        {"Hyundai",     "ccOS",           "Qualcomm SA8155P",  "Linux + RTOS",        "SOME/IP",        "Connected Car OS"},
    };
}

}  // namespace industry

// ═════════════════════════════════════════════════════════════════════════════
// MAIN — Demos
// ═════════════════════════════════════════════════════════════════════════════

int main() {
    std::cout << "╔══════════════════════════════════════════════════════════════╗\n"
              << "║ Module 17: SDV Architecture & C++ Role                       ║\n"
              << "╚══════════════════════════════════════════════════════════════╝\n\n";

    // --- Demo 1: Domain → Zonal Transition ---
    std::cout << "--- Demo 1: Domain → Zonal Architecture Transition ---\n";
    {
        auto trad = sdv_arch::CreateTraditional();
        auto zonal = sdv_arch::CreateZonal();

        std::cout << "\n  [TRADITIONAL]\n";
        trad.Print();
        std::cout << "\n  [ZONAL SDV]\n";
        zonal.Print();

        std::cout << "\n  Consolidation: " << trad.total_ecus << " ECUs → "
                  << zonal.total_ecus << " nodes ("
                  << (100 - (zonal.total_ecus * 100 / trad.total_ecus)) << "% reduction)\n";
    }

    // --- Demo 2: VSS Signal Tree ---
    std::cout << "\n--- Demo 2: Vehicle Signal Specification (VSS) Tree ---\n";
    {
        auto vss_tree = vss::CreateDefaultVSS();
        vss_tree.PrintAll();

        // EN: Update signals
        vss_tree.Set("Vehicle.Speed", uint32_t{120});
        vss_tree.Set("Vehicle.Body.Lights.Beam.Low.IsOn", true);

        std::cout << "\n  After update:\n";
        auto speed = vss_tree.Get("Vehicle.Speed");
        if (speed) {
            std::cout << "  Vehicle.Speed = "
                      << std::get<uint32_t>(*speed) << " km/h\n";
        }
        auto lights = vss_tree.Get("Vehicle.Body.Lights.Beam.Low.IsOn");
        if (lights) {
            std::cout << "  Vehicle.Body.Lights.Beam.Low.IsOn = "
                      << (std::get<bool>(*lights) ? "true" : "false") << "\n";
        }
    }

    // --- Demo 3: Mini SOA — Service Provider + Consumer + Observer ---
    std::cout << "\n--- Demo 3: SOA — VehicleSpeed + Dashboard + Logger ---\n";
    {
        soa::ServiceRegistry registry;

        soa::VehicleSpeedService speed_svc(registry);
        soa::DashboardService    dashboard;
        soa::DataLoggerService   logger;

        registry.Register(&speed_svc);
        registry.Register(&dashboard);
        registry.Register(&logger);

        // EN: Subscribe to speed events
        registry.Subscribe("VehicleSpeed.Changed",
            [&](const std::string& t, const std::string& d) { dashboard.OnSpeedChanged(t, d); });
        registry.Subscribe("VehicleSpeed.Changed",
            [&](const std::string& t, const std::string& d) { logger.OnEvent(t, d); });

        std::cout << "\n  Simulating speed changes:\n";
        speed_svc.UpdateSpeed(60);
        speed_svc.UpdateSpeed(100);
        speed_svc.UpdateSpeed(0);

        // EN: Query services via registry
        auto resp = registry.Call("VehicleSpeedService", "GetSpeed", "");
        std::cout << "\n  Registry.Call(VehicleSpeedService, GetSpeed) = "
                  << resp.value_or("N/A") << "\n";

        auto dash_resp = registry.Call("DashboardService", "GetDisplayedSpeed", "");
        std::cout << "  Registry.Call(DashboardService, GetDisplayedSpeed) = "
                  << dash_resp.value_or("N/A") << "\n";

        auto log_resp = registry.Call("DataLoggerService", "GetLogCount", "");
        std::cout << "  Logger entries: " << log_resp.value_or("0") << "\n";
    }

    // --- Demo 4: HPC Software Stack ---
    std::cout << "\n--- Demo 4: HPC Software Stack Layers ---\n";
    {
        auto stack = hpc_stack::GetHPCStack();
        std::cout << "  ┌─────┬──────────────────┬─────────────────┬───────────────────────────────────────────┐\n";
        std::cout << "  │ Lyr │ Name             │ Language        │ Description                               │\n";
        std::cout << "  ├─────┼──────────────────┼─────────────────┼───────────────────────────────────────────┤\n";
        int layer = static_cast<int>(stack.size());
        for (const auto& s : stack) {
            std::cout << "  │ L" << layer-- << "  │ "
                      << std::setw(16) << std::left << s.name << " │ "
                      << std::setw(15) << s.language << " │ "
                      << std::setw(41) << s.description << " │\n";
        }
        std::cout << std::right;
        std::cout << "  └─────┴──────────────────┴─────────────────┴───────────────────────────────────────────┘\n";
    }

    // --- Demo 5: Industry SDV Platforms ---
    std::cout << "\n--- Demo 5: Industry SDV Platforms ---\n";
    {
        auto platforms = industry::GetPlatforms();
        std::cout << "  ┌─────────────┬──────────────────┬──────────────────┬──────────────────────┬───────────────┐\n";
        std::cout << "  │ OEM         │ Platform         │ SoC              │ OS                   │ Middleware    │\n";
        std::cout << "  ├─────────────┼──────────────────┼──────────────────┼──────────────────────┼───────────────┤\n";
        for (const auto& p : platforms) {
            std::cout << "  │ " << std::setw(11) << std::left << p.oem
                      << " │ " << std::setw(16) << p.platform_name
                      << " │ " << std::setw(16) << p.soc
                      << " │ " << std::setw(20) << p.os
                      << " │ " << std::setw(13) << p.middleware << " │\n";
        }
        std::cout << std::right;
        std::cout << "  └─────────────┴──────────────────┴──────────────────┴──────────────────────┴───────────────┘\n";
    }

    // --- Demo 6: C++ Role Summary ---
    std::cout << "\n--- Demo 6: C++ Role in SDV Ecosystem ---\n";
    {
        struct LangRole { std::string_view lang; std::string_view domain; std::string_view pct; };
        LangRole roles[] = {
            {"C++",     "Middleware, ADAS, Sensor fusion, RT services",  "~45%"},
            {"C",       "Drivers, MCAL, RTOS kernel, bootloader",       "~25%"},
            {"Python",  "ML training, test automation, tooling",         "~15%"},
            {"Rust",    "Safety-critical new components (emerging)",     "~3%"},
            {"Java/Kt", "Android Automotive apps, infotainment",        "~8%"},
            {"Other",   "Shell, YAML, ARXML, build scripts",            "~4%"},
        };
        std::cout << "  Estimated language distribution in SDV projects:\n";
        for (const auto& r : roles) {
            std::cout << "  " << std::setw(8) << std::left << r.lang
                      << " " << std::setw(5) << r.pct << "  "
                      << r.domain << "\n";
        }
        std::cout << std::right;
    }

    std::cout << "\n[All demos completed successfully.]\n";
    return 0;
}

