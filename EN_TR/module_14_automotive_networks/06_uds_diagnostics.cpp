/**
 * @file module_14_automotive_networks/06_uds_diagnostics.cpp
 * @brief UDS — Unified Diagnostic Services (ISO 14229) & ISO-TP (ISO 15765)
 *
 * @details
 * =============================================================================
 * [THEORY: UDS — Unified Diagnostic Services / TEORİ: UDS — Birleşik Teşhis Hizmetleri]
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
 * TR: UDS (ISO 14229) tüm modern araçlarda kullanılan teşhis protokolüdür.
 *     Teşhis cihazları, OBD araçları ve fabrika araçları UDS ile:
 *     - DTC okuma/silme (Arıza Kodları)
 *     - ECU yazılımı güncelleme (yeniden programlama)
 *     - ECU verisi okuma/yazma (VIN, kalibrasyon, konfigürasyon)
 *     - Aktüatör kontrolü (test modu)
 *     - Güvenlik erişimi (tohum-anahtar doğrulama)
 *
 *     UDS Servis Kataloğu (temel SID'ılar):
 *     ┌──────┬──────────────────────────────┬──────────────────────────┐
 *     │ SID  │ Servis Adı                   │ Kullanım Alanı           │
 *     ├──────┼──────────────────────────────┼──────────────────────────┤
 *     │ 0x10 │ DiagnosticSessionControl     │ Oturum modu değiştir     │
 *     │ 0x11 │ ECUReset                     │ ECU sıfırla(sert/yumuşak)│
 *     │ 0x14 │ ClearDiagnosticInformation   │ DTC'ıları temizle        │
 *     │ 0x19 │ ReadDTCInformation           │ Arıza kodlarını oku      │
 *     │ 0x22 │ ReadDataByIdentifier         │ DID oku (VIN vb.)        │
 *     │ 0x23 │ ReadMemoryByAddress          │ Doğrudan bellek oku      │
 *     │ 0x27 │ SecurityAccess               │ Tohum-anahtar kilit aç   │
 *     │ 0x2E │ WriteDataByIdentifier        │ DID yaz                  │
 *     │ 0x2F │ InputOutputControlByID       │ Aktüatör kontrolü        │
 *     │ 0x31 │ RoutineControl               │ Teşhis rutini çalıştır   │
 *     │ 0x34 │ RequestDownload              │ Flash oturumu başlat     │
 *     │ 0x36 │ TransferData                 │ Flash verisi gönder      │
 *     │ 0x37 │ RequestTransferExit          │ Flash oturumu sonlandır  │
 *     │ 0x3E │ TesterPresent                │ Oturumu canlı tut        │
 *     │ 0x85 │ ControlDTCSetting            │ DTC etkin/devre dışı     │
 *     └──────┴──────────────────────────────┴──────────────────────────┘
 *
 *     UDS Oturumları:
 *     - 0x01: Varsayılan Oturum (sınırlı hizmetler)
 *     - 0x02: Programlama Oturumu (flash/indirme)
 *     - 0x03: Genişletilmiş Teşhis Oturumu (tam erişim)
 *
 *     Olumsuz Yanıt Kodları (NRC):
 *     ┌──────┬──────────────────────────────────────────────────────┐
 *     │ NRC  │ Anlam                                                │
 *     ├──────┼──────────────────────────────────────────────────────┤
 *     │ 0x10 │ Genel Red                                            │
 *     │ 0x11 │ Servis Desteklenmiyor                                │
 *     │ 0x12 │ Alt Fonksiyon Desteklenmiyor                         │
 *     │ 0x13 │ Yanlış Mesaj Uzunluğu                                │
 *     │ 0x14 │ Yanıt Çok Uzun                                       │
 *     │ 0x22 │ Koşullar Uygun Değil                                 │
 *     │ 0x24 │ İstek Sırası Hatası                                  │
 *     │ 0x25 │ Alt Ağ Bileşeninden Yanıt Yok                        │
 *     │ 0x31 │ İstek Aralık Dışı                                    │
 *     │ 0x33 │ Güvenlik Erişimi Reddedildi                          │
 *     │ 0x35 │ Geçersiz Anahtar                                     │
 *     │ 0x36 │ Deneme Sayısı Aşıldı                                 │
 *     │ 0x72 │ Genel Programlama Hatası                             │
 *     │ 0x78 │ İstek Doğru Alındı, Yanıt Beklemede                  │
 *     └──────┴──────────────────────────────────────────────────────┘
 *
 * =============================================================================
 * [THEORY: ISO-TP — Transport Protocol (ISO 15765-2) / TEORİ: ISO-TP — Taşıma Protokolü (ISO 15765-2)]
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
 * TR: UDS mesajları CAN frame boyutunu (8/64 bayt) aşabilir. ISO-TP segmentasyon sağlar:
 *     Çerçeve Türleri:
 *     - SF (Tek Çerçeve):  ≤7 bayt, tek CAN frame'çe sığar
 *     - FF (İlk Çerçeve):   Çok-çerçeveli mesajın ilk segmenti (toplam uzunluk)
 *     - CF (Ardışık):      Sonraki segmentler (sıra numarası 0-F)
 *     - FC (Akış Kontrolü): Alıcı gönderene bildirir: blok boyutu + zamanlama
 *
 * @note Compile: g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion 06_uds_diagnostics.cpp -o 06_uds_diagnostics
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
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
// TR: UDS Servis Kimlikleri — teşhis işlemlerini tanımlayan sabit kodlar
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
                                                       // TR: olumlu yanıt = SID + 0x40
}

// EN: Negative Response Codes
// TR: Olumsuz Yanıt Kodları — hata nedenini belirten NRC değerleri
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
    uint32_t code;        // 3-byte DTC (e.g., 0x012345) — TR: 3 baytlık arıza kodu
    uint8_t  status;      // DTC status mask — TR: DTC durum maskesi (aktif, onaylı vb.)
    std::string description;
};

class UDSServer {
    std::string name_;
    UDSSession session_ = UDSSession::Default;
    bool security_unlocked_ = false;
    uint16_t seed_ = 0;

    // EN: Data Identifiers (DIDs)
    // TR: Veri Tanımlayıcıları (DID) — VIN, parça no, yazılım sürümü vb.
    std::map<uint16_t, std::vector<uint8_t>> dids_;
    // EN: DTC storage
    // TR: DTC deposu — aktif hata kodlarını tutar
    std::vector<DTC> dtc_list_;

    // EN: Build negative response
    // TR: Olumsuz yanıt oluştur — [0x7F, SID, NRC] formatında
    static std::vector<uint8_t> negative(uint8_t sid, uint8_t nrc) {
        return {SID::NegativeResponse, sid, nrc};
    }

    // EN: Build positive response
    // TR: Olumlu yanıt oluştur — SID + 0x40 ile başlar
    static std::vector<uint8_t> positive(uint8_t sid, const std::vector<uint8_t>& data = {}) {
        std::vector<uint8_t> resp;
        resp.push_back(static_cast<uint8_t>(sid + SID::PositiveOffset));
        resp.insert(resp.end(), data.begin(), data.end());
        return resp;
    }

public:
    UDSServer(std::string name) : name_(std::move(name)) {
        // EN: Pre-populate DIDs
        // TR: DID'ıları önceden doldur — VIN, parça no, donanım/yazılım sürümü
        // DID 0xF190 = VIN
        // TR: DID 0xF190 = Araç Kimlik Numarası (VIN)
        std::string vin = "WVWZZZ3CZWE000001";
        dids_[0xF190] = std::vector<uint8_t>(vin.begin(), vin.end());
        // DID 0xF187 = Part Number
        // TR: DID 0xF187 = Parça Numarası
        std::string part = "8V0907115A";
        dids_[0xF187] = std::vector<uint8_t>(part.begin(), part.end());
        // DID 0xF191 = ECU Hardware Version
        // TR: DID 0xF191 = ECU Donanım Sürümü
        dids_[0xF191] = {0x01, 0x03};
        // DID 0xF195 = ECU Software Version
        // TR: DID 0xF195 = ECU Yazılım Sürümü
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
                    // TR: Tohum iste — güvenlik erişimi için ilk adım
                    seed_ = 0xA5B7;  // simple fixed seed for demo
                                     // TR: demo için sabit tohum değeri
                    return positive(sid, {0x01,
                        static_cast<uint8_t>(seed_ >> 8),
                        static_cast<uint8_t>(seed_ & 0xFF)});
                }
                if (sub == 0x02) {
                    // EN: Send Key (expected: seed XOR 0x1234)
                    // TR: Anahtar gönder (beklenen: seed XOR 0x1234)
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
                    // TR: Durum maskesine göre DTC sayısını raporla
                    uint8_t count = static_cast<uint8_t>(dtc_list_.size());
                    return positive(sid, {0x01, 0xFF, 0x00, count});
                }
                if (sub == 0x02) {
                    // EN: Report DTC by status mask
                    // TR: Durum maskesine göre DTC'ıları listele
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
    uint8_t        sequence_number = 0;   // for CF — TR: Ardışık Çerçeve sıra numarası
    uint16_t       total_length = 0;      // for FF — TR: İlk Çerçeve toplam mesaj uzunluğu
    uint8_t        block_size = 0;        // for FC — TR: Akış Kontrolü blok boyutu
    uint8_t        st_min = 0;            // for FC (separation time min, ms)
                                          // TR: Akış Kontrolü minimum ayırma süresi (ms)
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
    // TR: Mesajı ISO-TP çerçevelerine böl (CAN: çerçeve başına maks 8 bayt)
    static std::vector<ISOTPFrame> segment(const std::vector<uint8_t>& message) {
        std::vector<ISOTPFrame> frames;

        if (message.size() <= 7) {
            // EN: Single Frame
            // TR: Tek Çerçeve — ≤7 bayt, dolgu ile 8 bayta tamamlanır
            ISOTPFrame sf;
            sf.type = ISOTPFrameType::SingleFrame;
            sf.data.push_back(static_cast<uint8_t>(message.size()));  // PCI byte: 0x0N
                                                                       // TR: PCI baytı: 0x0N (uzunluk)
            sf.data.insert(sf.data.end(), message.begin(), message.end());
            // Pad to 8 bytes — TR: 8 bayta doldur  
            while (sf.data.size() < 8) sf.data.push_back(0xCC);
            frames.push_back(sf);
        } else {
            // EN: First Frame
            // TR: İlk Çerçeve — toplam uzunluk + ilk 6 veri baytı
            ISOTPFrame ff;
            ff.type = ISOTPFrameType::FirstFrame;
            ff.total_length = static_cast<uint16_t>(message.size());
            // PCI: 0x1NNN (N = total length, 12-bit)
            // TR: PCI: 0x1NNN (N = toplam uzunluk, 12-bit)
            ff.data.push_back(static_cast<uint8_t>(0x10 | ((message.size() >> 8) & 0x0F)));
            ff.data.push_back(static_cast<uint8_t>(message.size() & 0xFF));
            // First 6 data bytes — TR: İlk 6 veri baytı  
            size_t offset = 0;
            while (ff.data.size() < 8 && offset < message.size()) {
                ff.data.push_back(message[offset++]);
            }
            frames.push_back(ff);

            // EN: Consecutive Frames
            // TR: Ardışık Çerçeveler — sıra numarası ile kalan veriyi taşır
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
    // TR: ISO-TP çerçevelerini tekrar mesaja birleştir
    static std::vector<uint8_t> reassemble(const std::vector<ISOTPFrame>& frames) {
        if (frames.empty()) return {};

        auto& first = frames[0];
        if (first.type == ISOTPFrameType::SingleFrame) {
            uint8_t len = first.data[0] & 0x0F;
            return std::vector<uint8_t>(first.data.begin() + 1, first.data.begin() + 1 + len);
        }

        // First Frame
        // TR: İlk Çerçeve
        uint16_t total = static_cast<uint16_t>(
            ((static_cast<uint16_t>(first.data[0]) & 0x0F) << 8) | first.data[1]);
        std::vector<uint8_t> result;
        // Data from FF (bytes 2-7)
        // TR: FF'den veri (bayt 2-7)
        for (size_t i = 2; i < first.data.size() && result.size() < total; ++i)
            result.push_back(first.data[i]);

        // Consecutive Frames
        // TR: Ardışık Çerçeveler
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
        // TR: Genişletilmiş Oturuma geç — gelişmiş teşhis hizmetlerini açar
        auto req = std::vector<uint8_t>{SID::DiagnosticSessionControl, 0x03};
        print_uds("REQ", req);
        auto resp = ecu.handle_request(req);
        print_uds("RSP", resp);

        // EN: Invalid session
        // TR: Geçersiz oturum — desteklenmeyen alt fonksiyon reddedilir
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
        // TR: VIN oku (DID 0xF190) — araç kimlik numarası
        auto req = std::vector<uint8_t>{SID::ReadDataByIdentifier, 0xF1, 0x90};
        print_uds("REQ", req);
        auto resp = ecu.handle_request(req);
        print_uds("RSP", resp);
        if (resp[0] == 0x62 && resp.size() > 3) {
            std::string vin(resp.begin() + 3, resp.end());
            std::cout << "    → VIN: " << vin << "\n";
        }

        // EN: Read ECU SW version (DID 0xF195)
        // TR: ECU yazılım sürümü oku (DID 0xF195)
        req = {SID::ReadDataByIdentifier, 0xF1, 0x95};
        print_uds("REQ", req);
        resp = ecu.handle_request(req);
        print_uds("RSP", resp);

        // EN: Read non-existent DID
        // TR: Var olmayan DID oku — RequestOutOfRange hatasi beklenir
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
        // TR: Adım 1: Tohum iste — güvenlik erişimi başlat
        auto req = std::vector<uint8_t>{SID::SecurityAccess, 0x01};
        print_uds("REQ", req);
        auto resp = ecu.handle_request(req);
        print_uds("RSP", resp);

        uint16_t seed = static_cast<uint16_t>(
            (static_cast<uint16_t>(resp[2]) << 8) | resp[3]);
        std::cout << "    Seed: 0x" << std::hex << std::setw(4) << std::setfill('0')
                  << seed << std::dec << "\n";

        // EN: Step 2a: Wrong key → should fail
        // TR: Adım 2a: Yanlış anahtar → başarısız olmalı
        auto bad_req = std::vector<uint8_t>{SID::SecurityAccess, 0x02, 0x00, 0x00};
        print_uds("REQ", bad_req);
        auto bad_resp = ecu.handle_request(bad_req);
        print_uds("RSP", bad_resp);

        // EN: Step 2b: Correct key (seed XOR 0x1234)
        // TR: Adım 2b: Doğru anahtar (seed XOR 0x1234) → güvenlik kilit açılır
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
        // TR: DTC sayısını oku
        auto req = std::vector<uint8_t>{SID::ReadDTCInformation, 0x01};
        print_uds("REQ", req);
        auto resp = ecu.handle_request(req);
        print_uds("RSP", resp);
        std::cout << "    DTC count: " << static_cast<int>(resp[4]) << "\n";

        // EN: Read all DTCs
        // TR: Tüm DTC'ıları oku — durum maskesi ile filtreleme
        req = {SID::ReadDTCInformation, 0x02};
        print_uds("REQ", req);
        resp = ecu.handle_request(req);
        print_uds("RSP", resp);

        // EN: Parse DTCs from response
        // TR: Yanıttan DTC'ıları ayrıştır — her DTC: 3 bayt kod + 1 bayt durum
        std::cout << "    ┌─────────────┬────────┬───────────────────────────────────────┐\n";
        std::cout << "    │ DTC Code    │ Status │ Description                           │\n";
        std::cout << "    ├─────────────┼────────┼───────────────────────────────────────┤\n";
        // Response: [59 02 FF] [DTC_HI DTC_MI DTC_LO STATUS] × N
        // TR: Yanıt: [59 02 FF] [DTC_ÜST DTC_ORTA DTC_ALT DURUM] × N
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
        // TR: DTC'ıları temizle
        req = {SID::ClearDTC};
        print_uds("REQ", req);
        resp = ecu.handle_request(req);
        print_uds("RSP", resp);

        // EN: Verify cleared
        // TR: Temizlenmiş olduğunu doğrula — sayı 0 olmalı
        req = {SID::ReadDTCInformation, 0x01};
        resp = ecu.handle_request(req);
        std::cout << "    After clear → DTC count: " << static_cast<int>(resp[4]) << "\n\n";
    }

    // ─── Demo 5: Write DID (requires security) ─────────────────────────
    {
        std::cout << "--- Demo 5: WriteDataByIdentifier ---\n";

        // EN: Write without security → should fail
        // TR: Güvenlik kilidi açılmadan yazma → SecurityAccessDenied beklenir
        auto req = std::vector<uint8_t>{SID::WriteDataByIdentifier, 0xF1, 0x99, 0x42};
        print_uds("REQ", req);

        // EN: Need to be in extended session with security unlocked
        // TR: Genişletilmiş oturumda ve güvenlik açık olmalı
        // Session was set to Extended in Demo 1, security unlocked in Demo 3
        // TR: Oturum Demo 1'de Genişletilmiş'e geçti, güvenlik Demo 3'te açıldı
        auto resp = ecu.handle_request(req);
        print_uds("RSP", resp);

        // EN: Read it back
        // TR: Yazılanı geri oku — doğrulama
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
        // TR: Kısa mesaj → Tek Çerçeve (SF) — ≤7 bayt
        std::cout << "  Case 1: Short UDS message (fits in SF)\n";
        std::vector<uint8_t> short_msg = {0x22, 0xF1, 0x90};  // ReadDID(VIN)
                                                                // TR: VIN oku isteği
        auto sf_frames = ISOTPCodec::segment(short_msg);
        for (auto& f : sf_frames) f.print("SF");

        // EN: Long message → FF + CFs
        // TR: Uzun mesaj → FF + CF'ılar — çoklu CAN çerçevesine bölünür
        std::cout << "\n  Case 2: Long UDS response (requires FF + CFs)\n";
        // Simulating a ReadDID response with 25 bytes
        // TR: 25 baytlık ReadDID yanıtı simülasyonu
        std::vector<uint8_t> long_msg = {0x62, 0xF1, 0x90};
        std::string vin_str = "WVWZZZ3CZWE000001";
        long_msg.insert(long_msg.end(), vin_str.begin(), vin_str.end());
        // append more data to force multiple CFs
        // TR: çoklu CF zorlamak için ek veri ekle
        for (int i = 0; i < 10; ++i)
            long_msg.push_back(static_cast<uint8_t>(0x30 + i));

        std::cout << "    Original message: " << long_msg.size() << " bytes\n";
        auto multi_frames = ISOTPCodec::segment(long_msg);
        for (auto& f : multi_frames) f.print("");

        // EN: Reassemble
        // TR: Yeniden birleştir — orijinal mesajla eşleşmeli
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
            // TR: Güvenlik tohumu alındıysa, anahtarı gönder
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
