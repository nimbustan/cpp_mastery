/**
 * @file module_10_design_patterns/02_creational_patterns.cpp
 * @brief Design Patterns: Creational
 *
 * @details
 * =============================================================================
 * [THEORY: Creational Patterns Overview]
 * =============================================================================
 * EN: Creational Design Patterns control HOW objects are created. Instead of scattering `new`
 * calls throughout your codebase, these patterns centralize and encapsulate object creation.
 * This makes code more flexible, testable, and decoupled from concrete types.
 *
 *
 * =============================================================================
 * [THEORY: Singleton — One Instance, Global Access
 * =============================================================================
 * EN: The Singleton pattern ensures a class has EXACTLY ONE instance and provides a global point
 * of access to it. Common use cases: logging systems, configuration managers, hardware drivers.
 * The modern C++11 approach uses a static local variable inside a method — the compiler
 * guarantees thread-safe initialization ("Magic Static" / Scott Meyers' Singleton).
 *
 *
 * =============================================================================
 * [THEORY: Factory Method — Delegate Creation to Subclasses
 * =============================================================================
 * EN: The Factory Method defines an interface for creating objects but lets subclasses decide
 * WHICH class to instantiate. The client code calls a factory function that returns a base class
 * pointer — it never knows the concrete type. This follows the Open/Closed Principle: adding a
 * new product requires only a new subclass, no modification of existing code.
 *
 *
 * =============================================================================
 * [THEORY: Builder — Step-by-Step Complex Object Construction
 * =============================================================================
 * EN: The Builder pattern separates the CONSTRUCTION of a complex object from its
 * REPRESENTATION. Instead of a constructor with 15 parameters, you chain
 * `.setX().setY().setZ().build()` calls. Each method returns `*this` for fluent API chaining.
 * The Builder validates the object before returning it.
 *
 *
 * =============================================================================
 * [THEORY: Abstract Factory — Family of Related Objects
 * Nesneler Ailesi]
 * =============================================================================
 * EN: The Abstract Factory provides an interface for creating FAMILIES of related objects
 * without specifying their concrete classes. Example: a GUI toolkit factory that creates
 * buttons, checkboxes, and menus that all match the same theme (Windows vs. macOS). The client
 * uses the factory interface and never sees concrete implementations.
 *
 *
 * =============================================================================
 * [CPPREF DEPTH: Static Local Variable Thread Safety (C++11)
 * =============================================================================
 * EN: Since C++11, initialization of a function-local static variable is guaranteed to be
 * thread-safe. The compiler inserts a hidden guard variable to prevent double-initialization
 * even if multiple threads call the function simultaneously. This is called "Magic Statics" and
 * is the recommended way to implement Singleton in modern C++.
 * cppreference.com/w/cpp/language/storage_duration#Static_local_variables
 *
 *
 * =============================================================================
 * [CPPREF DEPTH: std::unique_ptr as Factory Return Type
 * Olarak std::unique_ptr]
 * =============================================================================
 * EN: Modern factories return `std::unique_ptr<Base>` instead of raw pointers. This enforces
 * RAII: the caller automatically owns the object and it's destroyed when the unique_ptr goes out
 * of scope. No manual `delete` needed. `std::make_unique<T>(args...)` is preferred over `new
 * T(args...)` as it's exception-safe and clearer in intent.
 * cppreference.com/w/cpp/memory/unique_ptr
 *
 *
 * @note Compile: g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion 02_creational_patterns.cpp -o 02_creational_patterns
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// [1. SINGLETON PATTERN — Thread-Safe "Magic Static"]
// ═════════════════════════════════════════════════════════════════════════════════════════════════

class Logger {
  Logger() { std::cout << "[Singleton] Logger created (only once)." << std::endl; }
  Logger(const Logger &) = delete;
  Logger &operator=(const Logger &) = delete;

public:
  // EN: C++11 guarantees thread-safe initialization of static locals.
  static Logger &getInstance() {
    static Logger instance; // Created exactly ONCE ("Magic Static")
    return instance;
  }

  void log(const std::string &msg) {
    std::cout << "[LOG] " << msg << std::endl;
  }
};

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// [2. FACTORY METHOD PATTERN]
// ═════════════════════════════════════════════════════════════════════════════════════════════════

// EN: Abstract ECU diagnostic module — each ECU type runs its own check.
class DiagnosticModule {
public:
  virtual ~DiagnosticModule() = default;
  virtual void runDiagnostic() const = 0;
  virtual std::string moduleName() const = 0;
};

class BodyControlModule : public DiagnosticModule {
public:
  void runDiagnostic() const override {
    std::cout << "BCM: Checking door locks, windows, lighting relays..."
              << std::endl;
  }
  std::string moduleName() const override { return "BodyControlModule"; }
};

class PowertrainECU : public DiagnosticModule {
public:
  void runDiagnostic() const override {
    std::cout << "Powertrain: Verifying fuel injection, ignition timing..."
              << std::endl;
  }
  std::string moduleName() const override { return "PowertrainECU"; }
};

class ClimateControlUnit : public DiagnosticModule {
public:
  void runDiagnostic() const override {
    std::cout << "HVAC: Testing compressor, blower motor, cabin temp sensor..."
              << std::endl;
  }
  std::string moduleName() const override { return "ClimateControlUnit"; }
};

// EN: Factory function — client never uses 'new' directly.
std::unique_ptr<DiagnosticModule> createModule(const std::string &type) {
  if (type == "bcm") return std::make_unique<BodyControlModule>();
  if (type == "powertrain") return std::make_unique<PowertrainECU>();
  if (type == "hvac") return std::make_unique<ClimateControlUnit>();
  return nullptr;
}

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// [3. BUILDER PATTERN — Fluent API for Complex Objects]
// ═════════════════════════════════════════════════════════════════════════════════════════════════

class HttpRequest {
  std::string method_;
  std::string url_;
  std::string body_;
  int timeout_;

  // EN: Private constructor — only Builder can create.
  HttpRequest(std::string m, std::string u, std::string b, int t)
      : method_(std::move(m)), url_(std::move(u)), body_(std::move(b)),
        timeout_(t) {}

public:
  class Builder {
    std::string method_ = "GET";
    std::string url_;
    std::string body_;
    int timeout_ = 30;

  public:
    Builder &setMethod(const std::string &m) { method_ = m; return *this; }
    Builder &setUrl(const std::string &u) { url_ = u; return *this; }
    Builder &setBody(const std::string &b) { body_ = b; return *this; }
    Builder &setTimeout(int t) { timeout_ = t; return *this; }

    HttpRequest build() {
      return HttpRequest(method_, url_, body_, timeout_);
    }
  };

  void display() const {
    std::cout << "[Builder] " << method_ << " " << url_;
    if (!body_.empty()) std::cout << " | Body: " << body_;
    std::cout << " | Timeout: " << timeout_ << "s" << std::endl;
  }
};

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// [4. ABSTRACT FACTORY PATTERN — Family of Related Objects]
// ═════════════════════════════════════════════════════════════════════════════════════════════════

// EN: Abstract product interfaces
class IButton {
public:
  virtual ~IButton() = default;
  virtual void render() const = 0;
};

class ICheckbox {
public:
  virtual ~ICheckbox() = default;
  virtual void render() const = 0;
};

// --- Windows family ---
class WindowsButton : public IButton {
public:
  void render() const override {
    std::cout << "[Win] Rendering Windows-style button." << std::endl;
  }
};
class WindowsCheckbox : public ICheckbox {
public:
  void render() const override {
    std::cout << "[Win] Rendering Windows-style checkbox." << std::endl;
  }
};

// --- macOS family ---
class MacButton : public IButton {
public:
  void render() const override {
    std::cout << "[Mac] Rendering macOS-style button." << std::endl;
  }
};
class MacCheckbox : public ICheckbox {
public:
  void render() const override {
    std::cout << "[Mac] Rendering macOS-style checkbox." << std::endl;
  }
};

// EN: Abstract Factory interface
class IGUIFactory {
public:
  virtual ~IGUIFactory() = default;
  virtual std::unique_ptr<IButton> createButton() const = 0;
  virtual std::unique_ptr<ICheckbox> createCheckbox() const = 0;
};

class WindowsFactory : public IGUIFactory {
public:
  std::unique_ptr<IButton> createButton() const override {
    return std::make_unique<WindowsButton>();
  }
  std::unique_ptr<ICheckbox> createCheckbox() const override {
    return std::make_unique<WindowsCheckbox>();
  }
};

class MacFactory : public IGUIFactory {
public:
  std::unique_ptr<IButton> createButton() const override {
    return std::make_unique<MacButton>();
  }
  std::unique_ptr<ICheckbox> createCheckbox() const override {
    return std::make_unique<MacCheckbox>();
  }
};

// EN: Client code works with ANY factory — no concrete types mentioned.
void buildUI(const IGUIFactory &factory) {
  auto btn = factory.createButton();
  auto chk = factory.createCheckbox();
  btn->render();
  chk->render();
}

int main() {
  std::cout << "=== MODULE 10: CREATIONAL DESIGN PATTERNS ===\n" << std::endl;

  // --- 1. Singleton ---
  std::cout << "--- SINGLETON ---\n" << std::endl;
  Logger::getInstance().log("Application started.");
  Logger::getInstance().log("Processing data...");
  Logger::getInstance().log("Shutdown complete.");
  std::cout << std::endl;

  // --- 2. Factory Method ---
  std::cout << "--- FACTORY METHOD ---\n" << std::endl;
  std::vector<std::string> ecuList = {"bcm", "bcm", "powertrain", "hvac"};
  for (const auto &type : ecuList) {
    auto mod = createModule(type);
    if (mod) {
      std::cout << "Loaded: " << mod->moduleName() << " -> ";
      mod->runDiagnostic();
    }
  }
  std::cout << std::endl;

  // --- 3. Builder ---
  std::cout << "--- BUILDER ---\n" << std::endl;
  auto req1 = HttpRequest::Builder()
                  .setMethod("POST")
                  .setUrl("https://api.example.com/users")
                  .setBody(R"({"name":"Ahmet"})")
                  .setTimeout(10)
                  .build();
  auto req2 = HttpRequest::Builder()
                  .setUrl("https://api.example.com/health")
                  .build(); // Uses defaults: GET, timeout=30
  req1.display();
  req2.display();
  std::cout << std::endl;

  // --- 4. Abstract Factory ---
  std::cout << "--- ABSTRACT FACTORY ---\n" << std::endl;
  WindowsFactory wf;
  MacFactory mf;
  std::cout << "Building Windows UI:" << std::endl;
  buildUI(wf);
  std::cout << "Building macOS UI:" << std::endl;
  buildUI(mf);

  std::cout << "\n=> All creational patterns demonstrated!" << std::endl;

  return 0;
}
