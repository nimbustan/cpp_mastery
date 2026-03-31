/**
 * @file module_10_design_patterns/06_pattern_decision_guide.cpp
 * @brief Pattern Decision Guide
 *
 * @details
 * =============================================================================
 * [THEORY: Why a Decision Guide?]
 * =============================================================================
 * EN: Knowing HOW to implement a pattern is only half the battle. The harder skill is knowing
 * WHEN to use it, WHEN NOT to use it, and which pattern to choose among similar alternatives.
 * This file provides:
 *     1. Anti-pattern warnings.
 *     2. Head-to-head comparisons between confusing pairs.
 *     3. Before/after refactoring examples.
 *     4. Two missing patterns: Proxy and Prototype.
 *
 *
 * =============================================================================
 * [THEORY: Singleton — Anti-Pattern Warnings]
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
 *
 * =============================================================================
 * [THEORY: Observer — Memory Leak & Lifetime Risks
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
 *
 * =============================================================================
 * [THEORY: Decorator — Complexity Cost]
 * =============================================================================
 * EN: Each Decorator layer adds a virtual function call and a unique_ptr indirection. Stacking
 * 5+ decorators creates a deep chain that's hard to debug (stack traces become unreadable).
 *   ✓ USE when: you need 2-4 combinable behaviors at runtime
 *   ✗ DON'T USE when: all combinations are known at compile time (use templates/CRTP instead)
 *   ✗ DON'T USE when: you need to inspect the inner object (decorators obscure the wrapped type)
 *   Alternative: Policy-Based Design (see 05_testable_design.cpp) provides compile-time
 *   decoration with ZERO runtime overhead.
 *
 *
 * =============================================================================
 * [THEORY: Factory Method vs Abstract Factory — When Which?
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
 *
 * =============================================================================
 * [THEORY: Strategy vs State — The Subtle Difference]
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
 *
 * =============================================================================
 * [THEORY: Adapter vs Facade — Wrapping for Different Reasons
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
 *
 * =============================================================================
 * [CPPREF DEPTH: std::unique_ptr and Prototype clone()
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
 *
 * =============================================================================
 * [CPPREF DEPTH: Proxy vs Smart Pointer — Overlapping Responsibility
 * =============================================================================
 * EN: C++'s smart pointers (unique_ptr, shared_ptr) are themselves a form of Proxy pattern!
 * They intercept access to the raw pointer and add behavior (reference counting, automatic
 * deletion). When implementing a custom Proxy, consider whether a smart pointer with a custom
 * deleter already solves the problem. Custom Proxy is needed when you want: access control,
 * lazy initialization, logging, caching, or remote communication (RPC/gRPC stub).
 * cppreference.com/w/cpp/memory/shared_ptr
 *
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
// ═════════════════════════════════════════════════════════════════════════════════════════════════

// EN: Abstract base — all prototypes must implement clone().
class VehicleConfig {
public:
  virtual ~VehicleConfig() = default;
  virtual std::unique_ptr<VehicleConfig> clone() const = 0;
  virtual void display() const = 0;
  virtual std::string type() const = 0;
};

// EN: Concrete prototype — full deep copy via copy constructor.
class ElectricVehicleConfig : public VehicleConfig {
  std::string model_;
  int batteryCapacity_; // kWh
  bool hasAutopilot_;

public:
  ElectricVehicleConfig(std::string model, int battery, bool autopilot)
      : model_(std::move(model)), batteryCapacity_(battery),
        hasAutopilot_(autopilot) {}

  // EN: clone() returns unique_ptr<Base> — covariant returns don't work with smart ptrs.
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
// ═════════════════════════════════════════════════════════════════════════════════════════════════

// EN: Subject interface — both real object and proxy implement this.
class IDatabase {
public:
  virtual ~IDatabase() = default;
  virtual std::string query(const std::string &sql) = 0;
  virtual bool isConnected() const = 0;
};

// EN: Real (heavy) object — expensive to create (simulates DB connection).
class RealDatabase : public IDatabase {
  std::string connectionString_;
  bool connected_ = false;

public:
  explicit RealDatabase(std::string connStr)
      : connectionString_(std::move(connStr)) {
    // EN: Simulates expensive connection setup.
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
    if (userRole_ != "admin" && userRole_ != "analyst") {
      return "[Proxy] ACCESS DENIED for role: " + userRole_;
    }

    // EN: Lazy initialization — create real DB only when needed.
    if (!realDb_) {
      std::cout << "  [Proxy] First query — initializing real database..."
                << std::endl;
      realDb_ = std::make_unique<RealDatabase>(connectionString_);
    }

    // EN: Logging (Proxy can also add logging, caching, metrics...).
    std::cout << "  [Proxy] Forwarding query: " << sql << std::endl;
    return realDb_->query(sql);
  }

  bool isConnected() const override {
    return realDb_ && realDb_->isConnected();
  }
};

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// SECTION 3: BEFORE/AFTER — Strategy Pattern Refactoring
// ═════════════════════════════════════════════════════════════════════════════════════════════════

// EN: BEFORE (Bad) — behavior selected by if/else chain.
//     Adding a new discount type requires modifying this function (violates OCP).
namespace before {
double applyDiscount(double price, const std::string &type) {
  if (type == "student") return price * 0.80;
  if (type == "military") return price * 0.75;
  if (type == "employee") return price * 0.70;
  // EN: Every new discount type → modify this function. Fragile!
  return price;
}
} // namespace before

// EN: AFTER (Good) — Strategy pattern, open for extension, closed for modification.
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
// ═════════════════════════════════════════════════════════════════════════════════════════════════

// EN: Safe Observer using weak_ptr — expired observers are automatically skipped.
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
    auto it = observers_.begin();
    while (it != observers_.end()) {
      if (auto locked = it->lock()) {
        locked->onEvent(event);
        ++it;
      } else {
        // EN: Observer was destroyed — remove from list.
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
