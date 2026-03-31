/**
 * @file module_10_design_patterns/06_pattern_decision_guide.cpp
 * @brief Pattern Decision Guide — When to Use, When NOT to Use, Comparisons, Missing Patterns /
 * Kalıp Karar Rehberi — Ne Zaman Kullan, Ne Zaman Kullanma, Karşılaştırmalar, Eksik Kalıplar
 *
 * @details
 * =============================================================================
 * [THEORY: Why a Decision Guide? / TEORİ: Neden Bir Karar Rehberi?]
 * =============================================================================
 * EN: Knowing HOW to implement a pattern is only half the battle. The harder skill is knowing
 * WHEN to use it, WHEN NOT to use it, and which pattern to choose among similar alternatives.
 * This file provides:
 *     1. Anti-pattern warnings.
 *     2. Head-to-head comparisons between confusing pairs.
 *     3. Before/after refactoring examples.
 *     4. Two missing patterns: Proxy and Prototype.
 *
 * TR: Bir kalıbı NASIL uygulayacağınızı bilmek savaşın sadece yarısıdır. Daha zor beceri, NE
 * ZAMAN kullanacağınızı, NE ZAMAN KULLANMAYACAĞINIZI ve benzer alternatifler arasında hangisini
 * seçeceğinizi bilmektir. Bu dosya şunları sunar:
 *     1. Anti-pattern uyarıları.
 *     2. Kafa karıştıran çiftler arası karşılaştırmalar.
 *     3. Öncesi/sonrası refactoring örnekleri.
 *     4. Eksik iki kalıp: Proxy ve Prototype.
 *
 * =============================================================================
 * [THEORY: Singleton — Anti-Pattern Warnings / TEORİ: Singleton — Anti-Pattern Uyarıları]
 * =============================================================================
 * EN: Singleton is the MOST overused and MOST criticized design pattern. Before using it, ask:
 *   ✓ USE when: there's a physical constraint (one GPU, one log file, one config file)
 *   ✗ DON'T USE when: you just want "global access" — that's a global variable in disguise
 *   ✗ DON'T USE when: you need testability — Singleton makes mocking nearly impossible
 *   ✗ DON'T USE when: constructor has side effects (DB connections, file I/O)
 *   ⚠ WARNING: Singleton hides dependencies. Functions that use Logger::getInstance() have an
 *     invisible dependency that doesn't appear in their parameter list, violating Dependency
 *     Inversion Principle (DIP). Prefer injecting the logger as a reference parameter.
 *
 * TR: Singleton en ÇOK aşırı kullanılan ve en ÇOK eleştirilen tasarım kalıbıdır. Kullanmadan
 * önce sorun:
 *   ✓ KULLAN: fiziksel kısıt varsa (tek GPU, tek log dosyası, tek config dosyası)
 *   ✗ KULLANMA: sadece "global erişim" istiyorsan — bu kılık değiştirmiş global değişkendir
 *   ✗ KULLANMA: test edilebilirlik gerekiyorsa — Singleton mock'lamayı neredeyse imkansız kılar
 *   ✗ KULLANMA: constructor yan etkilere sahipse (DB bağlantısı, dosya I/O)
 *   ⚠ UYARI: Singleton bağımlılıkları gizler. Logger::getInstance() kullanan fonksiyonların
 *     parametre listesinde görünmeyen gizli bağımlılığı vardır — DIP ihlali.
 *
 * =============================================================================
 * [THEORY: Observer — Memory Leak & Lifetime Risks / TEORİ: Observer — Bellek Sızıntısı ve Ömür
 * Riskleri]
 * =============================================================================
 * EN: Observer pattern has a dangerous trap: DANGLING OBSERVERS. If an observer is destroyed
 * without unsubscribing from the subject, the subject holds a dangling pointer. Next notify()
 * call → undefined behavior (crash, corruption). Solutions:
 *   • Use weak_ptr instead of raw pointers in the observer list
 *   • RAII-based subscription: destructor automatically unsubscribes
 *   • Use std::function + token-based unsubscribe (return an ID on subscribe)
 *   ⚠ WARNING: The simple EventBus in 03_behavioral_patterns.cpp uses raw pointers — this is
 *     intentionally minimal for teaching. Production code MUST handle observer lifetime.
 *
 * TR: Observer kalıbının tehlikeli tuzağı: SARKAN GÖZLEMCİLER. Bir gözlemci konudan aboneliğini
 * iptal etmeden yok edilirse, konu sarkan işaretçi tutar. Sonraki notify() çağrısı → tanımsız
 * davranış (çökme, bozulma). Çözümler:
 *   • Gözlemci listesinde ham işaretçi yerine weak_ptr kullan
 *   • RAII tabanlı abonelik: yıkıcı otomatik aboneliği iptal eder
 *   • std::function + token tabanlı abonelik iptali (subscribe'da ID döndür)
 *
 * =============================================================================
 * [THEORY: Decorator — Complexity Cost / TEORİ: Decorator — Karmaşıklık Maliyeti]
 * =============================================================================
 * EN: Each Decorator layer adds a virtual function call and a unique_ptr indirection. Stacking
 * 5+ decorators creates a deep chain that's hard to debug (stack traces become unreadable).
 *   ✓ USE when: you need 2-4 combinable behaviors at runtime
 *   ✗ DON'T USE when: all combinations are known at compile time (use templates/CRTP instead)
 *   ✗ DON'T USE when: you need to inspect the inner object (decorators obscure the wrapped type)
 *   Alternative: Policy-Based Design (see 05_testable_design.cpp) provides compile-time
 *   decoration with ZERO runtime overhead.
 *
 * TR: Her Decorator katmanı sanal fonksiyon çağrısı ve unique_ptr yönlendirmesi ekler. 5+
 * dekoratör yığmak, debug'ı zor derin zincir oluşturur (stack trace'ler okunamaz hale gelir).
 *   ✓ KULLAN: çalışma zamanında 2-4 birleştirilebilir davranış gerektiğinde
 *   ✗ KULLANMA: tüm kombinasyonlar derleme zamanında biliniyorsa (CRTP/template kullan)
 *   ✗ KULLANMA: iç nesneyi incelemeniz gerekiyorsa (dekoratörler sarılı tipi gizler)
 *
 * =============================================================================
 * [THEORY: Factory Method vs Abstract Factory — When Which? / TEORİ: Fabrika Metodu vs Soyut
 * Fabrika — Hangisi Ne Zaman?]
 * =============================================================================
 * EN: These two are the most confused pair:
 *   FACTORY METHOD: Creates ONE product. Uses inheritance — subclass overrides the factory
 *   method. The factory IS the base class itself.
 *     → Use when: you have a single product hierarchy and creation logic varies by subclass.
 *   ABSTRACT FACTORY: Creates a FAMILY of related products. Uses composition — the factory is
 *   a separate object injected into the client.
 *     → Use when: you need multiple related objects that must be from the same "theme" (e.g.,
 *       WindowsButton + WindowsCheckbox, never WindowsButton + MacCheckbox).
 *   RULE OF THUMB: "If creating ONE thing → Factory Method. If creating a SET → Abstract Factory."
 *
 * TR: Bu ikisi en çok karıştırılan çifttir:
 *   FABRİKA METODU: TEK ürün oluşturur. Kalıtım kullanır — alt sınıf fabrika metodunu override
 *   eder. Fabrika temel sınıfın KENDİSİDİR.
 *     → Kullan: tek ürün hiyerarşiniz varsa ve oluşturma mantığı alt sınıfa göre değişiyorsa.
 *   SOYUT FABRİKA: İLİŞKİLİ ürünler AİLESİ oluşturur. Bileşim kullanır — fabrika istemciye
 *   enjekte edilen ayrı bir nesnedir.
 *     → Kullan: aynı "tema"dan olması gereken birden fazla ilişkili nesne gerektiğinde.
 *   KURAL: "TEK şey oluşturuyorsan → Fabrika Metodu. SET oluşturuyorsan → Soyut Fabrika."
 *
 * =============================================================================
 * [THEORY: Strategy vs State — The Subtle Difference / TEORİ: Strategy vs State — İnce Fark]
 * =============================================================================
 * EN: Both patterns look identical in structure (context + interface + concrete classes). The
 * difference is INTENT:
 *   STRATEGY: The CLIENT chooses which algorithm to use. The object doesn't change state — the
 *   behavior is externally selected. Example: sorting algorithm selection.
 *   STATE: The OBJECT ITSELF changes its behavior based on internal transitions. The client
 *   just calls `next()` — the state decides the next state. Example: traffic light cycling.
 *   KEY TEST: "Does the object decide its own transitions?" → State.
 *              "Does the caller pick the behavior?" → Strategy.
 *
 * TR: Her iki kalıp yapısal olarak aynı görünür (bağlam + arayüz + somut sınıflar). Fark
 * NİYETTEDİR:
 *   STRATEGY: İSTEMCİ hangi algoritmayı kullanacağını seçer. Nesne durum değiştirmez —
 *   davranış dışarıdan seçilir. Örnek: sıralama algoritması seçimi.
 *   STATE: NESNENİN KENDİSİ iç geçişlere göre davranışını değiştirir. İstemci sadece `next()`
 *   çağırır — durum bir sonraki durumu belirler. Örnek: trafik ışığı döngüsü.
 *   TEST: "Nesne kendi geçişlerine mi karar veriyor?" → State.
 *          "Çağıran mı davranışı seçiyor?" → Strategy.
 *
 * =============================================================================
 * [THEORY: Adapter vs Facade — Wrapping for Different Reasons / TEORİ: Adapter vs Facade —
 * Farklı Amaçlarla Sarma]
 * =============================================================================
 * EN: Both "wrap" something, but the purpose differs:
 *   ADAPTER: Makes ONE incompatible interface match ANOTHER existing interface. The client
 *   already has an interface it expects — the adapter translates.
 *     → "I have a square peg and a round hole — I need an adapter."
 *   FACADE: Simplifies a COMPLEX SUBSYSTEM into one easy-to-use interface. There's no
 *   pre-existing interface to match — you're creating a new simplified API.
 *     → "I have 10 subsystems — I want ONE startCluster() call."
 *   ADAPTER = interface translation.  FACADE = interface simplification.
 *
 * TR: İkisi de "sarar" ama amaç farklıdır:
 *   ADAPTER: TEK uyumsuz arayüzü mevcut BAŞKA bir arayüzle eşleştirir. İstemci zaten
 *   beklediği bir arayüze sahiptir — adaptör çeviri yapar.
 *   FACADE: KARMAŞIK ALT SİSTEMİ kullanımı kolay tek bir arayüze basitleştirir. Eşleştirilecek
 *   önceden var arayüz yoktur — yeni basitleştirilmiş API oluşturursunuz.
 *   ADAPTER = arayüz çevirisi.  FACADE = arayüz basitleştirmesi.
 *
 * =============================================================================
 * [CPPREF DEPTH: std::unique_ptr and Prototype clone() / CPPREF DERİNLİK: std::unique_ptr ve
 * Prototype clone()]
 * =============================================================================
 * EN: The Prototype pattern uses a `clone()` method that returns a deep copy. In modern C++,
 * the return type should be `std::unique_ptr<Base>` to enforce clear ownership. The base class
 * declares `virtual std::unique_ptr<Base> clone() const = 0;` and each derived class returns
 * `std::make_unique<Derived>(*this)`. This relies on the copy constructor — ensure Rule of
 * Five compliance if the class manages resources. Covariant return types don't work with smart
 * pointers, so the return type must be the BASE type.
 * cppreference.com/w/cpp/memory/unique_ptr
 *
 * TR: Prototype kalıbı derin kopya döndüren bir `clone()` metodu kullanır. Modern C++'ta dönüş
 * tipi net sahiplik için `std::unique_ptr<Temel>` olmalıdır. Temel sınıf
 * `virtual std::unique_ptr<Temel> clone() const = 0;` bildirir ve her türetilmiş sınıf
 * `std::make_unique<Türetilmiş>(*this)` döndürür. Bu kopyalama yapıcısına dayanır — sınıf
 * kaynak yönetiyorsa Beşin Kuralı uyumunu sağlayın. Kovaryant dönüş tipleri akıllı
 * işaretçilerle çalışmaz, bu nedenle dönüş tipi TEMEL tip olmalıdır.
 *
 * =============================================================================
 * [CPPREF DEPTH: Proxy vs Smart Pointer — Overlapping Responsibility / CPPREF DERİNLİK: Proxy
 * vs Akıllı İşaretçi — Örtüşen Sorumluluk]
 * =============================================================================
 * EN: C++'s smart pointers (unique_ptr, shared_ptr) are themselves a form of Proxy pattern!
 * They intercept access to the raw pointer and add behavior (reference counting, automatic
 * deletion). When implementing a custom Proxy, consider whether a smart pointer with a custom
 * deleter already solves the problem. Custom Proxy is needed when you want: access control,
 * lazy initialization, logging, caching, or remote communication (RPC/gRPC stub).
 * cppreference.com/w/cpp/memory/shared_ptr
 *
 * TR: C++'ın akıllı işaretçileri (unique_ptr, shared_ptr) kendileri bir Proxy kalıbı biçimidir!
 * Ham işaretçiye erişimi yakalar ve davranış ekler (referans sayımı, otomatik silme). Özel Proxy
 * uygularken, özel silici ile akıllı işaretçinin sorunu zaten çözüp çözmediğini düşünün. Özel
 * Proxy şunlar için gereklidir: erişim kontrolü, tembel başlatma, loglama, önbellekleme veya
 * uzak iletişim (RPC/gRPC stub).
 *
 * @note Compile: g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion 06_pattern_decision_guide.cpp -o 06_pattern_decision_guide
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <iostream>
#include <memory>
#include <string>
#include <vector>

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// SECTION 1: PROTOTYPE PATTERN — Deep Cloning
// Eksik kalıp: Prototype (clone ile derin kopyalama)
// ═════════════════════════════════════════════════════════════════════════════════════════════════

// EN: Abstract base — all prototypes must implement clone().
// TR: Soyut temel — tüm prototipler clone() uygulamalıdır.
class VehicleConfig {
public:
  virtual ~VehicleConfig() = default;
  virtual std::unique_ptr<VehicleConfig> clone() const = 0;
  virtual void display() const = 0;
  virtual std::string type() const = 0;
};

// EN: Concrete prototype — full deep copy via copy constructor.
// TR: Somut prototip — kopyalama yapıcısı ile tam derin kopya.
class ElectricVehicleConfig : public VehicleConfig {
  std::string model_;
  int batteryCapacity_; // kWh
  bool hasAutopilot_;

public:
  ElectricVehicleConfig(std::string model, int battery, bool autopilot)
      : model_(std::move(model)), batteryCapacity_(battery),
        hasAutopilot_(autopilot) {}

  // EN: clone() returns unique_ptr<Base> — covariant returns don't work with smart ptrs.
  // TR: clone() unique_ptr<Temel> döndürür — kovaryant dönüş akıllı işaretçilerle çalışmaz.
  std::unique_ptr<VehicleConfig> clone() const override {
    return std::make_unique<ElectricVehicleConfig>(*this); // Uses copy ctor
  }

  void display() const override {
    std::cout << "  EV Config: " << model_ << " | " << batteryCapacity_
              << " kWh | Autopilot: " << (hasAutopilot_ ? "Yes" : "No")
              << std::endl;
  }

  std::string type() const override { return "ElectricVehicle"; }
};

class HybridVehicleConfig : public VehicleConfig {
  std::string model_;
  int engineCC_;
  int batteryCapacity_;

public:
  HybridVehicleConfig(std::string model, int cc, int battery)
      : model_(std::move(model)), engineCC_(cc), batteryCapacity_(battery) {}

  std::unique_ptr<VehicleConfig> clone() const override {
    return std::make_unique<HybridVehicleConfig>(*this);
  }

  void display() const override {
    std::cout << "  Hybrid Config: " << model_ << " | " << engineCC_
              << "cc + " << batteryCapacity_ << " kWh" << std::endl;
  }

  std::string type() const override { return "HybridVehicle"; }
};

// EN: Prototype Registry — store templates, clone on demand.
// TR: Prototip Kaydı — şablonları depola, talep üzerine klonla.
class VehicleConfigRegistry {
  std::vector<std::pair<std::string, std::unique_ptr<VehicleConfig>>>
      prototypes_;

public:
  void registerPrototype(const std::string &name,
                         std::unique_ptr<VehicleConfig> proto) {
    prototypes_.emplace_back(name, std::move(proto));
  }

  std::unique_ptr<VehicleConfig> create(const std::string &name) const {
    for (const auto &[key, proto] : prototypes_) {
      if (key == name) return proto->clone();
    }
    return nullptr;
  }
};

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// SECTION 2: PROXY PATTERN — Lazy Loading & Access Control
// Eksik kalıp: Proxy (tembel yükleme ve erişim kontrolü)
// ═════════════════════════════════════════════════════════════════════════════════════════════════

// EN: Subject interface — both real object and proxy implement this.
// TR: Konu arayüzü — hem gerçek nesne hem proxy bunu uygular.
class IDatabase {
public:
  virtual ~IDatabase() = default;
  virtual std::string query(const std::string &sql) = 0;
  virtual bool isConnected() const = 0;
};

// EN: Real (heavy) object — expensive to create (simulates DB connection).
// TR: Gerçek (ağır) nesne — oluşturması pahalı (DB bağlantısını simüle eder).
class RealDatabase : public IDatabase {
  std::string connectionString_;
  bool connected_ = false;

public:
  explicit RealDatabase(std::string connStr)
      : connectionString_(std::move(connStr)) {
    // EN: Simulates expensive connection setup.
    // TR: Pahalı bağlantı kurulumunu simüle eder.
    std::cout << "  [RealDB] Connecting to " << connectionString_
              << " (heavy operation)..." << std::endl;
    connected_ = true;
  }

  std::string query(const std::string &sql) override {
    return "[RealDB] Result for: " + sql;
  }

  bool isConnected() const override { return connected_; }
};

// EN: Proxy — Lazy initialization: creates RealDatabase only on first query().
//     Also adds access control: denies queries if user role is insufficient.
// TR: Proxy — Tembel başlatma: RealDatabase'i yalnızca ilk query()'de oluşturur.
//     Ayrıca erişim kontrolü ekler: kullanıcı rolü yetersizse sorguları reddeder.
class DatabaseProxy : public IDatabase {
  std::string connectionString_;
  std::string userRole_;
  mutable std::unique_ptr<RealDatabase> realDb_; // Lazy

public:
  DatabaseProxy(std::string connStr, std::string role)
      : connectionString_(std::move(connStr)), userRole_(std::move(role)) {
    std::cout << "  [Proxy] Created (no DB connection yet — lazy)." << std::endl;
  }

  std::string query(const std::string &sql) override {
    // EN: Access control — only "admin" and "analyst" can query.
    // TR: Erişim kontrolü — yalnızca "admin" ve "analyst" sorgulayabilir.
    if (userRole_ != "admin" && userRole_ != "analyst") {
      return "[Proxy] ACCESS DENIED for role: " + userRole_;
    }

    // EN: Lazy initialization — create real DB only when needed.
    // TR: Tembel başlatma — gerçek DB yalnızca gerektiğinde oluşturulur.
    if (!realDb_) {
      std::cout << "  [Proxy] First query — initializing real database..."
                << std::endl;
      realDb_ = std::make_unique<RealDatabase>(connectionString_);
    }

    // EN: Logging (Proxy can also add logging, caching, metrics...).
    // TR: Loglama (Proxy loglama, önbellekleme, metrik de ekleyebilir...).
    std::cout << "  [Proxy] Forwarding query: " << sql << std::endl;
    return realDb_->query(sql);
  }

  bool isConnected() const override {
    return realDb_ && realDb_->isConnected();
  }
};

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// SECTION 3: BEFORE/AFTER — Strategy Pattern Refactoring
// Öncesi/Sonrası — Strategy kalıbı ile refactoring
// ═════════════════════════════════════════════════════════════════════════════════════════════════

// EN: BEFORE (Bad) — behavior selected by if/else chain.
//     Adding a new discount type requires modifying this function (violates OCP).
// TR: ÖNCESİ (Kötü) — davranış if/else zinciriyle seçilir.
//     Yeni indirim tipi eklemek bu fonksiyonu değiştirmeyi gerektirir (OCP ihlali).
namespace before {
double applyDiscount(double price, const std::string &type) {
  if (type == "student") return price * 0.80;
  if (type == "military") return price * 0.75;
  if (type == "employee") return price * 0.70;
  // EN: Every new discount type → modify this function. Fragile!
  // TR: Her yeni indirim tipi → bu fonksiyonu değiştir. Kırılgan!
  return price;
}
} // namespace before

// EN: AFTER (Good) — Strategy pattern, open for extension, closed for modification.
// TR: SONRASI (İyi) — Strategy kalıbı, genişlemeye açık, değişikliğe kapalı.
namespace after {
class IDiscountStrategy {
public:
  virtual ~IDiscountStrategy() = default;
  virtual double apply(double price) const = 0;
  virtual std::string name() const = 0;
};

class StudentDiscount : public IDiscountStrategy {
public:
  double apply(double price) const override { return price * 0.80; }
  std::string name() const override { return "Student (20% off)"; }
};

class MilitaryDiscount : public IDiscountStrategy {
public:
  double apply(double price) const override { return price * 0.75; }
  std::string name() const override { return "Military (25% off)"; }
};

class EmployeeDiscount : public IDiscountStrategy {
public:
  double apply(double price) const override { return price * 0.70; }
  std::string name() const override { return "Employee (30% off)"; }
};

// EN: Adding VIPDiscount requires ZERO modification of existing code — just a new class.
// TR: VIPDiscount eklemek mevcut kodda SIFIR değişiklik gerektirir — sadece yeni sınıf.
class VIPDiscount : public IDiscountStrategy {
public:
  double apply(double price) const override { return price * 0.60; }
  std::string name() const override { return "VIP (40% off)"; }
};

double applyDiscount(double price, const IDiscountStrategy &strategy) {
  return strategy.apply(price);
}
} // namespace after

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// SECTION 4: BEFORE/AFTER — Observer with Safe Lifetime (weak_ptr)
// Öncesi/Sonrası — Güvenli ömürlü Observer (weak_ptr)
// ═════════════════════════════════════════════════════════════════════════════════════════════════

// EN: Safe Observer using weak_ptr — expired observers are automatically skipped.
// TR: weak_ptr kullanan güvenli Observer — süresi dolan gözlemciler otomatik atlanır.
class SafeObserver : public std::enable_shared_from_this<SafeObserver> {
public:
  virtual ~SafeObserver() = default;
  virtual void onEvent(const std::string &event) = 0;
};

class SafeEventBus {
  std::vector<std::weak_ptr<SafeObserver>> observers_;

public:
  void subscribe(std::shared_ptr<SafeObserver> obs) {
    observers_.push_back(obs); // Stores weak_ptr (won't prevent destruction)
  }

  void notify(const std::string &event) {
    // EN: Iterate and lock — skip expired observers, no dangling pointers.
    // TR: İtera et ve kilitle — süresi dolan gözlemcileri atla, sarkan işaretçi yok.
    auto it = observers_.begin();
    while (it != observers_.end()) {
      if (auto locked = it->lock()) {
        locked->onEvent(event);
        ++it;
      } else {
        // EN: Observer was destroyed — remove from list.
        // TR: Gözlemci yok edildi — listeden kaldır.
        it = observers_.erase(it);
      }
    }
  }

  size_t subscriberCount() const { return observers_.size(); }
};

class DashboardWidget : public SafeObserver {
  std::string name_;

public:
  explicit DashboardWidget(std::string n) : name_(std::move(n)) {}
  void onEvent(const std::string &event) override {
    std::cout << "  [" << name_ << "] Received: " << event << std::endl;
  }
  ~DashboardWidget() override {
    std::cout << "  [" << name_ << "] Destroyed (auto-unsubscribed)."
              << std::endl;
  }
};

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// SECTION 5: QUICK REFERENCE — Pattern Selection Cheat Sheet
// Hızlı Referans — Kalıp Seçim Tablosu
// ═════════════════════════════════════════════════════════════════════════════════════════════════

//  ┌─────────────────────┬──────────────────────────────────────────────────────┐
//  │ Problem             │ Pattern to Use                                      │
//  ├─────────────────────┼──────────────────────────────────────────────────────┤
//  │ One global instance │ Singleton (prefer DI / Dependency Injection first!) │
//  │ Hide concrete type  │ Factory Method                                      │
//  │ Family of objects   │ Abstract Factory                                    │
//  │ Complex constructor │ Builder                                             │
//  │ Clone existing obj  │ Prototype                                           │
//  │ Wrap legacy API     │ Adapter                                             │
//  │ Simplify subsystem  │ Facade                                              │
//  │ Stack behaviors     │ Decorator                                           │
//  │ Tree structure      │ Composite                                           │
//  │ Lazy/access control │ Proxy                                               │
//  │ Event notification  │ Observer                                            │
//  │ Swappable algorithm │ Strategy                                            │
//  │ Undo/redo actions   │ Command                                             │
//  │ State machine       │ State                                               │
//  ├─────────────────────┼──────────────────────────────────────────────────────┤
//  │ Compile-time strat. │ Policy-Based Design / CRTP (see 05_testable)        │
//  │ Type erasure        │ std::any / hand-rolled (see 05_testable)            │
//  └─────────────────────┴──────────────────────────────────────────────────────┘

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// MAIN — Demonstrations
// ═════════════════════════════════════════════════════════════════════════════════════════════════

int main() {
  std::cout << "=== MODULE 10: PATTERN DECISION GUIDE ===\n" << std::endl;

  // ── 1. Prototype ──
  std::cout << "--- PROTOTYPE (Clone & Registry) ---\n" << std::endl;

  VehicleConfigRegistry registry;
  registry.registerPrototype(
      "ev_standard",
      std::make_unique<ElectricVehicleConfig>("Model S", 100, true));
  registry.registerPrototype(
      "hybrid_base",
      std::make_unique<HybridVehicleConfig>("Camry Hybrid", 2500, 18));

  // EN: Clone from registry — no knowledge of concrete types needed.
  // TR: Kayıttan klonla — somut tip bilgisi gerekmez.
  auto car1 = registry.create("ev_standard");
  auto car2 = registry.create("ev_standard"); // Another clone of the same template
  auto car3 = registry.create("hybrid_base");

  std::cout << "Cloned configs:" << std::endl;
  if (car1) car1->display();
  if (car2) car2->display();
  if (car3) car3->display();
  std::cout << std::endl;

  // ── 2. Proxy ──
  std::cout << "--- PROXY (Lazy Init + Access Control) ---\n" << std::endl;

  // EN: Proxy created — no DB connection yet (lazy).
  // TR: Proxy oluşturuldu — henüz DB bağlantısı yok (tembel).
  DatabaseProxy adminDb("postgresql://prod:5432", "admin");
  DatabaseProxy guestDb("postgresql://prod:5432", "guest");

  std::cout << "\nAdmin queries:" << std::endl;
  std::cout << adminDb.query("SELECT * FROM sensors") << std::endl;
  std::cout << adminDb.query("SELECT * FROM diagnostics") << std::endl;

  std::cout << "\nGuest tries to query:" << std::endl;
  std::cout << guestDb.query("SELECT * FROM secrets") << std::endl;
  std::cout << std::endl;

  // ── 3. Before/After — Strategy ──
  std::cout << "--- BEFORE/AFTER: Strategy Refactoring ---\n" << std::endl;

  double price = 100.0;

  // BEFORE: if/else chain
  std::cout << "BEFORE (if/else): student=" << before::applyDiscount(price, "student")
            << ", military=" << before::applyDiscount(price, "military") << std::endl;

  // AFTER: Strategy pattern — new VIPDiscount added with ZERO modification
  after::StudentDiscount student;
  after::MilitaryDiscount military;
  after::VIPDiscount vip; // NEW type — no existing code changed

  std::cout << "AFTER  (Strategy): " << student.name() << "="
            << after::applyDiscount(price, student)
            << ", " << military.name() << "="
            << after::applyDiscount(price, military)
            << ", " << vip.name() << "="
            << after::applyDiscount(price, vip) << std::endl;
  std::cout << std::endl;

  // ── 4. Safe Observer (weak_ptr) ──
  std::cout << "--- SAFE OBSERVER (weak_ptr lifetime) ---\n" << std::endl;

  SafeEventBus safeBus;
  auto speedometer = std::make_shared<DashboardWidget>("Speedometer");
  auto tachometer = std::make_shared<DashboardWidget>("Tachometer");

  safeBus.subscribe(speedometer);
  safeBus.subscribe(tachometer);

  std::cout << "Subscribers: " << safeBus.subscriberCount() << std::endl;
  safeBus.notify("SPEED_UPDATE: 120 km/h");

  // EN: Destroy tachometer — it auto-unsubscribes (weak_ptr expires).
  // TR: Tachometer'ı yok et — otomatik abonelik iptali (weak_ptr süresi dolar).
  std::cout << "\nDestroying tachometer..." << std::endl;
  tachometer.reset();

  std::cout << "Notifying after destruction:" << std::endl;
  safeBus.notify("SPEED_UPDATE: 80 km/h");
  std::cout << "Subscribers after cleanup: " << safeBus.subscriberCount()
            << std::endl;

  // ── 5. Decision Summary ──
  std::cout << "\n--- PATTERN SELECTION SUMMARY ---\n" << std::endl;
  std::cout << "  Factory Method vs Abstract Factory: Single product vs Product family"
            << std::endl;
  std::cout << "  Strategy vs State: Caller picks vs Object transitions internally"
            << std::endl;
  std::cout << "  Adapter vs Facade: Interface translation vs Interface simplification"
            << std::endl;
  std::cout << "  Decorator vs CRTP: Runtime stacking vs Compile-time composition"
            << std::endl;
  std::cout << "  Singleton: Think twice — prefer Dependency Injection!" << std::endl;

  std::cout << "\n=> Pattern decision guide complete!" << std::endl;

  return 0;
}
