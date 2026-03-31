/**
 * @file module_02_oop/07_nested_and_local_classes.cpp
 * @brief OOP Part 7: Nested & Local Classes — OOP Kısım 7: İç İçe ve Yerel Sınıflar
 *
 * @details
 * =============================================================================
 * [THEORY: Nested Classes / TEORİ: İç İçe Sınıflar (Nested Classes)]
 * =============================================================================
 * EN: A nested class is a class defined inside another class. The inner class logically belongs
 * to the outer class and is scoped within it. Think of a CAN message: the Header (arbitration
 * ID, DLC, flags) only makes sense as part of a CanMessage. Defining Header inside CanMessage
 * keeps it tightly coupled and prevents polluting the global namespace.
 *
 * TR: İç içe sınıf (nested class), başka bir sınıfın içinde tanımlanan sınıftır. İç sınıf,
 * mantıksal olarak dış sınıfa aittir ve onun kapsamında yer alır. Bir CAN mesajı düşünün: Header
 * (arbitrasyon ID, DLC, bayraklar) yalnızca bir CanMessage'ın parçası olarak anlam taşır.
 * Header'ı CanMessage içinde tanımlamak, sıkı bağlılığı korur ve global isim alanını kirletmez.
 *
 * =============================================================================
 * [THEORY: Access Rules / TEORİ: Erişim Kuralları]
 * =============================================================================
 * EN: Since C++11, a nested class can access the private and protected members of its enclosing
 * class (given an instance of the outer class). The outer class, however, has NO special access
 * to the nested class's private members. This asymmetry is important for encapsulation.
 *
 * TR: C++11'den beri, iç içe sınıf, dış sınıfın private ve protected üyelerine erişebilir (dış
 * sınıfın bir örneği verildiğinde). Ancak dış sınıfın, iç sınıfın private üyelerine özel bir
 * erişimi YOKTUR. Bu asimetri, kapsülleme açısından önemlidir.
 *
 * =============================================================================
 * [THEORY: Local Classes / TEORİ: Yerel Sınıflar (Local Classes)]
 * =============================================================================
 * EN: A local class is defined inside a function body. Its scope is limited to that function —
 * it cannot be used outside. Local classes cannot access the function's automatic (local)
 * variables but can access static locals and global variables. Use cases are rare; lambdas often
 * replace them.
 *
 * TR: Yerel sınıf, bir fonksiyon gövdesi içinde tanımlanan sınıftır. Kapsamı o fonksiyonla
 * sınırlıdır — dışarıda kullanılamaz. Yerel sınıflar, fonksiyonun otomatik (yerel)
 * değişkenlerine erişemez ama statik yerel ve global değişkenlere erişebilir. Kullanım alanı
 * nadirdir; lambdalar genellikle onların yerini alır.
 *
 * =============================================================================
 * [THEORY: PIMPL Idiom / TEORİ: PIMPL Kalıbı]
 * =============================================================================
 * EN: PIMPL (Pointer to IMPLementation) hides implementation details behind a pointer to a
 * private nested class/struct (often called Impl). The public class header only forward-declares
 * Impl, keeping compile-time dependencies minimal. This is widely used in automotive ECU
 * codebases to stabilize ABIs.
 *
 * TR: PIMPL (Pointer to IMPLementation), uygulama detaylarını özel bir iç sınıf/yapıya
 * (genellikle Impl olarak adlandırılır) yönelik bir işaretçinin arkasına gizler. Public sınıf
 * başlığı yalnızca Impl'i ileri bildirir ve derleme zamanı bağımlılıklarını minimum tutar.
 * Otomotiv ECU kod tabanlarında ABI kararlılığı sağlamak için yaygın olarak kullanılır.
 *
 * [CPPREF DEPTH: Nested Class Access Rules and the Pimpl Compilation Firewall / CPPREF DERİNLİK:
 * İç İçe Sınıf Erişim Kuralları ve Pimpl Derleme Güvenlik Duvarı]
 * =============================================================================
 * EN: Before C++11, nested classes had NO special access to the enclosing class's private
 * members — they were treated like any other class. The C++11 standard (§11.7) changed this: a
 * nested class is now a MEMBER of the enclosing class and can access all private/protected names
 * (given an instance of the outer class). This is what makes the Pimpl idiom clean. The PIMPL
 * compilation firewall works because the public header only contains `class Impl;` (forward
 * declaration) and `std::unique_ptr<Impl>`. Since unique_ptr's destructor requires Impl to be
 * COMPLETE (it calls `delete`), you MUST define the outer class's destructor in the .cpp file
 * where Impl is fully defined. Forgetting this causes a compile error: "incomplete type" in the
 * unique_ptr destructor instantiation. PIMPL reduces transitive #include dependencies
 * dramatically. A change to Impl's data members or private methods does NOT change the public
 * header → no recompilation of dependent translation units. In large automotive codebases
 * (millions of LOC), this saves hours of build time. Performance cost: every call to an Impl
 * method is an extra pointer indirection, and Impl lives on the heap (cache-unfriendly). For hot
 * paths, consider "fast Pimpl" with aligned storage or a fixed buffer.
 *
 * TR: C++11 öncesinde iç içe sınıflar, çevreleyen sınıfın private üyelerine ÖZEL bir erişime
 * sahip değildi. C++11 standardı (§11.7) bunu değiştirdi: iç içe sınıf artık çevreleyen sınıfın
 * bir ÜYESİDİR ve tüm private/protected isimlere erişebilir (dış sınıfın bir örneği
 * verildiğinde). PIMPL derleme güvenlik duvarı şöyle çalışır: public başlık yalnızca `class
 * Impl;` (ileri bildirim) ve `std::unique_ptr<Impl>` içerir. unique_ptr'nin yıkıcısı Impl'in TAM
 * olmasını gerektirir (`delete` çağırır), bu yüzden dış sınıfın yıkıcısını Impl'in tamamen
 * tanımlandığı .cpp dosyasında tanımlamanız GEREKİR. Bunu unutmak derleme hatasına neden olur.
 * PIMPL, geçişli #include bağımlılıklarını dramatik biçimde azaltır. Impl'in veri üyelerindeki
 * değişiklik public başlığı DEĞİŞTİRMEZ → bağımlı çeviri birimlerinin yeniden derlenmesi
 * gerekmez. Büyük otomotiv kod tabanlarında bu, saatlerce derleme süresinden tasarruf sağlar.
 * Performans maliyeti: her Impl metodu çağrısı ekstra bir işaretçi yönlendirmesidir ve Impl
 * heap'te yaşar (önbellek dostu değil).
 *
 * ⚠  Ref: https://en.cppreference.com/w/cpp/language/nested_types
 *
 * @note Compile: g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion 07_nested_and_local_classes.cpp -o 07_nested_and_local_classes
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <iostream>   // EN: Standard I/O for console output
#include <string>     // EN: String class for text handling
#include <vector>     // EN: Dynamic array container
#include <array>      // EN: Fixed-size array container
#include <cstdint>    // EN: Fixed-width integer types (uint32_t, uint8_t)
#include <memory>     // EN: Smart pointers (unique_ptr) for PIMPL
#include <algorithm>  // EN: std::count_if for validation demo

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// 1. CanMessage with Nested Header and Payload Classes
// ═════════════════════════════════════════════════════════════════════════════════════════════════

// EN: A CAN message class with nested Header and Payload — mimics a real CAN 2.0 frame.
// TR: İç içe Header ve Payload içeren bir CAN mesaj sınıfı — gerçek bir CAN 2.0 çerçevesini
// taklit eder.
class CanMessage {
public:
    // ─── 1a. Nested Header ───────────────────────────────────────────────────────────────────────
    class Header {
    public:
        Header() = default;
        Header(uint32_t id, uint8_t dlc, bool is_extended, bool is_rtr)
            : arb_id_(id), dlc_(dlc), extended_frame_(is_extended), rtr_(is_rtr) {}

        uint32_t getArbId()       const { return arb_id_; }
        uint8_t  getDlc()         const { return dlc_; }
        bool     isExtended()     const { return extended_frame_; }
        bool     isRtr()          const { return rtr_; }

        void setArbId(uint32_t id)      { arb_id_ = id; }
        void setDlc(uint8_t dlc)        { dlc_ = (dlc <= 8) ? dlc : 8; }
        void setExtended(bool ext)      { extended_frame_ = ext; }
        void setRtr(bool rtr)           { rtr_ = rtr; }

    private:
        uint32_t arb_id_        = 0x000;
        uint8_t  dlc_           = 0;
        bool     extended_frame_ = false;
        bool     rtr_           = false;
    };

    // ─── 1b. Nested Payload ──────────────────────────────────────────────────────────────────────
    class Payload {
    public:
        Payload() { data_.fill(0x00); }

        void setByte(size_t index, uint8_t value) {
            if (index < 8) data_[index] = value;
        }
        uint8_t getByte(size_t index) const {
            return (index < 8) ? data_[index] : 0;
        }
        const std::array<uint8_t, 8>& raw() const { return data_; }

    private:
        std::array<uint8_t, 8> data_;
    };

    // ─── 1c. CanMessage public interface ─────────────────────────────────────────────────────────
    CanMessage() = default;
    CanMessage(uint32_t id, uint8_t dlc) {
        header_.setArbId(id);
        header_.setDlc(dlc);
    }

    Header&       header()        { return header_; }
    const Header& header()  const { return header_; }
    Payload&      payload()       { return payload_; }
    const Payload& payload() const { return payload_; }

    void print() const {
        std::cout << "CAN Frame [ID=0x" << std::hex << header_.getArbId()
                  << std::dec << " DLC=" << static_cast<int>(header_.getDlc())
                  << " EXT=" << header_.isExtended()
                  << " RTR=" << header_.isRtr() << "] Data: ";
        for (uint8_t i = 0; i < header_.getDlc(); ++i) {
            std::cout << "0x" << std::hex
                      << static_cast<int>(payload_.getByte(i)) << " ";
        }
        std::cout << std::dec << "\n";
    }

private:
    Header  header_;
    Payload payload_;
};

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// 2. ECUDiagnosticSession with PIMPL-like Nested Impl
// ═════════════════════════════════════════════════════════════════════════════════════════════════

// EN: Diagnostic session class hiding internal state via a nested Impl struct.
// TR: İç içe Impl yapısı ile iç durumu gizleyen teşhis oturumu sınıfı.
class ECUDiagnosticSession {
public:
    ECUDiagnosticSession();
    ~ECUDiagnosticSession();

    // EN: Public interface — delegates to Impl
    // TR: Public arayüz — Impl'e delege eder
    void startSession(uint16_t session_id);
    bool authenticate(const std::string& key);
    void storeDtc(const std::string& dtc_code);
    std::vector<std::string> readDtcs() const;
    void printStatus() const;

private:
    // EN: Forward-declared nested struct holds all private state
    // TR: İleri bildirimli iç yapı, tüm özel durumu tutar
    struct Impl;
    std::unique_ptr<Impl> pimpl_;
};

// ─── 2a. Impl definition ─────────────────────────────────────────────────────────────────────────
struct ECUDiagnosticSession::Impl {
    uint16_t session_id       = 0;
    bool     authenticated    = false;
    std::vector<std::string> dtc_buffer;

    static constexpr const char* SECRET_KEY = "ECU_AUTH_2026";
};

ECUDiagnosticSession::ECUDiagnosticSession()
    : pimpl_(std::make_unique<Impl>()) {}

ECUDiagnosticSession::~ECUDiagnosticSession() = default;

void ECUDiagnosticSession::startSession(uint16_t session_id) {
    pimpl_->session_id    = session_id;
    pimpl_->authenticated = false;
    pimpl_->dtc_buffer.clear();
    std::cout << "[ECU] Session 0x" << std::hex << session_id
              << std::dec << " started.\n";
}

bool ECUDiagnosticSession::authenticate(const std::string& key) {
    pimpl_->authenticated = (key == Impl::SECRET_KEY);
    std::cout << "[ECU] Authentication "
              << (pimpl_->authenticated ? "SUCCESS" : "FAILED") << "\n";
    return pimpl_->authenticated;
}

void ECUDiagnosticSession::storeDtc(const std::string& dtc_code) {
    if (!pimpl_->authenticated) {
        std::cout << "[ECU] ERROR: Not authenticated. Cannot store DTC.\n";
        return;
    }
    pimpl_->dtc_buffer.push_back(dtc_code);
}

std::vector<std::string> ECUDiagnosticSession::readDtcs() const {
    if (!pimpl_->authenticated) {
        std::cout << "[ECU] ERROR: Not authenticated. Cannot read DTCs.\n";
        return {};
    }
    return pimpl_->dtc_buffer;
}

void ECUDiagnosticSession::printStatus() const {
    std::cout << "[ECU] Session=0x" << std::hex << pimpl_->session_id
              << std::dec
              << " Auth=" << (pimpl_->authenticated ? "YES" : "NO")
              << " DTCs=" << pimpl_->dtc_buffer.size() << "\n";
}

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// 3. Local Class inside a Function
// ═════════════════════════════════════════════════════════════════════════════════════════════════

// EN: Demonstrates a local class used for single-purpose CAN traffic validation.
// TR: Tek amaçlı CAN trafik doğrulaması için kullanılan yerel sınıfı gösterir.
void validateCanTraffic(const std::vector<CanMessage>& traffic) {
    // EN: Local struct — only visible within this function
    // TR: Yerel yapı — yalnızca bu fonksiyon içinde görünür
    struct FrameValidator {
        static bool isValid(const CanMessage& msg) {
            // EN: Standard CAN ID must be <= 0x7FF, DLC must be 1-8
            // TR: Standart CAN ID <= 0x7FF olmalı, DLC 1-8 arasında olmalı
            if (msg.header().isExtended()) return true;  // skip extended check
            return msg.header().getArbId() <= 0x7FF &&
                   msg.header().getDlc() >= 1 &&
                   msg.header().getDlc() <= 8;
        }
    };

    auto valid_count = std::count_if(traffic.begin(), traffic.end(),
                                     FrameValidator::isValid);
    std::cout << "[Validator] " << valid_count << " / " << traffic.size()
              << " frames are valid.\n";

    for (size_t i = 0; i < traffic.size(); ++i) {
        std::cout << "  Frame " << i << ": "
                  << (FrameValidator::isValid(traffic[i]) ? "OK" : "INVALID")
                  << "\n";
    }
}

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// 4. Nested Class Accessing Outer's Private Members
// ═════════════════════════════════════════════════════════════════════════════════════════════════

// EN: Demonstrates that a nested class can access private members of the outer class.
// TR: İç sınıfın, dış sınıfın private üyelerine erişebildiğini gösterir.
class SensorHub {
public:
    // EN: Nested Inspector can read SensorHub's private calibration_offset_
    // TR: İç Inspector, SensorHub'ın private calibration_offset_ alanını okuyabilir
    class Inspector {
    public:
        static void diagnose(const SensorHub& hub) {
            std::cout << "[Inspector] Hub '" << hub.name_
                      << "' calibration offset = " << hub.calibration_offset_
                      << ", sensor count = " << hub.readings_.size() << "\n";
        }
    };

    explicit SensorHub(std::string name, double offset = 0.0)
        : name_(std::move(name)), calibration_offset_(offset) {}

    void addReading(double value) { readings_.push_back(value + calibration_offset_); }

    void printReadings() const {
        std::cout << "[SensorHub:" << name_ << "] Readings: ";
        for (double r : readings_) std::cout << r << " ";
        std::cout << "\n";
    }

private:
    std::string name_;
    double calibration_offset_;
    std::vector<double> readings_;
};

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// main() — Demos
// ═════════════════════════════════════════════════════════════════════════════════════════════════
int main() {
    std::cout << "=== Demo 1: CanMessage with Nested Header & Payload ===\n";
    // EN: Construct a CAN message, set header fields, fill payload, print
    // TR: Bir CAN mesajı oluştur, başlık alanlarını ayarla, veriyi doldur, yazdır
    CanMessage msg(0x1A3, 4);
    msg.payload().setByte(0, 0xDE);
    msg.payload().setByte(1, 0xAD);
    msg.payload().setByte(2, 0xBE);
    msg.payload().setByte(3, 0xEF);
    msg.print();

    // EN: Access nested type from outside via scope resolution
    // TR: İç türe dışarıdan kapsam çözücü ile erişim
    CanMessage::Header standalone_hdr(0x7DF, 8, false, false);
    std::cout << "Standalone Header ID: 0x" << std::hex
              << standalone_hdr.getArbId() << std::dec << "\n\n";

    std::cout << "=== Demo 2: ECUDiagnosticSession (PIMPL) ===\n";
    // EN: Create session, authenticate, store and read DTCs
    // TR: Oturum oluştur, kimlik doğrula, DTC kaydet ve oku
    ECUDiagnosticSession diag;
    diag.startSession(0x01);
    diag.printStatus();

    diag.storeDtc("P0301");   // EN: Should fail — not authenticated
    diag.authenticate("WRONG_KEY");
    diag.authenticate("ECU_AUTH_2026");
    diag.printStatus();

    diag.storeDtc("P0301");
    diag.storeDtc("P0420");
    diag.storeDtc("U0100");

    auto dtcs = diag.readDtcs();
    std::cout << "[ECU] Stored DTCs: ";
    for (const auto& d : dtcs) std::cout << d << " ";
    std::cout << "\n\n";

    std::cout << "=== Demo 3: Local Class — validateCanTraffic() ===\n";
    // EN: Build a small traffic sample with valid and invalid frames
    // TR: Geçerli ve geçersiz çerçevelerden oluşan küçük bir trafik örneği oluştur
    std::vector<CanMessage> traffic;
    traffic.emplace_back(0x100, 8);   // valid
    traffic.emplace_back(0x7FF, 1);   // valid (max standard ID)
    traffic.emplace_back(0x800, 4);   // invalid (ID > 0x7FF for standard)
    traffic.emplace_back(0x050, 0);   // invalid (DLC = 0)
    validateCanTraffic(traffic);
    std::cout << "\n";

    std::cout << "=== Demo 4: Nested Class Accessing Outer's Private ===\n";
    // EN: Inspector accesses SensorHub's private name_ and calibration_offset_
    // TR: Inspector, SensorHub'ın private name_ ve calibration_offset_ alanlarına erişir
    SensorHub hub("WheelSpeed_FL", 0.05);
    hub.addReading(1200.0);
    hub.addReading(1205.5);
    hub.addReading(1198.3);
    hub.printReadings();
    SensorHub::Inspector::diagnose(hub);

    return 0;
}
