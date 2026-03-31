/**
 * @file module_05_certification/04_exceptions_handling.cpp
 * @brief CPA/CPP Prep: Exception Handling in C++
 *
 * @details
 * =============================================================================
 * [THEORY: if-else Error Checking vs Exceptions]
 * =============================================================================
 * EN: In old C systems, when an error occurred, functions returned `-1` or `false`. The danger
 * was "Ignoring the Error Value". A developer might forget to check it.
 *
 * In C++, the `try / catch / throw` mechanism exists. If an error is thrown (`throw`), the
 * program starts tearing itself back down the Stack (Stack Unwinding) until it finds a `catch`
 * block. If you never catch it, it CRASHES (`std::terminate`).
 *
 * =============================================================================
 * [THEORY: The `noexcept` Keyword (Interview Question)]
 * =============================================================================
 * EN: C++11 introduced `noexcept` (The guarantee of NOT throwing). It tells the compiler: "I
 * swear this function is perfect. It will NEVER throw." The compiler therefore removes all
 * background try-catch safety netting, making the code INCREDIBLY FAST! Often used in 'Move
 * Constructors'.
 *
 * =============================================================================
 * [CPPREF DEPTH: The 4 Levels of Exception Safety Guarantees]
 * =============================================================================
 * EN: CppReference expects professional C++ code to provide one of these guarantees:
 *   1. Nothrow Guarantee: Will NEVER throw (like `noexcept`).
 *   2. Strong Guarantee: If an operation fails, the state is rolled back to exactly what it was
 *      before the call (Commit or Rollback - like Databases).
 *   3. Basic Guarantee: If it fails, no memory is leaked, but the object might be in an altered
 *      (but valid) state.
 *   4. No Guarantee: Fails and corrupts/leaks memory. (Unacceptable in modern C++).
 *
 * @note Compile: g++ -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion 04_exceptions_handling.cpp -o 04_exceptions_handling
 * @date 2026
 * @copyright © 2026 Murat Mecit KAHRAMANLI. All rights reserved.
 * @license Code: Apache License 2.0 | Book content: CC BY-NC-SA 4.0
 */

#include <iostream>
// EN: Contains many built-in standard exceptions
#include <stdexcept>

// EN: 1. Custom Exception Class
// EN: Usually inherited from std::exception.
class BankException : public std::exception {
public:
  // EN: Overriding what() to provide a custom message. Must be `noexcept`.
  const char *what() const noexcept override {
    return "Insufficient Funds!";
  }
};

class BankAccount {
private:
  int balance;

public:
  BankAccount(int initialBalance) : balance(initialBalance) {}

  // EN: 2. A function that THROWS an Exception
  void deposit(int amount) {
    if (amount < 0) {
      // EN: Throw! Execution stops instantly. Program searches for 'catch'.
      throw std::invalid_argument("Cannot deposit negative money!");
    }
    balance += amount;
  }

  void withdraw(int amount) {
    if (amount > balance) {
      // EN: Throwing our CUSTOM Object!
      throw BankException();
    }
    balance -= amount;
  }

  // EN: 3. NOEXCEPT GUARANTEE (Performance Boost)
  int getBalance() const noexcept {
    // EN: So simple, it's impossible to throw. Compiler optimizes it to O(1) raw speed.
    return balance;
  }
};

int main() {
  std::cout << "=== MODULE 5: EXCEPTION HANDLING (try/catch/noexcept) ===\n" << std::endl;

  BankAccount myAccount(100);

  // EN: RULE: Trap risky zones (where a throw might happen) inside a 'try' block!
  try {
    std::cout << "Transaction 1 in progress..." << std::endl;
    myAccount.deposit(50); // Balance: 150

    std::cout << "Transaction 2 in progress..." << std::endl;
    // CRASH!
    myAccount.withdraw(1000);

    std::cout << "You will never see this line!" << std::endl;
    myAccount.deposit(-5);

  }
  // EN: catch blocks are Polymorphic. Put the most specific ones FIRST, the generic ones LAST.

  catch (const BankException &e) {
    // EN: We caught our custom exception by reference (&).
    std::cout << "\n[ERROR CAUGHT - BANK]: " << e.what() << std::endl;
  } catch (const std::invalid_argument &e) {
    // EN: Standard argument error.
    std::cout << "\n[ERROR CAUGHT - ARGUMENT]: " << e.what() << std::endl;
  } catch (const std::exception &e) {
    // EN: The ultimate fallback catcher for any other std::exception.
    std::cout << "\n[ERROR CAUGHT - GENERIC]: " << e.what() << std::endl;
  }

  std::cout << "\nProgram did not crash! Life goes on!" << std::endl;
  std::cout << "Current Balance [noexcept]: " << myAccount.getBalance() <<
      std::endl;

  return 0;
}
