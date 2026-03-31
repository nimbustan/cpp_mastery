/**
 * @file module_02_oop/01_classes_objects.cpp
 * @brief Module 02 - Classes & Objects: C++ OOP'nin temel yapı taşları — Fundamental building blocks of C++ OOP
 *
 * @details
 * Bu dosya C++ sınıf ve nesne kavramlarını detaylı örneklerle açıklar.
 * This file explains C++ class and object concepts with detailed examples.
 *
 * =============================================================================
 * [THEORY: Class vs Object / TEORİ: Sınıf vs Nesne]
 * =============================================================================
 * EN: A CLASS is a blueprint/template that defines data (member variables) and
 *     behavior (member functions). An OBJECT is a concrete instance of that
 *     class living in memory with its own copy of member variables.
 *     Think: Class = Cookie cutter, Object = Cookie
 *
 * TR: SINIF (class), veri (üye değişkenler) ve davranışı (üye fonksiyonlar)
 *     tanımlayan bir şablondur. NESNE (object) ise o sınıfın bellekte yaşayan,
 *     kendi üye değişken kopyasına sahip somut bir örneğidir.
 *     Düşün: Sınıf = Kurabiye kalıbı, Nesne = Kurabiye
 *
 * =============================================================================
 * [THEORY: Access Specifiers / TEORİ: Erişim Belirleyiciler]
 * =============================================================================
 * EN: C++ has 3 access levels:
 *     • private:   Only accessible inside the class itself (DEFAULT for class)
 *     • protected: Accessible inside class + derived classes (inheritance)
 *     • public:    Accessible from anywhere
 *     Rule of thumb: Data → private, Interface → public
 *
 * TR: C++'da 3 erişim seviyesi var:
 *     • private:   Sadece sınıf içinden erişilebilir (class için VARSAYILAN)
 *     • protected: Sınıf içi + türetilmiş sınıflardan erişilebilir
 *     • public:    Her yerden erişilebilir
 *     Kural: Veri → private, Arayüz → public
 *
 * =============================================================================
 * [THEORY: Constructor & Destructor / TEORİ: Yapıcı & Yıkıcı]
 * =============================================================================
 * EN: Constructor = special function called automatically when object is created.
 *     - Same name as class, no return type
 *     - Can be overloaded (multiple constructors with different parameters)
 *     - Member Initializer List (MIL) is preferred over body assignment
 *       because MIL directly constructs members, body assigns AFTER default construction
 *     Destructor = called automatically when object goes out of scope.
 *     - Name: ~ClassName(), no parameters, no return type, CANNOT be overloaded
 *     - LIFO order: last constructed = first destroyed
 *
 * TR: Constructor = nesne oluşturulunca otomatik çağrılan özel fonksiyon.
 *     - Sınıf adıyla aynı, dönüş tipi yok
 *     - Overload edilebilir (farklı parametrelerle birden fazla constructor)
 *     - Member Initializer List (MIL), gövde atamadan tercih edilir
 *       çünkü MIL üyeleri doğrudan oluşturur, gövde varsayılan oluşturma SONRASI atar
 *     Destructor = nesne scope'dan çıkınca otomatik çağrılır.
 *     - İsim: ~SınıfAdı(), parametre yok, dönüş tipi yok, overload edilemez
 *     - LIFO sırası: son oluşturulan = ilk yıkılan
 *
 * =============================================================================
 * [THEORY: struct vs class / TEORİ: struct ve class Farkı]
 * =============================================================================
 * EN: In C++, struct and class are almost identical. The ONLY difference:
 *     • struct: members are PUBLIC by default
 *     • class:  members are PRIVATE by default
 *     Convention: struct for POD (Plain Old Data) with no invariants,
 *                 class for objects with encapsulation and behavior.
 *
 * TR: C++'da struct ve class neredeyse aynıdır. TEK FARK:
 *     • struct: üyeler varsayılan olarak PUBLIC
 *     • class:  üyeler varsayılan olarak PRIVATE
 *     Gelenek: struct → basit veri (POD), class → kapsülleme + davranış
 *
 * =============================================================================
 * [THEORY: this Pointer / TEORİ: this İşaretçisi]
 * =============================================================================
 * EN: Every non-static member function receives a hidden pointer called `this`
 *     that points to the object on which the function was called.
 *     this->member is implicit; you write it explicitly when:
 *     (1) Parameter name shadows member name
 *     (2) Returning *this for method chaining
 *     (3) Passing the current object to another function
 *
 * TR: Her non-static üye fonksiyon, fonksiyonun çağrıldığı nesneyi gösteren
 *     gizli bir `this` işaretçisi alır.
 *     this->member örtüktür; şu durumlarda açıkça yazılır:
 *     (1) Parametre adı üye adını gölgelediğinde
 *     (2) Method chaining için *this döndürürken
 *     (3) Mevcut nesneyi başka fonksiyona geçirirken
 *
 * =============================================================================
 * [THEORY: const Member Functions / TEORİ: const Üye Fonksiyonlar]
 * =============================================================================
 * EN: A member function marked `const` promises NOT to modify any member variable.
 *     - The `this` pointer becomes `const ClassName*` inside that function
 *     - const objects can ONLY call const member functions
 *     - Getters should ALWAYS be const
 *     - A const function calling a non-const function = COMPILE ERROR
 *
 * TR: `const` ile işaretlenen üye fonksiyon, hiçbir üye değişkeni DEĞİŞTİRMEYECEĞİNİ garanti eder.
 *     - `this` işaretçisi o fonksiyon içinde `const ClassName*` olur
 *     - const nesneler SADECE const üye fonksiyonları çağırabilir
 *     - Getter'lar HER ZAMAN const olmalı
 *     - const fonksiyondan non-const fonksiyon çağırmak = DERLEME HATASI
 *
 * @see https://en.cppreference.com/w/cpp/language/classes
 * @see https://en.cppreference.com/w/cpp/language/constructor
 * @see https://en.cppreference.com/w/cpp/language/destructor
 * @note Compile: g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion 01_classes_objects.cpp -o 01_classes_objects
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <iostream>
#include <string>
#include <cstdint>

// ─────────────────────────────────────────────────────────────────────────────
// GLOBAL: Automotive-themed class
// ─────────────────────────────────────────────────────────────────────────────

/**
 * @class TemperatureSensor
 * @brief EN: Represents an automotive temperature sensor with encapsulation
 *        TR: Kapsülleme ile otomotiv sıcaklık sensörünü temsil eder
 *
 * EN: This class demonstrates:
 *     - private data members (encapsulation)
 *     - constructor with Member Initializer List
 *     - destructor with cleanup message
 *     - const getter functions
 *     - setter with input validation
 *     - behavior method that operates on internal state
 *     - `this` pointer usage
 *
 * TR: Bu sınıf şunları gösterir:
 *     - private veri üyeleri (kapsülleme)
 *     - Member Initializer List ile constructor
 *     - temizleme mesajlı destructor
 *     - const getter fonksiyonları
 *     - girdi doğrulamalı setter
 *     - iç durum üzerinde çalışan davranış metodu
 *     - `this` işaretçisi kullanımı
 */
class TemperatureSensor {
private:
    // EN: Data is PRIVATE → cannot be accessed from outside the class
    // TR: Veri PRIVATE → sınıf dışından erişilemez
    std::string sensorId_;      // EN: Unique sensor identifier / TR: Benzersiz sensör kimliği
    double currentTempC_;       // EN: Current temperature in Celsius / TR: Mevcut sıcaklık (Celsius)
    double minThreshold_;       // EN: Minimum threshold (alert if below) / TR: Min eşik (altında uyarı)
    double maxThreshold_;       // EN: Maximum threshold (alert if above) / TR: Maks eşik (üstünde uyarı)
    int readingCount_;          // EN: How many readings taken / TR: Kaç okuma yapıldı

public:
    // ── CONSTRUCTOR ──────────────────────────────────────────────────────

    /**
     * EN: Parameterized constructor with Member Initializer List (MIL)
     *     MIL directly constructs each member → more efficient than body assignment
     *     Body assignment: first default-constructs, THEN assigns (2 steps for non-trivial types)
     *
     * TR: Member Initializer List (MIL) ile parametreli constructor
     *     MIL her üyeyi doğrudan oluşturur → gövde atamasından daha verimli
     *     Gövde ataması: önce varsayılan oluşturma, SONRA atama (basit olmayan tipler için 2 adım)
     */
    TemperatureSensor(const std::string& id, double minTh, double maxTh)
        : sensorId_(id)           // EN: Direct construction (1 step) / TR: Doğrudan oluşturma (1 adım)
        , currentTempC_(0.0)      // EN: Initialize to zero / TR: Sıfıra başlat
        , minThreshold_(minTh)    // EN: Set minimum threshold / TR: Min eşiği ayarla
        , maxThreshold_(maxTh)    // EN: Set maximum threshold / TR: Maks eşiği ayarla
        , readingCount_(0)        // EN: No readings yet / TR: Henüz okuma yok
    {
        // EN: Constructor body — runs AFTER MIL completes
        //     Use body for logic that can't go in MIL (validation, logging, etc.)
        // TR: Constructor gövdesi — MIL tamamlandıktan SONRA çalışır
        //     MIL'e koyulamayan mantık için gövdeyi kullan (doğrulama, loglama vb.)
        std::cout << "[CONSTRUCTOR] Sensor '" << sensorId_ << "' created"
                  << " [range: " << minThreshold_ << "C - " << maxThreshold_ << "C]\n";
    }

    // ── DESTRUCTOR ──────────────────────────────────────────────────────

    /**
     * EN: Destructor — called automatically when object goes out of scope
     *     Perfect place for cleanup: releasing resources, logging, etc.
     *     Cannot be overloaded, cannot take parameters, no return type.
     *     Destruction order is LIFO (Last In, First Out) — reverse of construction.
     *
     * TR: Destructor — nesne scope'dan çıkınca otomatik çağrılır
     *     Temizlik için ideal: kaynak serbest bırakma, loglama vb.
     *     Overload edilemez, parametre alamaz, dönüş tipi yok.
     *     Yıkım sırası LIFO (Son Giren, İlk Çıkar) — oluşturma sırasının tersi.
     */
    ~TemperatureSensor() {
        std::cout << "[DESTRUCTOR] Sensor '" << sensorId_ << "' destroyed"
                  << " (total readings: " << readingCount_ << ")\n";
    }

    // ── CONST GETTERS ───────────────────────────────────────────────────

    /**
     * EN: Getter marked `const` → promises not to modify any member
     *     The `const` after the parameter list makes `this` pointer const:
     *       TemperatureSensor* this  →  const TemperatureSensor* this
     *     Rule: ALL getters should be const. If you forget const,
     *     the function CANNOT be called on a const object.
     *
     * TR: `const` ile işaretli getter → hiçbir üyeyi değiştirmeyeceğini garanti eder
     *     Parametre listesinden sonraki `const`, this işaretçisini const yapar:
     *       TemperatureSensor* this  →  const TemperatureSensor* this
     *     Kural: TÜM getter'lar const olmalı. const unutulursa,
     *     fonksiyon const nesnede ÇAĞRILAMAZ.
     */
    std::string getId() const { return sensorId_; }
    double getTemp() const { return currentTempC_; }
    int getReadingCount() const { return readingCount_; }

    // ── SETTER WITH VALIDATION ──────────────────────────────────────────

    /**
     * EN: Setter demonstrates encapsulation: external code can't set temp directly,
     *     must go through this function which validates input.
     *     Without encapsulation: object.temp = -9999; (no protection!)
     *     With encapsulation:    object.updateTemp(-9999); → rejected with message
     *
     * TR: Setter kapsüllemeyi gösterir: dışarıdan temp'e doğrudan erişilemez,
     *     bu fonksiyon aracılığıyla girdi doğrulanır.
     *     Kapsülleme olmadan: object.temp = -9999; (korumasız!)
     *     Kapsülleme ile:     object.updateTemp(-9999); → mesajla reddedilir
     */
    void updateTemperature(double newTempC) {
        // EN: Validate — real sensors have physical limits
        // TR: Doğrula — gerçek sensörlerin fiziksel limitleri var
        if (newTempC < -273.15) {
            // EN: Absolute zero is -273.15°C, nothing can be colder
            // TR: Mutlak sıfır -273.15°C, daha soğuk olamaz
            std::cout << "  [WARN] Invalid temperature " << newTempC
                      << "C (below absolute zero!) — ignored\n";
            return;  // EN: Reject invalid input / TR: Geçersiz girdiyi reddet
        }
        currentTempC_ = newTempC;
        ++readingCount_;
    }

    // ── BEHAVIOR METHOD ─────────────────────────────────────────────────

    /**
     * EN: This method operates on internal state to produce a result.
     *     It checks thresholds and reports status. This is const because
     *     it only reads member variables, never modifies them.
     *
     * TR: Bu metod iç durum üzerinde çalışarak sonuç üretir. Eşikleri
     *     kontrol edip durum bildirir. Sadece üye değişkenleri okuduğu,
     *     değiştirmediği için const.
     */
    void reportStatus() const {
        std::cout << "  Sensor [" << sensorId_ << "]: " << currentTempC_ << "C";

        // EN: Compare against thresholds
        // TR: Eşiklerle karşılaştır
        if (currentTempC_ < minThreshold_) {
            std::cout << " ⚠ BELOW minimum (" << minThreshold_ << "C)";
        } else if (currentTempC_ > maxThreshold_) {
            std::cout << " ⚠ ABOVE maximum (" << maxThreshold_ << "C)";
        } else {
            std::cout << " ✓ Normal range";
        }
        std::cout << "  [readings: " << readingCount_ << "]\n";
    }

    // ── THIS POINTER DEMO ───────────────────────────────────────────────

    /**
     * EN: Demonstrates explicit `this` pointer usage.
     *     Case: parameter name 'id' shadows member — `this->` resolves ambiguity.
     *     Without `this->`, 'id' refers to the parameter, not the member.
     *
     * TR: Açık `this` işaretçi kullanımını gösterir.
     *     Durum: parametre adı 'sensorId_' üyeyi gölgeler — `this->` belirsizliği çözer.
     *     `this->` olmadan 'sensorId_' parametreye, üyeye değil, atıfta bulunur.
     */
    void reassignId(const std::string& sensorId_) {
        // EN: Without `this->`, both sides refer to the parameter (self-assignment, USELESS)
        // TR: `this->` olmadan iki taraf da parametreye atıfta bulunur (kendi kendine atama, FAYDASIZ)
        // sensorId_ = sensorId_;  // EN: BUG! Both are the parameter / TR: HATA! İkisi de parametre

        // EN: With `this->`, left side is the MEMBER, right side is the PARAMETER
        // TR: `this->` ile sol taraf ÜYE, sağ taraf PARAMETRE
        this->sensorId_ = sensorId_;
        std::cout << "  [INFO] Sensor ID reassigned to: " << this->sensorId_ << "\n";
    }

    /**
     * EN: Returns reference to *this for method chaining pattern.
     *     Usage: sensor.setMin(10).setMax(50);  ← each call returns the same object
     *
     * TR: Method chaining kalıbı için *this referansı döndürür.
     *     Kullanım: sensor.setMin(10).setMax(50);  ← her çağrı aynı nesneyi döndürür
     */
    TemperatureSensor& setMinThreshold(double val) {
        minThreshold_ = val;
        return *this;  // EN: Return the current object / TR: Mevcut nesneyi döndür
    }

    TemperatureSensor& setMaxThreshold(double val) {
        maxThreshold_ = val;
        return *this;
    }
};

// ─────────────────────────────────────────────────────────────────────────────
// GLOBAL: struct vs class demonstration
// ─────────────────────────────────────────────────────────────────────────────

/**
 * @struct CANFrame
 * @brief EN: POD struct — all members public by default, no invariants
 *        TR: POD struct — tüm üyeler varsayılan olarak public, değişmez yok
 *
 * EN: Use struct for simple data containers with no encapsulation needs.
 *     This represents a CAN bus frame — just raw data, no validation logic.
 *     struct members are PUBLIC by default (vs class: PRIVATE by default).
 *
 * TR: Kapsülleme ihtiyacı olmayan basit veri taşıyıcıları için struct kullan.
 *     Bu bir CAN bus çerçevesini temsil eder — sadece ham veri, doğrulama yok.
 *     struct üyeleri varsayılan PUBLIC (class'ta: varsayılan PRIVATE).
 */
struct CANFrame {
    // EN: No need to write "public:" — struct is public by default
    // TR: "public:" yazmaya gerek yok — struct varsayılan public
    uint32_t id;          // EN: CAN frame ID / TR: CAN çerçeve ID'si
    uint8_t  length;      // EN: Data Length Code (0-8) / TR: Veri Uzunluk Kodu (0-8)
    uint8_t  data[8];     // EN: Payload bytes / TR: Yük baytları
    bool     isExtended;  // EN: Standard (11-bit) or Extended (29-bit) / TR: Standart veya Genişletilmiş
};

// ─────────────────────────────────────────────────────────────────────────────
// GLOBAL: Constructor overloading demonstration
// ─────────────────────────────────────────────────────────────────────────────

/**
 * @class ECUModule
 * @brief EN: Demonstrates constructor overloading (default + parameterized)
 *        TR: Constructor overloading'i gösterir (varsayılan + parametreli)
 *
 * EN: A class can have MULTIPLE constructors with different parameter lists.
 *     The compiler selects the correct one based on arguments at the call site.
 *     If you define ANY constructor, the compiler does NOT auto-generate a default one.
 *     To get it back: ECUModule() = default;
 *
 * TR: Bir sınıfın farklı parametre listelerine sahip BİRDEN FAZLA constructor'ı olabilir.
 *     Derleyici çağrı yerindeki argümanlara göre doğru olanı seçer.
 *     HERHANGİ bir constructor tanımlarsan, derleyici varsayılan olanı otomatik OLUŞTURMAZ.
 *     Geri almak için: ECUModule() = default;
 */
class ECUModule {
private:
    std::string name_;
    std::string firmwareVersion_;
    bool isActive_;

public:
    // EN: Default constructor — creates module with placeholder values
    //     This exists so we can create an ECUModule without arguments: ECUModule ecu;
    // TR: Varsayılan constructor — yer tutucu değerlerle modül oluşturur
    //     Bu, argümansız ECUModule oluşturabilmemiz için var: ECUModule ecu;
    ECUModule()
        : name_("UNKNOWN")
        , firmwareVersion_("0.0.0")
        , isActive_(false)
    {
        std::cout << "[CONSTRUCTOR] ECU default created: " << name_ << "\n";
    }

    // EN: Parameterized constructor — user specifies name and firmware version
    // TR: Parametreli constructor — kullanıcı ad ve firmware sürümünü belirtir
    ECUModule(const std::string& name, const std::string& fw)
        : name_(name)
        , firmwareVersion_(fw)
        , isActive_(true)
    {
        std::cout << "[CONSTRUCTOR] ECU created: " << name_ << " (fw: " << firmwareVersion_ << ")\n";
    }

    // EN: Destructor
    // TR: Yıkıcı
    ~ECUModule() {
        std::cout << "[DESTRUCTOR] ECU '" << name_ << "' destroyed\n";
    }

    // EN: Getters — all const / TR: Getter'lar — hepsi const
    std::string getName() const { return name_; }
    std::string getFirmware() const { return firmwareVersion_; }
    bool isActive() const { return isActive_; }

    // EN: Display info / TR: Bilgi göster
    void printInfo() const {
        std::cout << "  ECU [" << name_ << "] fw=" << firmwareVersion_
                  << " active=" << (isActive_ ? "yes" : "no") << "\n";
    }
};

// ═════════════════════════════════════════════════════════════════════════════
// MAIN — Demonstrations
// ═════════════════════════════════════════════════════════════════════════════
int main() {
    std::cout << "============================================\n";
    std::cout << "   Module 02 - Classes & Objects\n";
    std::cout << "============================================\n\n";

    // ─── Section 1: Object Creation & Constructor ────────────────────────
    {
        std::cout << "--- Section 1: Object Creation & Constructor ---\n";

        // EN: Creating an object = calling the constructor.
        //     The constructor runs immediately when the object is declared.
        //     Memory: object lives on the STACK (automatic storage duration).
        // TR: Nesne oluşturmak = constructor'ı çağırmak.
        //     Constructor, nesne tanımlanınca hemen çalışır.
        //     Bellek: nesne STACK'te yaşar (otomatik ömür süresi).
        TemperatureSensor coolant("COOLANT_01", -40.0, 120.0);
        // EN: ↑ This single line does ALL of this:
        //     1. Allocates sizeof(TemperatureSensor) bytes on stack
        //     2. Calls constructor with ("COOLANT_01", -40.0, 120.0)
        //     3. MIL runs → members directly constructed
        //     4. Constructor body runs → prints creation message
        // TR: ↑ Bu tek satır TÜM bunları yapar:
        //     1. Stack üzerinde sizeof(TemperatureSensor) bayt ayırır
        //     2. Constructor'ı ("COOLANT_01", -40.0, 120.0) ile çağırır
        //     3. MIL çalışır → üyeler doğrudan oluşturulur
        //     4. Constructor gövdesi çalışır → oluşturma mesajı yazdırır

        std::cout << "\n";
    }
    // EN: ↑ coolant goes out of scope here → DESTRUCTOR automatically called
    // TR: ↑ coolant burada scope'dan çıkar → DESTRUCTOR otomatik çağrılır

    std::cout << "\n";

    // ─── Section 2: Encapsulation — Getter/Setter ────────────────────────
    {
        std::cout << "--- Section 2: Encapsulation — Getter/Setter ---\n";

        TemperatureSensor oil("OIL_TEMP", 0.0, 150.0);

        // EN: We CANNOT access private members directly:
        //     oil.currentTempC_ = 85.5;  ← COMPILE ERROR: 'currentTempC_' is private
        //     This is ENCAPSULATION: data is hidden, access is controlled.
        // TR: private üyelere doğrudan ERİŞEMEYİZ:
        //     oil.currentTempC_ = 85.5;  ← DERLEME HATASI: 'currentTempC_' private
        //     Bu KAPSÜLLEME: veri gizli, erişim kontrollü.

        // EN: Use public setter — validates input before modifying state
        // TR: Public setter kullan — durumu değiştirmeden önce girişi doğrular
        oil.updateTemperature(92.3);
        oil.updateTemperature(105.7);

        // EN: Use const getters to READ private data safely
        // TR: private veriyi güvenle OKUMAK için const getter kullan
        std::cout << "  Oil sensor ID: " << oil.getId() << "\n";
        std::cout << "  Oil temp: " << oil.getTemp() << "C\n";
        std::cout << "  Readings so far: " << oil.getReadingCount() << "\n";

        // EN: Try invalid temperature — setter will reject it
        // TR: Geçersiz sıcaklık dene — setter reddedecek
        oil.updateTemperature(-300.0);  // EN: Below absolute zero → rejected / TR: Mutlak sıfırın altı → reddedilir
        std::cout << "  Temp after invalid attempt: " << oil.getTemp() << "C (unchanged)\n";

        std::cout << "\n";
    }

    std::cout << "\n";

    // ─── Section 3: Behavior Methods ─────────────────────────────────────
    {
        std::cout << "--- Section 3: Behavior Methods ---\n";

        TemperatureSensor intake("INTAKE_AIR", -30.0, 60.0);

        // EN: Feed multiple temperature readings and check status
        // TR: Birden fazla sıcaklık okuması ver ve durumu kontrol et
        double readings[] = {25.0, -35.5, 62.0, 40.0};
        for (double temp : readings) {
            intake.updateTemperature(temp);
            intake.reportStatus();
            // EN: ↑ reportStatus() reads internal state and decides:
            //     below min → warning, above max → warning, else → normal
            // TR: ↑ reportStatus() iç durumu okuyup karar verir:
            //     min altı → uyarı, maks üstü → uyarı, yoksa → normal
        }

        std::cout << "\n";
    }

    std::cout << "\n";

    // ─── Section 4: `this` Pointer ──────────────────────────────────────
    {
        std::cout << "--- Section 4: this Pointer ---\n";

        TemperatureSensor exhaust("EXHAUST_01", 100.0, 900.0);

        // EN: Case 1: Resolving name shadowing with this->
        //     reassignId has a parameter named sensorId_ which shadows the member
        // TR: Durum 1: this-> ile isim gölgelemeyi çözme
        //     reassignId'nin sensorId_ adında üyeyi gölgeleyen bir parametresi var
        exhaust.reassignId("EXHAUST_LEFT_BANK");

        // EN: Case 2: Method chaining via return *this
        //     Each setter returns the same object, allowing call chaining.
        //     This is the same pattern used by std::cout: cout << a << b << c;
        // TR: Durum 2: return *this ile method chaining
        //     Her setter aynı nesneyi döndürerek zincirleme çağrıya izin verir.
        //     std::cout'un kullandığı kalıpla aynı: cout << a << b << c;
        exhaust.setMinThreshold(150.0).setMaxThreshold(950.0);
        // EN: ↑ Equivalent to:
        //     TemperatureSensor& ref1 = exhaust.setMinThreshold(150.0); // returns exhaust
        //     ref1.setMaxThreshold(950.0);                              // called on same object
        // TR: ↑ Eşdeğeri:
        //     TemperatureSensor& ref1 = exhaust.setMinThreshold(150.0); // exhaust'ı döndürür
        //     ref1.setMaxThreshold(950.0);                              // aynı nesne üzerinde çağrılır

        exhaust.updateTemperature(850.0);
        exhaust.reportStatus();

        std::cout << "\n";
    }

    std::cout << "\n";

    // ─── Section 5: Multiple Objects — Independence ──────────────────────
    {
        std::cout << "--- Section 5: Multiple Objects — Independence ---\n";

        // EN: Each object has its OWN copy of all member variables.
        //     Modifying one object does NOT affect the other.
        //     They share the same CODE (member functions) but have separate DATA.
        // TR: Her nesne tüm üye değişkenlerin KENDİ kopyasına sahiptir.
        //     Bir nesneyi değiştirmek diğerini ETKİLEMEZ.
        //     Aynı KODU (üye fonksiyonlar) paylaşırlar ama ayrı VERİLERİ vardır.
        TemperatureSensor cabin("CABIN", 15.0, 30.0);
        TemperatureSensor battery("BATTERY", -20.0, 45.0);

        cabin.updateTemperature(22.0);
        battery.updateTemperature(38.5);

        cabin.reportStatus();    // EN: Shows cabin data only / TR: Sadece kabin verisini gösterir
        battery.reportStatus();  // EN: Shows battery data only / TR: Sadece batarya verisini gösterir

        // EN: Changing cabin temp does NOT touch battery temp
        // TR: Kabin sıcaklığını değiştirmek batarya sıcaklığına DOKUNMAZ
        cabin.updateTemperature(33.0);
        std::cout << "  After cabin update:\n";
        cabin.reportStatus();
        battery.reportStatus();  // EN: Still 38.5 — unchanged / TR: Hâlâ 38.5 — değişmedi

        std::cout << "\n";
    }

    std::cout << "\n";

    // ─── Section 6: struct vs class & sizeof ─────────────────────────────
    {
        std::cout << "--- Section 6: struct vs class & sizeof ---\n";

        // EN: struct → POD, all members public by default, aggregate initialization
        //     Aggregate init: no constructor needed, just list values in braces {}
        // TR: struct → POD, tüm üyeler varsayılan public, aggregate başlatma
        //     Aggregate init: constructor gerekmez, süslü parantez içinde değerleri listele {}
        CANFrame frame = {0x1A3, 4, {0xDE, 0xAD, 0xBE, 0xEF, 0, 0, 0, 0}, false};

        std::cout << "  CAN Frame ID: 0x" << std::hex << frame.id << std::dec << "\n";
        std::cout << "  DLC: " << static_cast<int>(frame.length) << "\n";
        // EN: ↑ static_cast<int> because uint8_t would print as a character
        //     (uint8_t is often typedef'd from unsigned char)
        // TR: ↑ static_cast<int> çünkü uint8_t karakter olarak yazdırılır
        //     (uint8_t genellikle unsigned char'dan typedef'tir)

        std::cout << "  Data: ";
        for (int i = 0; i < static_cast<int>(frame.length); ++i) {
            std::cout << "0x" << std::hex << static_cast<int>(frame.data[i]) << " ";
        }
        std::cout << std::dec << "\n";

        // EN: sizeof shows the actual memory footprint of the type/object
        //     Note: sizeof includes PADDING bytes added by the compiler for alignment!
        //     struct layout in memory is NOT always sum of member sizes.
        // TR: sizeof, tip/nesnenin gerçek bellek ayak izini gösterir
        //     Not: sizeof, derleyicinin hizalama için eklediği PADDING baytlarını içerir!
        //     struct'ın bellekteki düzeni her zaman üye boyutları toplamı DEĞİLDİR.
        std::cout << "  sizeof(CANFrame): " << sizeof(CANFrame) << " bytes\n";
        std::cout << "  sizeof(TemperatureSensor): " << sizeof(TemperatureSensor) << " bytes\n";
        std::cout << "  sizeof(ECUModule): " << sizeof(ECUModule) << " bytes\n";

        std::cout << "\n";
    }

    std::cout << "\n";

    // ─── Section 7: Constructor Overloading ──────────────────────────────
    {
        std::cout << "--- Section 7: Constructor Overloading ---\n";

        // EN: Default constructor → no arguments needed
        //     The compiler selects ECUModule() because no args are provided
        // TR: Varsayılan constructor → argüman gerekmez
        //     Derleyici, argüman verilmediği için ECUModule()'ı seçer
        ECUModule unknown;
        unknown.printInfo();

        // EN: Parameterized constructor → 2 arguments
        //     The compiler selects ECUModule(string, string) based on arg types
        // TR: Parametreli constructor → 2 argüman
        //     Derleyici, argüman tiplerine göre ECUModule(string, string)'i seçer
        ECUModule brakeCtrl("BrakeController", "2.4.1");
        brakeCtrl.printInfo();

        ECUModule steeringCtrl("SteeringAssist", "1.8.0");
        steeringCtrl.printInfo();

        std::cout << "\n";
    }
    // EN: ↑ End of scope → destructors called in REVERSE (LIFO) order:
    //     steeringCtrl destroyed first, then brakeCtrl, then unknown
    //     (last constructed = first destroyed)
    // TR: ↑ Scope sonu → destructor'lar TERS (LIFO) sırada çağrılır:
    //     steeringCtrl önce yıkılır, sonra brakeCtrl, sonra unknown
    //     (son oluşturulan = ilk yıkılan)

    std::cout << "\n";

    // ─── Section 8: LIFO Destruction Order Demo ──────────────────────────
    {
        std::cout << "--- Section 8: LIFO Destruction Order ---\n";
        std::cout << "  Creating 3 sensors in order: A → B → C\n";

        // EN: Objects are constructed in declaration order: A, B, C
        //     But destroyed in REVERSE order: C, B, A  (LIFO)
        //     This matters when objects depend on each other!
        // TR: Nesneler tanım sırasında oluşturulur: A, B, C
        //     Ama TERS sırada yıkılır: C, B, A  (LIFO)
        //     Bu, nesneler birbirine bağımlıyken ÖNEMLİDİR!
        TemperatureSensor sA("A_first", 0, 100);
        TemperatureSensor sB("B_second", 0, 100);
        TemperatureSensor sC("C_third", 0, 100);

        std::cout << "  Now leaving scope — watch destruction order:\n";
    }
    // EN: ↑ Output will show: C destroyed, B destroyed, A destroyed (LIFO)
    // TR: ↑ Çıktı gösterecek: C yıkıldı, B yıkıldı, A yıkıldı (LIFO)

    std::cout << "\n";

    // ─── Section 9: const Objects ────────────────────────────────────────
    {
        std::cout << "--- Section 9: const Objects ---\n";

        // EN: A const object cannot be modified after construction.
        //     It can ONLY call const member functions.
        //     Attempting to call a non-const function → COMPILE ERROR.
        // TR: const nesne, oluşturulduktan sonra değiştirilemez.
        //     SADECE const üye fonksiyonları çağırabilir.
        //     non-const fonksiyon çağırma girişimi → DERLEME HATASI.
        const TemperatureSensor frozen("FROZEN_REF", -50.0, 50.0);

        // EN: ✓ These work — getId(), getTemp(), reportStatus() are all const
        // TR: ✓ Bunlar çalışır — getId(), getTemp(), reportStatus() hepsi const
        std::cout << "  Frozen sensor ID: " << frozen.getId() << "\n";
        std::cout << "  Frozen sensor temp: " << frozen.getTemp() << "C\n";
        frozen.reportStatus();

        // EN: ✗ These would FAIL to compile if uncommented:
        //     frozen.updateTemperature(10.0);  ← ERROR: non-const function on const object
        //     frozen.reassignId("NEW");        ← ERROR: same reason
        //     This is WHY making getters const matters!
        // TR: ✗ Bunlar yorum işaretleri kaldırılırsa derleme HATASI verir:
        //     frozen.updateTemperature(10.0);  ← HATA: const nesnede non-const fonksiyon
        //     frozen.reassignId("NEW");        ← HATA: aynı sebep
        //     Getter'ları const yapmanın önemli olmasının sebebi BU!

        std::cout << "\n";
    }

    std::cout << "\n============================================\n";
    std::cout << "   End of Module 02 - Classes & Objects\n";
    std::cout << "============================================\n";

    return 0;
}
