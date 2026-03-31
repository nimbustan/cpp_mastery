/**
 * @file module_02_oop/02_inheritance_polymorphism.cpp
 * @brief OOP Part 2: Architecture of Inheritance and Polymorphism
 *
 * @details
 * =============================================================================
 * [THEORY: What is Inheritance? Why use it?]
 * =============================================================================
 * EN: Software developers hate repeating code (DRY Principle - Don't Repeat Yourself). If we
 * have a 'Dog', a 'Cat', and a 'Bird', they all have an 'age', 'weight', and an 'eat()'
 * function. Instead of writing these properties 3 times, we create a "Base Class" called
 * 'Animal'. The Dog and Cat "inherit" everything from Animal, and only add their unique features
 * (like bark for Dog).
 *
 * =============================================================================
 * [THEORY: Polymorphism and V-Table Architecture]
 * =============================================================================
 * EN: "Give a command, let the object react according to its true nature." If we have an
 * 'Animal' pointer, but it actually points to a 'Dog', which sound will play if we call
 * `makeSound()`? This is where 'virtual' comes in. 'virtual' tells the compiler to do "Late
 * Binding / Dynamic Dispatch". "Look at the object's REAL type at runtime (using the V-Table)
 * and call the Dog's function."
 *
 * =============================================================================
 * [CPPREF DEPTH: Object Slicing & V-Table Performance Penalty]
 * =============================================================================
 * EN: "Object Slicing" occurs when you pass a Derived object by VALUE to a Base. The compiler
 * literally "slices off" the extra data! ALWAYS pass polymorphic objects by Pointer (*) or
 * Reference (&). Also, `virtual` functions have a slight CPU cost because they require an extra
 * pointer jump (vptr -> vtable) at runtime, which blocks the compiler from 'inlining' the
 * function.
 *
 * @note Compile: g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion 02_inheritance_polymorphism.cpp -o 02_inheritance_polymorphism
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <iostream>
#include <memory>
#include <vector>

/**
 * @class Animal
 * @brief Base class. Represents generic animal traits.
 */
class Animal {
public:
  Animal() { std::cout << "[Animal] Created." << std::endl; }

  /**
   * @brief [THEORY]: The Vital Importance of Virtual Destructor
   *
   * EN: If you're building a base class to be inherited from, its DESTRUCTOR MUST BE VIRTUAL!
   * Why? If we say `Animal* pet = new Dog();` and later `delete pet;`, without 'virtual', only
   * the Animal's destructor is called. The Dog's memory becomes a LEAK!
   */
  virtual ~Animal() {
    std::cout << "[Animal] Destroyed." << std::endl;
  }

  /**
   * @brief Polymorphic function.
   *
   * EN: 'virtual' allows derived classes to override this behavior.
   */
  virtual void makeSound() const {
    std::cout << "(Silence...)" << std::endl;
  }
};

/**
 * @class Dog
 * @brief Derived class. Inherits from Animal.
 */
// EN: Inheritance syntax
class Dog : public Animal {
public:
  Dog() {
    std::cout << "[Dog] Born." << std::endl;
  }

  ~Dog() override {
    std::cout << "[Dog] Destroyed." << std::endl;
  }

  /**
   * @brief Overrides the base class functionality.
   *
   * EN: 'override' is optional but HIGHLY RECOMMENDED. It tells the compiler to verify our
   * override.
   */
  void makeSound() const override {
    std::cout << "Dog says: Woof! Woof!" << std::endl;
  }
};

/**
 * @class Cat
 * @brief Another Derived class.
 */
class Cat : public Animal {
public:
  Cat() { std::cout << "[Cat] Born." << std::endl; }
  ~Cat() override {
    std::cout << "[Cat] Destroyed." << std::endl;
  }

  void makeSound() const override {
    std::cout << "Cat says: Meow!" << std::endl;
  }
};

// ═════════════════════════════════════════════════════════════════════════════════════════════════
// EN: IDiagnosticService — Pure Abstract Interface (Automotive Diagnostics)
// ═════════════════════════════════════════════════════════════════════════════════════════════════

/**
 * @class IDiagnosticService
 * @brief EN: Pure abstract interface for automotive diagnostic protocols. Any class implementing
 * this MUST provide all three methods.
 */
class IDiagnosticService {
public:
    // EN: Pure virtual — every implementation must provide a service name
    virtual std::string getName() const = 0;

    // EN: Pure virtual — runs a diagnostic check and returns pass/fail
    virtual bool runDiagnostic() = 0;

    // EN: Virtual destructor defaulted — required for safe polymorphic deletion
    virtual ~IDiagnosticService() = default;
};

// ─── OBD2Service — Concrete Implementation ───────────────────────────────────────────────────────
// EN: OBD-II (On-Board Diagnostics) standard used in all modern vehicles

class OBD2Service : public IDiagnosticService {
private:
    std::string vehicleVIN_;

public:
    explicit OBD2Service(const std::string& vin) : vehicleVIN_(vin) {
        std::cout << "[OBD2Service] Initialized for VIN: " << vehicleVIN_ << std::endl;
    }

    std::string getName() const override {
        return "OBD-II Diagnostic (VIN: " + vehicleVIN_ + ")";
    }

    bool runDiagnostic() override {
        std::cout << "[OBD2] Scanning DTCs (Diagnostic Trouble Codes) for "
                  << vehicleVIN_ << "..." << std::endl;
        std::cout << "[OBD2] Result: No fault codes detected. PASS." << std::endl;
        return true;
    }
};

// ─── CANDiagService — Another Concrete Implementation ────────────────────────────────────────────
// EN: CAN Bus (Controller Area Network) diagnostics for vehicle ECU networks

class CANDiagService : public IDiagnosticService {
private:
    int busSpeed_;

public:
    explicit CANDiagService(int busSpeedKbps) : busSpeed_(busSpeedKbps) {
        std::cout << "[CANDiagService] Initialized at " << busSpeed_
                  << " kbps" << std::endl;
    }

    std::string getName() const override {
        return "CAN Bus Diagnostic (" + std::to_string(busSpeed_) + " kbps)";
    }

    bool runDiagnostic() override {
        std::cout << "[CAN] Probing CAN bus at " << busSpeed_
                  << " kbps..." << std::endl;
        std::cout << "[CAN] Result: Bus load 42%, all nodes responsive. PASS." << std::endl;
        return true;
    }
};

// ─── FinalConfig — Demonstrates 'final' keyword on a class ───────────────────────────────────────
// EN: A class marked 'final' cannot be inherited from. Useful for sealed configuration objects
// that must not be extended.

class FinalConfig final {
private:
    std::string configName_;
    int configValue_;

public:
    FinalConfig(const std::string& name, int value)
        : configName_(name), configValue_(value) {}

    void print() const {
        std::cout << "Config: " << configName_ << " = " << configValue_ << std::endl;
    }
};

// EN: The following would cause a compile error:
// class DerivedConfig : public FinalConfig {};  // ERROR: cannot derive from 'final' class

// ─── Shape / Circle / FixedCircle — override and final on methods ────────────────────────────────
// EN: Demonstrates 'override' for correctness and 'final' to prevent further overriding

class Shape {
public:
    virtual double area() const {
        return 0.0;
    }
    virtual ~Shape() = default;
};

class Circle : public Shape {
protected:
    double radius_;

public:
    explicit Circle(double r) : radius_(r) {}

    // EN: 'override' ensures we are truly overriding a base virtual method. If we misspelled
    // 'area' as 'Area', the compiler catches it.
    double area() const override {
        return 3.14159265358979 * radius_ * radius_;
    }
};

class FixedCircle final : public Circle {
public:
    explicit FixedCircle(double r) : Circle(r) {}

    // EN: 'final' on this method means NO class inheriting from FixedCircle can override area()
    // again. (But FixedCircle itself is also 'final', so no class can inherit from it at all.)
    double area() const final {
        return 3.14159265358979 * radius_ * radius_;
    }
};

int main() {
  std::cout << "=== OOP Part 2: Polymorphism ===\n" << std::endl;

  // EN: Pointers are Base (Animal), but objects are Derived.
  Animal *pet1 = new Dog();
  std::cout << "---" << std::endl;
  Animal *pet2 = new Cat();

  std::cout << "\n[POLYMORPHISM IN ACTION]" << std::endl;

  // EN: Thanks to the V-Table, the REAL types will speak!
  pet1->makeSound();
  pet2->makeSound();

  std::cout << "\n[CLEANUP]" << std::endl;
  // EN: Virtual Destructor guarantees perfect teardown. Memory Leak = 0!
  delete pet1;
  std::cout << "---" << std::endl;
  delete pet2;

  // ═══════════════════════════════════════════════════════════════════════════════════════════════
  // EN: ADVANCED POLYMORPHISM — Automotive Diagnostic Service Demos
  // ═══════════════════════════════════════════════════════════════════════════════════════════════

  std::cout << "\n\n=== ADVANCED POLYMORPHISM: AUTOMOTIVE DIAGNOSTICS ===\n" << std::endl;

  // ─── 1. Abstract Interface — IDiagnosticService ────────────────────────────────────────────────
  // EN: We cannot instantiate IDiagnosticService directly (it has pure virtual methods). We
  // create concrete implementations through base pointers.
  std::cout << "--- Demo: Abstract Interface — IDiagnosticService ---" << std::endl;
  {
      IDiagnosticService* svc1 = new OBD2Service("WVWZZZ3CZWE123456");
      IDiagnosticService* svc2 = new CANDiagService(500);

      std::cout << "\nService 1: " << svc1->getName() << std::endl;
      svc1->runDiagnostic();

      std::cout << "\nService 2: " << svc2->getName() << std::endl;
      svc2->runDiagnostic();

      // EN: Virtual destructor ensures proper cleanup through base pointer
      delete svc1;
      delete svc2;
  }
  std::cout << std::endl;

  // ─── 2. final Class — FinalConfig ──────────────────────────────────────────────────────────────
  // EN: FinalConfig is marked 'final' — attempting to inherit from it would produce a
  // compile-time error. This is useful for sealed ECU configuration objects that must remain
  // unextended.
  std::cout << "--- Demo: final Class — FinalConfig ---" << std::endl;
  {
      FinalConfig ecuConfig("MaxRPM", 7500);
      ecuConfig.print();

      FinalConfig brakeConfig("ABSThreshold_mbar", 4200);
      brakeConfig.print();

      // EN: Uncommenting the following line would cause: error: cannot derive from 'final' base
      // 'FinalConfig'
      // class ExtendedConfig : public FinalConfig {};  // COMPILE ERROR!
  }
  std::cout << std::endl;

  // ─── 3. override Keyword — Shape / Circle ──────────────────────────────────────────────────────
  // EN: 'override' is a C++11 keyword that tells the compiler: "I intend to override a virtual
  // function in the base class. If I'm wrong (e.g., wrong signature), give me a compile error."
  // This prevents silent bugs.
  std::cout << "--- Demo: override Keyword — Shape/Circle ---" << std::endl;
  {
      Shape baseShape;
      Circle wheel(17.5);
      FixedCircle hubcap(7.0);

      // EN: Polymorphism through base pointer — correct area() called each time
      Shape* shapes[] = { &baseShape, &wheel, &hubcap };
      const char* names[] = { "Base Shape", "Wheel (Circle r=17.5)", "Hubcap (FixedCircle r=7.0)" };

      for (int i = 0; i < 3; ++i) {
          std::cout << names[i] << " area = " << shapes[i]->area() << std::endl;
      }
  }
  std::cout << std::endl;

  // ─── 4. Polymorphic Container — vector<unique_ptr<IDiagnosticService>> ─────────────────────────
  // EN: The idiomatic C++ way to manage polymorphic objects: store them in a vector of
  // unique_ptr to the base interface. No manual delete needed. This is how modern automotive
  // middleware (AUTOSAR Adaptive) manages service instances.
  std::cout << "--- Demo: Polymorphic Container — vector<unique_ptr> ---" << std::endl;
  {
      std::vector<std::unique_ptr<IDiagnosticService>> diagnostics;

      diagnostics.push_back(std::make_unique<OBD2Service>("TMBJB9NE1L0123456"));
      diagnostics.push_back(std::make_unique<CANDiagService>(250));
      diagnostics.push_back(std::make_unique<OBD2Service>("WBA3A5C55CF256789"));
      diagnostics.push_back(std::make_unique<CANDiagService>(1000));

      std::cout << "\nRunning all diagnostics in polymorphic container:" << std::endl;
      std::cout << "================================================" << std::endl;

      int passCount = 0;
      int totalCount = 0;
      for (const auto& svc : diagnostics) {
          std::cout << "\n>> " << svc->getName() << std::endl;
          if (svc->runDiagnostic()) {
              ++passCount;
          }
          ++totalCount;
      }

      std::cout << "\n================================================" << std::endl;
      std::cout << "Diagnostic Summary: " << passCount << "/" << totalCount
                << " passed." << std::endl;

      // EN: When 'diagnostics' vector goes out of scope, unique_ptr automatically deletes each
      // service. Zero memory leaks.
  }

  return 0;
}
