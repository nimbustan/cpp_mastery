/**
 * @file module_16_autosar/04_automotive_cybersecurity.cpp
 * @brief Automotive Cybersecurity — SecOC, HSM, ISO 21434 / Otomotiv Siber Güvenlik
 *
 * @details
 * =============================================================================
 * [THEORY: Automotive Attack Vectors / TEORİ: Otomotiv Saldırı Vektörleri]
 * =============================================================================
 *
 * EN: Modern vehicles expose multiple attack surfaces:
 *
 *     ┌───────────────────────────────────────────────────────────┐
 *     │                  ATTACK SURFACE MAP                       │
 *     ├─────────────────────┬─────────────────────────────────────┤
 *     │ Physical Access     │ OBD-II port, CAN bus, debug (JTAG)  │
 *     │ Wireless            │ Wi-Fi, Bluetooth, cellular (4G/5G)  │
 *     │ V2X                 │ V2V (DSRC/C-V2X), V2I, V2C (cloud)  │
 *     │ Infotainment        │ USB, CD/DVD, OTA payload            │
 *     │ Sensor Spoofing     │ GPS, radar, LiDAR, camera           │
 *     └─────────────────────┴─────────────────────────────────────┘
 *
 *     Common CAN Attacks:
 *     ┌─────────────────────┬───────────────────────────────────────┐
 *     │ Attack              │ Description                           │
 *     ├─────────────────────┼───────────────────────────────────────┤
 *     │ CAN Injection       │ Send forged frames (no auth on CAN)   │
 *     │ Bus-Off Attack      │ Force ECU into error-passive/bus-off  │
 *     │ Replay Attack       │ Record and replay valid CAN frames    │
 *     │ Fuzzing             │ Send random CAN data to find bugs     │
 *     │ DoS (flooding)      │ High-priority frame flood             │
 *     └─────────────────────┴───────────────────────────────────────┘
 *
 * TR: Modern araçlar birden fazla saldırı yüzeyine sahiptir:
 *
 *     ┌─────────────────────┬─────────────────────────────────────┐
 *     │ Fiziksel Erişim     │ OBD-II portu, CAN bus, JTAG debug   │
 *     │ Kablosuz            │ Wi-Fi, Bluetooth, hücresel (4G/5G)  │
 *     │ V2X                 │ V2V (DSRC/C-V2X), V2I, V2C (bulut)  │
 *     │ Infotainment        │ USB, CD/DVD, OTA yükü               │
 *     │ Sensör Sahteleme    │ GPS, radar, LiDAR, kamera           │
 *     └─────────────────────┴─────────────────────────────────────┘
 *
 *     Yaygın CAN Saldırıları:
 *     ┌─────────────────────┬──────────────────────────────────────────┐
 *     │ Saldırı             │ Açıklama                                 │
 *     ├─────────────────────┼──────────────────────────────────────────┤
 *     │ CAN Injection       │ Sahte frame gönder (CAN'de auth yok)     │
 *     │ Bus-Off Saldırısı   │ ECU'yu error-passive/bus-off'a zorla     │
 *     │ Replay Saldırısı    │ Geçerli CAN frame'leri kaydet ve tekrarla│
 *     │ Fuzzing             │ Hata bulmak için rastgele CAN verisi     │
 *     │ DoS (flooding)      │ Yüksek öncelikli frame selı              │
 *     └─────────────────────┴──────────────────────────────────────────┘
 *
 * =============================================================================
 * [THEORY: SecOC — Secure Onboard Communication (AUTOSAR) / TEORİ: SecOC — Güvenli Araç İçi İletişim (AUTOSAR)]
 * =============================================================================
 *
 * EN: SecOC adds authentication to CAN/FlexRay/Ethernet frames:
 *     - Appends MAC (Message Authentication Code) to PDU
 *     - Uses Freshness Value (counter/timestamp) against replay attacks
 *     - CMAC (Cipher-based MAC) using AES-128 is typical
 *
 *     SecOC PDU format:
 *     ┌────────────────────┬──────────────────┬──────────────────┐
 *     │ Authentic PDU Data │ Freshness Value  │ MAC (truncated)  │
 *     │ (original payload) │ (counter/time)   │ (e.g. 28 bits)   │
 *     └────────────────────┴──────────────────┴──────────────────┘
 *
 *     SecOC TX Flow: PDU → get FreshnessValue → compute MAC → append → send
 *     SecOC RX Flow: receive → extract MAC → recompute → compare → accept/reject
 *
 * TR: SecOC, CAN/FlexRay/Ethernet frame'lerine kimlik doğrulama ekler:
 *     - PDU'ya MAC (Message Authentication Code) ekler
 *     - Replay saldırılarına karşı Freshness Value (sayaç/zaman damgası) kullanır
 *     - CMAC (AES-128 tabanlı şifre MAC) tipiktir
 *
 *     SecOC PDU formatı:
 *     ┌────────────────────┬──────────────────┬──────────────────┐
 *     │ Otantik PDU Verisi │ Freshness Value  │ MAC (kesik)      │
 *     │ (orijinal veri)    │ (sayaç/zaman)    │ (örn. 28 bit)    │
 *     └────────────────────┴──────────────────┴──────────────────┘
 *
 *     SecOC TX Akışı: PDU → FreshnessValue al → MAC hesapla → ekle → gönder
 *     SecOC RX Akışı: al → MAC çıkar → yeniden hesapla → karşılaştır → kabul/ret
 *
 * =============================================================================
 * [THEORY: HSM — Hardware Security Module / TEORİ: HSM — Donanım Güvenlik Modülü]
 * =============================================================================
 *
 * EN: HSM provides hardware-level security for ECUs:
 *     - Secure key storage (cannot be read out)
 *     - Hardware crypto acceleration (AES, SHA, RSA, ECDSA)
 *     - Secure boot verification
 *     - Random Number Generation (TRNG)
 *     Examples: Infineon AURIX HSM, NXP SHE/CSE, Renesas ICU-S
 *
 * TR: HSM, ECU'lar için donanım seviyesinde güvenlik sağlar:
 *     - Güvenli anahtar depolama (dışarı okunamaz)
 *     - Donanım kripto hızlandırma (AES, SHA, RSA, ECDSA)
 *     - Güvenli önyükleme doğrulaması
 *     - Gerçek Rastgele Sayı Üretici (TRNG)
 *     Örnekler: Infineon AURIX HSM, NXP SHE/CSE, Renesas ICU-S
 *
 * =============================================================================
 * [THEORY: UN R155 / ISO 21434 — Cybersecurity Management / TEORİ: UN R155 / ISO 21434 — Siber Güvenlik Yönetimi]
 * =============================================================================
 *
 * EN: Since July 2024, UN R155 mandates a Cybersecurity Management System
 *     (CSMS) for all new vehicle type approvals. ISO/SAE 21434 provides
 *     the engineering standard covering the vehicle lifecycle.
 *
 * TR: Temmuz 2024'ten itibaren UN R155, tüm yeni araç tip onayları için
 *     Siber Güvenlik Yönetim Sistemi (CSMS) zorunlu kılmaktadır.
 *
 * @note Compile: g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion 04_automotive_cybersecurity.cpp -o 04_automotive_cybersecurity
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
#include <iomanip>
#include <algorithm>
#include <numeric>
#include <cassert>
#include <functional>
#include <map>
#include <random>
#include <optional>

// ═════════════════════════════════════════════════════════════════════════════
// PART 1: AES-128 CMAC Simulation (Simplified)
// ═════════════════════════════════════════════════════════════════════════════

namespace crypto {

// EN: Simplified AES-128 CMAC for educational purposes.
//     Real implementations use hardware AES (HSM) or crypto libraries.
// TR: Eğitim amaçlı basitleştirilmiş AES-128 CMAC.
//     Gerçek implementasyonlar HSM veya kripto kütüphaneleri kullanır.

using Key128 = std::array<uint8_t, 16>;
using Block  = std::array<uint8_t, 16>;

// EN: Simplified block cipher (XOR-based, NOT cryptographically secure)
//     Used only to demonstrate the SecOC MAC computation flow
// TR: Basitleştirilmiş blok şifre (sadece SecOC akışını göstermek için)
class SimplifiedAES {
    Key128 key_;

public:
    explicit SimplifiedAES(const Key128& key) : key_(key) {}

    Block Encrypt(const Block& plaintext) const {
        Block cipher{};
        for (size_t i = 0; i < 16; ++i) {
            // EN: Multi-round XOR substitution (educational, NOT secure)
            // TR: Çok turlu XOR ikamesi (eğitim amaçlı, güvenli DEĞİL)
            uint8_t val = plaintext[i] ^ key_[i];
            val = static_cast<uint8_t>((val << 3) | (val >> 5));  // rotate
            val ^= key_[(i + 7) % 16];
            cipher[i] = val;
        }
        return cipher;
    }
};

// EN: CMAC computation (simplified version of NIST SP 800-38B)
// TR: CMAC hesaplama (NIST SP 800-38B basitleştirilmiş versiyonu)
class CMAC {
    SimplifiedAES aes_;
    Block         subkey1_{};
    Block         subkey2_{};

    static void LeftShift(Block& block) {
        uint8_t carry = 0;
        for (int i = 15; i >= 0; --i) {
            uint8_t new_carry = static_cast<uint8_t>((block[static_cast<size_t>(i)] >> 7) & 1u);
            block[static_cast<size_t>(i)] = static_cast<uint8_t>(
                (block[static_cast<size_t>(i)] << 1) | carry);
            carry = new_carry;
        }
    }

    void GenerateSubkeys() {
        Block zero{};
        zero.fill(0);
        Block l = aes_.Encrypt(zero);
        subkey1_ = l;
        LeftShift(subkey1_);
        if ((l[0] & 0x80u) != 0) subkey1_[15] ^= 0x87u;  // Rb
        subkey2_ = subkey1_;
        LeftShift(subkey2_);
        if ((subkey1_[0] & 0x80u) != 0) subkey2_[15] ^= 0x87u;
    }

public:
    explicit CMAC(const Key128& key) : aes_(key) { GenerateSubkeys(); }

    // EN: Compute CMAC over arbitrary-length message
    // TR: Rastgele uzunluktaki mesaj üzerinde CMAC hesapla
    Block Compute(const std::vector<uint8_t>& message) {
        size_t n_blocks = (message.empty()) ? 1 : (message.size() + 15) / 16;
        bool complete_block = (!message.empty()) && (message.size() % 16 == 0);

        Block x{};
        x.fill(0);

        for (size_t i = 0; i < n_blocks - 1; ++i) {
            for (size_t j = 0; j < 16; ++j)
                x[j] ^= message[i * 16 + j];
            x = aes_.Encrypt(x);
        }

        // EN: Last block — apply subkey
        // TR: Son blok — alt anahtar uygula
        Block last_block{};
        last_block.fill(0);
        size_t last_start = (n_blocks - 1) * 16;
        size_t remaining = message.size() - last_start;
        for (size_t j = 0; j < remaining; ++j)
            last_block[j] = message[last_start + j];

        if (complete_block) {
            for (size_t j = 0; j < 16; ++j)
                last_block[j] ^= subkey1_[j];
        } else {
            last_block[remaining] = 0x80u;  // padding
            for (size_t j = 0; j < 16; ++j)
                last_block[j] ^= subkey2_[j];
        }

        for (size_t j = 0; j < 16; ++j)
            x[j] ^= last_block[j];
        return aes_.Encrypt(x);
    }

    // EN: Truncate MAC to specified number of bits
    // TR: MAC'i belirtilen bit sayısına kırp
    static std::vector<uint8_t> Truncate(const Block& mac, uint32_t bits) {
        uint32_t bytes = (bits + 7) / 8;
        std::vector<uint8_t> result(mac.begin(),
            mac.begin() + static_cast<ptrdiff_t>(std::min(static_cast<size_t>(bytes), mac.size())));
        if (bits % 8 != 0) {
            uint8_t mask = static_cast<uint8_t>(0xFF << (8 - (bits % 8)));
            result.back() &= mask;
        }
        return result;
    }
};

}  // namespace crypto

// ═════════════════════════════════════════════════════════════════════════════
// PART 2: SecOC — Secure Onboard Communication
// ═════════════════════════════════════════════════════════════════════════════

namespace secoc {

// EN: Freshness Value Manager — counter-based
// TR: Tazelik Değeri Yöneticisi — sayaç tabanlı
class FreshnessManager {
    std::map<uint16_t, uint64_t> counters_;  // PDU_ID → counter

public:
    uint64_t GetTxFreshness(uint16_t pdu_id) {
        return counters_[pdu_id]++;
    }

    bool VerifyRxFreshness(uint16_t pdu_id, uint64_t received_fv) {
        auto& expected = counters_[pdu_id];
        // EN: Accept if received >= expected (allows for small gaps)
        // TR: Alınan >= beklenen ise kabul et
        if (received_fv >= expected) {
            expected = received_fv + 1;
            return true;
        }
        return false;  // replay or stale
    }
};

struct SecOC_PDU {
    uint16_t               pdu_id;
    std::vector<uint8_t>   payload;       // authentic data
    uint64_t               freshness_value;
    std::vector<uint8_t>   mac;           // truncated MAC
    bool                   authenticated = false;
};

// EN: SecOC module — handles TX authentication and RX verification
// TR: SecOC modülü — TX kimlik doğrulama ve RX doğrulama
class SecOCModule {
    crypto::CMAC      cmac_;
    FreshnessManager  fm_;
    uint32_t          mac_bits_;
    uint32_t          tx_count_ = 0;
    uint32_t          rx_pass_ = 0;
    uint32_t          rx_fail_ = 0;

    std::vector<uint8_t> BuildAuthData(const std::vector<uint8_t>& payload,
                                       uint64_t fv) {
        std::vector<uint8_t> data = payload;
        // EN: Append freshness value (big-endian, 8 bytes)
        // TR: Tazelik değerini ekle (big-endian, 8 byte)
        for (int i = 7; i >= 0; --i)
            data.push_back(static_cast<uint8_t>((fv >> (i * 8)) & 0xFF));
        return data;
    }

public:
    SecOCModule(const crypto::Key128& key, uint32_t mac_bits = 28)
        : cmac_(key), mac_bits_(mac_bits) {}

    // EN: Authenticate PDU for transmission
    // TR: İletim için PDU'yu kimlik doğrulama
    SecOC_PDU AuthenticateTx(uint16_t pdu_id, const std::vector<uint8_t>& payload) {
        SecOC_PDU pdu{};
        pdu.pdu_id = pdu_id;
        pdu.payload = payload;
        pdu.freshness_value = fm_.GetTxFreshness(pdu_id);

        auto auth_data = BuildAuthData(payload, pdu.freshness_value);
        auto full_mac = cmac_.Compute(auth_data);
        pdu.mac = crypto::CMAC::Truncate(full_mac, mac_bits_);
        pdu.authenticated = true;
        ++tx_count_;
        return pdu;
    }

    // EN: Verify received PDU
    // TR: Alınan PDU'yu doğrula
    bool VerifyRx(const SecOC_PDU& pdu) {
        // EN: Step 1: Check freshness
        // TR: Adım 1: Tazelik kontrolü
        if (!fm_.VerifyRxFreshness(pdu.pdu_id, pdu.freshness_value)) {
            ++rx_fail_;
            return false;
        }

        // EN: Step 2: Recompute MAC and compare
        // TR: Adım 2: MAC'i yeniden hesapla ve karşılaştır
        auto auth_data = BuildAuthData(pdu.payload, pdu.freshness_value);
        auto full_mac = cmac_.Compute(auth_data);
        auto expected_mac = crypto::CMAC::Truncate(full_mac, mac_bits_);

        if (expected_mac != pdu.mac) {
            ++rx_fail_;
            return false;
        }

        ++rx_pass_;
        return true;
    }

    void PrintStats() const {
        std::cout << "  [SecOC] TX=" << tx_count_
                  << " RX_PASS=" << rx_pass_
                  << " RX_FAIL=" << rx_fail_ << "\n";
    }
};

}  // namespace secoc

// ═════════════════════════════════════════════════════════════════════════════
// PART 3: HSM Simulation — Key Management
// ═════════════════════════════════════════════════════════════════════════════

namespace hsm {

// EN: Simulated Hardware Security Module
// TR: Simüle edilmiş Donanım Güvenlik Modülü

enum class KeySlot : uint8_t {
    MASTER_ECU_KEY = 0,
    SECOC_KEY_1    = 1,
    SECOC_KEY_2    = 2,
    SECURE_BOOT_KEY= 3,
    OTA_VERIFY_KEY = 4,
    MAX_SLOTS      = 8
};

class HSM {
    std::array<crypto::Key128, 8> key_store_{};
    std::array<bool, 8>           key_valid_{};
    bool                          locked_ = false;
    uint32_t                      crypto_ops_ = 0;

public:
    HSM() { key_valid_.fill(false); }

    // EN: Store key in HSM (only before lock)
    // TR: HSM'e anahtar depola (sadece kilitlenmeden önce)
    bool StoreKey(KeySlot slot, const crypto::Key128& key) {
        if (locked_) {
            std::cout << "  [HSM] ERROR: Cannot store key — HSM is locked\n";
            return false;
        }
        auto idx = static_cast<size_t>(slot);
        if (idx >= key_store_.size()) return false;
        key_store_[idx] = key;
        key_valid_[idx] = true;
        std::cout << "  [HSM] Key stored in slot " << idx << "\n";
        return true;
    }

    // EN: Lock HSM — no more key provisioning
    // TR: HSM'i kilitle — artık anahtar yüklenemez
    void Lock() {
        locked_ = true;
        std::cout << "  [HSM] Locked — key provisioning disabled\n";
    }

    // EN: Compute MAC using key from slot (key never leaves HSM)
    // TR: Slot'taki anahtarı kullanarak MAC hesapla (anahtar HSM'den çıkmaz)
    std::optional<crypto::Block> ComputeMAC(KeySlot slot,
                                            const std::vector<uint8_t>& data) {
        auto idx = static_cast<size_t>(slot);
        if (idx >= key_store_.size() || !key_valid_[idx]) return std::nullopt;
        crypto::CMAC cmac(key_store_[idx]);
        ++crypto_ops_;
        return cmac.Compute(data);
    }

    // EN: Generate random number (TRNG)
    // TR: Rastgele sayı üret (TRNG)
    std::array<uint8_t, 16> GenerateRandom() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<uint16_t> dist(0, 255);
        std::array<uint8_t, 16> random{};
        for (auto& b : random) b = static_cast<uint8_t>(dist(gen));
        ++crypto_ops_;
        return random;
    }

    void PrintStatus() const {
        std::cout << "  [HSM] Locked=" << locked_
                  << " CryptoOps=" << crypto_ops_ << "\n";
        for (size_t i = 0; i < key_valid_.size(); ++i) {
            if (key_valid_[i])
                std::cout << "    Slot " << i << ": KEY_PRESENT\n";
        }
    }
};

}  // namespace hsm

// ═════════════════════════════════════════════════════════════════════════════
// PART 4: Secure Boot Chain
// ═════════════════════════════════════════════════════════════════════════════

namespace secure_boot {

// EN: Simple hash for educational secure boot simulation
// TR: Eğitim amaçlı güvenli başlatma simülasyonu için basit hash

uint32_t SimpleHash(const std::vector<uint8_t>& data) {
    uint32_t hash = 0x811C9DC5u;  // FNV-1a offset basis
    for (auto b : data) {
        hash ^= b;
        hash *= 0x01000193u;      // FNV-1a prime
    }
    return hash;
}

struct BootStage {
    std::string        name;
    std::vector<uint8_t> image;
    uint32_t           expected_hash;
    bool               verified = false;
};

class SecureBootChain {
    std::vector<BootStage> stages_;

public:
    void AddStage(const std::string& name, const std::vector<uint8_t>& image) {
        uint32_t hash = SimpleHash(image);
        stages_.push_back({name, image, hash, false});
    }

    bool ExecuteBootChain() {
        std::cout << "  Secure Boot Chain:\n";
        for (auto& stage : stages_) {
            uint32_t computed = SimpleHash(stage.image);
            stage.verified = (computed == stage.expected_hash);
            std::cout << "    " << stage.name << " — hash=0x"
                      << std::hex << computed << " expected=0x"
                      << stage.expected_hash << std::dec
                      << (stage.verified ? " → PASS" : " → FAIL") << "\n";
            if (!stage.verified) {
                std::cout << "    BOOT HALTED at " << stage.name << "!\n";
                return false;
            }
        }
        std::cout << "    All stages verified → Boot complete\n";
        return true;
    }

    // EN: Simulate tampered image
    // TR: Değiştirilmiş image simülasyonu
    void TamperStage(size_t index) {
        if (index < stages_.size() && !stages_[index].image.empty()) {
            stages_[index].image[0] ^= 0xFF;  // flip bits
        }
    }
};

}  // namespace secure_boot

// ═════════════════════════════════════════════════════════════════════════════
// PART 5: X.509 Certificate Simplified & TLS Handshake Overview
// ═════════════════════════════════════════════════════════════════════════════

namespace pki {

struct Certificate {
    std::string subject;
    std::string issuer;
    uint32_t    serial_number;
    std::string valid_from;
    std::string valid_to;
    std::string public_key_algo;
    uint32_t    fingerprint;  // simplified
    bool        is_ca = false;
};

class CertificateChain {
    std::vector<Certificate> chain_;

public:
    void AddCert(const Certificate& cert) { chain_.push_back(cert); }

    bool Verify() const {
        if (chain_.empty()) return false;
        std::cout << "  Certificate Chain Verification:\n";
        for (size_t i = 0; i < chain_.size(); ++i) {
            const auto& cert = chain_[i];
            bool issuer_ok = (i + 1 < chain_.size()) ?
                (cert.issuer == chain_[i + 1].subject) : cert.is_ca;
            std::cout << "    [" << i << "] Subject: " << cert.subject
                      << " | Issuer: " << cert.issuer
                      << " | " << (issuer_ok ? "OK" : "BROKEN") << "\n";
            if (!issuer_ok) return false;
        }
        return true;
    }
};

}  // namespace pki

// ═════════════════════════════════════════════════════════════════════════════
// PART 6: CAN Intrusion Detection System (IDS)
// ═════════════════════════════════════════════════════════════════════════════

namespace ids {

struct CANFrame {
    uint32_t  id;
    uint8_t   dlc;
    std::array<uint8_t, 8> data;
    uint64_t  timestamp_us;
};

// EN: Simple IDS — detects frequency anomalies and unknown IDs
// TR: Basit IDS — frekans anomalileri ve bilinmeyen ID'leri tespit eder
class AutomotiveIDS {
    std::map<uint32_t, uint32_t> known_ids_;    // id → expected period_us
    std::map<uint32_t, uint64_t> last_seen_;     // id → last timestamp
    std::map<uint32_t, uint32_t> violation_cnt_; // id → violation count
    uint32_t unknown_id_count_ = 0;
    static constexpr double FREQ_TOLERANCE = 0.5;  // 50% tolerance

public:
    void RegisterExpectedID(uint32_t id, uint32_t period_us) {
        known_ids_[id] = period_us;
    }

    enum class Verdict { OK, FREQUENCY_ANOMALY, UNKNOWN_ID };

    Verdict Analyze(const CANFrame& frame) {
        auto it = known_ids_.find(frame.id);
        if (it == known_ids_.end()) {
            ++unknown_id_count_;
            return Verdict::UNKNOWN_ID;
        }

        auto last_it = last_seen_.find(frame.id);
        if (last_it != last_seen_.end()) {
            uint64_t delta = frame.timestamp_us - last_it->second;
            double expected = static_cast<double>(it->second);
            double ratio = static_cast<double>(delta) / expected;
            if (ratio < (1.0 - FREQ_TOLERANCE) || ratio > (1.0 + FREQ_TOLERANCE)) {
                violation_cnt_[frame.id]++;
                last_seen_[frame.id] = frame.timestamp_us;
                return Verdict::FREQUENCY_ANOMALY;
            }
        }
        last_seen_[frame.id] = frame.timestamp_us;
        return Verdict::OK;
    }

    void PrintReport() const {
        std::cout << "  [IDS] Unknown IDs detected: " << unknown_id_count_ << "\n";
        for (const auto& [id, cnt] : violation_cnt_) {
            std::cout << "  [IDS] ID 0x" << std::hex << id << std::dec
                      << " frequency violations: " << cnt << "\n";
        }
    }
};

}  // namespace ids

// ═════════════════════════════════════════════════════════════════════════════
// MAIN — Demos
// ═════════════════════════════════════════════════════════════════════════════

int main() {
    std::cout << "╔══════════════════════════════════════════════════════════════╗\n"
              << "║ Module 16: Automotive Cybersecurity — SecOC, HSM, IDS        ║\n"
              << "╚══════════════════════════════════════════════════════════════╝\n\n";

    // --- Demo 1: SecOC — Authenticate and Verify ---
    std::cout << "--- Demo 1: SecOC — Secure Onboard Communication ---\n";
    {
        crypto::Key128 key = {0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6,
                              0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C};
        secoc::SecOCModule tx_secoc(key);
        secoc::SecOCModule rx_secoc(key);

        // EN: TX side authenticates, RX side verifies
        // TR: TX tarafı kimlik doğrular, RX tarafı doğrular
        auto pdu1 = tx_secoc.AuthenticateTx(0x100, {0x11, 0x22, 0x33, 0x44});
        auto pdu2 = tx_secoc.AuthenticateTx(0x100, {0xAA, 0xBB, 0xCC, 0xDD});

        std::cout << "  PDU1 MAC: ";
        for (auto b : pdu1.mac) std::cout << std::hex << static_cast<int>(b) << " ";
        std::cout << std::dec << " FV=" << pdu1.freshness_value << "\n";

        bool v1 = rx_secoc.VerifyRx(pdu1);
        bool v2 = rx_secoc.VerifyRx(pdu2);
        std::cout << "  PDU1 verify: " << (v1 ? "PASS" : "FAIL") << "\n";
        std::cout << "  PDU2 verify: " << (v2 ? "PASS" : "FAIL") << "\n";

        // EN: Replay attack — send PDU1 again (stale freshness)
        // TR: Tekrar saldırısı — PDU1'i tekrar gönder (eski tazelik)
        bool v3 = rx_secoc.VerifyRx(pdu1);
        std::cout << "  PDU1 replay: " << (v3 ? "PASS" : "FAIL (replay blocked)") << "\n";

        // EN: Tampered payload
        // TR: Değiştirilmiş veri
        auto pdu_tampered = tx_secoc.AuthenticateTx(0x100, {0xFF, 0xEE, 0xDD, 0xCC});
        pdu_tampered.payload[0] = 0x00;  // tamper
        bool v4 = rx_secoc.VerifyRx(pdu_tampered);
        std::cout << "  Tampered PDU: " << (v4 ? "PASS" : "FAIL (tamper detected)") << "\n";

        tx_secoc.PrintStats();
        rx_secoc.PrintStats();
    }

    // --- Demo 2: HSM — Key Management ---
    std::cout << "\n--- Demo 2: HSM — Hardware Security Module ---\n";
    {
        hsm::HSM module;
        crypto::Key128 secoc_key = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                                    0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10};
        module.StoreKey(hsm::KeySlot::SECOC_KEY_1, secoc_key);
        module.StoreKey(hsm::KeySlot::SECURE_BOOT_KEY, {0xAA, 0xBB, 0xCC, 0xDD,
            0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC});
        module.Lock();

        // EN: Try to store key after lock — should fail
        // TR: Kilitlenmeden sonra anahtar depolamayı dene — başarısız olmalı
        module.StoreKey(hsm::KeySlot::SECOC_KEY_2, secoc_key);

        // EN: Compute MAC using HSM (key stays inside)
        // TR: HSM kullanarak MAC hesapla (anahtar içeride kalır)
        auto mac = module.ComputeMAC(hsm::KeySlot::SECOC_KEY_1,
                                     {0x10, 0x20, 0x30, 0x40});
        if (mac) {
            std::cout << "  MAC computed: ";
            for (size_t i = 0; i < 4; ++i)
                std::cout << std::hex << static_cast<int>((*mac)[i]) << " ";
            std::cout << "...\n" << std::dec;
        }

        auto rng = module.GenerateRandom();
        std::cout << "  TRNG: ";
        for (size_t i = 0; i < 4; ++i)
            std::cout << std::hex << static_cast<int>(rng[i]) << " ";
        std::cout << "...\n" << std::dec;

        module.PrintStatus();
    }

    // --- Demo 3: Secure Boot Chain ---
    std::cout << "\n--- Demo 3: Secure Boot Chain ---\n";
    {
        secure_boot::SecureBootChain chain;
        chain.AddStage("ROM_Bootloader",  {0x10, 0x20, 0x30, 0x40, 0x50});
        chain.AddStage("1st_Stage_Boot",  {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF});
        chain.AddStage("2nd_Stage_Boot",  {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07});
        chain.AddStage("Application_FW",  {0x55, 0x66, 0x77, 0x88, 0x99});

        std::cout << "  === Normal Boot ===\n";
        chain.ExecuteBootChain();

        std::cout << "\n  === Tampered Boot (stage 2) ===\n";
        secure_boot::SecureBootChain chain2;
        chain2.AddStage("ROM_Bootloader", {0x10, 0x20, 0x30, 0x40, 0x50});
        chain2.AddStage("1st_Stage_Boot", {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF});
        chain2.AddStage("2nd_Stage_TAMPERED", {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07});
        chain2.TamperStage(2);
        chain2.ExecuteBootChain();
    }

    // --- Demo 4: Certificate Chain ---
    std::cout << "\n--- Demo 4: X.509 Certificate Chain (Simplified) ---\n";
    {
        pki::CertificateChain chain;
        chain.AddCert({"ECU_RadarFrontLeft", "OEM_SubCA", 1001, "2025-01-01",
                        "2030-01-01", "ECDSA-P256", 0xABCD, false});
        chain.AddCert({"OEM_SubCA", "OEM_RootCA", 500, "2024-01-01",
                        "2034-01-01", "ECDSA-P384", 0x1234, true});
        chain.AddCert({"OEM_RootCA", "OEM_RootCA", 1, "2020-01-01",
                        "2040-01-01", "RSA-4096", 0x0001, true});
        bool valid = chain.Verify();
        std::cout << "  Chain verification: " << (valid ? "VALID" : "INVALID") << "\n";
    }

    // --- Demo 5: CAN Intrusion Detection ---
    std::cout << "\n--- Demo 5: CAN Intrusion Detection System (IDS) ---\n";
    {
        ids::AutomotiveIDS detector;
        detector.RegisterExpectedID(0x100, 10000);   // 10ms period
        detector.RegisterExpectedID(0x200, 20000);   // 20ms period
        detector.RegisterExpectedID(0x300, 100000);  // 100ms period

        // EN: Normal traffic
        // TR: Normal trafik
        ids::CANFrame frames[] = {
            {0x100, 8, {}, 0},
            {0x100, 8, {}, 10000},
            {0x100, 8, {}, 20000},
            {0x200, 8, {}, 5000},
            {0x200, 8, {}, 25000},
            // EN: Anomaly: 0x100 too fast (injection attempt)
            // TR: Anomali: 0x100 çok hızlı (enjeksiyon denemesi)
            {0x100, 8, {}, 21000},  // 1ms after last → too fast
            // EN: Unknown ID (possible injection)
            // TR: Bilinmeyen ID (olası enjeksiyon)
            {0x666, 8, {}, 22000},
            {0x100, 8, {}, 31000},  // normal again
        };

        const char* verdict_str[] = {"OK", "FREQ_ANOMALY", "UNKNOWN_ID"};
        for (const auto& f : frames) {
            auto v = detector.Analyze(f);
            std::cout << "  ID=0x" << std::hex << f.id << std::dec
                      << " @" << f.timestamp_us << "us → "
                      << verdict_str[static_cast<int>(v)] << "\n";
        }
        detector.PrintReport();
    }

    // --- Demo 6: Attack Vector Summary ---
    std::cout << "\n--- Demo 6: Cybersecurity Standards Summary ---\n";
    {
        std::cout << "  ┌──────────────────────┬──────────────────────────────────────────┐\n";
        std::cout << "  │ Standard             │ Scope                                    │\n";
        std::cout << "  ├──────────────────────┼──────────────────────────────────────────┤\n";
        std::cout << "  │ UN R155              │ CSMS mandatory for type approval (2024)  │\n";
        std::cout << "  │ ISO/SAE 21434        │ Cybersecurity engineering lifecycle      │\n";
        std::cout << "  │ ISO 11452 / CISPR 25 │ EMC, but related to signal integrity     │\n";
        std::cout << "  │ AUTOSAR SecOC        │ Onboard CAN/FR/ETH message auth          │\n";
        std::cout << "  │ AUTOSAR IdsM         │ Intrusion Detection System Manager       │\n";
        std::cout << "  │ IEEE 1609.2          │ V2X security (PKI, certificates)         │\n";
        std::cout << "  │ Uptane (TUF)         │ Secure OTA framework for automotive      │\n";
        std::cout << "  └──────────────────────┴──────────────────────────────────────────┘\n";
    }

    std::cout << "\n[All demos completed successfully.]\n";
    return 0;
}
