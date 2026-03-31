/**
 * @file module_02_oop/06_static_and_friends.cpp
 * @brief OOP Part 6: Static Members & Friend Functions — OOP Kısım 6: Statik Üyeler ve Friend
 * Fonksiyonlar
 *
 * @details
 * =============================================================================
 * [THEORY: Static Class Members / TEORİ: Statik Sınıf Üyeleri]
 * =============================================================================
 * EN: A `static` data member belongs to the CLASS itself, not to any individual object. All
 * instances share the same copy. Think of a global configuration register inside an ECU — every
 * software module reads and writes the same register, not a per-module copy. Declaring `static
 * int errorCount;` in a class means there is exactly ONE `errorCount` for every object ever
 * created. Static members must be defined (allocated) outside the class body unless they are
 * `inline` or `constexpr`.
 *
 * TR: Bir `static` veri üyesi, herhangi bir nesneye değil SINIFIN kendisine aittir. Tüm örnekler
 * aynı kopyayı paylaşır. Bir ECU içindeki global konfigürasyon kaydını düşünün — her yazılım
 * modülü aynı kaydı okur ve yazar, her modülün ayrı kopyası yoktur. Sınıf içinde `static int
 * errorCount;` tanımladığınızda, oluşturulan tüm nesneler için TAM OLARAK BİR `errorCount`
 * vardır. Statik üyeler, `inline` veya `constexpr` olmadıkça sınıf gövdesinin dışında
 * tanımlanmalıdır (bellek ayrılmalıdır).
 *
 * =============================================================================
 * [THEORY: Static Member Functions / TEORİ: Statik Üye Fonksiyonlar]
 * =============================================================================
 * EN: A `static` member function can be called WITHOUT creating an object:
 * `VehicleTelemetry::getFleetCount()`. It has NO `this` pointer, so it can only access other
 * static members. Use it for factory methods, counters, or utility operations that logically
 * belong to the class but need no instance.
 *
 * TR: Bir `static` üye fonksiyon, nesne oluşturmadan çağrılabilir:
 * `VehicleTelemetry::getFleetCount()`. `this` işaretçisi YOKTUR, bu yüzden yalnızca diğer statik
 * üyelere erişebilir. Fabrika metotları, sayaçlar veya mantıksal olarak sınıfa ait olan ancak
 * bir örneğe ihtiyaç duymayan yardımcı işlemler için kullanılır.
 *
 * =============================================================================
 * [THEORY: Friend Functions / TEORİ: Friend (Arkadaş) Fonksiyonlar]
 * =============================================================================
 * EN: A `friend` function is declared inside a class but is NOT a member. It can access all
 * private and protected members. Think of a diagnostic tool that plugs into an ECU's OBD-II port
 * — it needs direct access to private sensor readings and DTC logs that normal external code
 * cannot see. The class explicitly grants trust by declaring `friend void diagnosticDump(...)`.
 *
 * TR: Bir `friend` fonksiyon sınıf içinde bildirilir ancak üye DEĞİLDİR. Tüm private ve
 * protected üyelere erişebilir. Bir ECU'nun OBD-II portuna takılan bir teşhis aracını düşünün —
 * normal harici kodun göremeyeceği özel sensör okumalarına ve DTC kayıtlarına doğrudan erişmesi
 * gerekir. Sınıf, `friend void diagnosticDump(...)` bildirerek güveni açıkça verir.
 *
 * =============================================================================
 * [THEORY: Friend Classes / TEORİ: Friend (Arkadaş) Sınıflar]
 * =============================================================================
 * EN: When you write `friend class CalibrationTool;` inside `Engine`, EVERY member function of
 * `CalibrationTool` can access `Engine`'s private data. This is like a factory calibration
 * station that must tune private engine parameters (fuel maps, timing offsets) — access that
 * regular drivers never have. Friendship is NOT symmetric: Engine trusts CalibrationTool, but
 * NOT vice versa. Friendship is NOT inherited.
 *
 * TR: `Engine` içinde `friend class CalibrationTool;` yazdığınızda, `CalibrationTool`'un TÜM üye
 * fonksiyonları `Engine`'in private verilerine erişebilir. Bu, özel motor parametrelerini (yakıt
 * haritaları, zamanlama ofsetleri) ayarlaması gereken bir fabrika kalibrasyon istasyonu gibidir
 * — normal sürücülerin asla sahip olmadığı bir erişim. Arkadaşlık SİMETRİK DEĞİLDİR: Engine
 * CalibrationTool'a güvenir, ama tersi geçerli değildir. Arkadaşlık miras ALINMAZ.
 *
 * =============================================================================
 * [THEORY: When to Use / Abuse Warning / TEORİ: Ne Zaman Kullanılır / Kötüye Kullanım Uyarısı]
 * =============================================================================
 * EN:
 *     1. USE `friend` when two classes are tightly coupled BY DESIGN
 *        (e.g., an iterator and its container, a serializer and its data class).
 *     2. DO NOT use `friend` to bypass poor encapsulation. If you need many
 *        friends, your design is probably wrong — add proper public/protected API.
 *     3. `static` is great for counters, caches, and singletons. Beware of
 *        initialization order across translation units (the "static init fiasco").
 *
 * TR:
 *     1. `friend` kullanın: İki sınıf TASARIM GEREĞİ sıkı bağlıysa
 *        (örn. bir yineleyici ve konteyneri, bir serileştirici ve veri sınıfı).
 *     2. `friend` KULLANMAYIN: Zayıf kapsüllemeyi atlatmak için. Çok fazla
 *        friend'e ihtiyacınız varsa tasarımınız muhtemelen yanlıştır — uygun
 *        public/protected API ekleyin.
 *     3. `static`, sayaçlar, önbellekler ve tekil nesneler (singleton) için
 *        harikadır. Çeviri birimleri arasında başlatma sırası konusunda dikkatli
 *        olun ("static init fiasco" sorunu).
 *
 * [CPPREF DEPTH: Static Members — Linkage, Initialization Order, and Thread Safety / CPPREF
 * DERİNLİK: Statik Üyeler — Bağlantı, Başlatma Sırası ve İş Parçacığı Güvenliği]
 * =============================================================================
 * EN: Static class data members have EXTERNAL LINKAGE by default — the linker must find exactly
 * one definition across all translation units (ODR). If you declare `static int count;` in a
 * header included by 3 .cpp files without `inline`, you get a linker error: multiple
 * definitions. The "Static Initialization Order Fiasco": globals and namespace-scope statics
 * across different TUs have NO GUARANTEED construction order. If static A in file1.cpp depends
 * on static B in file2.cpp, B might not be initialized yet → undefined behavior. The classic
 * fix: use a local static inside a function ("Construct On First Use" idiom). C++11 §6.7
 * guarantees that local static variables are initialized in a THREAD-SAFE manner — the compiler
 * inserts a hidden guard variable and mutex-like mechanism (often a spinlock or CAS). This makes
 * the Meyer's Singleton pattern safe: `static T& instance() { static T t; return t; }` C++17
 * introduced `inline static` members, allowing in-class definition of static data members
 * without a separate .cpp definition. This also guarantees a single definition across TUs — the
 * linker deduplicates. `constexpr static` members are implicitly inline since C++17.
 *
 * TR: Statik sınıf veri üyeleri varsayılan olarak DIŞ BAĞLANTIYA sahiptir — bağlayıcı tüm çeviri
 * birimleri arasında tam olarak bir tanım bulmalıdır (ODR). Bir başlıkta `static int count;`
 * bildirip 3 .cpp dosyasına dahil ederseniz `inline` olmadan bağlayıcı hatası alırsınız. "Statik
 * Başlatma Sırası Felaketi": farklı çeviri birimlerindeki global ve namespace kapsamlı statikler
 * GUARANTEED yapılandırma sırasına sahip DEĞİLDİR. Dosya1'deki statik A, dosya2'deki statik B'ye
 * bağlıysa, B henüz başlatılmamış olabilir → tanımsız davranış. Klasik çözüm: fonksiyon içi
 * yerel statik kullanmak ("İlk Kullanımda Oluştur" deyimi). C++11 §6.7, yerel statik
 * değişkenlerin İŞ PARÇACIĞI GÜVENLİ biçimde başlatılacağını garanti eder — derleyici gizli bir
 * koruma değişkeni ve mutex benzeri mekanizma ekler. Bu, Meyer's Singleton kalıbını güvenli
 * kılar: `static T& instance() { static T t; return t; }` C++17, `inline static` üyeleri tanıttı
 * ve sınıf içi tanıma izin verdi. `constexpr static` üyeler C++17'den beri örtük olarak
 * inline'dır.
 *
 * ⚠  Ref: https://en.cppreference.com/w/cpp/language/static
 *
 * @note Compile: g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion 06_static_and_friends.cpp -o 06_static_and_friends
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <iostream>   // EN: Standard I/O for console output
#include <string>     // EN: std::string for VIN and identifiers
#include <cassert>    // EN: Runtime assertions for fleet-size guard

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// SECTION 1: VehicleTelemetry — static members & friend function
// ═════════════════════════════════════════════════════════════════════════════════════════════════

// EN: Forward declaration of friend function so the class can reference it.
// TR: Sınıfın referans verebilmesi için friend fonksiyonun önceden bildirimi.
class VehicleTelemetry;
void diagnosticDump(const VehicleTelemetry& vt);

class VehicleTelemetry {
public:
    // EN: Static constexpr — compile-time constant shared by all instances.
    // TR: Static constexpr — tüm örnekler tarafından paylaşılan derleme zamanı sabiti.
    static constexpr int MAX_FLEET_SIZE = 100;

    // EN: Constructor registers vehicle into the fleet.
    // TR: Yapıcı, aracı filoya kaydeder.
    explicit VehicleTelemetry(std::string vin, double speed = 0.0)
        : vin_(std::move(vin)), lastSpeed_(speed) {
        assert(totalVehiclesTracked_ < MAX_FLEET_SIZE &&
               "Fleet capacity exceeded!");
        ++totalVehiclesTracked_;
    }

    // EN: Destructor unregisters vehicle from the fleet.
    // TR: Yıkıcı, aracı filodan çıkarır.
    ~VehicleTelemetry() {
        --totalVehiclesTracked_;
    }

    // EN: Static member function — callable without an object.
    // TR: Statik üye fonksiyon — nesne olmadan çağrılabilir.
    static int getFleetCount() {
        return totalVehiclesTracked_;
    }

    // EN: Regular member to update speed telemetry.
    // TR: Hız telemetrisini güncellemek için normal üye fonksiyon.
    void updateSpeed(double newSpeed) {
        lastSpeed_ = newSpeed;
    }

    // EN: Grant friend access to the free function diagnosticDump.
    // TR: diagnosticDump serbest fonksiyonuna friend erişimi ver.
    friend void diagnosticDump(const VehicleTelemetry& vt);

private:
    // EN: Shared counter — one copy for all VehicleTelemetry objects.
    // TR: Paylaşılan sayaç — tüm VehicleTelemetry nesneleri için tek kopya.
    static int totalVehiclesTracked_;

    // EN: Per-instance private data: VIN and last recorded speed (km/h).
    // TR: Örneğe özel private veri: VIN ve son kaydedilen hız (km/sa).
    std::string vin_;
    double lastSpeed_;
};

// EN: Out-of-class definition of the static member (required for non-inline statics).
// TR: Statik üyenin sınıf dışı tanımı (inline olmayan statikler için gereklidir).
int VehicleTelemetry::totalVehiclesTracked_ = 0;

// ─── 1.1 Friend Function Implementation ──────────────────────────────────────────────────────────

// EN: diagnosticDump can read private vin_ and lastSpeed_ because it is a friend.
// TR: diagnosticDump, friend olduğu için private vin_ ve lastSpeed_'e erişebilir.
void diagnosticDump(const VehicleTelemetry& vt) {
    std::cout << "[DIAG] VIN: " << vt.vin_
              << " | Speed: " << vt.lastSpeed_ << " km/h"
              << " | Fleet size: " << VehicleTelemetry::totalVehiclesTracked_
              << "\n";
}

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// SECTION 2: Engine + CalibrationTool — friend class
// ═════════════════════════════════════════════════════════════════════════════════════════════════

// EN: Forward declaration so Engine can declare CalibrationTool as a friend.
// TR: Engine'in CalibrationTool'u friend olarak bildirebilmesi için önceden bildirim.
class CalibrationTool;

class Engine {
public:
    // EN: Construct with default factory parameters.
    // TR: Varsayılan fabrika parametreleriyle oluştur.
    explicit Engine(std::string ecuId)
        : ecuId_(std::move(ecuId)),
          fuelMapOffset_(0.0),
          timingAdvanceDeg_(12.0),
          boostPressureBar_(1.0) {}

    // EN: Public read-only interface — safe for any caller.
    // TR: Genel salt-okunur arayüz — herhangi bir çağrıcı için güvenli.
    void printStatus() const {
        std::cout << "[ENGINE " << ecuId_ << "] "
                  << "FuelOffset=" << fuelMapOffset_
                  << " | Timing=" << timingAdvanceDeg_ << "deg"
                  << " | Boost=" << boostPressureBar_ << "bar\n";
    }

    // EN: Grant full private access to CalibrationTool.
    // TR: CalibrationTool'a tam private erişim ver.
    friend class CalibrationTool;

private:
    std::string ecuId_;
    double fuelMapOffset_;     // EN: fuel-map trim offset
    double timingAdvanceDeg_;  // EN: ignition timing in degrees
    double boostPressureBar_;  // EN: turbo boost pressure in bar
};

class CalibrationTool {
public:
    // EN: Adjust fuel map offset — accesses Engine's private member directly.
    // TR: Yakıt haritası ofsetini ayarla — Engine'in private üyesine doğrudan erişir.
    static void adjustFuelMap(Engine& engine, double offset) {
        std::cout << "[CAL] Adjusting fuel map offset from "
                  << engine.fuelMapOffset_ << " to " << offset << "\n";
        engine.fuelMapOffset_ = offset;
    }

    // EN: Adjust ignition timing — accesses Engine's private member directly.
    // TR: Ateşleme zamanlamasını ayarla — Engine'in private üyesine doğrudan erişir.
    static void adjustTiming(Engine& engine, double degrees) {
        std::cout << "[CAL] Adjusting timing advance from "
                  << engine.timingAdvanceDeg_ << " to " << degrees << "deg\n";
        engine.timingAdvanceDeg_ = degrees;
    }

    // EN: Adjust boost pressure — accesses Engine's private member directly.
    // TR: Turbo basıncını ayarla — Engine'in private üyesine doğrudan erişir.
    static void adjustBoost(Engine& engine, double bar) {
        std::cout << "[CAL] Adjusting boost pressure from "
                  << engine.boostPressureBar_ << " to " << bar << "bar\n";
        engine.boostPressureBar_ = bar;
    }
};

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// SECTION 3: main() — Demonstrations
// ═════════════════════════════════════════════════════════════════════════════════════════════════

int main() {
    std::cout << "=== Module 02 | Lesson 06: Static Members & Friends ===\n\n";

    // ─── Demo 1: Static counter increments with each vehicle ─────────────────────────────────────
    // EN: Each VehicleTelemetry construction bumps the shared counter.
    // TR: Her VehicleTelemetry oluşturulduğunda paylaşılan sayaç artar.
    std::cout << "--- Demo 1: Static counter ---\n";
    std::cout << "Fleet count before any vehicle: "
              << VehicleTelemetry::getFleetCount() << "\n";

    VehicleTelemetry v1("WBA3A5C55DF123456", 85.0);
    std::cout << "After v1: fleet count = "
              << VehicleTelemetry::getFleetCount() << "\n";

    VehicleTelemetry v2("WVWZZZ3CZWE987654", 120.5);
    std::cout << "After v2: fleet count = "
              << VehicleTelemetry::getFleetCount() << "\n";

    {
        VehicleTelemetry v3("TMBJB9NE1L0123456");
        std::cout << "After v3 (scoped): fleet count = "
                  << VehicleTelemetry::getFleetCount() << "\n";
    }
    // EN: v3 destroyed when scope ends — counter decrements.
    // TR: Kapsam sona erdiğinde v3 yok edilir — sayaç azalır.
    std::cout << "After v3 destroyed: fleet count = "
              << VehicleTelemetry::getFleetCount() << "\n\n";

    // ─── Demo 2: Static method called without an object ──────────────────────────────────────────
    // EN: No object required — call directly on the class.
    // TR: Nesne gerekmez — doğrudan sınıf üzerinden çağrılır.
    std::cout << "--- Demo 2: Static method without object ---\n";
    std::cout << "VehicleTelemetry::getFleetCount() = "
              << VehicleTelemetry::getFleetCount() << "\n\n";

    // ─── Demo 3: Friend function accessing private data ──────────────────────────────────────────
    // EN: diagnosticDump reads private vin_ and lastSpeed_ directly.
    // TR: diagnosticDump, private vin_ ve lastSpeed_'i doğrudan okur.
    std::cout << "--- Demo 3: Friend function ---\n";
    v1.updateSpeed(92.3);
    diagnosticDump(v1);
    diagnosticDump(v2);
    std::cout << "\n";

    // ─── Demo 4: Friend class CalibrationTool modifying Engine ───────────────────────────────────
    // EN: CalibrationTool accesses Engine's private internals to tune params.
    // TR: CalibrationTool, parametreleri ayarlamak için Engine'in private içeriğine erişir.
    std::cout << "--- Demo 4: Friend class CalibrationTool ---\n";
    Engine myEngine("ECU-2026-TDI");
    std::cout << "Before calibration:\n";
    myEngine.printStatus();

    CalibrationTool::adjustFuelMap(myEngine, 2.5);
    CalibrationTool::adjustTiming(myEngine, 14.0);
    CalibrationTool::adjustBoost(myEngine, 1.8);

    std::cout << "After calibration:\n";
    myEngine.printStatus();
    std::cout << "\n";

    // ─── Demo 5: constexpr static constant usage ─────────────────────────────────────────────────
    // EN: MAX_FLEET_SIZE is a compile-time constant available without an instance.
    // TR: MAX_FLEET_SIZE, bir örneğe ihtiyaç duymadan kullanılabilen derleme zamanı sabitidir.
    std::cout << "--- Demo 5: constexpr static constant ---\n";
    std::cout << "Maximum fleet capacity: "
              << VehicleTelemetry::MAX_FLEET_SIZE << " vehicles\n";
    std::cout << "Current fleet utilization: "
              << VehicleTelemetry::getFleetCount() << " / "
              << VehicleTelemetry::MAX_FLEET_SIZE << "\n\n";

    std::cout << "=== All demos completed successfully. ===\n";
    return 0;
}
