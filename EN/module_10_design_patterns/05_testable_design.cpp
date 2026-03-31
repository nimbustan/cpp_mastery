/**
 * @file module_10_design_patterns/05_testable_design.cpp
 * @brief Design Patterns: Testable Design
 *
 * @details
 * =============================================================================
 * [THEORY: CRTP — Curiously Recurring Template Pattern
 * =============================================================================
 * EN: CRTP is a technique where a class inherits from a template that takes ITSELF as the
 * template parameter: `class Derived : public Base<Derived>`. This enables "static polymorphism"
 * — the base class calls methods on Derived at COMPILE TIME without any virtual function
 * overhead (no vtable, no indirection). Used for: mixin injection, compile-time interfaces,
 * expression templates, and the "static interface" pattern.
 *
 *
 * =============================================================================
 * [THEORY: Type Erasure — Polymorphism Without Inheritance
 * Polimorfizm]
 * =============================================================================
 * EN: Type Erasure hides concrete types behind a uniform interface WITHOUT requiring inheritance
 * from a common base class. It combines templates, a polymorphic wrapper class, and internal
 * virtual dispatch. The standard library uses this in `std::function`, `std::any`, and
 * `std::format`. The pattern: store any type-erased object, access it through a fixed interface.
 * "Duck typing in C++".
 *
 *
 * =============================================================================
 * [THEORY: Policy-Based Design — Compile-Time Strategy Pattern
 * =============================================================================
 * EN: Policy-Based Design uses template parameters as "policies" that define behavior at COMPILE
 * TIME. Instead of runtime virtual dispatch (Strategy pattern), the behavior is baked into the
 * type itself. Zero runtime overhead! Popularized by Andrei Alexandrescu's "Modern C++ Design".
 * You can mix and match policies freely: `Widget<ThreadSafe, JSONLogging, PoolAllocator>`.
 *
 *
 * =============================================================================
 * [THEORY: Compile-Time Polymorphism vs Runtime Polymorphism
 * =============================================================================
 * EN: Runtime polymorphism (virtual functions) selects behavior at RUNTIME via vtable lookup (~1
 * cache miss per call). Compile-time polymorphism (CRTP, templates, concepts) selects behavior
 * at COMPILE TIME with zero overhead. Trade-off: compile-time = faster execution, longer compile
 * times, larger binary; runtime = slower execution, faster compilation, smaller binary. Embedded
 * & HPC prefer compile-time; plugin architectures need runtime.
 *
 *
 * =============================================================================
 * [CPPREF DEPTH: static_cast<Derived*>(this) in CRTP — The Core Mechanism
 * CRTP'de static_cast<Derived*>(this) — Temel Mekanizma]
 * =============================================================================
 * EN: In CRTP, the base class accesses derived methods via
 * `static_cast<Derived*>(this)->method()`. This is safe because the base class KNOWS (via the
 * template parameter) that `this` is actually a `Derived` object. The cast is resolved at
 * compile time with zero cost. However, calling a derived method that doesn't exist causes a
 * compile error — functioning as a static interface checker.
 * cppreference.com/w/cpp/language/static_cast
 *
 *
 * =============================================================================
 * [CPPREF DEPTH: std::any, std::function — Standard Library Type Erasure
 * =============================================================================
 * EN: `std::any` (C++17) can hold ANY copyable type and retrieve it with `std::any_cast<T>`. It
 * stores small objects inline (SBO - Small Buffer Optimization, typically 16-32 bytes) and
 * allocates larger ones on the heap. `std::function` uses the same technique for callable
 * objects. Both incur type-erasure overhead but provide maximum flexibility.
 * cppreference.com/w/cpp/utility/any
 *
 *
 * @note Compile: g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion 05_testable_design.cpp -o 05_testable_design
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <cmath>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// [1. CRTP — Curiously Recurring Template Pattern]
// ═════════════════════════════════════════════════════════════════════════════════════════════════

// EN: Base class uses static_cast to call Derived methods at compile time.
template <typename Derived> class Shape {
public:
  double area() const {
    // EN: Compile-time dispatch — no vtable!
    return static_cast<const Derived *>(this)->areaImpl();
  }

  std::string name() const {
    return static_cast<const Derived *>(this)->nameImpl();
  }

  void print() const {
    std::cout << "[CRTP] " << name() << " area = " << area() << std::endl;
  }
};

class CRTPCircle : public Shape<CRTPCircle> {
  double radius_;

public:
  explicit CRTPCircle(double r) : radius_(r) {}
  double areaImpl() const { return 3.14159 * radius_ * radius_; }
  std::string nameImpl() const { return "Circle"; }
};

class CRTPSquare : public Shape<CRTPSquare> {
  double side_;

public:
  explicit CRTPSquare(double s) : side_(s) {}
  double areaImpl() const { return side_ * side_; }
  std::string nameImpl() const { return "Square"; }
};

// EN: Static polymorphism — works with any Shape<T> at compile time.
template <typename T> void printShape(const Shape<T> &shape) {
  shape.print();
}

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// [2. TYPE ERASURE — Polymorphism Without Inheritance]
// ═════════════════════════════════════════════════════════════════════════════════════════════════
// EN: Any object that has a `draw()` method can be stored in our Drawable container — WITHOUT
// inheriting from any base class! "Duck typing".

class Drawable {
  struct Concept {
    virtual ~Concept() = default;
    virtual void draw() const = 0;
    virtual std::unique_ptr<Concept> clone() const = 0;
  };

  template <typename T> struct Model : Concept {
    T obj_;
    explicit Model(T o) : obj_(std::move(o)) {}
    void draw() const override { obj_.draw(); }
    std::unique_ptr<Concept> clone() const override {
      return std::make_unique<Model>(obj_);
    }
  };

  std::unique_ptr<Concept> pimpl_;

public:
  template <typename T>
  Drawable(T x) : pimpl_(std::make_unique<Model<T>>(std::move(x))) {}

  Drawable(const Drawable &other) : pimpl_(other.pimpl_->clone()) {}
  Drawable &operator=(const Drawable &other) {
    pimpl_ = other.pimpl_->clone();
    return *this;
  }
  Drawable(Drawable &&) = default;
  Drawable &operator=(Drawable &&) = default;

  void draw() const { pimpl_->draw(); }
};

// EN: These classes DON'T inherit from anything — but they have draw()!
struct CANMessage {
  void draw() const {
    std::cout << "[TypeErasure] Rendering CAN frame on bus monitor."
              << std::endl;
  }
};

struct LINFrame {
  std::string label;
  void draw() const {
    std::cout << "[TypeErasure] Rendering LIN frame: " << label << std::endl;
  }
};

struct DiagnosticReport {
  std::string text;
  void draw() const {
    std::cout << "[TypeErasure] Rendering DTC report: " << text << std::endl;
  }
};

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// [3. POLICY-BASED DESIGN — Compile-Time Strategy]
// ═════════════════════════════════════════════════════════════════════════════════════════════════

// --- Logging Policies ---
struct ConsoleLog {
  static void log(const std::string &msg) {
    std::cout << "[ConsoleLog] " << msg << std::endl;
  }
};

struct SilentLog {
  static void log(const std::string &) {} // No output
};

// --- Validation Policies ---
struct StrictValidation {
  static bool validate(int value) { return value > 0 && value < 1000; }
  static std::string policyName() { return "Strict"; }
};

struct RelaxedValidation {
  static bool validate(int) { return true; } // Accept everything
  static std::string policyName() { return "Relaxed"; }
};

// EN: Widget class configured by policies at COMPILE TIME.
template <typename LogPolicy, typename ValidationPolicy> class Widget {
  int value_ = 0;

public:
  void setValue(int v) {
    if (ValidationPolicy::validate(v)) {
      value_ = v;
      LogPolicy::log("Value set to " + std::to_string(v) +
                      " [" + ValidationPolicy::policyName() + "]");
    } else {
      LogPolicy::log("Validation FAILED for " + std::to_string(v) +
                      " [" + ValidationPolicy::policyName() + "]");
    }
  }
  int getValue() const { return value_; }
};

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// [4. COMPILE-TIME INTERFACE CHECKING WITH CRTP]
// ═════════════════════════════════════════════════════════════════════════════════════════════════
// EN: CRTP can enforce that Derived implements required methods — compile error if missing. This
// acts as a "static interface" / concept.

template <typename Derived> class Serializable {
public:
  std::string serialize() const {
    return static_cast<const Derived *>(this)->serializeImpl();
  }
};

class Config : public Serializable<Config> {
  std::string key_;
  std::string value_;

public:
  Config(std::string k, std::string v)
      : key_(std::move(k)), value_(std::move(v)) {}

  // EN: Required by Serializable CRTP — compile error if missing!
  std::string serializeImpl() const {
    return key_ + "=" + value_;
  }
};

int main() {
  std::cout << "=== MODULE 10: TESTABLE & ADVANCED DESIGN PATTERNS ===\n"
            << std::endl;

  // --- 1. CRTP (Static Polymorphism) ---
  std::cout << "--- CRTP (Static Polymorphism) ---\n" << std::endl;
  CRTPCircle circle(5.0);
  CRTPSquare square(4.0);
  printShape(circle);
  printShape(square);
  std::cout << std::endl;

  // --- 2. Type Erasure ---
  std::cout << "--- TYPE ERASURE (Duck Typing) ---\n" << std::endl;
  // EN: Store completely unrelated types in the same container!
  std::vector<Drawable> canvas;
  canvas.emplace_back(CANMessage{});
  canvas.emplace_back(LINFrame{"Door_Status"});
  canvas.emplace_back(DiagnosticReport{"P0300 — Random Misfire"});

  for (const auto &drawable : canvas) {
    drawable.draw();
  }
  std::cout << std::endl;

  // --- 3. Policy-Based Design ---
  std::cout << "--- POLICY-BASED DESIGN ---\n" << std::endl;

  // EN: Strict validation + Console logging
  Widget<ConsoleLog, StrictValidation> strictWidget;
  strictWidget.setValue(42);     // OK
  strictWidget.setValue(-1);     // FAILS validation
  strictWidget.setValue(99999);  // FAILS validation

  std::cout << std::endl;

  // EN: Relaxed validation + Silent logging (zero overhead!)
  Widget<SilentLog, RelaxedValidation> relaxedWidget;
  relaxedWidget.setValue(-999); // Accepted silently
  std::cout << "Relaxed widget value: " << relaxedWidget.getValue() << std::endl;

  std::cout << std::endl;

  // --- 4. CRTP as Static Interface (Serializable) ---
  std::cout << "--- CRTP STATIC INTERFACE ---\n" << std::endl;
  Config cfg("database_host", "localhost:5432");
  std::cout << "Serialized config: " << cfg.serialize() << std::endl;

  std::cout << "\n=> CRTP, Type Erasure, and Policy-Based Design demonstrated!"
            << std::endl;

  return 0;
}
