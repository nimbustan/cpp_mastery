/**
 * @file module_05_certification/06_interview_gotchas.cpp
 * @brief CPA/CPP Prep: C++ Interview Gotchas & Tricky Behavior / Sertifikasyon: C++ Mülakat
 * Tuzakları ve Yanıltıcı Davranışlar
 *
 * @details
 * =============================================================================
 * [THEORY: Most Vexing Parse / TEORİ: En Sinir Bozucu Ayrıştırma]
 * =============================================================================
 * EN: `SensorReading sr(TemperatureSource());` looks like object construction, but the compiler
 * parses it as a FUNCTION DECLARATION: a function `sr` that takes a function-pointer parameter
 * and returns `SensorReading`. This is the "Most Vexing Parse" — the C++ grammar always prefers
 * a declaration over an expression. Fix: use brace initialization `{}`.
 *
 * TR: `SensorReading sr(TemperatureSource());` nesne oluşturma gibi görünür, ama derleyici bunu
 * bir FONKSİYON BİLDİRİMİ olarak ayrıştırır: `sr` adında, fonksiyon-pointer parametresi alan ve
 * `SensorReading` döndüren bir fonksiyon. Bu "Most Vexing Parse"tır — C++ grameri bildirim
 * olabilecek her şeyi bildirim olarak yorumlar. Çözüm: süslü parantez `{}`.
 *
 * =============================================================================
 * [THEORY: RVO/NRVO (Return Value Optimization) / TEORİ: Dönüş Değeri Opt.]
 * =============================================================================
 * EN: When a function returns a local object by value, the compiler can eliminate the copy/move
 * entirely — constructing the object directly in the caller's memory. Named RVO (NRVO) applies
 * to named local variables. C++17 mandates RVO for prvalues (unnamed temporaries). NRVO is not
 * guaranteed by the standard but virtually all major compilers apply it.
 *
 * TR: Bir fonksiyon yerel bir nesneyi değer ile döndürdüğünde, derleyici kopyalama/taşımayı
 * tamamen ortadan kaldırabilir — nesneyi doğrudan çağıranın belleğinde oluşturur. İsimli RVO
 * (NRVO) isimli yerel değişkenlere uygulanır. C++17, prvalue'lar (isimsiz geçiciler) için RVO'yu
 * zorunlu kılar. NRVO standart tarafından garanti edilmez ama neredeyse tüm büyük derleyiciler
 * uygular.
 *
 * =============================================================================
 * [THEORY: Dangling References / TEORİ: Sarkan (Dangling) Referanslar]
 * =============================================================================
 * EN: Returning a reference to a local variable is ALWAYS dangling — the local is destroyed when
 * the function returns. Returning a reference to a temporary has the same problem. Temporaries
 * die at end of the full expression, EXCEPT when bound to a `const&` or `&&` in the same scope,
 * which extends their lifetime to match the reference's scope.
 *
 * TR: Yerel bir değişkene referans döndürmek HER ZAMAN sarkan (dangling) olur — yerel, fonksiyon
 * döndüğünde yok edilir. Geçiciye referans döndürmek de aynı sorundur. Geçiciler tam ifadenin
 * sonunda ölür, ANCAK aynı kapsamda `const&` veya `&&`'ye bağlanırlarsa ömürleri referansın
 * kapsamına kadar uzatılır.
 *
 * =============================================================================
 * [THEORY: Evaluation Order / TEORİ: Değerlendirme Sırası]
 * =============================================================================
 * EN: Before C++17, in `f(a(), b())` the order of evaluating `a()` and `b()` was UNSPECIFIED —
 * either could run first! C++17 guarantees: chained function calls (e.g., `a.b(x).c(y)`) are
 * evaluated left-to-right, and the object expression is evaluated before its arguments.
 *
 * TR: C++17 öncesinde, `f(a(), b())` ifadesinde `a()` ve `b()` değerlendirme sırası
 * BELİRTİLMEMİŞTİ — hangisi önce çalışır bilinmezdi! C++17 garantisi: zincirleme fonksiyon
 * çağrılarında (ör. `a.b(x).c(y)`) soldan sağa değerlendirilir ve nesne ifadesi argümanlarından
 * önce değerlendirilir.
 *
 * =============================================================================
 * [THEORY: Implicit Conversions & explicit / TEORİ: Örtülü Dönüşümler]
 * =============================================================================
 * EN: A single-argument constructor acts as an implicit conversion operator! `Voltage v = 3.3;`
 * silently constructs a Voltage from a double. This causes subtle bugs. Mark constructors
 * `explicit` to prevent this. `explicit` is one of the most important defensive keywords in C++.
 *
 * TR: Tek argümanlı yapıcı, örtülü bir dönüşüm operatörü gibi davranır! `Voltage v = 3.3;`
 * sessizce bir double'dan Voltage oluşturur. Bu sinsi hatalara yol açar. Yapıcıları `explicit`
 * olarak işaretleyerek önleyin. `explicit`, C++'taki en önemli savunma anahtar kelimelerden
 * biridir.
 *
 * [CPPREF DEPTH: RVO, NRVO and Copy Elision — Guaranteed vs Optional / CPPREF DERİNLİK: RVO,
 * NRVO ve Kopya Eleme — Garantili vs İsteğe Bağlı]
 * =============================================================================
 * EN: C++17 guarantees RVO for prvalue returns — the copy/move constructor need not even exist!
 * The object is materialized directly in the caller's storage. NRVO (Named Return Value
 * Optimization) applies to named local variables returned by value; it is NOT guaranteed by the
 * standard but every major compiler (GCC, Clang, MSVC) performs it. CRITICAL PITFALL: writing
 * `return std::move(local);` PREVENTS NRVO because the expression is no longer an lvalue naming
 * a local — the compiler sees an xvalue and must use the move constructor instead of eliding
 * entirely. Never `std::move` a return value of a local object. Copy elision also applies to
 * throw/catch: throwing a local by value can elide the copy into the exception object. In
 * practice, returning by value is often FASTER than returning through an output parameter or
 * pointer, because elision eliminates all copies while an output param forces at least one
 * construction + assignment.
 *
 * TR: C++17, prvalue dönüşleri için RVO'yu garanti eder — kopya/taşıma yapıcısının var olması
 * bile gerekmez! Nesne doğrudan çağıranın belleğinde oluşturulur. NRVO (İsimli Dönüş Değeri
 * Optimizasyonu) değer ile döndürülen isimli yerel değişkenlere uygulanır; standart tarafından
 * garanti EDİLMEZ ama tüm büyük derleyiciler uygular. KRİTİK TUZAK: `return std::move(local);`
 * yazmak NRVO'yu ENGELLER çünkü ifade artık bir yerel'i isimlendiren lvalue değildir — derleyici
 * bir xvalue görür ve tamamen elemek yerine taşıma yapıcısını kullanmak zorunda kalır. Asla
 * yerel nesnenin dönüş değerini `std::move` ile sarmalamayın. Kopya eleme throw/catch için de
 * geçerlidir. Pratikte, değer ile dönüş genellikle çıktı parametresi veya pointer ile dönüşten
 * DAHA HIZLIDIR çünkü eleme tüm kopyaları ortadan kaldırır.
 *
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <iostream>   // EN: Standard I/O for console output
#include <string>     // EN: String class for text data
#include <vector>     // EN: Dynamic array for sensor data

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// SECTION 1: Most Vexing Parse — ECUConfig
// EN: Parentheses cause the compiler to see a function declaration, not a ctor
// TR: Parantezler derleyicinin nesne değil fonksiyon bildirimi görmesine yol açar
// ═════════════════════════════════════════════════════════════════════════════════════════════════

struct DataSource {
    // EN: Simulates an automotive data bus (CAN/LIN)
    // TR: Otomotiv veri yolunu simüle eder (CAN/LIN)
    int channelId = 1;
};

class ECUConfig {
    std::string name_;
    int channel_;
public:
    // EN: Default constructor
    // TR: Varsayılan yapıcı
    ECUConfig() : name_("DefaultECU"), channel_(0) {
        std::cout << "  ECUConfig() default ctor\n";
    }

    // EN: Constructor taking a DataSource
    // TR: DataSource alan yapıcı
    explicit ECUConfig(DataSource src)
        : name_("ECU_CH" + std::to_string(src.channelId)),
          channel_(src.channelId) {
        std::cout << "  ECUConfig(DataSource) ctor — channel "
                  << channel_ << "\n";
    }

    const std::string& getName() const { return name_; }
};

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// SECTION 2: RVO/NRVO Demo — SensorPacket with Noisy Ctors
// EN: Tracks every copy/move to prove they are elided by the compiler
// TR: Derleyicinin kopyalama/taşımayı ortadan kaldırdığını kanıtlamak için izler
// ═════════════════════════════════════════════════════════════════════════════════════════════════

class SensorPacket {
    std::string tag_;
    double value_;
public:
    explicit SensorPacket(std::string tag, double val)
        : tag_(std::move(tag)), value_(val) {
        std::cout << "  [SensorPacket CONSTRUCTED: " << tag_ << "]\n";
    }

    SensorPacket(const SensorPacket& other)
        : tag_(other.tag_), value_(other.value_) {
        std::cout << "  [SensorPacket COPY ctor: " << tag_ << "]\n";
    }

    SensorPacket(SensorPacket&& other) noexcept
        : tag_(std::move(other.tag_)), value_(other.value_) {
        std::cout << "  [SensorPacket MOVE ctor: " << tag_ << "]\n";
    }

    SensorPacket& operator=(const SensorPacket&) = default;
    SensorPacket& operator=(SensorPacket&&) noexcept = default;
    ~SensorPacket() = default;

    friend std::ostream& operator<<(std::ostream& os, const SensorPacket& sp) {
        return os << sp.tag_ << " = " << sp.value_;
    }
};

// EN: Returns a local object by value — NRVO should elide the copy/move
// TR: Yerel nesneyi değer ile döndürür — NRVO kopyayı/taşımayı ortadan kaldırır
SensorPacket createPacket() {
    SensorPacket packet("EngineTemp", 92.5);
    return packet;
}

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// SECTION 3: explicit Constructor — Voltage
// EN: Prevents dangerous implicit double → Voltage conversions
// TR: Tehlikeli örtülü double → Voltage dönüşümlerini engeller
// ═════════════════════════════════════════════════════════════════════════════════════════════════

class Voltage {
    double volts_;
public:
    // EN: explicit prevents: Voltage v = 3.3; (implicit conversion)
    // TR: explicit şunu engeller: Voltage v = 3.3; (örtülü dönüşüm)
    explicit Voltage(double v) : volts_(v) {}

    double get() const { return volts_; }

    friend std::ostream& operator<<(std::ostream& os, const Voltage& v) {
        return os << v.volts_ << "V";
    }
};

// EN: Without explicit, this function could be called with a plain double!
// TR: explicit olmasaydı, bu fonksiyona düz bir double ile çağrı yapılabilirdi!
void checkVoltage(const Voltage& v) {
    std::cout << "  Checking voltage: " << v << "\n";
    if (v.get() > 14.0) {
        std::cout << "  WARNING: Over-voltage detected!\n";
    }
}

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// SECTION 4: Dangling Reference Anti-Pattern (safe demonstration)
// EN: Shows the WRONG way and the RIGHT way to return data from functions
// TR: Fonksiyonlardan veri döndürmenin YANLIŞ ve DOĞRU yolunu gösterir
// ═════════════════════════════════════════════════════════════════════════════════════════════════

// EN: WRONG — returns reference to a local (dangling after return!)
// TR: YANLIŞ — yerel değişkene referans döndürür (dönüşten sonra sarkık!) const std::string&
// getBrandDangling() { std::string brand = "AutomotiveCorp"; return brand;  // DANGLING: brand
// is destroyed here! }

// EN: RIGHT — return by value (RVO will optimize it anyway)
// TR: DOĞRU — değer ile döndür (RVO zaten optimize edecek)
std::string getBrandSafe() {
    std::string brand = "AutomotiveCorp";
    return brand;
}

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// SECTION 5: Evaluation Order Demo (C++17)
// EN: Demonstrates guaranteed left-to-right evaluation in chained calls
// TR: Zincirleme çağrılarda garantili soldan sağa değerlendirmeyi gösterir
// ═════════════════════════════════════════════════════════════════════════════════════════════════

class DiagnosticChain {
    std::vector<std::string> log_;
public:
    // EN: Each method returns *this for chaining, logs order of calls
    // TR: Her metot zincirleme için *this döndürür, çağrı sırasını loglar
    DiagnosticChain& checkEngine(int code) {
        log_.push_back("Engine[" + std::to_string(code) + "]");
        return *this;
    }

    DiagnosticChain& checkBrakes(int code) {
        log_.push_back("Brakes[" + std::to_string(code) + "]");
        return *this;
    }

    DiagnosticChain& checkTransmission(int code) {
        log_.push_back("Transmission[" + std::to_string(code) + "]");
        return *this;
    }

    void printLog() const {
        std::cout << "  Evaluation order: ";
        for (size_t i = 0; i < log_.size(); ++i) {
            if (i > 0) std::cout << " -> ";
            std::cout << log_[i];
        }
        std::cout << "\n";
    }
};

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// MAIN — Demonstrations
// ═════════════════════════════════════════════════════════════════════════════════════════════════

int main() {
    std::cout << "══════════════════════════════════════════════════════\n";
    std::cout << " Interview Gotchas — C++ Certification Prep\n";
    std::cout << "══════════════════════════════════════════════════════\n\n";

    // ─── Demo 1: Most Vexing Parse ───────────────────────────────────────────────────────────────
    // EN: Parentheses = function declaration! Braces = correct construction.
    // TR: Parantez = fonksiyon bildirimi! Süslü parantez = doğru oluşturma.
    std::cout << "--- Demo 1: Most Vexing Parse ---\n";
    {
        // EN: BROKEN — this declares a FUNCTION, not an object:
        // TR: BOZUK — bu bir NESNE değil, FONKSİYON bildirir: ECUConfig config(DataSource());  
        // // Function declaration!

        // EN: FIX 1 — brace initialization (preferred in modern C++)
        // TR: ÇÖZÜM 1 — süslü parantez başlatma (modern C++'ta tercih edilir)
        ECUConfig config1{DataSource()};
        std::cout << "  config1 name: " << config1.getName() << "\n";

        // EN: FIX 2 — extra parentheses around argument
        // TR: ÇÖZÜM 2 — argüman etrafında ekstra parantez
        ECUConfig config2((DataSource()));
        std::cout << "  config2 name: " << config2.getName() << "\n";

        // EN: FIX 3 — named variable for the argument
        // TR: ÇÖZÜM 3 — argüman için isimli değişken
        DataSource src;
        ECUConfig config3(src);
        std::cout << "  config3 name: " << config3.getName() << "\n";
    }

    // ─── Demo 2: RVO — zero copies ───────────────────────────────────────────────────────────────
    // EN: Noisy constructors prove that no copy or move happens
    // TR: Gürültülü yapıcılar kopya veya taşıma olmadığını kanıtlar
    std::cout << "\n--- Demo 2: RVO/NRVO (Return Value Optimization) ---\n";
    {
        std::cout << "Creating packet via function return:\n";
        SensorPacket pkt = createPacket();
        std::cout << "  Packet: " << pkt << "\n";
        std::cout << "  (If no COPY/MOVE printed above, NRVO worked!)\n";
    }

    // ─── Demo 3: explicit prevents implicit conversion ───────────────────────────────────────────
    // EN: Without explicit, checkVoltage(3.3) would silently compile
    // TR: explicit olmasaydı, checkVoltage(3.3) sessizce derlenir
    std::cout << "\n--- Demo 3: explicit constructor ---\n";
    {
        Voltage batteryVoltage(12.6);
        checkVoltage(batteryVoltage);

        // EN: This would fail to compile — no implicit double → Voltage:
        // TR: Bu derlenmez — örtülü double → Voltage dönüşümü yok: checkVoltage(3.3);  // ERROR:
        // no implicit conversion

        // EN: Must be explicit:
        // TR: Açık olmalı:
        checkVoltage(Voltage(14.5));
    }

    // ─── Demo 4: Temporary lifetime extension ────────────────────────────────────────────────────
    // EN: Binding a temporary to const& extends its lifetime
    // TR: Bir geçiciyi const&'ye bağlamak ömrünü uzatır
    std::cout << "\n--- Demo 4: Temporary lifetime extension ---\n";
    {
        // EN: Safe — return by value, stored in local variable
        // TR: Güvenli — değer ile dönüş, yerel değişkende saklanır
        std::string brand = getBrandSafe();
        std::cout << "  Brand (by value): " << brand << "\n";

        // EN: Safe — const& extends the temporary's lifetime to this scope
        // TR: Güvenli — const& geçicinin ömrünü bu kapsama kadar uzatır
        const std::string& tempRef = getBrandSafe();
        std::cout << "  Brand (const& to temp): " << tempRef << "\n";

        // EN: Safe — auto&& (forwarding ref) also extends lifetime
        // TR: Güvenli — auto&& (yönlendirme ref) de ömrü uzatır
        auto&& fwdRef = getBrandSafe();
        std::cout << "  Brand (auto&& to temp): " << fwdRef << "\n";
    }

    // ─── Demo 5: Evaluation order — C++17 left-to-right ──────────────────────────────────────────
    // EN: Chained member function calls are evaluated left-to-right in C++17
    // TR: Zincirleme üye fonksiyon çağrıları C++17'de soldan sağa değerlendirilir
    std::cout << "\n--- Demo 5: Evaluation order (C++17) ---\n";
    {
        DiagnosticChain diag;
        // EN: C++17 guarantees: checkEngine → checkBrakes → checkTransmission
        // TR: C++17 garantisi: checkEngine → checkBrakes → checkTransmission
        diag.checkEngine(101)
            .checkBrakes(202)
            .checkTransmission(303);
        diag.printLog();
    }

    std::cout << "\n══════════════════════════════════════════════════════\n";
    std::cout << " All interview gotcha demos completed successfully!\n";
    std::cout << "══════════════════════════════════════════════════════\n";

    return 0;
}
