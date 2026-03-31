/**
 * @file module_16_autosar/02_adaptive_platform.cpp
 * @brief AUTOSAR Adaptive Platform
 *
 * @details
 * =============================================================================
 * [THEORY: AUTOSAR Adaptive Platform — Overview]
 * =============================================================================
 *
 * EN: AUTOSAR Adaptive Platform (AP) is designed for high-performance ECUs
 *     running POSIX-based OS (Linux, QNX). Unlike Classic (C-based, static),
 *     Adaptive uses C++14/17, dynamic communication, and SOA.
 *
 *     Adaptive vs Classic:
 *     ┌─────────────────────┬──────────────────┬──────────────────────────┐
 *     │ Aspect              │ Classic Platform │ Adaptive Platform        │
 *     ├─────────────────────┼──────────────────┼──────────────────────────┤
 *     │ Language            │ C (MISRA C)      │ C++14/17 (MISRA C++ '23) │
 *     │ OS                  │ AUTOSAR OS(OSEK) │ POSIX PSE51 (Linux/QNX)  │
 *     │ Communication       │ Signal-based     │ Service-oriented (SOA)   │
 *     │ Update              │ Reflash ECU      │ OTA, dynamic deploy      │
 *     │ Scheduling          │ Static (offline) │ Dynamic (OS scheduler)   │
 *     │ Memory              │ Static alloc     │ Dynamic alloc possible   │
 *     │ Binding             │ Compile-time     │ Runtime service binding  │
 *     │ MCU/SoC             │ AURIX, RH850     │ SA8155P, i.MX8, Orin     │
 *     │ Typical Use         │ Powertrain, body │ ADAS, cockpit, gateway   │
 *     │ Safety              │ ASIL-D           │ ASIL-B+ (QNX → ASIL-D)   │
 *     └─────────────────────┴──────────────────┴──────────────────────────┘
 *
 * =============================================================================
 * [THEORY: ARA — AUTOSAR Runtime for Adaptive Applications]
 * =============================================================================
 *
 * EN: ARA is the middleware API layer exposing platform services:
 *
 *     ┌───────────────────────────────────────────────────────────┐
 *     │           Adaptive Application (C++14/17)                 │
 *     ├─────────┬─────────┬─────────┬────────┬────────┬───────────┤
 *     │ara::com │ara::exec│ara::diag│ara::per│ara::log│ara::crypto│
 *     ├─────────┴─────────┴─────────┴────────┴────────┴───────────┤
 *     │     Operating System (POSIX PSE51 — Linux/QNX)            │
 *     ├───────────────────────────────────────────────────────────┤
 *     │     Hardware (SoC: ARM Cortex-A, GPU, NPU)                │
 *     └───────────────────────────────────────────────────────────┘
 *
 *     ARA Functional Clusters:
 *     ┌─────────────┬──────────────────────────────────────────────┐
 *     │ Cluster     │ Purpose                                      │
 *     ├─────────────┼──────────────────────────────────────────────┤
 *     │ ara::com    │ Service-oriented communication (SOME/IP,DDS) │
 *     │ ara::exec   │ Execution & state management, process life   │
 *     │ ara::diag   │ UDS-over-DoIP diagnostic communication       │
 *     │ ara::per    │ Persistency (key-value store, file storage)  │
 *     │ ara::log    │ Logging (DLT — Diagnostic Log and Trace)     │
 *     │ ara::crypto │ Crypto services (HSM abstraction)            │
 *     │ ara::iam    │ Identity & Access Mgmt (security policy)     │
 *     │ ara::ucm    │ Update & Config Mgmt (OTA deployment)        │
 *     │ ara::phm    │ Platform Health Mgmt (watchdog, supervision) │
 *     │ ara::tsync  │ Time Synchronization (gPTP, StbM)            │
 *     └─────────────┴──────────────────────────────────────────────┘
 *
 * =============================================================================
 * [THEORY: ara::com — Proxy/Skeleton Pattern]
 * =============================================================================
 *
 * EN: ara::com uses the Proxy/Skeleton pattern for service communication:
 *     - Skeleton: Service provider (server-side implementation)
 *     - Proxy: Service consumer (client-side stub)
 *     - Communication elements: Methods, Events, Fields
 *
 *     ┌─────────────┐            ┌───────────────┐
 *     │  Consumer   │            │   Provider    │
 *     │ Application │            │  Application  │
 *     ├─────────────┤            ├───────────────┤
 *     │ Proxy       │◄══════════►│ Skeleton      │
 *     │ (generated) │  SOME/IP   │ (generated)   │
 *     │  FindService│  or DDS    │  OfferService │
 *     │  .Method()  │            │  .Method()    │
 *     │  .Event     │            │  .Event.Send()│
 *     │  .Field.Get │            │  .Field.Update│
 *     └─────────────┘            └───────────────┘
 *
 *     Discovery:
 *       1. Skeleton calls OfferService()
 *       2. Proxy calls FindService() → gets ServiceHandle
 *       3. Proxy creates instance from handle → communicates
 *
 * =============================================================================
 * [THEORY: POSIX PSE51 Restrictions]
 * =============================================================================
 *
 * EN: Adaptive Platform mandates POSIX PSE51 profile (minimal realtime):
 *     ALLOWED: threads, mutexes, condition variables, timers, file I/O (basic)
 *     FORBIDDEN: fork(), exec(), signals (SIGKILL etc.), shared memory (shm_open),
 *                System V IPC, dynamic library loading (dlopen)
 *   → Purpose: Ensure deterministic behavior on safety-certified RTOS (QNX)
 *
 * @note Compile: g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion 02_adaptive_platform.cpp -o 02_adaptive_platform
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
#include <optional>
#include <variant>
#include <memory>
#include <algorithm>
#include <iomanip>
#include <cassert>
#include <sstream>
#include <chrono>
#include <any>

// ═════════════════════════════════════════════════════════════════════════════
// PART 1: ara::com — Service-Oriented Communication (Proxy/Skeleton)
// ═════════════════════════════════════════════════════════════════════════════

namespace ara::com {

// EN: Result type (ara::core::Result analog)
template <typename T, typename E = int>
class Result {
    std::variant<T, E> storage_;
public:
    explicit Result(T value) : storage_(std::move(value)) {}
    explicit Result(E error) : storage_(std::move(error)) {}
    [[nodiscard]] bool HasValue() const { return std::holds_alternative<T>(storage_); }
    [[nodiscard]] const T& Value() const { return std::get<T>(storage_); }
    [[nodiscard]] const E& Error() const { return std::get<E>(storage_); }
};

// EN: Future — simplified async result (ara::core::Future analog)
template <typename T>
class Future {
    std::optional<T> value_;
    bool ready_ = false;
public:
    void SetValue(T val) { value_ = std::move(val); ready_ = true; }
    [[nodiscard]] bool is_ready() const { return ready_; }
    T get() { return *value_; }
};

// EN: ServiceHandle — returned by FindService, used to create Proxy
struct ServiceHandle {
    uint16_t service_id = 0;
    uint16_t instance_id = 0;
    std::string instance_name;
};

// EN: Service discovery registry — simulates SOME/IP-SD
class ServiceRegistry {
    std::vector<ServiceHandle> offered_services_;
    static ServiceRegistry* instance_;
public:
    static ServiceRegistry& Instance() {
        static ServiceRegistry reg;
        return reg;
    }
    void OfferService(uint16_t sid, uint16_t iid, const std::string& name) {
        offered_services_.push_back({sid, iid, name});
    }
    void StopOfferService(uint16_t sid, uint16_t iid) {
        std::erase_if(offered_services_, [&](const ServiceHandle& h) {
            return h.service_id == sid && h.instance_id == iid;
        });
    }
    std::vector<ServiceHandle> FindService(uint16_t sid) const {
        std::vector<ServiceHandle> result;
        for (const auto& h : offered_services_)
            if (h.service_id == sid) result.push_back(h);
        return result;
    }
    void Clear() { offered_services_.clear(); }
};

// -------------------------------------------------------------------------
// Radar Service — Skeleton (Provider)
// -------------------------------------------------------------------------

// EN: Service interface definition: RadarService
//     - Methods: Calibrate(mode) → CalibResult
//     - Events:  ObjectDetected (range, angle, velocity)
//     - Fields:  UpdateRate (get/set)

struct RadarObject {
    float range_m = 0.0f;
    float angle_deg = 0.0f;
    float velocity_mps = 0.0f;
    uint32_t object_id = 0;
};

struct CalibResult {
    bool success = false;
    std::string message;
};

// EN: Skeleton — service provider (server implementation)
class RadarServiceSkeleton {
    uint16_t service_id_;
    uint16_t instance_id_;
    uint32_t update_rate_hz_ = 20;

    // EN: Event subscribers
    std::vector<std::function<void(const RadarObject&)>> object_subscribers_;

public:
    RadarServiceSkeleton(uint16_t sid, uint16_t iid)
        : service_id_(sid), instance_id_(iid) {}

    void OfferService() {
        ServiceRegistry::Instance().OfferService(service_id_, instance_id_,
            "RadarService_" + std::to_string(instance_id_));
        std::cout << "  [Skeleton] OfferService(sid=0x"
                  << std::hex << service_id_ << ", iid=" << instance_id_
                  << std::dec << ")\n";
    }

    void StopOfferService() {
        ServiceRegistry::Instance().StopOfferService(service_id_, instance_id_);
    }

    // --- Method: Calibrate ---
    CalibResult Calibrate(uint8_t mode) {
        std::cout << "  [Skeleton] Calibrate(mode=" << static_cast<int>(mode) << ")\n";
        if (mode <= 3) return {true, "Calibration OK, mode=" + std::to_string(mode)};
        return {false, "Invalid calibration mode"};
    }

    // --- Event: ObjectDetected (send to all subscribers) ---
    void RegisterSubscriber(std::function<void(const RadarObject&)> cb) {
        object_subscribers_.push_back(std::move(cb));
    }

    void SendObjectEvent(const RadarObject& obj) {
        for (const auto& sub : object_subscribers_)
            sub(obj);
    }

    // --- Field: UpdateRate ---
    [[nodiscard]] uint32_t GetUpdateRate() const { return update_rate_hz_; }
    void SetUpdateRate(uint32_t hz) { update_rate_hz_ = hz; }
};

// -------------------------------------------------------------------------
// Radar Service — Proxy (Consumer)
// -------------------------------------------------------------------------

// EN: Proxy — service consumer (client stub)
class RadarServiceProxy {
    ServiceHandle handle_;
    RadarServiceSkeleton* skeleton_ = nullptr;  // direct binding for simulation

public:
    explicit RadarServiceProxy(const ServiceHandle& h) : handle_(h) {}

    // EN: In real AP, FindService returns Future<ServiceHandleContainer>
    static std::vector<ServiceHandle> FindService(uint16_t sid) {
        return ServiceRegistry::Instance().FindService(sid);
    }

    void BindToSkeleton(RadarServiceSkeleton* skel) { skeleton_ = skel; }

    // --- Method call ---
    Future<CalibResult> Calibrate(uint8_t mode) {
        Future<CalibResult> fut;
        if (skeleton_) {
            auto result = skeleton_->Calibrate(mode);
            fut.SetValue(result);
        }
        return fut;
    }

    // --- Event subscription ---
    void SubscribeObjectDetected(std::function<void(const RadarObject&)> cb) {
        if (skeleton_) skeleton_->RegisterSubscriber(std::move(cb));
    }

    // --- Field access ---
    Future<uint32_t> GetUpdateRate() {
        Future<uint32_t> fut;
        if (skeleton_) fut.SetValue(skeleton_->GetUpdateRate());
        return fut;
    }

    void SetUpdateRate(uint32_t hz) {
        if (skeleton_) skeleton_->SetUpdateRate(hz);
    }
};

}  // namespace ara::com

// ═════════════════════════════════════════════════════════════════════════════
// PART 2: ara::exec — Execution Management
// ═════════════════════════════════════════════════════════════════════════════

namespace ara::exec {

// EN: Application states in Adaptive Platform

enum class ExecutionState : uint8_t {
    kInit, kRunning, kTerminating, kTerminated
};

// EN: Execution Client — reports state to Execution Management
class ExecutionClient {
    std::string app_name_;
    ExecutionState state_ = ExecutionState::kInit;

public:
    explicit ExecutionClient(std::string name) : app_name_(std::move(name)) {}

    void ReportExecutionState(ExecutionState state) {
        state_ = state;
        std::cout << "  [ara::exec] " << app_name_ << " → "
                  << StateStr(state) << "\n";
    }

    [[nodiscard]] ExecutionState GetState() const { return state_; }

    static std::string_view StateStr(ExecutionState s) {
        switch (s) {
            case ExecutionState::kInit:        return "kInit";
            case ExecutionState::kRunning:     return "kRunning";
            case ExecutionState::kTerminating: return "kTerminating";
            case ExecutionState::kTerminated:  return "kTerminated";
        }
        return "Unknown";
    }
};

// EN: Deterministic Client — for deterministic execution (ADAS pipelines)
class DeterministicClient {
    uint64_t cycle_counter_ = 0;
    uint32_t cycle_time_us_ = 0;

public:
    explicit DeterministicClient(uint32_t cycle_us) : cycle_time_us_(cycle_us) {}

    // EN: WaitForNextActivation() blocks until next deterministic cycle
    uint64_t WaitForNextActivation() {
        ++cycle_counter_;
        // EN: In real AP, this synchronizes with OS timer
        return cycle_counter_;
    }

    [[nodiscard]] uint32_t GetCycleTimeUs() const { return cycle_time_us_; }
};

}  // namespace ara::exec

// ═════════════════════════════════════════════════════════════════════════════
// PART 3: ara::diag — Diagnostic Communication (UDS-over-DoIP)
// ═════════════════════════════════════════════════════════════════════════════

namespace ara::diag {

// EN: Diagnostic conversation — UDS session over DoIP

enum class SessionType : uint8_t {
    kDefault = 0x01, kProgramming = 0x02, kExtended = 0x03
};

enum class NRC : uint8_t {
    kPositiveResponse           = 0x00,
    kSubFunctionNotSupported    = 0x12,
    kIncorrectMessageLength     = 0x13,
    kConditionsNotCorrect       = 0x22,
    kRequestOutOfRange          = 0x31,
    kSecurityAccessDenied       = 0x33,
    kRequestSequenceError       = 0x24
};

struct DiagRequest {
    uint8_t              sid;
    std::vector<uint8_t> sub_data;
};

struct DiagResponse {
    uint8_t              sid;          // SID + 0x40 for positive
    NRC                  nrc;
    std::vector<uint8_t> data;
};

class DiagServer {
    SessionType session_ = SessionType::kDefault;
    bool security_unlocked_ = false;

    // EN: DID storage (simplified)
    std::map<uint16_t, std::vector<uint8_t>> did_data_;

public:
    DiagServer() {
        // EN: Pre-populate some DIDs
        did_data_[0xF190] = {'V', 'I', 'N', '1', '2', '3', '4', '5',
                             '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E'};
        did_data_[0xF187] = {'S', 'W', '_', 'v', '2', '.', '1'};
        did_data_[0x0100] = {0x03, 0xE8};  // engine RPM
    }

    DiagResponse HandleRequest(const DiagRequest& req) {
        DiagResponse resp{};
        switch (req.sid) {
            case 0x10: return HandleSessionControl(req);
            case 0x22: return HandleReadDID(req);
            case 0x27: return HandleSecurityAccess(req);
            case 0x3E: return HandleTesterPresent(req);
            default:
                resp.sid = 0x7F;
                resp.nrc = NRC::kSubFunctionNotSupported;
                return resp;
        }
    }

private:
    DiagResponse HandleSessionControl(const DiagRequest& req) {
        DiagResponse resp{};
        if (req.sub_data.empty()) {
            resp.sid = 0x7F; resp.nrc = NRC::kIncorrectMessageLength;
            return resp;
        }
        session_ = static_cast<SessionType>(req.sub_data[0]);
        resp.sid = 0x50;  // positive
        resp.nrc = NRC::kPositiveResponse;
        resp.data = {req.sub_data[0]};
        std::cout << "  [ara::diag] Session → 0x"
                  << std::hex << static_cast<int>(req.sub_data[0]) << std::dec << "\n";
        return resp;
    }

    DiagResponse HandleReadDID(const DiagRequest& req) {
        DiagResponse resp{};
        if (req.sub_data.size() < 2) {
            resp.sid = 0x7F; resp.nrc = NRC::kIncorrectMessageLength;
            return resp;
        }
        uint16_t did = static_cast<uint16_t>(
            (static_cast<uint16_t>(req.sub_data[0]) << 8) | req.sub_data[1]);
        auto it = did_data_.find(did);
        if (it == did_data_.end()) {
            resp.sid = 0x7F; resp.nrc = NRC::kRequestOutOfRange;
            return resp;
        }
        resp.sid = 0x62;  // positive ReadDID
        resp.nrc = NRC::kPositiveResponse;
        resp.data = {req.sub_data[0], req.sub_data[1]};
        resp.data.insert(resp.data.end(), it->second.begin(), it->second.end());
        return resp;
    }

    DiagResponse HandleSecurityAccess(const DiagRequest& req) {
        DiagResponse resp{};
        if (req.sub_data.empty()) {
            resp.sid = 0x7F; resp.nrc = NRC::kIncorrectMessageLength;
            return resp;
        }
        if (req.sub_data[0] == 0x01) {
            // EN: Seed request
            resp.sid = 0x67;
            resp.nrc = NRC::kPositiveResponse;
            resp.data = {0x01, 0xDE, 0xAD, 0xBE, 0xEF};  // seed
        } else if (req.sub_data[0] == 0x02) {
            // EN: Key response — accept any key for demo
            security_unlocked_ = true;
            resp.sid = 0x67;
            resp.nrc = NRC::kPositiveResponse;
            resp.data = {0x02};
        } else {
            resp.sid = 0x7F;
            resp.nrc = NRC::kSubFunctionNotSupported;
        }
        return resp;
    }

    DiagResponse HandleTesterPresent(const DiagRequest& /*req*/) {
        return {0x7E, NRC::kPositiveResponse, {}};
    }
};

}  // namespace ara::diag

// ═════════════════════════════════════════════════════════════════════════════
// PART 4: ara::per — Persistency (Key-Value Store)
// ═════════════════════════════════════════════════════════════════════════════

namespace ara::per {

// EN: Key-Value Storage — simplified ara::per::KeyValueStorage

class KeyValueStorage {
    std::string storage_id_;
    std::map<std::string, std::vector<uint8_t>> store_;

public:
    explicit KeyValueStorage(std::string id) : storage_id_(std::move(id)) {}

    bool SetValue(const std::string& key, const std::vector<uint8_t>& value) {
        store_[key] = value;
        return true;
    }

    std::optional<std::vector<uint8_t>> GetValue(const std::string& key) const {
        auto it = store_.find(key);
        if (it == store_.end()) return std::nullopt;
        return it->second;
    }

    bool RemoveKey(const std::string& key) {
        return store_.erase(key) > 0;
    }

    // EN: Persist data (in real AP: write to file system)
    bool SyncToStorage() {
        std::cout << "  [ara::per] Syncing '" << storage_id_
                  << "' (" << store_.size() << " keys) to NV\n";
        return true;
    }

    void PrintContents() const {
        std::cout << "  [ara::per] Storage '" << storage_id_ << "':\n";
        for (const auto& [key, val] : store_) {
            std::cout << "    " << key << " = [";
            for (size_t i = 0; i < val.size(); ++i) {
                if (i > 0) std::cout << ",";
                std::cout << "0x" << std::hex << static_cast<int>(val[i]);
            }
            std::cout << std::dec << "]\n";
        }
    }
};

}  // namespace ara::per

// ═════════════════════════════════════════════════════════════════════════════
// PART 5: ara::log — Logging (DLT Compatible)
// ═════════════════════════════════════════════════════════════════════════════

namespace ara::log {

enum class LogLevel : uint8_t {
    kFatal = 1, kError, kWarn, kInfo, kDebug, kVerbose
};

// EN: Logger — DLT-compatible logging (app_id + context_id)
class Logger {
    std::string app_id_;
    std::string ctx_id_;
    LogLevel    threshold_ = LogLevel::kInfo;

public:
    Logger(std::string app, std::string ctx)
        : app_id_(std::move(app)), ctx_id_(std::move(ctx)) {}

    void SetThreshold(LogLevel lvl) { threshold_ = lvl; }

    void Log(LogLevel lvl, std::string_view msg) const {
        if (static_cast<uint8_t>(lvl) > static_cast<uint8_t>(threshold_)) return;
        std::cout << "  [DLT:" << app_id_ << "/" << ctx_id_ << "] "
                  << LevelStr(lvl) << ": " << msg << "\n";
    }

    void Fatal(std::string_view msg) const { Log(LogLevel::kFatal, msg); }
    void Error(std::string_view msg) const { Log(LogLevel::kError, msg); }
    void Warn(std::string_view msg)  const { Log(LogLevel::kWarn, msg); }
    void Info(std::string_view msg)  const { Log(LogLevel::kInfo, msg); }
    void Debug(std::string_view msg) const { Log(LogLevel::kDebug, msg); }

private:
    static std::string_view LevelStr(LogLevel l) {
        switch (l) {
            case LogLevel::kFatal:   return "FATAL";
            case LogLevel::kError:   return "ERROR";
            case LogLevel::kWarn:    return " WARN";
            case LogLevel::kInfo:    return " INFO";
            case LogLevel::kDebug:   return "DEBUG";
            case LogLevel::kVerbose: return " VERB";
        }
        return "?????";
    }
};

}  // namespace ara::log

// ═════════════════════════════════════════════════════════════════════════════
// PART 6: ara::phm — Platform Health Management
// ═════════════════════════════════════════════════════════════════════════════

namespace ara::phm {

// EN: Supervision modes: Alive, Deadline, Logical

enum class SupervisionResult : uint8_t {
    kCorrect, kFailed, kExpired
};

struct AliveSupervision {
    std::string checkpoint_name;
    uint32_t    min_margin_ms = 0;
    uint32_t    max_margin_ms = 0;
    uint32_t    expected_alive_indications = 0;
    uint32_t    actual_indications = 0;
    uint32_t    failed_count = 0;
};

class HealthManager {
    std::vector<AliveSupervision> supervisions_;

public:
    void AddAliveSupervision(const std::string& name, uint32_t expected,
                             uint32_t min_ms, uint32_t max_ms) {
        supervisions_.push_back({name, min_ms, max_ms, expected, 0, 0});
    }

    void ReportAlive(const std::string& checkpoint) {
        for (auto& s : supervisions_) {
            if (s.checkpoint_name == checkpoint) {
                ++s.actual_indications;
                return;
            }
        }
    }

    // EN: Called periodically to evaluate supervision
    void Evaluate() {
        for (auto& s : supervisions_) {
            if (s.actual_indications < s.expected_alive_indications) {
                ++s.failed_count;
                std::cout << "  [ara::phm] " << s.checkpoint_name
                          << " FAILED (got " << s.actual_indications
                          << ", expected " << s.expected_alive_indications << ")\n";
            } else {
                std::cout << "  [ara::phm] " << s.checkpoint_name << " OK\n";
            }
            s.actual_indications = 0;  // reset for next cycle
        }
    }

    void PrintStatus() const {
        for (const auto& s : supervisions_) {
            std::cout << "    " << s.checkpoint_name
                      << " — failures=" << s.failed_count
                      << " expected=" << s.expected_alive_indications << "/cycle\n";
        }
    }
};

}  // namespace ara::phm

// ═════════════════════════════════════════════════════════════════════════════
// MAIN — Demos
// ═════════════════════════════════════════════════════════════════════════════

int main() {
    std::cout << "╔══════════════════════════════════════════════════════════════╗\n"
              << "║ Module 16: AUTOSAR Adaptive Platform — ara::* Simulation     ║\n"
              << "╚══════════════════════════════════════════════════════════════╝\n\n";

    // --- Demo 1: ara::com — Proxy/Skeleton Pattern ---
    std::cout << "--- Demo 1: ara::com — Proxy/Skeleton (Radar Service) ---\n";
    {
        ara::com::ServiceRegistry::Instance().Clear();

        // EN: Provider side — create skeleton and offer service
        ara::com::RadarServiceSkeleton skeleton(0x1234, 1);
        skeleton.OfferService();

        // EN: Consumer side — find service and create proxy
        auto handles = ara::com::RadarServiceProxy::FindService(0x1234);
        assert(!handles.empty());
        std::cout << "  [Proxy] Found " << handles.size() << " instance(s)\n";

        ara::com::RadarServiceProxy proxy(handles[0]);
        proxy.BindToSkeleton(&skeleton);  // simulation binding

        // --- Method call ---
        auto calib_result = proxy.Calibrate(2);
        if (calib_result.is_ready()) {
            auto cr = calib_result.get();
            std::cout << "  [Proxy] Calibrate result: "
                      << (cr.success ? "OK" : "FAIL") << " — " << cr.message << "\n";
        }

        // --- Event ---
        proxy.SubscribeObjectDetected([](const ara::com::RadarObject& obj) {
            std::cout << "  [Proxy] Object detected: id=" << obj.object_id
                      << " range=" << obj.range_m << "m"
                      << " angle=" << obj.angle_deg << "deg"
                      << " vel=" << obj.velocity_mps << "m/s\n";
        });
        skeleton.SendObjectEvent({25.5f, -3.2f, 12.0f, 42});
        skeleton.SendObjectEvent({8.1f, 15.7f, -5.0f, 43});

        // --- Field ---
        auto rate = proxy.GetUpdateRate();
        std::cout << "  [Proxy] UpdateRate = " << rate.get() << " Hz\n";
        proxy.SetUpdateRate(50);
        auto rate2 = proxy.GetUpdateRate();
        std::cout << "  [Proxy] UpdateRate (after set) = " << rate2.get() << " Hz\n";
    }

    // --- Demo 2: ara::exec — Execution State Management ---
    std::cout << "\n--- Demo 2: ara::exec — Execution Client ---\n";
    {
        ara::exec::ExecutionClient app("RadarApp");
        app.ReportExecutionState(ara::exec::ExecutionState::kRunning);

        ara::exec::DeterministicClient det(20000);  // 20ms cycle
        for (int i = 0; i < 5; ++i) {
            auto cycle = det.WaitForNextActivation();
            std::cout << "  [DeterministicClient] Cycle " << cycle
                      << " (every " << det.GetCycleTimeUs() << " us)\n";
        }

        app.ReportExecutionState(ara::exec::ExecutionState::kTerminating);
        app.ReportExecutionState(ara::exec::ExecutionState::kTerminated);
    }

    // --- Demo 3: ara::diag — UDS over DoIP ---
    std::cout << "\n--- Demo 3: ara::diag — UDS Diagnostic Server ---\n";
    {
        ara::diag::DiagServer server;

        // EN: Switch to extended session
        auto resp = server.HandleRequest({0x10, {0x03}});
        std::cout << "  DiagSession resp SID=0x" << std::hex
                  << static_cast<int>(resp.sid) << std::dec << "\n";

        // EN: Read VIN (DID 0xF190)
        resp = server.HandleRequest({0x22, {0xF1, 0x90}});
        std::cout << "  ReadDID(F190) SID=0x" << std::hex
                  << static_cast<int>(resp.sid) << std::dec << " data='";
        for (size_t i = 2; i < resp.data.size(); ++i)
            std::cout << static_cast<char>(resp.data[i]);
        std::cout << "'\n";

        // EN: Security Access — seed request
        resp = server.HandleRequest({0x27, {0x01}});
        std::cout << "  SecurityAccess seed: ";
        for (auto b : resp.data)
            std::cout << "0x" << std::hex << static_cast<int>(b) << " ";
        std::cout << std::dec << "\n";

        // EN: Tester Present
        resp = server.HandleRequest({0x3E, {0x00}});
        std::cout << "  TesterPresent resp SID=0x" << std::hex
                  << static_cast<int>(resp.sid) << std::dec << "\n";
    }

    // --- Demo 4: ara::per — Persistency ---
    std::cout << "\n--- Demo 4: ara::per — Key-Value Storage ---\n";
    {
        ara::per::KeyValueStorage store("radar_config");
        store.SetValue("detection_threshold", {0x00, 0x64});  // 100
        store.SetValue("max_range_m", {0x01, 0xF4});          // 500
        store.SetValue("firmware_ver", {'2', '.', '0', '.', '1'});

        auto val = store.GetValue("detection_threshold");
        if (val) {
            std::cout << "  detection_threshold = ";
            for (auto b : *val) std::cout << std::hex << static_cast<int>(b) << " ";
            std::cout << std::dec << "\n";
        }
        store.PrintContents();
        store.SyncToStorage();
    }

    // --- Demo 5: ara::log — DLT Logging ---
    std::cout << "\n--- Demo 5: ara::log — DLT-Compatible Logging ---\n";
    {
        ara::log::Logger logger("RADR", "MAIN");
        logger.SetThreshold(ara::log::LogLevel::kDebug);

        logger.Info("Radar application starting");
        logger.Debug("Initializing antenna array");
        logger.Warn("Signal strength below threshold");
        logger.Error("Lost tracking on object #42");
        logger.Fatal("Hardware fault detected on channel 2");
    }

    // --- Demo 6: ara::phm — Health Management ---
    std::cout << "\n--- Demo 6: ara::phm — Platform Health Management ---\n";
    {
        ara::phm::HealthManager hm;
        hm.AddAliveSupervision("RadarCycle", 3, 0, 25);
        hm.AddAliveSupervision("FusionCycle", 2, 0, 50);

        // EN: Cycle 1 — radar OK, fusion missed
        hm.ReportAlive("RadarCycle");
        hm.ReportAlive("RadarCycle");
        hm.ReportAlive("RadarCycle");
        hm.ReportAlive("FusionCycle");  // only 1 of 2
        std::cout << "  === Evaluate Cycle 1 ===\n";
        hm.Evaluate();

        // EN: Cycle 2 — both OK
        for (int i = 0; i < 3; ++i) hm.ReportAlive("RadarCycle");
        for (int i = 0; i < 2; ++i) hm.ReportAlive("FusionCycle");
        std::cout << "  === Evaluate Cycle 2 ===\n";
        hm.Evaluate();

        std::cout << "  --- Final Status ---\n";
        hm.PrintStatus();
    }

    // --- Demo 7: Manifest Summary ---
    std::cout << "\n--- Demo 7: Adaptive Platform Manifest Types ---\n";
    {
        std::cout << "  ┌─────────────────────────────┬─────────────────────────────────────┐\n";
        std::cout << "  │ Manifest                    │ Content                             │\n";
        std::cout << "  ├─────────────────────────────┼─────────────────────────────────────┤\n";
        std::cout << "  │ Execution Manifest          │ Processes, startup config, deps     │\n";
        std::cout << "  │ Service Instance Manifest   │ Port bindings (SOME/IP, DDS)        │\n";
        std::cout << "  │ Machine Manifest            │ Network, HW capabilities, services  │\n";
        std::cout << "  │ Diagnostic Extract          │ DID/DTC definitions, sessions       │\n";
        std::cout << "  │ Persistency Deployment      │ Key-value/file storage layout       │\n";
        std::cout << "  └─────────────────────────────┴─────────────────────────────────────┘\n";
    }

    std::cout << "\n[All demos completed successfully.]\n";
    return 0;
}
