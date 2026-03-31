/**
 * @file module_14_automotive_networks/06_uds_diagnostics.cpp
 * @brief UDS — Unified Diagnostic Services (ISO 14229) & ISO-TP (ISO 15765)
 *
 * @details
 * =============================================================================
 * =============================================================================
 *
 * EN: UDS (ISO 14229) is the diagnostic protocol used in all modern vehicles.
 *     Diagnostic testers, OBD tools, and factory tools use UDS to:
 *     - Read/clear DTCs (Diagnostic Trouble Codes)
 *     - Flash ECU firmware (reprogramming)
 *     - Read/write ECU data (VIN, calibration, configuration)
 *     - Control actuators (test mode)
 *     - Security access (seed-key authentication)
 *
 *     UDS Service Catalog (key SIDs):
 *     ┌──────┬──────────────────────────────┬──────────────────────┐
 *     │ SID  │ Service Name                 │ Use Case             │
 *     ├──────┼──────────────────────────────┼──────────────────────┤
 *     │ 0x10 │ DiagnosticSessionControl     │ Switch session mode  │
 *     │ 0x11 │ ECUReset                     │ Hard/soft reset ECU  │
 *     │ 0x14 │ ClearDiagnosticInformation   │ Clear DTCs           │
 *     │ 0x19 │ ReadDTCInformation           │ Read fault codes     │
 *     │ 0x22 │ ReadDataByIdentifier         │ Read DID (VIN, etc.) │
 *     │ 0x23 │ ReadMemoryByAddress          │ Direct memory read   │
 *     │ 0x27 │ SecurityAccess               │ Seed-key unlock      │
 *     │ 0x2E │ WriteDataByIdentifier        │ Write DID            │
 *     │ 0x2F │ InputOutputControlByID       │ Actuator control     │
 *     │ 0x31 │ RoutineControl               │ Run diagnostics      │
 *     │ 0x34 │ RequestDownload              │ Start flash session  │
 *     │ 0x36 │ TransferData                 │ Send flash data      │
 *     │ 0x37 │ RequestTransferExit          │ End flash session    │
 *     │ 0x3E │ TesterPresent                │ Keep session alive   │
 *     │ 0x85 │ ControlDTCSetting            │ Enable/Disable DTCs  │
 *     └──────┴──────────────────────────────┴──────────────────────┘
 *
 *     UDS Sessions:
 *     - 0x01: Default Session (limited services)
 *     - 0x02: Programming Session (flash/download)
 *     - 0x03: Extended Diagnostic Session (full access)
 *
 *     Negative Response Codes (NRC):
 *     ┌──────┬──────────────────────────────────────────────────────┐
 *     │ NRC  │ Meaning                                              │
 *     ├──────┼──────────────────────────────────────────────────────┤
 *     │ 0x10 │ General Reject                                       │
 *     │ 0x11 │ Service Not Supported                                │
 *     │ 0x12 │ Sub-Function Not Supported                           │
 *     │ 0x13 │ Incorrect Message Length                             │
 *     │ 0x14 │ Response Too Long                                    │
 *     │ 0x22 │ Conditions Not Correct                               │
 *     │ 0x24 │ Request Sequence Error                               │
 *     │ 0x25 │ No Response From Sub-Net Component                   │
 *     │ 0x31 │ Request Out Of Range                                 │
 *     │ 0x33 │ Security Access Denied                               │
 *     │ 0x35 │ Invalid Key                                          │
 *     │ 0x36 │ Exceeded Number Of Attempts                          │
 *     │ 0x72 │ General Programming Failure                          │
 *     │ 0x78 │ Request Correctly Received, Response Pending         │
 *     └──────┴──────────────────────────────────────────────────────┘
 *
 * =============================================================================
 * =============================================================================
 *
 * EN: UDS messages can exceed 8 bytes (CAN) or 64 bytes (CAN FD). ISO-TP
 *     handles segmentation into multiple CAN frames:
 *
 *     Frame Types:
 *     - SF (Single Frame):  ≤7 bytes, fits in one CAN frame
 *     - FF (First Frame):   First segment of multi-frame message (total length)
 *     - CF (Consecutive):   Subsequent segments (sequence number 0-F)
 *     - FC (Flow Control):  Receiver tells sender: block size + timing
 *
 */

#include <iostream>
#include <cstdint>
#include <vector>
#include <string>
#include <iomanip>
#include <map>
#include <algorithm>
#include <cassert>
#include <functional>
#include <array>
#include <sstream>

// ═════════════════════════════════════════════════════════════════════════════
// PART 1: UDS Core — SID, NRC, Sessions
// ═════════════════════════════════════════════════════════════════════════════

enum class UDSSession : uint8_t {
    Default     = 0x01,
    Programming = 0x02,
    Extended    = 0x03,
};

// EN: UDS Service IDs
namespace SID {
    constexpr uint8_t DiagnosticSessionControl = 0x10;
    constexpr uint8_t ECUReset                 = 0x11;
    constexpr uint8_t ClearDTC                 = 0x14;
    constexpr uint8_t ReadDTCInformation       = 0x19;
    constexpr uint8_t ReadDataByIdentifier     = 0x22;
    constexpr uint8_t ReadMemoryByAddress      = 0x23;
    constexpr uint8_t SecurityAccess           = 0x27;
    constexpr uint8_t WriteDataByIdentifier    = 0x2E;
    constexpr uint8_t IOControl                = 0x2F;
    constexpr uint8_t RoutineControl           = 0x31;
    constexpr uint8_t RequestDownload          = 0x34;
    constexpr uint8_t TransferData             = 0x36;
    constexpr uint8_t RequestTransferExit      = 0x37;
    constexpr uint8_t TesterPresent            = 0x3E;
    constexpr uint8_t ControlDTCSetting        = 0x85;
    constexpr uint8_t NegativeResponse         = 0x7F;
    constexpr uint8_t PositiveOffset           = 0x40; // positive response = SID + 0x40
}

// EN: Negative Response Codes
namespace NRC {
    constexpr uint8_t GeneralReject           = 0x10;
    constexpr uint8_t ServiceNotSupported     = 0x11;
    constexpr uint8_t SubFunctionNotSupported = 0x12;
    constexpr uint8_t IncorrectMessageLength  = 0x13;
    constexpr uint8_t ConditionsNotCorrect    = 0x22;
    constexpr uint8_t RequestSequenceError    = 0x24;
    constexpr uint8_t RequestOutOfRange       = 0x31;
    constexpr uint8_t SecurityAccessDenied    = 0x33;
    constexpr uint8_t InvalidKey              = 0x35;
    constexpr uint8_t ExceededAttempts        = 0x36;
    constexpr uint8_t ResponsePending         = 0x78;
}

std::string nrc_to_string(uint8_t nrc) {
    switch (nrc) {
        case NRC::GeneralReject:           return "General Reject";
        case NRC::ServiceNotSupported:     return "Service Not Supported";
        case NRC::SubFunctionNotSupported: return "Sub-Function Not Supported";
        case NRC::IncorrectMessageLength:  return "Incorrect Message Length";
        case NRC::ConditionsNotCorrect:    return "Conditions Not Correct";
        case NRC::RequestSequenceError:    return "Request Sequence Error";
        case NRC::RequestOutOfRange:       return "Request Out Of Range";
        case NRC::SecurityAccessDenied:    return "Security Access Denied";
        case NRC::InvalidKey:              return "Invalid Key";
        case NRC::ExceededAttempts:        return "Exceeded Attempts";
        case NRC::ResponsePending:         return "Response Pending";
        default: return "Unknown NRC (0x" + 
            ([](uint8_t v) { 
                std::ostringstream o; 
                o << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(v); 
                return o.str();
            })(nrc) + ")";
    }
}

std::string sid_to_string(uint8_t sid) {
    switch (sid) {
        case SID::DiagnosticSessionControl: return "DiagnosticSessionControl";
        case SID::ECUReset:                 return "ECUReset";
        case SID::ClearDTC:                 return "ClearDTC";
        case SID::ReadDTCInformation:       return "ReadDTCInformation";
        case SID::ReadDataByIdentifier:     return "ReadDataByIdentifier";
        case SID::SecurityAccess:           return "SecurityAccess";
        case SID::WriteDataByIdentifier:    return "WriteDataByIdentifier";
        case SID::IOControl:                return "IOControl";
        case SID::RoutineControl:           return "RoutineControl";
        case SID::RequestDownload:          return "RequestDownload";
        case SID::TransferData:             return "TransferData";
        case SID::RequestTransferExit:      return "RequestTransferExit";
        case SID::TesterPresent:            return "TesterPresent";
        case SID::ControlDTCSetting:        return "ControlDTCSetting";
        default: return "Unknown";
    }
}

// ═════════════════════════════════════════════════════════════════════════════
// PART 2: UDS ECU Server Simulation
// ═════════════════════════════════════════════════════════════════════════════

struct DTC {
    uint32_t code;        // 3-byte DTC (e.g., 0x012345)
    uint8_t  status;      // DTC status mask
    std::string description;
};

class UDSServer {
    std::string name_;
    UDSSession session_ = UDSSession::Default;
    bool security_unlocked_ = false;
    uint16_t seed_ = 0;

    // EN: Data Identifiers (DIDs)
    std::map<uint16_t, std::vector<uint8_t>> dids_;
    // EN: DTC storage
    std::vector<DTC> dtc_list_;

    // EN: Build negative response
    static std::vector<uint8_t> negative(uint8_t sid, uint8_t nrc) {
        return {SID::NegativeResponse, sid, nrc};
    }

    // EN: Build positive response
    static std::vector<uint8_t> positive(uint8_t sid, const std::vector<uint8_t>& data = {}) {
        std::vector<uint8_t> resp;
        resp.push_back(static_cast<uint8_t>(sid + SID::PositiveOffset));
        resp.insert(resp.end(), data.begin(), data.end());
        return resp;
    }

public:
    UDSServer(std::string name) : name_(std::move(name)) {
        // EN: Pre-populate DIDs
        // DID 0xF190 = VIN
        std::string vin = "WVWZZZ3CZWE000001";
        dids_[0xF190] = std::vector<uint8_t>(vin.begin(), vin.end());
        // DID 0xF187 = Part Number
        std::string part = "8V0907115A";
        dids_[0xF187] = std::vector<uint8_t>(part.begin(), part.end());
        // DID 0xF191 = ECU Hardware Version
        dids_[0xF191] = {0x01, 0x03};
        // DID 0xF195 = ECU Software Version
        dids_[0xF195] = {0x02, 0x01, 0x00};
    }

    void add_dtc(uint32_t code, uint8_t status, const std::string& desc) {
        dtc_list_.push_back({code, status, desc});
    }

    std::vector<uint8_t> handle_request(const std::vector<uint8_t>& request) {
        if (request.empty()) return negative(0x00, NRC::IncorrectMessageLength);

        uint8_t sid = request[0];

        switch (sid) {
            case SID::DiagnosticSessionControl: {
                if (request.size() < 2) return negative(sid, NRC::IncorrectMessageLength);
                uint8_t sub = request[1];
                if (sub >= 0x01 && sub <= 0x03) {
                    session_ = static_cast<UDSSession>(sub);
                    security_unlocked_ = false;
                    return positive(sid, {sub});
                }
                return negative(sid, NRC::SubFunctionNotSupported);
            }

            case SID::TesterPresent: {
                return positive(sid, {0x00});
            }

            case SID::ReadDataByIdentifier: {
                if (request.size() < 3) return negative(sid, NRC::IncorrectMessageLength);
                uint16_t did = static_cast<uint16_t>((static_cast<uint16_t>(request[1]) << 8) | request[2]);
                auto it = dids_.find(did);
                if (it == dids_.end()) return negative(sid, NRC::RequestOutOfRange);
                std::vector<uint8_t> data = {request[1], request[2]};
                data.insert(data.end(), it->second.begin(), it->second.end());
                return positive(sid, data);
            }

            case SID::WriteDataByIdentifier: {
                if (session_ != UDSSession::Extended)
                    return negative(sid, NRC::ConditionsNotCorrect);
                if (!security_unlocked_)
                    return negative(sid, NRC::SecurityAccessDenied);
                if (request.size() < 4) return negative(sid, NRC::IncorrectMessageLength);
                uint16_t did = static_cast<uint16_t>((static_cast<uint16_t>(request[1]) << 8) | request[2]);
                dids_[did] = std::vector<uint8_t>(request.begin() + 3, request.end());
                return positive(sid, {request[1], request[2]});
            }

            case SID::SecurityAccess: {
                if (request.size() < 2) return negative(sid, NRC::IncorrectMessageLength);
                uint8_t sub = request[1];
                if (sub == 0x01) {
                    // EN: Request Seed
                    seed_ = 0xA5B7;  // simple fixed seed for demo
                    return positive(sid, {0x01,
                        static_cast<uint8_t>(seed_ >> 8),
                        static_cast<uint8_t>(seed_ & 0xFF)});
                }
                if (sub == 0x02) {
                    // EN: Send Key (expected: seed XOR 0x1234)
                    if (request.size() < 4) return negative(sid, NRC::IncorrectMessageLength);
                    uint16_t key = static_cast<uint16_t>((static_cast<uint16_t>(request[2]) << 8) | request[3]);
                    uint16_t expected = seed_ ^ 0x1234;
                    if (key == expected) {
                        security_unlocked_ = true;
                        return positive(sid, {0x02});
                    }
                    return negative(sid, NRC::InvalidKey);
                }
                return negative(sid, NRC::SubFunctionNotSupported);
            }

            case SID::ReadDTCInformation: {
                if (request.size() < 2) return negative(sid, NRC::IncorrectMessageLength);
                uint8_t sub = request[1];
                if (sub == 0x01) {
                    // EN: Report number of DTCs by status mask
                    uint8_t count = static_cast<uint8_t>(dtc_list_.size());
                    return positive(sid, {0x01, 0xFF, 0x00, count});
                }
                if (sub == 0x02) {
                    // EN: Report DTC by status mask
                    std::vector<uint8_t> data = {0x02, 0xFF};
                    for (auto& dtc : dtc_list_) {
                        data.push_back(static_cast<uint8_t>((dtc.code >> 16) & 0xFF));
                        data.push_back(static_cast<uint8_t>((dtc.code >> 8) & 0xFF));
                        data.push_back(static_cast<uint8_t>(dtc.code & 0xFF));
                        data.push_back(dtc.status);
                    }
                    return positive(sid, data);
                }
                return negative(sid, NRC::SubFunctionNotSupported);
            }

            case SID::ClearDTC: {
                dtc_list_.clear();
                return positive(sid);
            }

            case SID::ECUReset: {
                if (request.size() < 2) return negative(sid, NRC::IncorrectMessageLength);
                session_ = UDSSession::Default;
                security_unlocked_ = false;
                return positive(sid, {request[1]});
            }

            default:
                return negative(sid, NRC::ServiceNotSupported);
        }
    }

    const std::string& name() const { return name_; }
};

// ═════════════════════════════════════════════════════════════════════════════
// PART 3: ISO-TP Segmentation (ISO 15765-2)
// ═════════════════════════════════════════════════════════════════════════════

enum class ISOTPFrameType : uint8_t {
    SingleFrame     = 0x0,
    FirstFrame      = 0x1,
    ConsecutiveFrame = 0x2,
    FlowControl     = 0x3,
};

struct ISOTPFrame {
    ISOTPFrameType type;
    uint8_t        sequence_number = 0;   // for CF
    uint16_t       total_length = 0;      // for FF
    uint8_t        block_size = 0;        // for FC
    uint8_t        st_min = 0;            // for FC (separation time min, ms)
    std::vector<uint8_t> data;

    void print(const std::string& label) const {
        std::cout << "    " << label << " [";
        switch (type) {
            case ISOTPFrameType::SingleFrame:     std::cout << "SF"; break;
            case ISOTPFrameType::FirstFrame:      std::cout << "FF"; break;
            case ISOTPFrameType::ConsecutiveFrame: std::cout << "CF#" << static_cast<int>(sequence_number); break;
            case ISOTPFrameType::FlowControl:     std::cout << "FC"; break;
        }
        std::cout << "] ";
        if (type == ISOTPFrameType::FirstFrame)
            std::cout << "TotalLen=" << total_length << " ";
        if (type == ISOTPFrameType::FlowControl)
            std::cout << "BS=" << static_cast<int>(block_size) << " STmin=" << static_cast<int>(st_min) << "ms ";
        std::cout << "Data:";
        for (auto b : data)
            std::cout << " " << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(b);
        std::cout << std::dec << "\n";
    }
};

class ISOTPCodec {
public:
    // EN: Segment a message into ISO-TP frames (CAN: max 8 bytes per frame)
    static std::vector<ISOTPFrame> segment(const std::vector<uint8_t>& message) {
        std::vector<ISOTPFrame> frames;

        if (message.size() <= 7) {
            // EN: Single Frame
            ISOTPFrame sf;
            sf.type = ISOTPFrameType::SingleFrame;
            sf.data.push_back(static_cast<uint8_t>(message.size()));  // PCI byte: 0x0N
            sf.data.insert(sf.data.end(), message.begin(), message.end());
            // Pad to 8 bytes
            while (sf.data.size() < 8) sf.data.push_back(0xCC);
            frames.push_back(sf);
        } else {
            // EN: First Frame
            ISOTPFrame ff;
            ff.type = ISOTPFrameType::FirstFrame;
            ff.total_length = static_cast<uint16_t>(message.size());
            // PCI: 0x1NNN (N = total length, 12-bit)
            ff.data.push_back(static_cast<uint8_t>(0x10 | ((message.size() >> 8) & 0x0F)));
            ff.data.push_back(static_cast<uint8_t>(message.size() & 0xFF));
            size_t offset = 0;
            while (ff.data.size() < 8 && offset < message.size()) {
                ff.data.push_back(message[offset++]);
            }
            frames.push_back(ff);

            // EN: Consecutive Frames
            uint8_t sn = 1;
            while (offset < message.size()) {
                ISOTPFrame cf;
                cf.type = ISOTPFrameType::ConsecutiveFrame;
                cf.sequence_number = sn;
                cf.data.push_back(static_cast<uint8_t>(0x20 | (sn & 0x0F)));
                while (cf.data.size() < 8 && offset < message.size()) {
                    cf.data.push_back(message[offset++]);
                }
                while (cf.data.size() < 8) cf.data.push_back(0xCC);
                frames.push_back(cf);
                sn = (sn + 1) & 0x0F;
            }
        }
        return frames;
    }

    // EN: Reassemble ISO-TP frames back into a message
    static std::vector<uint8_t> reassemble(const std::vector<ISOTPFrame>& frames) {
        if (frames.empty()) return {};

        auto& first = frames[0];
        if (first.type == ISOTPFrameType::SingleFrame) {
            uint8_t len = first.data[0] & 0x0F;
            return std::vector<uint8_t>(first.data.begin() + 1, first.data.begin() + 1 + len);
        }

        // First Frame
        uint16_t total = static_cast<uint16_t>(
            ((static_cast<uint16_t>(first.data[0]) & 0x0F) << 8) | first.data[1]);
        std::vector<uint8_t> result;
        // Data from FF (bytes 2-7)
        for (size_t i = 2; i < first.data.size() && result.size() < total; ++i)
            result.push_back(first.data[i]);

        // Consecutive Frames
        for (size_t f = 1; f < frames.size() && result.size() < total; ++f) {
            auto& cf = frames[f];
            if (cf.type != ISOTPFrameType::ConsecutiveFrame) continue;
            for (size_t i = 1; i < cf.data.size() && result.size() < total; ++i)
                result.push_back(cf.data[i]);
        }
        return result;
    }
};

// ═════════════════════════════════════════════════════════════════════════════
// Helper: print UDS request/response
// ═════════════════════════════════════════════════════════════════════════════
void print_uds(const std::string& dir, const std::vector<uint8_t>& msg) {
    std::cout << "    " << dir << ": [";
    for (size_t i = 0; i < msg.size(); ++i) {
        if (i > 0) std::cout << " ";
        std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(msg[i]);
    }
    std::cout << std::dec << "]";

    if (!msg.empty()) {
        if (msg[0] == SID::NegativeResponse && msg.size() >= 3) {
            std::cout << " → NRC: " << nrc_to_string(msg[2]);
        } else if (msg[0] >= 0x50) {
            std::cout << " → Positive: " << sid_to_string(static_cast<uint8_t>(msg[0] - SID::PositiveOffset));
        }
    }
    std::cout << "\n";
}

// ═════════════════════════════════════════════════════════════════════════════
// MAIN
// ═════════════════════════════════════════════════════════════════════════════
int main() {
    std::cout << "============================================\n";
    std::cout << "   Module 14 - UDS Diagnostics & ISO-TP\n";
    std::cout << "============================================\n\n";

    UDSServer ecu("EngineECU");
    ecu.add_dtc(0x012345, 0x2F, "P0123 - Throttle Position Sensor A - High");
    ecu.add_dtc(0x050067, 0x01, "P0500 - Vehicle Speed Sensor A");
    ecu.add_dtc(0x030089, 0x08, "P0300 - Random Misfire Detected");

    // ─── Demo 1: Session Control ────────────────────────────────────────
    {
        std::cout << "--- Demo 1: DiagnosticSessionControl ---\n";

        // EN: Switch to Extended Session
        auto req = std::vector<uint8_t>{SID::DiagnosticSessionControl, 0x03};
        print_uds("REQ", req);
        auto resp = ecu.handle_request(req);
        print_uds("RSP", resp);

        // EN: Invalid session
        req = {SID::DiagnosticSessionControl, 0x99};
        print_uds("REQ", req);
        resp = ecu.handle_request(req);
        print_uds("RSP", resp);
        std::cout << "\n";
    }

    // ─── Demo 2: Read Data by Identifier ────────────────────────────────
    {
        std::cout << "--- Demo 2: ReadDataByIdentifier ---\n";

        // EN: Read VIN (DID 0xF190)
        auto req = std::vector<uint8_t>{SID::ReadDataByIdentifier, 0xF1, 0x90};
        print_uds("REQ", req);
        auto resp = ecu.handle_request(req);
        print_uds("RSP", resp);
        if (resp[0] == 0x62 && resp.size() > 3) {
            std::string vin(resp.begin() + 3, resp.end());
            std::cout << "    → VIN: " << vin << "\n";
        }

        // EN: Read ECU SW version (DID 0xF195)
        req = {SID::ReadDataByIdentifier, 0xF1, 0x95};
        print_uds("REQ", req);
        resp = ecu.handle_request(req);
        print_uds("RSP", resp);

        // EN: Read non-existent DID
        req = {SID::ReadDataByIdentifier, 0xFF, 0xFF};
        print_uds("REQ", req);
        resp = ecu.handle_request(req);
        print_uds("RSP", resp);
        std::cout << "\n";
    }

    // ─── Demo 3: Security Access (Seed-Key) ────────────────────────────
    {
        std::cout << "--- Demo 3: SecurityAccess (Seed-Key) ---\n";

        // EN: Step 1: Request seed
        auto req = std::vector<uint8_t>{SID::SecurityAccess, 0x01};
        print_uds("REQ", req);
        auto resp = ecu.handle_request(req);
        print_uds("RSP", resp);

        uint16_t seed = static_cast<uint16_t>(
            (static_cast<uint16_t>(resp[2]) << 8) | resp[3]);
        std::cout << "    Seed: 0x" << std::hex << std::setw(4) << std::setfill('0')
                  << seed << std::dec << "\n";

        // EN: Step 2a: Wrong key → should fail
        auto bad_req = std::vector<uint8_t>{SID::SecurityAccess, 0x02, 0x00, 0x00};
        print_uds("REQ", bad_req);
        auto bad_resp = ecu.handle_request(bad_req);
        print_uds("RSP", bad_resp);

        // EN: Step 2b: Correct key (seed XOR 0x1234)
        uint16_t key = seed ^ 0x1234;
        auto good_req = std::vector<uint8_t>{SID::SecurityAccess, 0x02,
            static_cast<uint8_t>(key >> 8), static_cast<uint8_t>(key & 0xFF)};
        print_uds("REQ", good_req);
        auto good_resp = ecu.handle_request(good_req);
        print_uds("RSP", good_resp);
        std::cout << "    Key: 0x" << std::hex << std::setw(4) << std::setfill('0')
                  << key << std::dec << " → Security UNLOCKED ✓\n\n";
    }

    // ─── Demo 4: Read / Clear DTCs ──────────────────────────────────────
    {
        std::cout << "--- Demo 4: DTC Read & Clear ---\n";

        // EN: Read DTC count
        auto req = std::vector<uint8_t>{SID::ReadDTCInformation, 0x01};
        print_uds("REQ", req);
        auto resp = ecu.handle_request(req);
        print_uds("RSP", resp);
        std::cout << "    DTC count: " << static_cast<int>(resp[4]) << "\n";

        // EN: Read all DTCs
        req = {SID::ReadDTCInformation, 0x02};
        print_uds("REQ", req);
        resp = ecu.handle_request(req);
        print_uds("RSP", resp);

        // EN: Parse DTCs from response
        std::cout << "    ┌─────────────┬────────┬───────────────────────────────────────┐\n";
        std::cout << "    │ DTC Code    │ Status │ Description                           │\n";
        std::cout << "    ├─────────────┼────────┼───────────────────────────────────────┤\n";
        // Response: [59 02 FF] [DTC_HI DTC_MI DTC_LO STATUS] × N
        std::map<uint32_t, std::string> dtc_names = {
            {0x012345, "P0123 - Throttle Pos Sensor A - High"},
            {0x050067, "P0500 - Vehicle Speed Sensor A"},
            {0x030089, "P0300 - Random Misfire Detected"},
        };
        for (size_t i = 3; i + 3 < resp.size(); i += 4) {
            uint32_t code = (static_cast<uint32_t>(resp[i]) << 16) |
                            (static_cast<uint32_t>(resp[i + 1]) << 8) |
                            static_cast<uint32_t>(resp[i + 2]);
            uint8_t status = resp[i + 3];
            std::string desc = dtc_names.count(code) ? dtc_names[code] : "Unknown";
            std::cout << "    │ 0x" << std::hex << std::setw(6) << std::setfill('0') << code
                      << std::dec << "    │ 0x" << std::hex << std::setw(2)
                      << static_cast<int>(status) << std::dec
                      << "   │ " << std::left << std::setw(37) << desc << std::right << " │\n";
        }
        std::cout << "    └─────────────┴────────┴───────────────────────────────────────┘\n";

        // EN: Clear DTCs
        req = {SID::ClearDTC};
        print_uds("REQ", req);
        resp = ecu.handle_request(req);
        print_uds("RSP", resp);

        // EN: Verify cleared
        req = {SID::ReadDTCInformation, 0x01};
        resp = ecu.handle_request(req);
        std::cout << "    After clear → DTC count: " << static_cast<int>(resp[4]) << "\n\n";
    }

    // ─── Demo 5: Write DID (requires security) ─────────────────────────
    {
        std::cout << "--- Demo 5: WriteDataByIdentifier ---\n";

        // EN: Write without security → should fail
        auto req = std::vector<uint8_t>{SID::WriteDataByIdentifier, 0xF1, 0x99, 0x42};
        print_uds("REQ", req);

        // EN: Need to be in extended session with security unlocked
        // Session was set to Extended in Demo 1, security unlocked in Demo 3
        auto resp = ecu.handle_request(req);
        print_uds("RSP", resp);

        // EN: Read it back
        req = {SID::ReadDataByIdentifier, 0xF1, 0x99};
        print_uds("REQ", req);
        resp = ecu.handle_request(req);
        print_uds("RSP", resp);
        std::cout << "\n";
    }

    // ─── Demo 6: ISO-TP Segmentation ────────────────────────────────────
    {
        std::cout << "--- Demo 6: ISO-TP Segmentation ---\n";

        // EN: Short message → Single Frame
        std::cout << "  Case 1: Short UDS message (fits in SF)\n";
        std::vector<uint8_t> short_msg = {0x22, 0xF1, 0x90};  // ReadDID(VIN)
        auto sf_frames = ISOTPCodec::segment(short_msg);
        for (auto& f : sf_frames) f.print("SF");

        // EN: Long message → FF + CFs
        std::cout << "\n  Case 2: Long UDS response (requires FF + CFs)\n";
        // Simulating a ReadDID response with 25 bytes
        std::vector<uint8_t> long_msg = {0x62, 0xF1, 0x90};
        std::string vin_str = "WVWZZZ3CZWE000001";
        long_msg.insert(long_msg.end(), vin_str.begin(), vin_str.end());
        // append more data to force multiple CFs
        for (int i = 0; i < 10; ++i)
            long_msg.push_back(static_cast<uint8_t>(0x30 + i));

        std::cout << "    Original message: " << long_msg.size() << " bytes\n";
        auto multi_frames = ISOTPCodec::segment(long_msg);
        for (auto& f : multi_frames) f.print("");

        // EN: Reassemble
        auto reassembled = ISOTPCodec::reassemble(multi_frames);
        std::cout << "    Reassembled: " << reassembled.size() << " bytes"
                  << " | Match: " << (reassembled == long_msg ? "YES ✓" : "NO ✗") << "\n\n";
    }

    // ─── Demo 7: Full Diagnostic Sequence ───────────────────────────────
    {
        std::cout << "--- Demo 7: Full Diagnostic Sequence (Flash Prep) ---\n";
        std::cout << "  Typical ECU reprogramming preparation sequence:\n\n";

        UDSServer flash_ecu("ADAS_ECU");
        flash_ecu.add_dtc(0xC10000, 0x01, "U0100 - Lost Comm with ECM");

        struct Step {
            std::string description;
            std::vector<uint8_t> request;
        };

        std::vector<Step> sequence = {
            {"1. Switch to Extended Session",
             {SID::DiagnosticSessionControl, 0x03}},
            {"2. Tester Present (keep-alive)",
             {SID::TesterPresent, 0x00}},
            {"3. Read ECU SW Version",
             {SID::ReadDataByIdentifier, 0xF1, 0x95}},
            {"4. Security Access: Request Seed",
             {SID::SecurityAccess, 0x01}},
            {"5. Read DTCs before flash",
             {SID::ReadDTCInformation, 0x01}},
            {"6. Clear DTCs",
             {SID::ClearDTC}},
            {"7. Verify DTCs cleared",
             {SID::ReadDTCInformation, 0x01}},
            {"8. Switch to Programming Session",
             {SID::DiagnosticSessionControl, 0x02}},
            {"9. ECU Reset (post-flash)",
             {SID::ECUReset, 0x01}},
        };

        for (auto& step : sequence) {
            std::cout << "  " << step.description << "\n";
            print_uds("REQ", step.request);
            auto resp = flash_ecu.handle_request(step.request);
            print_uds("RSP", resp);

            // EN: If security seed received, send the key
            if (step.request[0] == SID::SecurityAccess && step.request[1] == 0x01 &&
                resp[0] == 0x67 && resp.size() >= 4) {
                uint16_t seed = static_cast<uint16_t>(
                    (static_cast<uint16_t>(resp[2]) << 8) | resp[3]);
                uint16_t key = seed ^ 0x1234;
                std::cout << "  4b. Security Access: Send Key\n";
                std::vector<uint8_t> key_req = {SID::SecurityAccess, 0x02,
                    static_cast<uint8_t>(key >> 8), static_cast<uint8_t>(key & 0xFF)};
                print_uds("REQ", key_req);
                auto key_resp = flash_ecu.handle_request(key_req);
                print_uds("RSP", key_resp);
            }
            std::cout << "\n";
        }
    }

    std::cout << "============================================\n";
    std::cout << "   End of UDS Diagnostics & ISO-TP\n";
    std::cout << "============================================\n";

    return 0;
}
