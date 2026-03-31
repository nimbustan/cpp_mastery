/**
 * @file module_02_oop/05_encapsulation_abstraction.cpp
 * @brief OOP Part 5: Encapsulation & Abstraction
 *
 * @details
 * =============================================================================
 * [THEORY: The 4 Pillars of OOP]
 * =============================================================================
 * EN:
 *   1. Inheritance -> Prevents code duplication (DRY).
 *   2. Polymorphism -> Provides dynamic flexibility through the V-Table.
 *   3. Encapsulation -> Data hiding and security shield.
 *   4. Abstraction -> Hides the "How" and only shows the "What".
 *
 * =============================================================================
 * [THEORY: Why is Encapsulation INDISPENSABLE?]
 * =============================================================================
 * EN: Imagine a 'BankAccount' class. If the 'balance' variable is public, ANYONE from anywhere
 * in the code can do: `account.balance = 1000000;`. This breaks reality! Money should only
 * change through `deposit()` or `withdraw()`. Encapsulation hides the data (`private`) and
 * exposes safety checks (`public` methods).
 *
 * =============================================================================
 * [THEORY: What is Abstraction?]
 * =============================================================================
 * EN: When you drive a car, do you need to know the piston angles or spark plug timings? No. You
 * just press the gas pedal (Interface). That's Abstraction. "Hide the complex internal
 * mechanism, expose a simple interface." "Interface Classes" created with Pure Virtual Functions
 * (= 0) are the peak of this.
 *
 * [CPPREF DEPTH: Access Specifiers and Friend — Compiler Enforcement vs Runtime]
 * =============================================================================
 * EN: Access specifiers (`public`, `private`, `protected`) are COMPILE-TIME ONLY checks. The
 * compiler rejects `obj.privateVar` in source code, but at runtime ALL memory is flat and
 * accessible — a raw pointer cast can read any byte. This is by design: C++ trusts the
 * programmer after compilation. Security comes from the type system, not from memory hiding.
 * `friend` is not a design flaw — it is a CONTROLLED GATE. When two classes are tightly coupled
 * by design (e.g., an iterator and its container, or `operator<<` for a class), `friend` grants
 * precise access without exposing internals to the entire world via public getters. The PIMPL
 * (Pointer to IMPLementation) idiom achieves ABI-stable encapsulation: the public header
 * contains only a forward-declared Impl struct and a `std::unique_ptr<Impl>`. Changing Impl's
 * members does NOT change the public header's sizeof — downstream code need not recompile. This
 * is critical in shared libraries and automotive ECU software where ABI breaks cause fleet-wide
 * update cascades. Note: `protected` is weaker than `private` — any derived class in any
 * translation unit can access protected members, making it a wider trust boundary than many
 * developers realize.
 *
 * ⚠  Ref: https://en.cppreference.com/w/cpp/language/access
 *
 * @note Compile: g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion 05_encapsulation_abstraction.cpp -o 05_encapsulation_abstraction
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <iostream>
#include <string>

// EN: --- ENCAPSULATION EXAMPLE ---
class BankAccount {
private: // DATA HIDING
  std::string ownerName;
  double balance;

  // EN: Internal private helper function. CANNOT be called from outside. (Abstraction)
  bool isTransactionValid(double amount) const { return amount > 0; }

public:
  BankAccount(std::string name, double initialDeposit)
      : ownerName(name), balance(initialDeposit) {}

  // EN: GETTER method retrieving the encapsulated real data (Read-Only access):
  double getBalance() const {
    // [Look, but don't touch!]
    return balance;
  }

  // EN: SETTER method modifying the encapsulated data through rules:
  void deposit(double amount) {
    if (isTransactionValid(amount)) {
      balance += amount;
      std::cout << "[Encapsulation] Safe deposit successful. Amount: " << amount << std::endl;
    } else {
      std::cout << "[ERROR] Cannot deposit zero or negative funds!"
                << std::endl;
    }
  }
};

// --- ABSTRACTION EXAMPLE ---

/**
 * @class IEspressoMachine
 * @brief EN: 'I' prefix indicates it is an Interface.
 *
 * EN: Contains a "Pure Virtual" function (= 0). This class has NO body. You CANNOT create an
 * object like 'new IEspressoMachine()'. It is just a "CONTRACT".
 */
class IEspressoMachine {
public:
  virtual ~IEspressoMachine() = default;

  // EN: '= 0' means just show the rule (interface), no body!
  virtual void makeCoffee() = 0;
};

// EN: The concrete class implementing the real mechanism (the 'How' details).
class ModernCoffeeMaker : public IEspressoMachine {
private:
  void grindBeans() {
    std::cout << "1. Grinding beans (0.3mm)..." << std::endl;
  }
  void boilWater() {
    std::cout << "2. Boiling water to 94C..." << std::endl;
  }
  void filterEspresso() {
    std::cout << "3. Filtering with 9 Bars..." << std::endl;
  }

public:
  void makeCoffee() override {
    // EN: The Core of Abstraction! User ONLY calls makeCoffee, they don't see the complex 3
    // steps.
    grindBeans();
    boilWater();
    filterEspresso();
    std::cout << "[Result] Your espresso is ready!\n" << std::endl;
  }
};

int main() {
  std::cout << "=== MODULE 2: ENCAPSULATION & ABSTRACTION ===\n" << std::endl;

  // EN: 1. Encapsulation Test
  std::cout << "--- 1. ENCAPSULATION ---" << std::endl;
  BankAccount myAccount("Alice", 1000.0);

  // myAccount.balance = 9999999; // ERROR!
  std::cout << "Current Balance: " << myAccount.getBalance() << std::endl;

  // EN: Security shield engages!
  myAccount.deposit(-500);

  // 2. Abstraction Test
  std::cout << "\n--- 2. ABSTRACTION ---" << std::endl;

  // EN: We use the coffee machine through the IEspressoMachine interface. We don't worry about
  // pistons, pressure, or temp. We just press the button.

  IEspressoMachine *myMachine = new ModernCoffeeMaker();
  myMachine->makeCoffee();

  delete myMachine;

  return 0;
}
