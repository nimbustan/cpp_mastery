/**
 * @file module_09_advanced_mechanics/07_crtp_pattern.cpp
 * @brief CRTP — Curiously Recurring Template Pattern for Static Polymorphism
 * EN: Static polymorphism, mixins, and instance counting via CRTP — achieving virtual-like
 * dispatch with zero vtable overhead for automotive firmware.
 * TR: CRTP ile statik polimorfizm, mixin'ler ve örnek sayımı — otomotiv yazılımlarında vtable
 * maliyeti olmadan sanal fonksiyon benzeri dağıtım.
 *
 * @details
 * [THEORY / TEORİ]
 *
 * ===============================================================================
 * 1. CRTP — Curiously Recurring Template Pattern
 * ===============================================================================
 * EN: CRTP is a C++ idiom where a class Derived inherits from a base class template
 * parameterized by Derived itself: class Derived : public Base<Derived> The base class can then
 * static_cast<Derived*>(this) to call methods on the derived class — all resolved at compile
 * time with no virtual dispatch.
 * TR: CRTP, bir Derived sınıfının kendisi ile parametrize edilmiş bir temel sınıf şablonundan
 * türetildiği C++ deyimidir: class Derived : public Base<Derived> Temel sınıf, türetilmiş
 * sınıftaki metotları çağırmak için static_cast<Derived*>(this) kullanabilir — hepsi derleme
 * zamanında çözülür.
 *
 * ===============================================================================
 * 2. Static Polymorphism vs Virtual Dispatch
 * ===============================================================================
 * EN: Virtual functions require a vtable pointer per object and an indirect call through the
 * vtable at runtime. CRTP resolves the call at compile time:
 *     1. No vtable pointer (8 bytes saved per object on 64-bit).
 *     2. No indirect branch (better branch prediction, inlineable by compiler).
 *     3. No heap allocation required for polymorphic behavior.
 *     Trade-off: CRTP types are not runtime-polymorphic (no base-class pointer).
 *
 * TR: Sanal fonksiyonlar nesne başına bir vtable işaretçisi ve çalışma zamanında vtable
 * üzerinden dolaylı çağrı gerektirir. CRTP çağrıyı derleme zamanında çözer:
 *     1. vtable işaretçisi yok (64-bit'te nesne başına 8 bayt tasarruf).
 *     2. Dolaylı dal yok (daha iyi dal tahmini, derleyici tarafından inline edilebilir).
 *     3. Polimorfik davranış için yığın tahsisi gerekmez.
 *     Takas: CRTP türleri çalışma zamanında polimorfik değildir (temel sınıf pointer'ı yok).
 *
 * ===============================================================================
 * 3. Use Cases: Mixins, Static Interfaces, Expression Templates
 * ===============================================================================
 * EN:
 *     1. Mixins: inject reusable functionality (logging, counting, serialization)
 *        into unrelated classes without virtual overhead.
 *     2. Static interfaces: enforce that derived classes implement certain methods,
 *        checked at compile time via static_cast.
 *     3. Expression templates: build lazy evaluation trees (e.g., Eigen library).
 *
 * TR:
 *     1. Mixin'ler: sanal maliyet olmadan ilgisiz sınıflara yeniden kullanılabilir
 *        işlevsellik (loglama, sayma, serileştirme) enjekte eder.
 *     2. Statik arayüzler: türetilmiş sınıfların belirli metotları uygulamasını
 *        derleme zamanında zorlar.
 *     3. İfade şablonları: tembel değerlendirme ağaçları oluşturur.
 *
 * ===============================================================================
 * 4. Common Pitfall: Calling Derived Methods from Base Constructor
 * ===============================================================================
 * EN: When Base<Derived>'s constructor runs, the Derived object is NOT fully constructed yet.
 * Calling static_cast<Derived*>(this)->method() from the base constructor is undefined behavior!
 * Always call derived methods from regular member functions, never from constructors or
 * destructors.
 * TR: Base<Derived> yapıcısı çalıştığında, Derived nesnesi henüz tam olarak oluşturulmamıştır.
 * Temel yapıcıdan static_cast<Derived*>(this)->method() çağırmak tanımsız davranıştır!
 * Türetilmiş metotları her zaman normal üye fonksiyonlarından çağırın, asla yapıcı veya
 * yıkıcılardan çağırmayın.
 *
 * [CPPREF DEPTH: CRTP Pitfalls and the Deducing-This Alternative (C++23) / CPPREF DERİNLİK: CRTP
 * Tuzakları ve Deducing-This Alternatifi (C++23)]
 * =============================================================================
 * EN: Pitfall #1: calling `static_cast<Derived*>(this)` in the base constructor — Derived is not
 * yet constructed, invoking its methods is UB. Pitfall #2: forgetting to pass the correct
 * Derived type (copy-paste error such as inheriting from `Base<WrongClass>` instead of
 * `Base<Derived>`). Pitfall #3: CRTP does not support runtime polymorphism — there is no common
 * base-class pointer for heterogeneous collections. C++23 deducing-this (`this auto&& self`)
 * eliminates CRTP for most use cases: the base class method deduces the exact derived type from
 * the implicit object parameter — cleaner syntax, same zero-cost dispatch, and no inheritance
 * required.
 *
 * TR: Tuzak #1: temel yapıcıda `static_cast<Derived*>(this)` çağırmak — Derived henüz
 * oluşturulmamıştır, metotlarını çağırmak UB'dir. Tuzak #2: doğru Derived türünü geçirmeyi
 * unutmak (kopyala-yapıştır hatası, örneğin `Base<Derived>` yerine `Base<YanlışSınıf>`'tan
 * türetme). Tuzak #3: CRTP çalışma zamanı polimorfizmini desteklemez — heterojen koleksiyonlar
 * için ortak bir temel sınıf işaretçisi yoktur. C++23 deducing-this (`this auto&& self`) çoğu
 * kullanım için CRTP'yi ortadan kaldırır: temel sınıf metodu, örtük nesne parametresinden tam
 * türetilmiş türü çıkarır — daha temiz sözdizimi, aynı sıfır maliyetli dağıtım, kalıtım
 * gerekmez.
 *
 * @note Compile: g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion 07_crtp_pattern.cpp -o 07_crtp_pattern
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <chrono>    // EN: High-resolution clock for benchmarking
#include <cstdint>   // EN: Fixed-width integer types
#include <iostream>   // EN: Standard I/O for console output
#include <string>     // EN: std::string for log messages

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// SECTION 1: Static Polymorphism — SensorBase with CRTP
// ═════════════════════════════════════════════════════════════════════════════════════════════════

// EN: CRTP base — defines the common algorithm, delegates specifics to Derived
// TR: CRTP tabanı — ortak algoritmayı tanımlar, özellikleri Derived'a devreder
template <typename Derived>
class SensorBase {
public:
    // EN: Template method: read → process → report (algorithm skeleton)
    // TR: Şablon metodu: oku → işle → raporla (algoritma iskeleti)
    void readAndProcess() {
        // EN: static_cast to Derived — resolved at compile time, zero overhead
        // TR: Derived'a static_cast — derleme zamanında çözülür, sıfır maliyet
        auto& self = *static_cast<Derived*>(this);
        double raw = self.doRead();
        std::cout << "  [" << self.name() << "] Raw: " << raw
                  << " → Processed: " << raw * self.scaleFactor() << std::endl;
    }
};

// ─── 1a. TemperatureSensor (NTC thermistor on coolant line) ──────────────────────────────────────

// EN: Reads coolant temperature via ADC, applies NTC conversion factor
// TR: ADC üzerinden soğutma suyu sıcaklığını okur, NTC dönüşüm katsayısı uygular
class TemperatureSensor : public SensorBase<TemperatureSensor> {
public:
    double doRead() const { return 2048.0; }
    double scaleFactor() const { return 0.0489; }
    const char* name() const { return "CoolantTemp"; }
};

// ─── 1b. PressureSensor (MAP sensor on intake manifold) ──────────────────────────────────────────

// EN: Reads manifold absolute pressure, applies bar conversion
// TR: Manifold mutlak basıncını okur, bar dönüşümü uygular
class PressureSensor : public SensorBase<PressureSensor> {
public:
    double doRead() const { return 3200.0; }
    double scaleFactor() const { return 0.000305; }
    const char* name() const { return "MAP_Sensor"; }
};

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// SECTION 2: CRTP Mixin — Loggable (adds logging to any class)
// ═════════════════════════════════════════════════════════════════════════════════════════════════

// EN: Mixin that injects logging capability into any Derived class via CRTP
// TR: CRTP aracılığıyla herhangi bir Derived sınıfına loglama yeteneği enjekte eden mixin
template <typename Derived>
class Loggable {
public:
    void log(const std::string& message) const {
        // EN: Access derived class's identifier without virtual dispatch
        // TR: Sanal dağıtım olmadan türetilmiş sınıfın tanımlayıcısına erişim
        const auto& self = *static_cast<const Derived*>(this);
        std::cout << "  [LOG:" << self.moduleName() << "] " << message << std::endl;
    }
};

// EN: ECU module that gets logging functionality for free via CRTP mixin
// TR: CRTP mixin aracılığıyla ücretsiz loglama işlevselliği alan ECU modülü
class ECUModule : public Loggable<ECUModule> {
    std::string name_;
public:
    explicit ECUModule(std::string name) : name_(std::move(name)) {}
    const std::string& moduleName() const { return name_; }

    void initialize() {
        log("Initialization sequence started");
        log("Self-test PASSED, module ready");
    }

    void shutdown() {
        log("Graceful shutdown initiated");
    }
};

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// SECTION 3: CRTP Mixin — InstanceCounter (per-type instance tracking)
// ═════════════════════════════════════════════════════════════════════════════════════════════════

// EN: Each Derived type gets its OWN static counter (separate template instantiation)
// TR: Her Derived türü KENDİ statik sayacını alır (ayrı şablon somutlaştırması)
template <typename T>
class InstanceCounter {
    static inline int count_ = 0;  // EN: C++17 inline static
public:
    InstanceCounter()  { ++count_; }
    InstanceCounter(const InstanceCounter&) { ++count_; }
    InstanceCounter(InstanceCounter&&) noexcept { ++count_; }
    ~InstanceCounter() { --count_; }

    static int instanceCount() { return count_; }
};

// EN: Injector hardware driver — tracks how many instances are alive
// TR: Enjektör donanım sürücüsü — kaç örneğin canlı olduğunu izler
class InjectorDriver : public InstanceCounter<InjectorDriver> {
    int cylinder_;
public:
    explicit InjectorDriver(int cyl) : cylinder_(cyl) {}
    int cylinder() const { return cylinder_; }
};

// EN: Ignition coil driver — separate counter from InjectorDriver
// TR: Ateşleme bobini sürücüsü — InjectorDriver'dan ayrı sayaç
class IgnitionCoilDriver : public InstanceCounter<IgnitionCoilDriver> {
    int cylinder_;
public:
    explicit IgnitionCoilDriver(int cyl) : cylinder_(cyl) {}
    int cylinder() const { return cylinder_; }
};

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// SECTION 4: Virtual Dispatch Baseline (for benchmark comparison)
// ═════════════════════════════════════════════════════════════════════════════════════════════════

// EN: Abstract sensor using traditional virtual dispatch
// TR: Geleneksel sanal dağıtım kullanan soyut sensör
class VirtualSensorBase {
public:
    virtual ~VirtualSensorBase() = default;
    virtual double doRead() const = 0;
    virtual double scaleFactor() const = 0;

    double readAndProcess() const {
        return doRead() * scaleFactor();
    }
};

class VirtualTemperatureSensor : public VirtualSensorBase {
public:
    double doRead() const override { return 2048.0; }
    double scaleFactor() const override { return 0.0489; }
};

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// SECTION 5: Template Method Pattern via CRTP
// ═════════════════════════════════════════════════════════════════════════════════════════════════

// EN: Diagnostic routine with fixed steps, customizable per ECU subsystem
// TR: Sabit adımları olan, ECU alt sistemi başına özelleştirilebilir teşhis rutini
template <typename Derived>
class DiagnosticRoutine {
public:
    void runFullDiagnostic() {
        auto& self = *static_cast<Derived*>(this);
        std::cout << "  [DIAG] Starting diagnostic for: " << self.subsystemName() << std::endl;
        std::cout << "  [DIAG] Step 1 — Pre-check:  " << (self.preCheck()  ? "OK" : "FAIL") <<
            std::endl;
        std::cout << "  [DIAG] Step 2 — Core test:   " << (self.coreTest()  ? "OK" : "FAIL") <<
            std::endl;
        std::cout << "  [DIAG] Step 3 — Post-check:  " << (self.postCheck() ? "OK" : "FAIL") <<
            std::endl;
        std::cout << "  [DIAG] Diagnostic complete." << std::endl;
    }
};

// EN: Fuel system diagnostic — implements the three customizable steps
// TR: Yakıt sistemi teşhisi — üç özelleştirilebilir adımı uygular
class FuelSystemDiag : public DiagnosticRoutine<FuelSystemDiag> {
public:
    const char* subsystemName() const { return "FuelSystem"; }
    bool preCheck()  const { return true;  }
    bool coreTest()  const { return true;  }
    bool postCheck() const { return true;  }
};

// EN: Brake system diagnostic
// TR: Fren sistemi teşhisi
class BrakeSystemDiag : public DiagnosticRoutine<BrakeSystemDiag> {
public:
    const char* subsystemName() const { return "BrakeSystem"; }
    bool preCheck()  const { return true;  }
    bool coreTest()  const { return false; }
    bool postCheck() const { return true;  }
};

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// MAIN — Demonstrations
// ═════════════════════════════════════════════════════════════════════════════════════════════════

int main() {
    std::cout << "╔══════════════════════════════════════════════════════════════╗\n";
    std::cout << "║  MODULE 09: CRTP — Curiously Recurring Template Pattern      ║\n";
    std::cout << "╚══════════════════════════════════════════════════════════════╝\n\n";

    // ─── Demo 1: Static polymorphism via SensorBase CRTP ─────────────────────────────────────────
    std::cout << "── Demo 1: Static Polymorphism (SensorBase CRTP) ──\n";
    TemperatureSensor tempSensor;
    PressureSensor    mapSensor;
    // EN: Same interface, different implementations — no vtable involved
    // TR: Aynı arayüz, farklı uygulamalar — vtable yok
    tempSensor.readAndProcess();
    mapSensor.readAndProcess();
    std::cout << std::endl;

    // ─── Demo 2: Loggable mixin — ECUModule gets logging for free ────────────────────────────────
    std::cout << "── Demo 2: Loggable Mixin (ECU Module) ──\n";
    ECUModule engineControl("EngineCtrl");
    ECUModule absModule("ABS_Unit");
    // EN: Logging injected via CRTP, no virtual dispatch overhead
    // TR: CRTP aracılığıyla enjekte edilen loglama, sanal dağıtım yükü yok
    engineControl.initialize();
    absModule.initialize();
    engineControl.shutdown();
    std::cout << std::endl;

    // ─── Demo 3: InstanceCounter — separate counts per type ──────────────────────────────────────
    std::cout << "── Demo 3: InstanceCounter (Per-Type Tracking) ──\n";
    InjectorDriver inj1(1), inj2(2), inj3(3), inj4(4);
    IgnitionCoilDriver coil1(1), coil2(2);
    std::cout << "  InjectorDriver instances:     "
              << InjectorDriver::instanceCount() << std::endl;
    std::cout << "  IgnitionCoilDriver instances:  "
              << IgnitionCoilDriver::instanceCount() << std::endl;
    {
        // EN: Create temporary instances inside a scope
        // TR: Bir kapsam içinde geçici örnekler oluştur
        InjectorDriver temp5(5);
        IgnitionCoilDriver temp3(3);
        std::cout << "  [inner scope] Injectors: "
                  << InjectorDriver::instanceCount()
                  << ", Coils: " << IgnitionCoilDriver::instanceCount() << std::endl;
    }
    // EN: Temporaries destroyed — count decremented
    // TR: Geçiciler yok edildi — sayaç azaltıldı
    std::cout << "  [after scope] Injectors: "
              << InjectorDriver::instanceCount()
              << ", Coils: " << IgnitionCoilDriver::instanceCount() << std::endl;
    std::cout << std::endl;

    // ─── Demo 4: Performance comparison — CRTP vs virtual dispatch ───────────────────────────────
    std::cout << "── Demo 4: CRTP vs Virtual Dispatch Benchmark ──\n";
    constexpr int iterations = 10'000'000;
    volatile double sink = 0.0;

    // EN: CRTP benchmark (compile-time dispatch, inlineable)
    // TR: CRTP karşılaştırması (derleme zamanı dağıtımı, satır içi yapılabilir)
    {
        TemperatureSensor crtp_sensor;
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < iterations; ++i) {
            auto& base = static_cast<SensorBase<TemperatureSensor>&>(crtp_sensor);
            // EN: Inline-able static dispatch
            // TR: Satır içi yapılabilen statik dağıtım
            sink = crtp_sensor.doRead() * crtp_sensor.scaleFactor();
            (void)base;
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto crtp_us = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        std::cout << "  CRTP dispatch:    " << crtp_us << " µs (" << iterations << " iterations)"
            << std::endl;
    }

    // EN: Virtual dispatch benchmark (runtime indirect call)
    // TR: Sanal dağıtım karşılaştırması (çalışma zamanı dolaylı çağrı)
    {
        VirtualTemperatureSensor virt_sensor;
        VirtualSensorBase* base_ptr = &virt_sensor;
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < iterations; ++i) {
            // EN: Indirect call through vtable
            // TR: Vtable üzerinden dolaylı çağrı
            sink = base_ptr->readAndProcess();
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto virt_us = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        std::cout << "  Virtual dispatch: " << virt_us << " µs (" << iterations << " iterations)"
            << std::endl;
    }
    (void)sink;
    std::cout << std::endl;

    // ─── Demo 5: Template method pattern via CRTP ────────────────────────────────────────────────
    std::cout << "── Demo 5: Template Method Pattern via CRTP ──\n";
    FuelSystemDiag  fuelDiag;
    BrakeSystemDiag brakeDiag;
    // EN: Same runFullDiagnostic() algorithm, different per-subsystem steps
    // TR: Aynı runFullDiagnostic() algoritması, alt sisteme göre farklı adımlar
    fuelDiag.runFullDiagnostic();
    std::cout << std::endl;
    brakeDiag.runFullDiagnostic();
    std::cout << std::endl;

    // ─── Summary ─────────────────────────────────────────────────────────────────────────────────
    std::cout << "╔════════════════════════════════════════════════════════════╗\n";
    std::cout << "║  SUMMARY / ÖZET                                            ║\n";
    std::cout << "╠════════════════════════════════════════════════════════════╣\n";
    std::cout << "║  1. CRTP: static polymorphism — zero vtable overhead       ║\n";
    std::cout << "║  2. Mixin: inject reusable behavior via CRTP inheritance   ║\n";
    std::cout << "║  3. InstanceCounter: per-type tracking with CRTP + static  ║\n";
    std::cout << "║  4. Benchmark: CRTP enables inlining, virtual does not     ║\n";
    std::cout << "║  5. Template method: CRTP enforces interface at compile    ║\n";
    std::cout << "╚════════════════════════════════════════════════════════════╝\n";

    return 0;
}
