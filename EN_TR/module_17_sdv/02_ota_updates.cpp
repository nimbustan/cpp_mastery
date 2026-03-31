/**
 * @file module_17_sdv/02_ota_updates.cpp
 * @brief OTA (Over-The-Air) Update Mechanism — Automotive Firmware Updates
 *
 * @details
 * =============================================================================
 * [THEORY: OTA Update Lifecycle / TEORİ: OTA Güncelleme Yaşam Döngüsü]
 * =============================================================================
 *
 * EN: OTA allows updating vehicle software without physical access.
 *
 *     Campaign    Download   Verify     Install     Activate   Confirm
 *     Creation    Firmware   Integrity  to Slot B   Boot B     Success
 *       │           │          │           │          │           │
 *       ▼           ▼          ▼           ▼          ▼           ▼
 *     [Plan] → [Download] → [Verify] → [Install] → [Swap] → [Monitor]
 *                                ↓ FAIL                    ↓ FAIL
 *                            [Abort]                   [Rollback→A]
 *
 * TR: OTA, araç yazılımını fiziksel erişim olmadan güncellemeye olanak tanır.
 *
 *     Kampanya    İndirme    Doğrulama  Kurulum     Aktive et   Onay
 *     Oluşturma   Firmware   Bütünlük   Slot B'ye   B'yi başlat Başarı
 *       │           │          │           │          │           │
 *       ▼           ▼          ▼           ▼          ▼           ▼
 *     [Plan] → [İndir] → [Doğrula] → [Kur] → [Takas] → [İzle]
 *                            ↓ BAŞARISIZ                ↓ BAŞARISIZ
 *                          [İptal]                   [Rollback→A]
 *
 * =============================================================================
 * [THEORY: A/B Partitioning — Atomic Slot Swap / TEORİ: A/B Bölümleme — Atomik Slot Değişimi]
 * =============================================================================
 *
 * EN: Flash Memory Layout:
 *     ┌───────────────────┬──────────────────────────┐
 *     │ Bootloader (ROM)  │ Protected, immutable     │
 *     ├───────────────────┼──────────────────────────┤
 *     │ Slot A (Active)   │ Currently running FW     │
 *     ├───────────────────┼──────────────────────────┤
 *     │ Slot B (Inactive) │ Target for OTA update    │
 *     ├───────────────────┼──────────────────────────┤
 *     │ NvM / Config      │ Persistent data          │
 *     └──────────────────────────────────────────────┘
 *
 *     Update flow:
 *     1. Download firmware → write to Slot B
 *     2. Verify Slot B (hash + signature)
 *     3. Set boot flag: next_boot = B
 *     4. Reboot → bootloader reads flag → boots Slot B
 *     5. Application confirms success → commit B as active
 *     6. If no confirm within timeout → rollback to Slot A
 *
 * TR: A/B bölümleme — atomik slot takası:
 *     Flash bellek iki eşit "slot" içerir (A ve B). Güncelleme her zaman
 *     pasif slot'a (B) yazılır; aktif slot (A) çalışmaya devam eder.
 *     Akış: B'ye indir → hash+imza doğrula → önyükleyici bayrağını B'ye ayarla
 *     → yeniden başlat → B başarılıysa onayla → başarısızsa A'ya rollback.
 *     Avantaj: güncelleme sırasında araç kullanılabilir, brick riski sıfır.
 *
 * =============================================================================
 * [THEORY: Secure Boot Chain / TEORİ: Güvenli Önyükleme Zinciri]
 * =============================================================================
 *
 * EN: ROM BL (HW root of trust)
 *       │ verify signature
 *       ▼
 *     1st Stage BL (SPL)
 *       │ verify signature
 *       ▼
 *     2nd Stage BL (U-Boot)
 *       │ verify signature
 *       ▼
 *     Application FW
 *
 * TR: Güvenli önyükleme zinciri (Secure Boot Chain):
 *     Her aşama bir öncekini doğrular (ROM BL → 1. Aşama BL → 2. Aşama BL → Uygulama).
 *     Kök güven (root of trust) donanımda ROM BL'de saklanır; değiştirilemez.
 *     İmza doğrulaması başarısız olursa önyükleme durur — manipüle edilmiş
 *     firmware çalıştırılmaz.
 *
 * =============================================================================
 * [THEORY: UDS Flash Programming Sequence / TEORİ: UDS Flash Programlama Sırası]
 * =============================================================================
 *
 * EN: Step │ UDS Service               │ Description
 *     ─────┼───────────────────────────┼──────────────────────────
 *      1   │ 0x10 (DiagSession)        │ Enter Programming session
 *      2   │ 0x27 (SecurityAccess)     │ Authenticate tester
 *      3   │ 0x34 (RequestDownload)    │ Negotiate transfer params
 *      4   │ 0x36 (TransferData)       │ Send firmware blocks
 *      5   │ 0x37 (RequestTransferExit)│ Finalize transfer
 *      6   │ 0x31 (RoutineControl)     │ Verify checksum / activate
 *
 * TR: UDS Flash programlama sırası:
 *     1. DiagSession (0x10): Programlama oturumuna gir
 *     2. SecurityAccess (0x27): Test cihazını doğrula
 *     3. RequestDownload (0x34): Transfer parametrelerini müzakere et
 *     4. TransferData (0x36): Firmware bloklarını gönder
 *     5. RequestTransferExit (0x37): Transferi sonlandır
 *     6. RoutineControl (0x31): Checksum doğrula / aktive et
 *
 * @note Compile: g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion 02_ota_updates.cpp -o 02_ota
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <iostream>
#include <cstdint>
#include <vector>
#include <string>
#include <string_view>
#include <array>
#include <iomanip>
#include <algorithm>
#include <optional>
#include <numeric>
#include <cassert>
#include <cstring>
#include <functional>
#include <memory>

// ═════════════════════════════════════════════════════════════════════════════
// PART 1: Firmware Image Format
// ═════════════════════════════════════════════════════════════════════════════

namespace ota {

// EN: Firmware image header — precedes the actual binary payload
// TR: Firmware image başlığı — gerçek ikili yükün önünde yer alır
struct FirmwareHeader {
    uint32_t magic;             // 0x4F544131 = "OTA1" — TR: sihirli sayı (geçerlilik kontrolü)
    uint32_t version_major;
    uint32_t version_minor;
    uint32_t version_patch;
    uint32_t payload_size;
    uint32_t payload_crc32;
    uint32_t header_crc32;      // CRC of header fields (excluding this field) — TR: başlık alanlarının CRC'si
    uint64_t timestamp;         // build timestamp (epoch) — TR: derleme zaman damgası
    char     target_ecu[16];    // target ECU identifier — TR: hedef ECU kimliği

    void Print() const {
        std::cout << "    Magic:    0x" << std::hex << magic << std::dec << "\n"
                  << "    Version:  " << version_major << "." << version_minor
                  << "." << version_patch << "\n"
                  << "    Payload:  " << payload_size << " bytes\n"
                  << "    CRC32:    0x" << std::hex << payload_crc32 << std::dec << "\n"
                  << "    Target:   " << std::string(target_ecu, strnlen(target_ecu, 16)) << "\n";
    }
};

// EN: Simple CRC32 for verification (same as MISRA file for consistency)
// TR: Doğrulama için basit CRC32
constexpr uint32_t CRC32_POLY = 0xEDB88320u;

inline uint32_t ComputeCRC32(const uint8_t* data, size_t length) {
    uint32_t crc = 0xFFFFFFFFu;
    for (size_t i = 0; i < length; ++i) {
        crc ^= data[i];
        for (int j = 0; j < 8; ++j) {
            if ((crc & 1u) != 0)
                crc = (crc >> 1) ^ CRC32_POLY;
            else
                crc >>= 1;
        }
    }
    return crc ^ 0xFFFFFFFFu;
}

// EN: Build a firmware image (header + payload)
// TR: Firmware image oluştur (başlık + yük)
struct FirmwareImage {
    FirmwareHeader          header;
    std::vector<uint8_t>    payload;

    static FirmwareImage Build(uint32_t major, uint32_t minor, uint32_t patch,
                               const std::string& target,
                               const std::vector<uint8_t>& fw_data) {
        FirmwareImage img;
        img.payload = fw_data;

        auto& h = img.header;
        h.magic = 0x4F544131;
        h.version_major = major;
        h.version_minor = minor;
        h.version_patch = patch;
        h.payload_size = static_cast<uint32_t>(fw_data.size());
        h.payload_crc32 = ComputeCRC32(fw_data.data(), fw_data.size());
        h.timestamp = 1717200000;  // 2024-06-01
        std::memset(h.target_ecu, 0, sizeof(h.target_ecu));
        auto copy_len = std::min(target.size(), size_t{15});
        std::memcpy(h.target_ecu, target.c_str(), copy_len);

        // EN: Header CRC covers all fields except header_crc32 itself
        // TR: Header CRC, header_crc32 alanı hariç tüm başlık alanlarını kapsar
        const auto* hdr_bytes = reinterpret_cast<const uint8_t*>(&h);
        h.header_crc32 = ComputeCRC32(hdr_bytes, offsetof(FirmwareHeader, header_crc32));

        return img;
    }

    [[nodiscard]] bool VerifyPayload() const {
        auto actual = ComputeCRC32(payload.data(), payload.size());
        return actual == header.payload_crc32;
    }

    [[nodiscard]] bool VerifyHeader() const {
        const auto* hdr_bytes = reinterpret_cast<const uint8_t*>(&header);
        auto actual = ComputeCRC32(hdr_bytes, offsetof(FirmwareHeader, header_crc32));
        return actual == header.header_crc32;
    }
};

// ═════════════════════════════════════════════════════════════════════════════
// PART 2: A/B Partition Manager
// ═════════════════════════════════════════════════════════════════════════════

enum class Slot : uint8_t { A, B };

inline std::string_view SlotName(Slot s) {
    return s == Slot::A ? "Slot_A" : "Slot_B";
}

struct PartitionInfo {
    Slot     slot;
    uint32_t version_major = 0;
    uint32_t version_minor = 0;
    uint32_t version_patch = 0;
    uint32_t fw_crc32 = 0;
    bool     valid = false;
    bool     confirmed = false;    // EN: set after successful boot / TR: başarılı boot sonrası set edilir
    uint8_t  boot_attempts = 0;
};

class ABPartitionManager {
    PartitionInfo slot_a_;
    PartitionInfo slot_b_;
    Slot          active_slot_ = Slot::A;
    Slot          next_boot_  = Slot::A;
    static constexpr uint8_t MAX_BOOT_ATTEMPTS = 3;

public:
    ABPartitionManager() {
        slot_a_.slot = Slot::A;
        slot_b_.slot = Slot::B;
    }

    // EN: Install firmware to inactive slot
    // TR: Firmware'i aktif olmayan slot'a yükle
    bool InstallToInactive(const FirmwareImage& img) {
        auto& target = (active_slot_ == Slot::A) ? slot_b_ : slot_a_;
        if (!img.VerifyPayload() || !img.VerifyHeader()) {
            std::cout << "    [AB] Image verification FAILED\n";
            return false;
        }
        target.version_major = img.header.version_major;
        target.version_minor = img.header.version_minor;
        target.version_patch = img.header.version_patch;
        target.fw_crc32 = img.header.payload_crc32;
        target.valid = true;
        target.confirmed = false;
        target.boot_attempts = 0;

        std::cout << "    [AB] Installed v" << target.version_major
                  << "." << target.version_minor << "." << target.version_patch
                  << " to " << SlotName(target.slot) << "\n";
        return true;
    }

    // EN: Swap active slot for next boot
    // TR: Sonraki boot için aktif slot'u değiştir
    void SetNextBoot(Slot s) {
        next_boot_ = s;
        std::cout << "    [AB] Next boot set to " << SlotName(s) << "\n";
    }

    // EN: Simulate bootloader selecting slot
    // TR: Bootloader'ın slot seçimini simüle et
    bool Boot() {
        auto& target = (next_boot_ == Slot::A) ? slot_a_ : slot_b_;
        if (!target.valid) {
            std::cout << "    [AB] " << SlotName(next_boot_) << " invalid, falling back\n";
            next_boot_ = (next_boot_ == Slot::A) ? Slot::B : Slot::A;
            active_slot_ = next_boot_;
            return false;
        }
        ++target.boot_attempts;
        if (target.boot_attempts > MAX_BOOT_ATTEMPTS && !target.confirmed) {
            std::cout << "    [AB] " << SlotName(next_boot_)
                      << " exceeded max attempts, ROLLBACK\n";
            next_boot_ = (next_boot_ == Slot::A) ? Slot::B : Slot::A;
            active_slot_ = next_boot_;
            return false;
        }
        active_slot_ = next_boot_;
        std::cout << "    [AB] Booted from " << SlotName(active_slot_)
                  << " (attempt " << static_cast<int>(target.boot_attempts) << ")\n";
        return true;
    }

    // EN: Application confirms successful boot
    // TR: Uygulama başarılı boot'u onaylar
    void ConfirmBoot() {
        auto& active = (active_slot_ == Slot::A) ? slot_a_ : slot_b_;
        active.confirmed = true;
        std::cout << "    [AB] " << SlotName(active_slot_) << " CONFIRMED\n";
    }

    void InitSlotA(uint32_t maj, uint32_t min, uint32_t pat) {
        slot_a_.version_major = maj;
        slot_a_.version_minor = min;
        slot_a_.version_patch = pat;
        slot_a_.valid = true;
        slot_a_.confirmed = true;
        active_slot_ = Slot::A;
        next_boot_ = Slot::A;
    }

    void PrintStatus() const {
        auto print_slot = [](const PartitionInfo& p) {
            std::cout << "    " << SlotName(p.slot) << ": v"
                      << p.version_major << "." << p.version_minor
                      << "." << p.version_patch
                      << " valid=" << p.valid
                      << " confirmed=" << p.confirmed
                      << " attempts=" << static_cast<int>(p.boot_attempts) << "\n";
        };
        print_slot(slot_a_);
        print_slot(slot_b_);
        std::cout << "    Active: " << SlotName(active_slot_)
                  << "  Next boot: " << SlotName(next_boot_) << "\n";
    }
};

// ═════════════════════════════════════════════════════════════════════════════
// PART 3: OTA State Machine
// ═════════════════════════════════════════════════════════════════════════════

enum class OTAState : uint8_t {
    IDLE, DOWNLOADING, VERIFYING, INSTALLING, ACTIVATING,
    MONITORING, CONFIRMED, ROLLBACK, ERROR
};

inline std::string_view OTAStateStr(OTAState s) {
    switch (s) {
        case OTAState::IDLE:        return "IDLE";
        case OTAState::DOWNLOADING: return "DOWNLOADING";
        case OTAState::VERIFYING:   return "VERIFYING";
        case OTAState::INSTALLING:  return "INSTALLING";
        case OTAState::ACTIVATING:  return "ACTIVATING";
        case OTAState::MONITORING:  return "MONITORING";
        case OTAState::CONFIRMED:   return "CONFIRMED";
        case OTAState::ROLLBACK:    return "ROLLBACK";
        case OTAState::ERROR:       return "ERROR";
    }
    return "UNKNOWN";
}

class OTAManager {
    OTAState           state_ = OTAState::IDLE;
    ABPartitionManager ab_mgr_;
    FirmwareImage      pending_image_;
    bool               image_ready_ = false;

    void Transition(OTAState next) {
        std::cout << "  [OTA] " << OTAStateStr(state_) << " → " << OTAStateStr(next) << "\n";
        state_ = next;
    }

public:
    void InitCurrentFirmware(uint32_t maj, uint32_t min, uint32_t pat) {
        ab_mgr_.InitSlotA(maj, min, pat);
    }

    // EN: Simulate full OTA flow
    // TR: Tam OTA akışını simüle et
    bool StartUpdate(const FirmwareImage& img) {
        Transition(OTAState::DOWNLOADING);
        pending_image_ = img;
        image_ready_ = true;
        std::cout << "  [OTA] Downloaded " << img.header.payload_size << " bytes\n";

        // Verify
        Transition(OTAState::VERIFYING);
        if (!pending_image_.VerifyPayload() || !pending_image_.VerifyHeader()) {
            Transition(OTAState::ERROR);
            return false;
        }
        std::cout << "  [OTA] Verification PASSED\n";

        // Install
        Transition(OTAState::INSTALLING);
        if (!ab_mgr_.InstallToInactive(pending_image_)) {
            Transition(OTAState::ERROR);
            return false;
        }

        // Activate
        Transition(OTAState::ACTIVATING);
        ab_mgr_.SetNextBoot(Slot::B);
        ab_mgr_.Boot();

        // Monitor
        Transition(OTAState::MONITORING);
        // EN: Simulate application health check
        // TR: Uygulama sağlık kontrolünü simüle et — başarısız olursa rollback tetiklenir
        bool app_healthy = true;
        if (app_healthy) {
            ab_mgr_.ConfirmBoot();
            Transition(OTAState::CONFIRMED);
        } else {
            Transition(OTAState::ROLLBACK);
            ab_mgr_.SetNextBoot(Slot::A);
            ab_mgr_.Boot();
        }

        return state_ == OTAState::CONFIRMED;
    }

    // EN: Simulate a failed update (corrupted image)
    // TR: Başarısız güncelleme simülasyonu (bozuk image)
    bool StartCorruptUpdate(FirmwareImage img) {
        Transition(OTAState::DOWNLOADING);
        // EN: Corrupt the payload
        // TR: Yük verisini boz — CRC uyuşmazlığı tespit mekanizmasını test eder
        if (!img.payload.empty()) {
            img.payload[0] ^= 0xFF;
        }
        Transition(OTAState::VERIFYING);
        if (!img.VerifyPayload()) {
            std::cout << "  [OTA] CRC MISMATCH — image corrupted!\n";
            Transition(OTAState::ERROR);
            return false;
        }
        return false;
    }

    void PrintStatus() const {
        std::cout << "  State: " << OTAStateStr(state_) << "\n";
        ab_mgr_.PrintStatus();
    }
};

// ═════════════════════════════════════════════════════════════════════════════
// PART 4: Delta Update Simulation
// ═════════════════════════════════════════════════════════════════════════════

namespace delta {

// EN: Simple delta patch (byte-level diff)
// TR: Basit delta yama (byte seviyesi fark)
struct DeltaPatch {
    struct PatchEntry {
        uint32_t offset;
        uint8_t  old_value;
        uint8_t  new_value;
    };

    std::vector<PatchEntry> entries;
    uint32_t target_crc32;

    [[nodiscard]] size_t PatchSize() const {
        return entries.size() * sizeof(PatchEntry);
    }
};

inline DeltaPatch CreateDelta(const std::vector<uint8_t>& old_fw,
                               const std::vector<uint8_t>& new_fw) {
    DeltaPatch patch;
    auto min_len = std::min(old_fw.size(), new_fw.size());
    for (size_t i = 0; i < min_len; ++i) {
        if (old_fw[i] != new_fw[i]) {
            patch.entries.push_back({static_cast<uint32_t>(i), old_fw[i], new_fw[i]});
        }
    }
    // EN: Appended bytes
    // TR: Eklenmiş byte'lar — yeni firmware eski firmware'den uzunsa, fazla kısım ekleme olarak kaydedilir
    for (size_t i = min_len; i < new_fw.size(); ++i) {
        patch.entries.push_back({static_cast<uint32_t>(i), 0, new_fw[i]});
    }
    patch.target_crc32 = ComputeCRC32(new_fw.data(), new_fw.size());
    return patch;
}

inline bool ApplyDelta(std::vector<uint8_t>& fw, const DeltaPatch& patch) {
    for (const auto& e : patch.entries) {
        if (e.offset >= fw.size()) fw.resize(e.offset + 1, 0);
        fw[e.offset] = e.new_value;
    }
    auto crc = ComputeCRC32(fw.data(), fw.size());
    return crc == patch.target_crc32;
}

}  // namespace delta

// ═════════════════════════════════════════════════════════════════════════════
// PART 5: UDS Flash Programming Sequence
// ═════════════════════════════════════════════════════════════════════════════

namespace uds_flash {

enum class FlashState : uint8_t {
    IDLE, SESSION_OPEN, SECURITY_UNLOCKED, DOWNLOAD_STARTED,
    TRANSFERRING, TRANSFER_DONE, VERIFIED, ERROR
};

inline std::string_view FlashStateStr(FlashState s) {
    switch (s) {
        case FlashState::IDLE:              return "IDLE";
        case FlashState::SESSION_OPEN:       return "SESSION_OPEN";
        case FlashState::SECURITY_UNLOCKED:  return "SECURITY_UNLOCKED";
        case FlashState::DOWNLOAD_STARTED:   return "DOWNLOAD_STARTED";
        case FlashState::TRANSFERRING:       return "TRANSFERRING";
        case FlashState::TRANSFER_DONE:      return "TRANSFER_DONE";
        case FlashState::VERIFIED:           return "VERIFIED";
        case FlashState::ERROR:              return "ERROR";
    }
    return "UNKNOWN";
}

class FlashProgrammer {
    FlashState state_ = FlashState::IDLE;
    uint32_t   total_size_ = 0;
    uint32_t   received_ = 0;
    uint8_t    block_seq_ = 0;

    void Log(std::string_view service, std::string_view detail) {
        std::cout << "    [UDS] " << service << " — " << detail
                  << " [state=" << FlashStateStr(state_) << "]\n";
    }

public:
    // EN: 0x10 — DiagnosticSessionControl (Programming session)
    // TR: 0x10 — Tanı Oturum Kontrolü (Programlama oturumuna geçiş)
    bool DiagSessionControl(uint8_t session_type) {
        if (session_type == 0x02) {  // Programming session
            state_ = FlashState::SESSION_OPEN;
            Log("0x10 DiagSession", "Programming session opened");
            return true;
        }
        Log("0x10 DiagSession", "Invalid session type");
        return false;
    }

    // EN: 0x27 — SecurityAccess (seed/key)
    // TR: 0x27 — Güvenlik Erişimi (seed/key ile test cihazı kimlik doğrulaması)
    bool SecurityAccess(uint32_t key) {
        if (state_ != FlashState::SESSION_OPEN) {
            Log("0x27 SecurityAccess", "Wrong state");
            return false;
        }
        // EN: Simplified key validation
        // TR: Basitleştirilmiş anahtar doğrulaması — gerçek sistemde HSM kullanılır
        if (key == 0xDEADBEEF) {
            state_ = FlashState::SECURITY_UNLOCKED;
            Log("0x27 SecurityAccess", "Security UNLOCKED");
            return true;
        }
        Log("0x27 SecurityAccess", "Invalid key");
        return false;
    }

    // EN: 0x34 — RequestDownload
    // TR: 0x34 — İndirme Talebi (hedef adres ve boyut bilgisi ile transfer başlatılır)
    bool RequestDownload(uint32_t address, uint32_t size) {
        if (state_ != FlashState::SECURITY_UNLOCKED) {
            Log("0x34 RequestDownload", "Wrong state");
            return false;
        }
        total_size_ = size;
        received_ = 0;
        block_seq_ = 1;
        state_ = FlashState::DOWNLOAD_STARTED;
        Log("0x34 RequestDownload",
            "addr=0x" + ([&]{ std::ostringstream s; s << std::hex << address; return s.str(); })()
            + " size=" + std::to_string(size));
        return true;
    }

    // EN: 0x36 — TransferData (one block)
    // TR: 0x36 — Veri Transferi (tek blok) — sıra numarası kontrolü ile blok blok gönderilir
    bool TransferData(uint8_t seq, const std::vector<uint8_t>& block) {
        if (state_ != FlashState::DOWNLOAD_STARTED && state_ != FlashState::TRANSFERRING) {
            Log("0x36 TransferData", "Wrong state");
            return false;
        }
        if (seq != block_seq_) {
            Log("0x36 TransferData", "Sequence mismatch");
            state_ = FlashState::ERROR;
            return false;
        }
        state_ = FlashState::TRANSFERRING;
        received_ += static_cast<uint32_t>(block.size());
        Log("0x36 TransferData",
            "block#" + std::to_string(seq) + " (" + std::to_string(block.size())
            + "B) total=" + std::to_string(received_) + "/" + std::to_string(total_size_));
        ++block_seq_;
        return true;
    }

    // EN: 0x37 — RequestTransferExit
    // TR: 0x37 — Transfer Sonlandırma (tüm bloklar alındıktan sonra transferi kapatır)
    bool RequestTransferExit() {
        if (state_ != FlashState::TRANSFERRING || received_ < total_size_) {
            Log("0x37 TransferExit", "Incomplete transfer");
            return false;
        }
        state_ = FlashState::TRANSFER_DONE;
        Log("0x37 TransferExit", "Transfer complete");
        return true;
    }

    // EN: 0x31 — RoutineControl (verify + activate)
    // TR: 0x31 — Rutin Kontrol (checksum doğrulama + aktivasyon hazırlığı)
    bool RoutineControl_Verify() {
        if (state_ != FlashState::TRANSFER_DONE) {
            Log("0x31 RoutineControl", "Wrong state");
            return false;
        }
        state_ = FlashState::VERIFIED;
        Log("0x31 RoutineControl", "Checksum VERIFIED, activation ready");
        return true;
    }
};

}  // namespace uds_flash

// ═════════════════════════════════════════════════════════════════════════════
// PART 6: Uptane Framework Overview
// ═════════════════════════════════════════════════════════════════════════════

namespace uptane {

// EN: Uptane role simulation (simplified TUF-based)
// TR: Uptane rol simülasyonu (basitleştirilmiş TUF tabanlı)
struct UptaneRole {
    std::string name;
    std::string description;
    uint32_t    key_id;
};

inline std::vector<UptaneRole> GetRoles() {
    return {
        {"Root",      "Top-level trust anchor, signs other keys", 0x0001},
        {"Targets",   "Signs firmware metadata (hash, version)",  0x0002},
        {"Snapshot",  "Signs consistent set of target metadata",  0x0003},
        {"Timestamp", "Prevents replay attacks (freshness)",      0x0004},
        {"Director",  "Vehicle-specific update instructions",     0x0005},
    };
}

}  // namespace uptane

}  // namespace ota

// ═════════════════════════════════════════════════════════════════════════════
// MAIN — Demos
// ═════════════════════════════════════════════════════════════════════════════

int main() {
    std::cout << "╔══════════════════════════════════════════════════════════════╗\n"
              << "║ Module 17: OTA Updates — Automotive Firmware Update          ║\n"
              << "╚══════════════════════════════════════════════════════════════╝\n\n";

    // --- Demo 1: Firmware Image Build & Verify ---
    std::cout << "--- Demo 1: Firmware Image Build & Verify ---\n";
    {
        // EN: Simulated firmware payload
        // TR: Simüle edilmiş firmware yükü — 512 byte'lık test verisi oluşturulur
        std::vector<uint8_t> fw_data(512, 0);
        for (size_t i = 0; i < fw_data.size(); ++i)
            fw_data[i] = static_cast<uint8_t>(i & 0xFF);

        auto img = ota::FirmwareImage::Build(2, 1, 0, "BCM_ECU", fw_data);
        std::cout << "  Built firmware image:\n";
        img.header.Print();
        std::cout << "  Header valid: " << img.VerifyHeader() << "\n";
        std::cout << "  Payload valid: " << img.VerifyPayload() << "\n";

        // EN: Corrupt and re-verify
        // TR: Boz ve tekrar doğrula — CRC uyuşmazlığının tespitini test eder
        img.payload[0] ^= 0xFF;
        std::cout << "  After corruption: payload valid = " << img.VerifyPayload() << "\n";
    }

    // --- Demo 2: A/B Partition OTA Flow ---
    std::cout << "\n--- Demo 2: A/B Partition — Normal Update ---\n";
    {
        ota::OTAManager mgr;
        mgr.InitCurrentFirmware(1, 0, 0);
        std::cout << "  Initial state:\n";
        mgr.PrintStatus();

        std::vector<uint8_t> fw(256, 0xAB);
        auto img = ota::FirmwareImage::Build(2, 0, 0, "BCM_ECU", fw);

        std::cout << "\n  Starting OTA update to v2.0.0:\n";
        bool ok = mgr.StartUpdate(img);
        std::cout << "\n  Result: " << (ok ? "SUCCESS" : "FAILED") << "\n";
        mgr.PrintStatus();
    }

    // --- Demo 3: Corrupted Image Detection ---
    std::cout << "\n--- Demo 3: Corrupted Image Detection ---\n";
    {
        ota::OTAManager mgr;
        mgr.InitCurrentFirmware(1, 0, 0);

        std::vector<uint8_t> fw(256, 0xCD);
        auto img = ota::FirmwareImage::Build(2, 1, 0, "BCM_ECU", fw);

        std::cout << "  Attempting corrupted update:\n";
        bool ok = mgr.StartCorruptUpdate(img);
        std::cout << "  Result: " << (ok ? "SUCCESS" : "BLOCKED") << "\n";
    }

    // --- Demo 4: Delta Update ---
    std::cout << "\n--- Demo 4: Delta Update (Binary Diff) ---\n";
    {
        std::vector<uint8_t> old_fw = {0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80};
        std::vector<uint8_t> new_fw = {0x10, 0x21, 0x30, 0x40, 0x55, 0x60, 0x70, 0x80, 0x90};

        auto patch = ota::delta::CreateDelta(old_fw, new_fw);
        std::cout << "  Old FW: " << old_fw.size() << " bytes\n"
                  << "  New FW: " << new_fw.size() << " bytes\n"
                  << "  Delta:  " << patch.entries.size() << " entries ("
                  << patch.PatchSize() << " bytes)\n"
                  << "  Savings: " << (100 - static_cast<int>(patch.PatchSize() * 100 / new_fw.size())) << "%\n";

        std::cout << "  Patch entries:\n";
        for (const auto& e : patch.entries) {
            std::cout << "    offset=" << e.offset
                      << " old=0x" << std::hex << static_cast<int>(e.old_value)
                      << " new=0x" << static_cast<int>(e.new_value) << std::dec << "\n";
        }

        auto patched = old_fw;
        bool ok = ota::delta::ApplyDelta(patched, patch);
        std::cout << "  Apply result: " << (ok ? "CRC MATCH" : "CRC MISMATCH") << "\n";
        std::cout << "  Patched == New: " << (patched == new_fw ? "YES" : "NO") << "\n";
    }

    // --- Demo 5: UDS Flash Sequence ---
    std::cout << "\n--- Demo 5: UDS Flash Programming Sequence ---\n";
    {
        ota::uds_flash::FlashProgrammer prog;

        prog.DiagSessionControl(0x02);  // Programming session
        prog.SecurityAccess(0xDEADBEEF);  // Authenticate
        prog.RequestDownload(0x08000000, 768);  // Start download

        // EN: Transfer 3 blocks of 256 bytes
        // TR: 256 byte'lık 3 blok transfer et — toplam 768 byte = RequestDownload boyutu
        std::vector<uint8_t> block(256, 0xAA);
        prog.TransferData(1, block);
        prog.TransferData(2, block);
        prog.TransferData(3, block);

        prog.RequestTransferExit();
        prog.RoutineControl_Verify();
    }

    // --- Demo 6: Uptane Framework Roles ---
    std::cout << "\n--- Demo 6: Uptane Framework (TUF-based OTA Security) ---\n";
    {
        auto roles = ota::uptane::GetRoles();
        std::cout << "  ┌──────────────┬──────────────────────────────────────────────┬────────┐\n";
        std::cout << "  │ Role         │ Description                                  │ Key ID │\n";
        std::cout << "  ├──────────────┼──────────────────────────────────────────────┼────────┤\n";
        for (const auto& r : roles) {
            std::cout << "  │ " << std::setw(12) << std::left << r.name
                      << " │ " << std::setw(44) << r.description
                      << " │ 0x" << std::hex << std::setw(4) << std::setfill('0')
                      << r.key_id << std::dec << std::setfill(' ') << " │\n";
        }
        std::cout << std::right;
        std::cout << "  └──────────────┴──────────────────────────────────────────────┴────────┘\n";

        std::cout << "\n  Uptane update flow:\n"
                  << "    1. OEM Backend → signs image (Targets role)\n"
                  << "    2. Snapshot role → consistent metadata set\n"
                  << "    3. Timestamp role → freshness (anti-replay)\n"
                  << "    4. Director → vehicle-specific instructions\n"
                  << "    5. Primary ECU → verifies all signatures\n"
                  << "    6. Primary ECU → distributes to secondaries\n";
    }

    std::cout << "\n[All demos completed successfully.]\n";
    return 0;
}
